#pragma once

#include <string>
#include <vector>

#include "FilterCriteria.h"
#include "IRepository.h"
#include "ReminderService.h"
#include "Scheduler.h"
#include "UserData.h"

namespace studyflow {

/**
 * @brief Summary of completed versus planned hours over a recent time window.
 *
 * This lightweight struct is returned by analytics features and is displayed by
 * the CLI `analytics` command.
 *
 * @author Group 30
 */
struct WeeklySummary {
    double hoursCompleted{0.0}; /**< Total completed hours inside the window. */
    double hoursPlanned{0.0};   /**< Total planned hours inside the window. */
};

/**
 * @brief Per-course progress statistics.
 *
 * The completion rate is derived from completed hours divided by estimated
 * hours for all items in the same course.
 *
 * @author Group 30
 */
struct CourseCompletion {
    std::string course;          /**< Course code or name. */
    double completedHours{0.0};  /**< Sum of completed hours across course items. */
    double estimatedHours{0.0};  /**< Sum of estimated hours across course items. */
    double completionRate{0.0};  /**< Fraction completed in the range 0..1 when defined. */
};

/**
 * @brief Application-level service for item management, scheduling, analytics, and persistence.
 *
 * PlannerService is the main coordinator behind the console UI. It validates
 * user actions, updates the bound UserData object, auto-saves changes, builds
 * reminders, regenerates study plans, and computes analytics.
 *
 * @author Group 30
 */
class PlannerService {
public:
    /**
     * @brief Construct the planner service.
     *
     * @param repo Repository used for loading and saving planner state.
     *
     * @author Group 30
     */
    explicit PlannerService(IRepository* repo);

    /**
     * @brief Bind the service to the currently logged-in user's data.
     *
     * After binding, planner operations modify this user's in-memory state and
     * auto-save it back to the repository.
     *
     * @param username Username of the active account.
     * @param data Pointer to the active UserData object.
     *
     * @author Group 30
     */
    void bindUser(const std::string& username, UserData* data);

    /**
     * @brief Unbind the service from the current user.
     *
     * This is typically called during logout.
     *
     * @author Group 30
     */
    void unbindUser();

    /**
     * @brief Update the preferred study session length.
     *
     * @param hours New preferred session length in hours.
     * @param errMsg Optional output parameter for validation errors.
     * @return True when the setting is accepted and saved.
     *
     * @author Group 30
     */
    bool setSessionLength(double hours, std::string* errMsg = nullptr);

    /**
     * @brief Update the maximum number of study hours allowed per day.
     *
     * @param hours New daily cap in hours.
     * @param errMsg Optional output parameter for validation errors.
     * @return True when the setting is accepted and saved.
     *
     * @author Group 30
     */
    bool setDailyMaxHours(double hours, std::string* errMsg = nullptr);

    /**
     * @brief Enable or disable weekend scheduling.
     *
     * @param include True to allow Saturday and Sunday sessions.
     * @param errMsg Optional output parameter for validation errors.
     * @return True when the setting is saved.
     *
     * @author Group 30
     */
    bool setIncludeWeekends(bool include, std::string* errMsg = nullptr);

    /**
     * @brief Access the current user's scheduling settings.
     *
     * @param errMsg Optional output parameter used when no user is bound.
     * @return Pointer to the current settings, or nullptr if unavailable.
     *
     * @author Group 30
     */
    const SmarterSchedulingSettings* getSettings(std::string* errMsg = nullptr) const;

    /**
     * @brief Add a new study item.
     *
     * @param item Item data to validate and insert.
     * @param errMsg Optional output parameter for validation or capacity errors.
     * @return Newly assigned item id, or -1 on failure.
     *
     * @author Group 30
     */
    int addItem(StudyItem item, std::string* errMsg = nullptr);

    /**
     * @brief Edit an existing study item.
     *
     * Only fields populated in `updatedFields` are applied. When hours or due
     * date change, a note may be returned recommending plan regeneration.
     *
     * @param itemId Identifier of the item to modify.
     * @param updatedFields Partial replacement fields.
     * @param errMsg Optional output parameter for errors or informational notes.
     * @return True when the item is updated successfully.
     *
     * @author Group 30
     */
    bool editItem(int itemId, const StudyItem& updatedFields, std::string* errMsg = nullptr);

    /**
     * @brief Delete a study item and its associated sessions.
     *
     * @param itemId Identifier of the item to remove.
     * @param errMsg Optional output parameter for failure reasons.
     * @return True when the item exists and is removed.
     *
     * @author Group 30
     */
    bool deleteItem(int itemId, std::string* errMsg = nullptr);

    /**
     * @brief Return all items sorted by due date and priority.
     *
     * @return Sorted copy of the current user's study items.
     *
     * @author Group 30
     */
    std::vector<StudyItem> getAllItemsSortedByDue() const;

    /**
     * @brief Search study items by title, description, or course text.
     *
     * @param query Case-insensitive substring query.
     * @return Matching items sorted by due date and priority.
     *
     * @author Group 30
     */
    std::vector<StudyItem> searchItems(const std::string& query) const;

    /**
     * @brief Filter study items using optional criteria.
     *
     * @param criteria Optional item filter values.
     * @return Items satisfying all supplied filters.
     *
     * @author Group 30
     */
    std::vector<StudyItem> filterItems(const FilterCriteria& criteria) const;

    /**
     * @brief Mark a session as started.
     *
     * @param sessionId Session identifier.
     * @param errMsg Optional output parameter for failure reasons.
     * @return True when the session transitions to in-progress.
     *
     * @author Group 30
     */
    bool startSession(int sessionId, std::string* errMsg = nullptr);

    /**
     * @brief Mark a session as completed and update item progress.
     *
     * @param sessionId Session identifier.
     * @param errMsg Optional output parameter for failure reasons.
     * @return True when the session is completed successfully.
     *
     * @author Group 30
     */
    bool markSessionDone(int sessionId, std::string* errMsg = nullptr);

    /**
     * @brief Get session history within a date range.
     *
     * @param from Inclusive start date.
     * @param to Inclusive end date.
     * @return Matching sessions sorted chronologically.
     *
     * @author Group 30
     */
    std::vector<StudySession> getHistory(const Date& from, const Date& to) const;

    /**
     * @brief Get sessions scheduled for one day.
     *
     * @param today Day to inspect.
     * @return Sessions scheduled for that date.
     *
     * @author Group 30
     */
    std::vector<StudySession> getTodayView(const Date& today) const;

    /**
     * @brief Get sessions for a rolling multi-day window.
     *
     * @param today Start date of the window.
     * @param days Number of days to include, defaulting to one week.
     * @return Sessions scheduled in the requested window.
     *
     * @author Group 30
     */
    std::vector<StudySession> getWeekView(const Date& today, int days = 7) const;

    /**
     * @brief Build human-readable reminder messages.
     *
     * @param today Reference date used for due-soon and overdue checks.
     * @return Reminder message strings.
     *
     * @author Group 30
     */
    std::vector<std::string> getReminders(const Date& today) const;

    /**
     * @brief Compute the consecutive-day completion streak ending near today.
     *
     * @param today Reference date.
     * @return Number of consecutive days with at least one completed session.
     *
     * @author Group 30
     */
    int getStreakDays(const Date& today) const;

    /**
     * @brief Summarize planned and completed hours over a recent window.
     *
     * @param today Reference date.
     * @param days Number of days to include.
     * @return WeeklySummary struct with totals.
     *
     * @author Group 30
     */
    WeeklySummary getWeeklySummary(const Date& today, int days = 7) const;

    /**
     * @brief Compute per-course completion rates.
     *
     * @return Vector of course progress summaries.
     *
     * @author Group 30
     */
    std::vector<CourseCompletion> getCourseCompletionRates() const;

    /**
     * @brief Save the bound user's state explicitly.
     *
     * @param errMsg Optional output parameter for persistence errors.
     * @return True when the save succeeds.
     *
     * @author Group 30
     */
    bool save(std::string* errMsg = nullptr);

    /**
     * @brief Reload the bound user's state from storage.
     *
     * @param errMsg Optional output parameter for persistence errors or warnings.
     * @return True when the load succeeds.
     *
     * @author Group 30
     */
    bool load(std::string* errMsg = nullptr);

    /**
     * @brief Regenerate all planned study sessions from current items and settings.
     *
     * Existing non-completed planned sessions are replaced by a fresh schedule.
     * Completed sessions remain preserved as history.
     *
     * @param today Reference date used to place new sessions.
     * @param warnings Optional output vector for best-effort scheduling warnings.
     *
     * @author Group 30
     */
    void regeneratePlan(const Date& today, std::vector<std::string>* warnings = nullptr);

private:
    IRepository* repo_{nullptr};  /**< Repository used for persistence. */
    std::string username_;        /**< Username bound to the active planner session. */
    UserData* data_{nullptr};     /**< Non-owning pointer to active user data. */
    Scheduler scheduler_;         /**< Scheduling engine for study sessions. */
    ReminderService reminders_;   /**< Reminder builder for due and overdue items. */

    /**
     * @brief Validate required item fields before insertion or update.
     *
     * @param item Candidate study item.
     * @param errMsg Optional output parameter for validation errors.
     * @return True when the item is valid.
     *
     * @author Group 30
     */
    static bool validateItemCore_(const StudyItem& item, std::string* errMsg);

    /**
     * @brief Confirm that a user is currently bound to the planner.
     *
     * @param errMsg Optional output parameter for the not-logged-in message.
     * @return True when a valid user session is active.
     *
     * @author Group 30
     */
    bool ensureBound_(std::string* errMsg) const;

    /**
     * @brief Save the active user's data while suppressing exceptions.
     *
     * This helper is used for best-effort auto-save after successful mutations.
     *
     * @author Group 30
     */
    void autoSave_();
};

} // namespace studyflow
