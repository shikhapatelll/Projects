#pragma once

#include <string>
#include <vector>

#include "Date.h"
#include "StudyItem.h"

namespace studyflow {

/**
 * @brief Builds in-app reminder messages for overdue and upcoming work.
 *
 * Reminders are generated from saved study items rather than from study
 * sessions. This keeps the reminder logic aligned with deadlines and remaining
 * workload.
 *
 * @author Group 30
 */
class ReminderService {
public:
    /**
     * @brief Generate reminder messages for the supplied items.
     *
     * The reminder list may include overdue work and work that is due soon.
     *
     * @param items Study items to inspect.
     * @param today Reference date for due-soon calculations.
     * @return Vector of user-facing reminder strings.
     *
     * @author Group 30
     */
    std::vector<std::string> buildReminders(const std::vector<StudyItem>& items, const Date& today) const;
};

} // namespace studyflow
