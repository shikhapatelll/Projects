#pragma once

#include <optional>
#include <string>

#include "Date.h"
#include "Types.h"

namespace studyflow {

/**
 * @brief Optional filters applied when listing study items.
 *
 * All fields are optional. When a field contains a value, only items that
 * match the corresponding criterion are returned.
 *
 * @author Group 30
 */
struct FilterCriteria {
    std::optional<ItemType> type;         /**< Restrict results to one item type. */
    std::optional<int> priority;          /**< Restrict results to one priority value. */
    std::optional<Date> dueFrom;          /**< Lower inclusive due-date bound. */
    std::optional<Date> dueTo;            /**< Upper inclusive due-date bound. */
    std::optional<std::string> course;    /**< Case-insensitive course filter. */
};

} // namespace studyflow
