#pragma once

#include <string>
#include <vector>

#include "Settings.h"
#include "StudyItem.h"
#include "StudySession.h"

namespace studyflow {

/**
 * @brief Generates planned study sessions from study items and user settings.
 *
 * The scheduler uses a simple best-effort strategy: items are ordered by due
 * date and priority, then distributed across available days starting from the
 * current day. It never schedules work after an item's due date and respects
 * daily maximum hours plus the weekend inclusion toggle.
 *
 * @author Group 30
 */
class Scheduler {
public:
    /**
     * @brief Break one study item into session-sized chunks.
     *
     * The final chunk may be shorter than the preferred session length when the
     * remaining required hours do not divide evenly.
     *
     * @param item Item whose remaining work should be split.
     * @param sessionHours Preferred session length in hours.
     * @return Vector of unscheduled planned sessions linked to the item.
     *
     * @author Group 30
     */
    std::vector<StudySession> splitIntoSessions(const StudyItem& item, double sessionHours) const;

    /**
     * @brief Generate a best-effort schedule for a list of study items.
     *
     * @param items Items that still have remaining work.
     * @param today Scheduling start date.
     * @param settings User scheduling preferences.
     * @param warnings Optional output vector for insufficient-time warnings.
     * @return Planned study sessions with dates assigned.
     *
     * @author Group 30
     */
    std::vector<StudySession> schedule(const std::vector<StudyItem>& items,
                                       const Date& today,
                                       const SmarterSchedulingSettings& settings,
                                       std::vector<std::string>* warnings = nullptr) const;
};

} // namespace studyflow
