#include "studyflow/Scheduler.h"

#include <algorithm>
#include <sstream>

namespace studyflow {

std::vector<StudySession> Scheduler::splitIntoSessions(const StudyItem& item, double sessionHours) const {
    std::vector<StudySession> sessions;
    if (sessionHours <= 0.0) sessionHours = 1.0;

    double remaining = item.getRemainingHours();
    while (remaining > 1e-9) {
        StudySession session;
        session.itemId = item.id;
        session.durationHours = (remaining >= sessionHours) ? sessionHours : remaining;
        session.status = SessionStatus::Planned;
        sessions.push_back(session);
        remaining -= session.durationHours;
    }

    return sessions;
}

std::vector<StudySession> Scheduler::schedule(const std::vector<StudyItem>& items,
                                              const Date& today,
                                              const SmarterSchedulingSettings& settings,
                                              std::vector<std::string>* warnings) const {
    std::vector<StudySession> planned;

    std::vector<StudyItem> work;
    for (const auto& item : items) {
        if (item.getRemainingHours() > 0.0) {
            work.push_back(item);
        }
    }

    std::sort(work.begin(), work.end(), [](const StudyItem& a, const StudyItem& b) {
        if (a.dueDate == b.dueDate) return a.priority > b.priority;
        return a.dueDate < b.dueDate;
    });

    if (work.empty()) return planned;

    Date latest = work.front().dueDate;
    for (const auto& item : work) {
        if (latest < item.dueDate) latest = item.dueDate;
    }

    struct RemainingWork {
        int id;
        double remainingHours;
        Date due;
        int priority;
    };

    std::vector<RemainingWork> remaining;
    for (const auto& item : work) {
        remaining.push_back({item.id, item.getRemainingHours(), item.dueDate, item.priority});
    }

    double dailyMax = settings.dailyMaxHours;
    if (dailyMax < 0.0) dailyMax = 0.0;
    if (dailyMax > 24.0) dailyMax = 24.0;

    double sessionLength = settings.sessionLengthHours;
    if (sessionLength <= 0.0) sessionLength = 1.0;

    Date day = today;
    int totalDays = today.daysUntil(latest);
    if (totalDays < 0) totalDays = 0;

    for (int offset = 0; offset <= totalDays; ++offset, day = day.addDays(1)) {
        if (!settings.includeWeekends && day.isWeekend()) {
            continue;
        }

        double capacity = dailyMax;
        if (capacity <= 1e-9) {
            continue;
        }

        bool assignedAny = true;
        while (capacity > 1e-9 && assignedAny) {
            assignedAny = false;

            std::sort(remaining.begin(), remaining.end(), [](const RemainingWork& a, const RemainingWork& b) {
                if (a.due == b.due) return a.priority > b.priority;
                return a.due < b.due;
            });

            for (auto& item : remaining) {
                if (item.remainingHours <= 1e-9) continue;
                if (item.due < day) continue;

                const double hours = std::min({sessionLength, item.remainingHours, capacity});
                if (hours <= 1e-9) continue;

                StudySession session;
                session.itemId = item.id;
                session.scheduledDate = day;
                session.durationHours = hours;
                session.status = SessionStatus::Planned;
                planned.push_back(session);

                item.remainingHours -= hours;
                capacity -= hours;
                assignedAny = true;

                if (capacity <= 1e-9) break;
            }
        }
    }

    double leftover = 0.0;
    for (const auto& item : remaining) {
        if (item.remainingHours > 0.0) leftover += item.remainingHours;
    }

    if (leftover > 1e-9 && warnings) {
        std::ostringstream oss;
        oss << "Warning: Not enough available time before deadlines to schedule all work. Remaining hours: "
            << leftover;
        warnings->push_back(oss.str());
    }

    return planned;
}

} // namespace studyflow
