#pragma once

#include <string>

#include "Command.h"

namespace studyflow {

/**
 * @brief Converts raw command-line text into a structured Command object.
 *
 * The parser understands quoted strings and both `--key value` and
 * `--key=value` option styles. Unknown commands are not rejected here; the UI
 * decides how to respond after parsing.
 *
 * @author Group 30
 */
class CommandParser {
public:
    /**
     * @brief Parse one line of user input.
     *
     * The returned Command contains the command name, positional arguments,
     * and named options extracted from the raw input string.
     *
     * @param input Raw text typed by the user.
     * @return Parsed command structure.
     *
     * @author Group 30
     */
    Command parse(const std::string& input) const;
};

} // namespace studyflow
