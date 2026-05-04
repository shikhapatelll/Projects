#pragma once

#include <algorithm>
#include <cctype>
#include <string>

namespace studyflow {

/**
 * @brief Supported categories for a study item.
 *
 * StudyFlow uses item types to distinguish between assignments, quizzes,
 * exams, readings, and generic tasks. The type is used for display,
 * filtering, and persistence.
 *
 * @author Group 30
 */
enum class ItemType { Assignment, Quiz, Exam, Reading, Task, Unknown };

/**
 * @brief Lifecycle state of an auto-generated study session.
 *
 * A session starts as planned, may move into in-progress, and is eventually
 * marked completed once the user finishes it.
 *
 * @author Group 30
 */
enum class SessionStatus { Planned, InProgress, Completed };

/**
 * @brief Convert an item type into a readable string.
 *
 * @param t The item type to convert.
 * @return Human-readable item type text.
 *
 * @author Group 30
 */
inline std::string toString(ItemType t) {
    switch (t) {
        case ItemType::Assignment: return "Assignment";
        case ItemType::Quiz:       return "Quiz";
        case ItemType::Exam:       return "Exam";
        case ItemType::Reading:    return "Reading";
        case ItemType::Task:       return "Task";
        default:                   return "Unknown";
    }
}

/**
 * @brief Parse a case-insensitive string into an item type.
 *
 * @param s Input string supplied by the user or loaded from JSON.
 * @return Matching item type, or ItemType::Unknown if unsupported.
 *
 * @author Group 30
 */
inline ItemType itemTypeFromString(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    if (s == "assignment") return ItemType::Assignment;
    if (s == "quiz")       return ItemType::Quiz;
    if (s == "exam")       return ItemType::Exam;
    if (s == "reading")    return ItemType::Reading;
    if (s == "task")       return ItemType::Task;
    return ItemType::Unknown;
}

/**
 * @brief Convert a session status into a readable string.
 *
 * @param st Session status value.
 * @return Human-readable status text.
 *
 * @author Group 30
 */
inline std::string toString(SessionStatus st) {
    switch (st) {
        case SessionStatus::Planned:    return "Planned";
        case SessionStatus::InProgress: return "InProgress";
        case SessionStatus::Completed:  return "Completed";
        default:                        return "Planned";
    }
}

/**
 * @brief Parse a case-insensitive string into a session status.
 *
 * @param s Input string representation of a session status.
 * @return Parsed session status, defaulting to Planned when unrecognized.
 *
 * @author Group 30
 */
inline SessionStatus sessionStatusFromString(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    if (s == "planned") return SessionStatus::Planned;
    if (s == "inprogress" || s == "in_progress" || s == "in-progress") {
        return SessionStatus::InProgress;
    }
    if (s == "completed") return SessionStatus::Completed;
    return SessionStatus::Planned;
}

} // namespace studyflow
