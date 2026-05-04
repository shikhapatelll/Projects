#pragma once

#include <map>
#include <string>
#include <vector>

namespace studyflow {

/**
 * @brief Parsed representation of one user-entered command line.
 *
 * The command name stores the first token, positional arguments are stored in
 * order, and `opts` stores `--key value` or `--key=value` style flags.
 *
 * @author Group 30
 */
struct Command {
    std::string name;                           /**< Command verb, such as `add` or `login`. */
    std::vector<std::string> args;             /**< Positional arguments after the command name. */
    std::map<std::string, std::string> opts;   /**< Named options without the leading `--`. */
};

} // namespace studyflow
