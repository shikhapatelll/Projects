#pragma once

namespace studyflow {

/**
 * @brief Scheduling preferences that influence auto-generated study plans.
 *
 * These settings are stored per user and persisted to JSON. The scheduler uses
 * them to decide how many hours may be placed on one day, whether weekends are
 * available, and how long each individual study session should be.
 *
 * @author Group 30
 */
struct SmarterSchedulingSettings {
    double dailyMaxHours{4.0};      /**< Maximum total scheduled hours allowed per day. */
    double sessionLengthHours{1.0}; /**< Preferred study session length in hours. */
    bool includeWeekends{true};     /**< True when Saturday and Sunday may receive sessions. */
};

} // namespace studyflow
