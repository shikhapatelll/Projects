#pragma once

#include <string>

#include "Date.h"
#include "Types.h"

namespace studyflow {

/**
 * @brief Represents a piece of work the student wants to plan and track.
 *
 * A study item stores the academic context (course, type, title, due date),
 * rough workload estimate, priority, and how much progress has already been
 * completed. Auto-generated study sessions are derived from the remaining hours
 * on each item.
 *
 * @author Group 30
 */
class StudyItem {
public:
    int id{0};                        /**< Unique per-user item identifier. */
    ItemType type{ItemType::Unknown}; /**< Category of study item. */
    std::string course;               /**< Associated course name or code. */
    std::string title;                /**< Short user-facing title. */
    std::string description;          /**< Optional extra notes or details. */
    Date dueDate;                     /**< Due date for the item. */
    double estimatedHours{0.0};       /**< Total number of hours expected for the item. */
    int priority{1};                  /**< Priority on a 1..5 scale where 5 is highest. */
    double hoursCompleted{0.0};       /**< Progress already completed for the item. */

    /**
     * @brief Check whether the item is overdue.
     *
     * @param today Reference date.
     * @return True when the due date is earlier than today and work remains.
     *
     * @author Group 30
     */
    bool isOverdue(const Date& today) const;

    /**
     * @brief Compute the number of days until the item is due.
     *
     * @param today Reference date.
     * @return Signed days from today to the due date.
     *
     * @author Group 30
     */
    int daysUntilDue(const Date& today) const;

    /**
     * @brief Add completed progress hours to the item.
     *
     * Progress is capped at the estimated total hours.
     *
     * @param hours Number of hours to add.
     *
     * @author Group 30
     */
    void updateProgress(double hours);

    /**
     * @brief Get the remaining unfinished hours for the item.
     *
     * @return Non-negative remaining hours.
     *
     * @author Group 30
     */
    double getRemainingHours() const;
};

} // namespace studyflow
