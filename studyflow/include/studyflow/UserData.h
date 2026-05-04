#pragma once

#include <vector>

#include "Settings.h"
#include "StudyItem.h"
#include "StudySession.h"

namespace studyflow {

/**
 * @brief Per-user application state that is persisted to disk.
 *
 * Each account stores its own list of study items, generated sessions,
 * scheduling settings, and incrementing identifier counters. The planner binds
 * directly to one UserData object after a successful login.
 *
 * @author Group 30
 */
class UserData {
public:
    std::vector<StudyItem> items;         /**< All saved study items for the user. */
    std::vector<StudySession> sessions;   /**< Planned, in-progress, and completed sessions. */
    SmarterSchedulingSettings settings;   /**< Per-user scheduling preferences. */
    int nextItemId{1};                    /**< Next item identifier to assign. */
    int nextSessionId{1};                 /**< Next session identifier to assign. */

    /**
     * @brief Find a study item by id.
     *
     * @param itemId Identifier to search for.
     * @return Pointer to the matching item, or nullptr when absent.
     *
     * @author Group 30
     */
    StudyItem* findItem(int itemId);

    /**
     * @brief Find a study session by id.
     *
     * @param sessionId Identifier to search for.
     * @return Pointer to the matching session, or nullptr when absent.
     *
     * @author Group 30
     */
    StudySession* findSession(int sessionId);
};

} // namespace studyflow
