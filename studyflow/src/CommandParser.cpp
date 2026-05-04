#include "studyflow/CommandParser.h"

#include <cctype>

namespace studyflow {

/**
 * @brief Break a raw command line into tokens while preserving quoted text.
 *
 * Tokens are separated by whitespace unless the text is wrapped in double
 * quotes. Quote characters themselves are not kept in the output.
 *
 * @param input Raw line typed by the user.
 * @return Ordered list of parsed tokens.
 *
 * @author Group 30
 */
static std::vector<std::string> tokenize(const std::string& input) {
    std::vector<std::string> tokens;
    std::string current;
    bool inQuotes = false;

    for (size_t i = 0; i < input.size(); ++i) {
        const char c = input[i];
        if (c == '"') {
            inQuotes = !inQuotes;
            continue;
        }

        if (!inQuotes && std::isspace(static_cast<unsigned char>(c))) {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
            continue;
        }

        current.push_back(c);
    }

    if (!current.empty()) {
        tokens.push_back(current);
    }
    return tokens;
}

Command CommandParser::parse(const std::string& input) const {
    Command cmd;
    const auto tokens = tokenize(input);
    if (tokens.empty()) return cmd;

    cmd.name = tokens[0];

    for (size_t i = 1; i < tokens.size(); ++i) {
        const std::string& token = tokens[i];
        if (token.rfind("--", 0) == 0) {
            const auto eq = token.find('=');
            if (eq != std::string::npos) {
                const std::string key = token.substr(2, eq - 2);
                const std::string val = token.substr(eq + 1);
                cmd.opts[key] = val;
            } else {
                const std::string key = token.substr(2);
                std::string val;
                if (i + 1 < tokens.size() && tokens[i + 1].rfind("--", 0) != 0) {
                    val = tokens[i + 1];
                    ++i;
                }
                cmd.opts[key] = val;
            }
        } else {
            cmd.args.push_back(token);
        }
    }

    return cmd;
}

} // namespace studyflow
