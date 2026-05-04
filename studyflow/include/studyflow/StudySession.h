#pragma once

#include "Date.h"
#include "Types.h"

namespace studyflow {

/**
 * @brief Represents one scheduled block of work for a study item.
 *
 * Study sessions are generated automatically by the scheduler from remaining
 * item hours. Each session belongs to exactly one study item and tracks its own
 * date, duration, and completion state.
 *
 * @author Group 30
 */
struct StudySession {
    int id{0};                            /**< Unique per-user session identifier. */
    int itemId{0};                        /**< Identifier of the related StudyItem. */
    Date scheduledDate;                   /**< Calendar day on which the session should be done. */
    double durationHours{1.0};            /**< Length of the session in hours. */
    SessionStatus status{SessionStatus::Planned}; /**< Current session lifecycle state. */

    /**
     * @brief Transition the session into the in-progress state.
     *
     * @author Group 30
     */
    void start();

    /**
     * @brief Transition the session into the completed state.
     *
     * @author Group 30
     */
    void complete();
};

} // namespace studyflow
