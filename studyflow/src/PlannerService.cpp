#include "studyflow/PlannerService.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <map>
#include <set>

namespace studyflow {

PlannerService::PlannerService(IRepository* repo) : repo_(repo) {}

void PlannerService::bindUser(const std::string& username, UserData* data) {
    username_ = username;
    data_ = data;
}

void PlannerService::unbindUser() {
    username_.clear();
    data_ = nullptr;
}

bool PlannerService::ensureBound_(std::string* errMsg) const {
    if (!data_ || username_.empty()) {
        if (errMsg) *errMsg = "You must be logged in to do that.";
        return false;
    }
    return true;
}

/**
 * @brief Check whether a string contains only whitespace characters.
 *
 * @param s String to test.
 * @return True when the string has no visible non-space content.
 *
 * @author Group 30
 */
static bool isBlank(const std::string& s) {
    for (unsigned char c : s) {
        if (!std::isspace(c)) return false;
    }
    return true;
}

bool PlannerService::validateItemCore_(const StudyItem& item, std::string* errMsg) {
    if (item.title.empty() || isBlank(item.title)) {
        if (errMsg) *errMsg = "Validation error: title cannot be empty.";
        return false;
    }
    if (item.course.empty() || isBlank(item.course)) {
        if (errMsg) *errMsg = "Validation error: course cannot be empty.";
        return false;
    }
    if (item.dueDate == Date()) {
        if (errMsg) *errMsg = "Validation error: due date is required.";
        return false;
    }
    if (item.type == ItemType::Unknown) {
        if (errMsg) *errMsg = "Validation error: type must be assignment/exam/reading/task/quiz.";
        return false;
    }
    if (item.estimatedHours <= 0.0) {
        if (errMsg) *errMsg = "Validation error: hours must be positive.";
        return false;
    }
    if (item.priority < 1 || item.priority > 5) {
        if (errMsg) *errMsg = "Validation error: priority must be 1..5.";
        return false;
    }
    return true;
}

void PlannerService::autoSave_() {
    if (!data_ || username_.empty()) return;
    try {
        repo_->saveUser(username_, *data_);
    } catch (...) {
    }
}

bool PlannerService::setSessionLength(double hours, std::string* errMsg) {
    if (!ensureBound_(errMsg)) return false;
    if (hours <= 0.0) {
        if (errMsg) *errMsg = "Validation error: session length must be positive.";
        return false;
    }
    if (hours > 24.0) {
        if (errMsg) *errMsg = "Validation error: session length cannot exceed 24 hours.";
        return false;
    }
    data_->settings.sessionLengthHours = hours;
    autoSave_();
    return true;
}

bool PlannerService::setDailyMaxHours(double hours, std::string* errMsg) {
    if (!ensureBound_(errMsg)) return false;
    if (hours < 0.0 || hours > 24.0) {
        if (errMsg) *errMsg = "Validation error: daily max hours must be between 0 and 24.";
        return false;
    }
    data_->settings.dailyMaxHours = hours;
    autoSave_();
    return true;
}

bool PlannerService::setIncludeWeekends(bool include, std::string* errMsg) {
    if (!ensureBound_(errMsg)) return false;
    data_->settings.includeWeekends = include;
    autoSave_();
    return true;
}

const SmarterSchedulingSettings* PlannerService::getSettings(std::string* errMsg) const {
    if (!ensureBound_(errMsg)) return nullptr;
    return &data_->settings;
}

bool PlannerService::startSession(int sessionId, std::string* errMsg) {
    if (!ensureBound_(errMsg)) return false;

    StudySession* session = data_->findSession(sessionId);
    if (!session) {
        if (errMsg) *errMsg = "Session not found.";
        return false;
    }
    if (session->status == SessionStatus::Completed) {
        if (errMsg) *errMsg = "Session already completed.";
        return false;
    }
    if (session->status == SessionStatus::InProgress) {
        if (errMsg) *errMsg = "Session already in progress.";
        return false;
    }

    session->start();
    autoSave_();
    return true;
}

int PlannerService::addItem(StudyItem item, std::string* errMsg) {
    if (!ensureBound_(errMsg)) return -1;
    if (!validateItemCore_(item, errMsg)) return -1;

    if (static_cast<int>(data_->items.size()) >= 70) {
        if (errMsg) *errMsg = "Maximum number of study items reached (70).";
        return -1;
    }

    item.id = data_->nextItemId++;
    item.hoursCompleted = std::min(item.hoursCompleted, item.estimatedHours);
    data_->items.push_back(item);
    autoSave_();
    return item.id;
}

bool PlannerService::editItem(int itemId, const StudyItem& updatedFields, std::string* errMsg) {
    if (!ensureBound_(errMsg)) return false;

    StudyItem* existing = data_->findItem(itemId);
    if (!existing) {
        if (errMsg) *errMsg = "Edit failed: item not found.";
        return false;
    }

    StudyItem candidate = *existing;
    bool dueChanged = false;
    bool hoursChanged = false;

    if (!updatedFields.title.empty()) candidate.title = updatedFields.title;
    if (!updatedFields.course.empty()) candidate.course = updatedFields.course;
    if (!updatedFields.description.empty()) candidate.description = updatedFields.description;
    if (updatedFields.type != ItemType::Unknown) candidate.type = updatedFields.type;

    if (!(updatedFields.dueDate == Date())) {
        if (!(candidate.dueDate == updatedFields.dueDate)) dueChanged = true;
        candidate.dueDate = updatedFields.dueDate;
    }

    if (updatedFields.estimatedHours > 0.0) {
        if (std::fabs(candidate.estimatedHours - updatedFields.estimatedHours) > 1e-9) {
            hoursChanged = true;
        }
        candidate.estimatedHours = updatedFields.estimatedHours;
        if (candidate.hoursCompleted > candidate.estimatedHours) {
            candidate.hoursCompleted = candidate.estimatedHours;
        }
    }

    if (updatedFields.priority >= 1 && updatedFields.priority <= 5) {
        candidate.priority = updatedFields.priority;
    }

    if (!validateItemCore_(candidate, errMsg)) return false;

    *existing = candidate;
    if ((dueChanged || hoursChanged) && errMsg) {
        *errMsg = "Note: due date or hours changed — regenerate plan recommended.";
    }

    autoSave_();
    return true;
}

bool PlannerService::deleteItem(int itemId, std::string* errMsg) {
    if (!ensureBound_(errMsg)) return false;

    const auto itemIt = std::find_if(data_->items.begin(), data_->items.end(),
                                     [&](const StudyItem& item) { return item.id == itemId; });
    if (itemIt == data_->items.end()) {
        if (errMsg) *errMsg = "Delete failed: item not found.";
        return false;
    }

    data_->items.erase(itemIt);
    data_->sessions.erase(
        std::remove_if(data_->sessions.begin(), data_->sessions.end(),
                       [&](const StudySession& session) { return session.itemId == itemId; }),
        data_->sessions.end());

    autoSave_();
    return true;
}

std::vector<StudyItem> PlannerService::getAllItemsSortedByDue() const {
    std::vector<StudyItem> out;
    if (!data_) return out;

    out = data_->items;
    std::sort(out.begin(), out.end(), [](const StudyItem& a, const StudyItem& b) {
        if (a.dueDate == b.dueDate) return a.priority > b.priority;
        return a.dueDate < b.dueDate;
    });
    return out;
}

/**
 * @brief Convert text to lowercase for case-insensitive comparisons.
 *
 * @param s Input text.
 * @return Lowercased copy of the input.
 *
 * @author Group 30
 */
static std::string lower(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    for (char c : s) {
        out.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    }
    return out;
}

std::vector<StudyItem> PlannerService::searchItems(const std::string& query) const {
    std::vector<StudyItem> out;
    if (!data_) return out;

    const std::string q = lower(query);
    for (const auto& item : data_->items) {
        const std::string haystack = lower(item.title + " " + item.description + " " + item.course);
        if (haystack.find(q) != std::string::npos) {
            out.push_back(item);
        }
    }

    std::sort(out.begin(), out.end(), [](const StudyItem& a, const StudyItem& b) {
        if (a.dueDate == b.dueDate) return a.priority > b.priority;
        return a.dueDate < b.dueDate;
    });
    return out;
}

std::vector<StudyItem> PlannerService::filterItems(const FilterCriteria& criteria) const {
    std::vector<StudyItem> out;
    if (!data_) return out;

    for (const auto& item : data_->items) {
        if (criteria.type && item.type != *criteria.type) continue;
        if (criteria.priority && item.priority != *criteria.priority) continue;
        if (criteria.course && lower(item.course) != lower(*criteria.course)) continue;
        if (criteria.dueFrom && item.dueDate < *criteria.dueFrom) continue;
        if (criteria.dueTo && (*criteria.dueTo < item.dueDate)) continue;
        out.push_back(item);
    }

    std::sort(out.begin(), out.end(), [](const StudyItem& a, const StudyItem& b) {
        if (a.dueDate == b.dueDate) return a.priority > b.priority;
        return a.dueDate < b.dueDate;
    });
    return out;
}

bool PlannerService::markSessionDone(int sessionId, std::string* errMsg) {
    if (!ensureBound_(errMsg)) return false;

    StudySession* session = data_->findSession(sessionId);
    if (!session) {
        if (errMsg) *errMsg = "Session not found.";
        return false;
    }
    if (session->status == SessionStatus::Completed) {
        if (errMsg) *errMsg = "Session already completed.";
        return false;
    }

    session->complete();
    StudyItem* item = data_->findItem(session->itemId);
    if (item) item->updateProgress(session->durationHours);
    autoSave_();
    return true;
}

std::vector<StudySession> PlannerService::getHistory(const Date& from, const Date& to) const {
    std::string dummy;
    if (!ensureBound_(&dummy)) return {};

    std::vector<StudySession> out;
    for (const auto& session : data_->sessions) {
        if (session.status != SessionStatus::Completed) continue;
        if (session.scheduledDate < from) continue;
        if (to < session.scheduledDate) continue;
        out.push_back(session);
    }
    return out;
}

std::vector<StudySession> PlannerService::getTodayView(const Date& today) const {
    std::vector<StudySession> out;
    if (!data_) return out;

    for (const auto& session : data_->sessions) {
        if (session.status == SessionStatus::Completed) continue;
        if (session.scheduledDate == today) out.push_back(session);
    }

    std::sort(out.begin(), out.end(), [](const StudySession& a, const StudySession& b) {
        return a.id < b.id;
    });
    return out;
}

std::vector<StudySession> PlannerService::getWeekView(const Date& today, int days) const {
    std::vector<StudySession> out;
    if (!data_) return out;

    const Date end = today.addDays(days - 1);
    for (const auto& session : data_->sessions) {
        if (session.status == SessionStatus::Completed) continue;
        if (session.scheduledDate < today) continue;
        if (end < session.scheduledDate) continue;
        out.push_back(session);
    }

    std::sort(out.begin(), out.end(), [](const StudySession& a, const StudySession& b) {
        if (a.scheduledDate == b.scheduledDate) return a.id < b.id;
        return a.scheduledDate < b.scheduledDate;
    });
    return out;
}

std::vector<std::string> PlannerService::getReminders(const Date& today) const {
    if (!data_) return {};
    return reminders_.buildReminders(data_->items, today);
}

int PlannerService::getStreakDays(const Date& today) const {
    if (!data_) return 0;

    std::set<std::string> completedDays;
    for (const auto& session : data_->sessions) {
        if (session.status == SessionStatus::Completed) {
            completedDays.insert(session.scheduledDate.toString());
        }
    }

    int streak = 0;
    Date cursor = today;
    while (completedDays.count(cursor.toString()) > 0) {
        ++streak;
        cursor = cursor.addDays(-1);
    }
    return streak;
}

WeeklySummary PlannerService::getWeeklySummary(const Date& today, int days) const {
    WeeklySummary summary;
    if (!data_) return summary;

    const Date from = today.addDays(-(days - 1));
    for (const auto& session : data_->sessions) {
        if (session.scheduledDate < from || today < session.scheduledDate) continue;
        summary.hoursPlanned += session.durationHours;
        if (session.status == SessionStatus::Completed) {
            summary.hoursCompleted += session.durationHours;
        }
    }
    return summary;
}

std::vector<CourseCompletion> PlannerService::getCourseCompletionRates() const {
    std::vector<CourseCompletion> out;
    if (!data_) return out;

    std::map<std::string, CourseCompletion> byCourse;
    for (const auto& item : data_->items) {
        CourseCompletion& stats = byCourse[item.course];
        stats.course = item.course;
        stats.completedHours += item.hoursCompleted;
        stats.estimatedHours += item.estimatedHours;
    }

    for (auto& [course, stats] : byCourse) {
        if (stats.estimatedHours > 0.0) {
            stats.completionRate = (stats.completedHours / stats.estimatedHours) * 100.0;
            if (stats.completionRate > 100.0) stats.completionRate = 100.0;
        }
        out.push_back(stats);
    }

    std::sort(out.begin(), out.end(), [](const CourseCompletion& a, const CourseCompletion& b) {
        return lower(a.course) < lower(b.course);
    });
    return out;
}

bool PlannerService::save(std::string* errMsg) {
    if (!ensureBound_(errMsg)) return false;
    try {
        repo_->saveUser(username_, *data_);
        return true;
    } catch (const std::exception& e) {
        if (errMsg) *errMsg = std::string("Save failed: ") + e.what();
        return false;
    }
}

bool PlannerService::load(std::string* errMsg) {
    if (!ensureBound_(errMsg)) return false;
    try {
        *data_ = repo_->loadUser(username_);
        return true;
    } catch (const std::exception& e) {
        if (errMsg) *errMsg = std::string("Load failed: ") + e.what();
        return false;
    }
}

void PlannerService::regeneratePlan(const Date& today, std::vector<std::string>* warnings) {
    if (!data_) return;

    std::vector<StudySession> preserved;
    for (const auto& session : data_->sessions) {
        if (session.status == SessionStatus::Completed || session.status == SessionStatus::InProgress) {
            preserved.push_back(session);
        }
    }

    std::vector<std::string> localWarnings;
    auto planned = scheduler_.schedule(data_->items, today, data_->settings, &localWarnings);

    data_->sessions = preserved;
    for (auto& session : planned) {
        session.id = data_->nextSessionId++;
        data_->sessions.push_back(session);
    }

    if (warnings) *warnings = localWarnings;
    autoSave_();
}

} // namespace studyflow
