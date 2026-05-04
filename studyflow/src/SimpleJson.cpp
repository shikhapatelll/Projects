#include "studyflow/SimpleJson.h"

#include <cmath>
#include <cctype>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace studyflow {

/**
 * @brief Advance a cursor past ASCII whitespace.
 *
 * @param s Source JSON text.
 * @param i Cursor position updated in place.
 *
 * @author Group 30
 */
static void skipWs(const std::string& s, size_t& i) {
    while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i]))) {
        ++i;
    }
}

/**
 * @brief Build a descriptive parse exception.
 *
 * @param msg Human-readable error detail.
 * @param i Character position where parsing failed.
 * @return Runtime error carrying the formatted message.
 *
 * @author Group 30
 */
static std::runtime_error parseErr(const std::string& msg, size_t i) {
    std::ostringstream oss;
    oss << "JSON parse error at pos " << i << ": " << msg;
    return std::runtime_error(oss.str());
}

/**
 * @brief Parse a JSON string literal.
 *
 * @param s Source JSON text.
 * @param i Cursor position, advanced past the parsed string.
 * @return Decoded string value.
 *
 * @author Group 30
 */
static std::string parseString(const std::string& s, size_t& i) {
    if (s[i] != '"') throw parseErr("expected '\"'", i);
    ++i;

    std::string out;
    while (i < s.size()) {
        const char c = s[i++];
        if (c == '"') break;
        if (c == '\\') {
            if (i >= s.size()) throw parseErr("bad escape", i);
            const char e = s[i++];
            switch (e) {
                case '"': out.push_back('"'); break;
                case '\\': out.push_back('\\'); break;
                case '/': out.push_back('/'); break;
                case 'b': out.push_back('\b'); break;
                case 'f': out.push_back('\f'); break;
                case 'n': out.push_back('\n'); break;
                case 'r': out.push_back('\r'); break;
                case 't': out.push_back('\t'); break;
                default: throw parseErr("unsupported escape", i);
            }
        } else {
            out.push_back(c);
        }
    }
    return out;
}

static Json parseValue(const std::string& s, size_t& i);

/**
 * @brief Parse a JSON array value.
 *
 * @param s Source JSON text.
 * @param i Cursor position, advanced past the parsed array.
 * @return Parsed JSON array.
 *
 * @author Group 30
 */
static Json parseArray(const std::string& s, size_t& i) {
    if (s[i] != '[') throw parseErr("expected '['", i);
    ++i;

    Json::array arr;
    skipWs(s, i);
    if (i < s.size() && s[i] == ']') {
        ++i;
        return Json(arr);
    }

    while (i < s.size()) {
        skipWs(s, i);
        arr.push_back(parseValue(s, i));
        skipWs(s, i);
        if (i >= s.size()) throw parseErr("unexpected end in array", i);
        if (s[i] == ',') {
            ++i;
            continue;
        }
        if (s[i] == ']') {
            ++i;
            break;
        }
        throw parseErr("expected ',' or ']'", i);
    }

    return Json(arr);
}

/**
 * @brief Parse a JSON object value.
 *
 * @param s Source JSON text.
 * @param i Cursor position, advanced past the parsed object.
 * @return Parsed JSON object.
 *
 * @author Group 30
 */
static Json parseObject(const std::string& s, size_t& i) {
    if (s[i] != '{') throw parseErr("expected '{'", i);
    ++i;

    Json::object obj;
    skipWs(s, i);
    if (i < s.size() && s[i] == '}') {
        ++i;
        return Json(obj);
    }

    while (i < s.size()) {
        skipWs(s, i);
        if (i >= s.size() || s[i] != '"') throw parseErr("expected string key", i);
        const std::string key = parseString(s, i);
        skipWs(s, i);
        if (i >= s.size() || s[i] != ':') throw parseErr("expected ':'", i);
        ++i;
        skipWs(s, i);
        obj[key] = parseValue(s, i);
        skipWs(s, i);
        if (i >= s.size()) throw parseErr("unexpected end in object", i);
        if (s[i] == ',') {
            ++i;
            continue;
        }
        if (s[i] == '}') {
            ++i;
            break;
        }
        throw parseErr("expected ',' or '}'", i);
    }

    return Json(obj);
}

/**
 * @brief Parse a JSON number.
 *
 * @param s Source JSON text.
 * @param i Cursor position, advanced past the parsed number.
 * @return Parsed JSON numeric value.
 *
 * @author Group 30
 */
static Json parseNumber(const std::string& s, size_t& i) {
    const size_t start = i;
    if (s[i] == '-') ++i;
    while (i < s.size() && std::isdigit(static_cast<unsigned char>(s[i]))) ++i;
    if (i < s.size() && s[i] == '.') {
        ++i;
        while (i < s.size() && std::isdigit(static_cast<unsigned char>(s[i]))) ++i;
    }
    if (i < s.size() && (s[i] == 'e' || s[i] == 'E')) {
        ++i;
        if (i < s.size() && (s[i] == '+' || s[i] == '-')) ++i;
        while (i < s.size() && std::isdigit(static_cast<unsigned char>(s[i]))) ++i;
    }

    try {
        return Json(std::stod(s.substr(start, i - start)));
    } catch (...) {
        throw parseErr("invalid number", start);
    }
}

/**
 * @brief Match a fixed JSON keyword such as true, false, or null.
 *
 * @param s Source JSON text.
 * @param i Cursor position, advanced on success.
 * @param lit Literal keyword to match.
 * @return True when the keyword matches at the current position.
 *
 * @author Group 30
 */
static bool matchLiteral(const std::string& s, size_t& i, const char* lit) {
    const size_t n = std::strlen(lit);
    if (s.compare(i, n, lit) == 0) {
        i += n;
        return true;
    }
    return false;
}

/**
 * @brief Parse any JSON value.
 *
 * @param s Source JSON text.
 * @param i Cursor position, advanced past the parsed value.
 * @return Parsed JSON value.
 *
 * @author Group 30
 */
static Json parseValue(const std::string& s, size_t& i) {
    skipWs(s, i);
    if (i >= s.size()) throw parseErr("unexpected end", i);

    const char c = s[i];
    if (c == '"') return Json(parseString(s, i));
    if (c == '{') return parseObject(s, i);
    if (c == '[') return parseArray(s, i);
    if (c == '-' || std::isdigit(static_cast<unsigned char>(c))) return parseNumber(s, i);
    if (matchLiteral(s, i, "true")) return Json(true);
    if (matchLiteral(s, i, "false")) return Json(false);
    if (matchLiteral(s, i, "null")) return Json(nullptr);
    throw parseErr("unexpected token", i);
}

bool Json::isNull() const { return std::holds_alternative<std::nullptr_t>(v); }
bool Json::isBool() const { return std::holds_alternative<bool>(v); }
bool Json::isNumber() const { return std::holds_alternative<double>(v); }
bool Json::isString() const { return std::holds_alternative<std::string>(v); }
bool Json::isArray() const { return std::holds_alternative<array>(v); }
bool Json::isObject() const { return std::holds_alternative<object>(v); }

const Json::array& Json::asArray() const { return std::get<array>(v); }
const Json::object& Json::asObject() const { return std::get<object>(v); }
const std::string& Json::asString() const { return std::get<std::string>(v); }
double Json::asNumber() const { return std::get<double>(v); }
bool Json::asBool() const { return std::get<bool>(v); }

Json::array& Json::asArray() { return std::get<array>(v); }
Json::object& Json::asObject() { return std::get<object>(v); }
std::string& Json::asString() { return std::get<std::string>(v); }

Json Json::parse(const std::string& text) {
    size_t i = 0;
    Json root = parseValue(text, i);
    skipWs(text, i);
    if (i != text.size()) throw parseErr("trailing characters", i);
    return root;
}

/**
 * @brief Escape a plain string for JSON output.
 *
 * @param s Unescaped string.
 * @return JSON-safe escaped text without surrounding quotes.
 *
 * @author Group 30
 */
static std::string escapeString(const std::string& s) {
    std::ostringstream oss;
    for (char c : s) {
        switch (c) {
            case '"': oss << "\\\""; break;
            case '\\': oss << "\\\\"; break;
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    oss << "\\u" << std::hex << std::setw(4) << std::setfill('0')
                        << static_cast<int>(static_cast<unsigned char>(c));
                } else {
                    oss << c;
                }
        }
    }
    return oss.str();
}

static void dumpImpl(const Json& j, std::ostringstream& oss, int indent, int depth);

/**
 * @brief Write indentation spaces for pretty-printed JSON.
 *
 * @param oss Output stream.
 * @param indent Spaces per nesting level.
 * @param depth Current nesting depth.
 *
 * @author Group 30
 */
static void dumpIndent(std::ostringstream& oss, int indent, int depth) {
    if (indent <= 0) return;
    for (int k = 0; k < indent * depth; ++k) {
        oss << ' ';
    }
}

/**
 * @brief Serialize a JSON array.
 *
 * @param a Array value to write.
 * @param oss Output stream.
 * @param indent Spaces per nesting level.
 * @param depth Current nesting depth.
 *
 * @author Group 30
 */
static void dumpArray(const Json::array& a, std::ostringstream& oss, int indent, int depth) {
    oss << "[";
    if (!a.empty()) {
        if (indent > 0) oss << "\n";
        for (size_t idx = 0; idx < a.size(); ++idx) {
            dumpIndent(oss, indent, depth + 1);
            dumpImpl(a[idx], oss, indent, depth + 1);
            if (idx + 1 != a.size()) oss << ",";
            if (indent > 0) oss << "\n";
        }
        dumpIndent(oss, indent, depth);
    }
    oss << "]";
}

/**
 * @brief Serialize a JSON object.
 *
 * @param o Object value to write.
 * @param oss Output stream.
 * @param indent Spaces per nesting level.
 * @param depth Current nesting depth.
 *
 * @author Group 30
 */
static void dumpObject(const Json::object& o, std::ostringstream& oss, int indent, int depth) {
    oss << "{";
    if (!o.empty()) {
        if (indent > 0) oss << "\n";
        size_t idx = 0;
        for (const auto& kv : o) {
            dumpIndent(oss, indent, depth + 1);
            oss << '"' << escapeString(kv.first) << "\":";
            if (indent > 0) oss << ' ';
            dumpImpl(kv.second, oss, indent, depth + 1);
            if (idx + 1 != o.size()) oss << ",";
            if (indent > 0) oss << "\n";
            ++idx;
        }
        dumpIndent(oss, indent, depth);
    }
    oss << "}";
}

/**
 * @brief Serialize any JSON value.
 *
 * @param j JSON value to write.
 * @param oss Output stream.
 * @param indent Spaces per nesting level.
 * @param depth Current nesting depth.
 *
 * @author Group 30
 */
static void dumpImpl(const Json& j, std::ostringstream& oss, int indent, int depth) {
    if (j.isNull()) {
        oss << "null";
        return;
    }
    if (j.isBool()) {
        oss << (j.asBool() ? "true" : "false");
        return;
    }
    if (j.isNumber()) {
        const double d = j.asNumber();
        if (std::fabs(d - std::round(d)) < 1e-9) {
            oss << static_cast<long long>(std::llround(d));
        } else {
            oss << d;
        }
        return;
    }
    if (j.isString()) {
        oss << '"' << escapeString(j.asString()) << '"';
        return;
    }
    if (j.isArray()) {
        dumpArray(j.asArray(), oss, indent, depth);
        return;
    }
    dumpObject(j.asObject(), oss, indent, depth);
}

std::string Json::dump(int indent) const {
    std::ostringstream oss;
    dumpImpl(*this, oss, indent, 0);
    return oss.str();
}

} // namespace studyflow
