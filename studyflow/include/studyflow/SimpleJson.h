#pragma once

#include <map>
#include <string>
#include <variant>
#include <vector>

namespace studyflow {

/**
 * @brief Tiny JSON value type plus parser and serializer.
 *
 * This project intentionally avoids external JSON dependencies. The Json type
 * supports the subset needed by StudyFlow: null, bool, number, string, array,
 * and object values.
 *
 * @author Group 30
 */
struct Json {
    using array = std::vector<Json>;                /**< JSON array representation. */
    using object = std::map<std::string, Json>;     /**< JSON object representation. */

    std::variant<std::nullptr_t, bool, double, std::string, array, object> v; /**< Stored value. */

    /** @brief Construct a null JSON value. @author Group 30 */
    Json() : v(nullptr) {}
    /** @brief Construct a null JSON value. @author Group 30 */
    Json(std::nullptr_t) : v(nullptr) {}
    /** @brief Construct a boolean JSON value. @param b Boolean value. @author Group 30 */
    Json(bool b) : v(b) {}
    /** @brief Construct a numeric JSON value. @param d Numeric value. @author Group 30 */
    Json(double d) : v(d) {}
    /** @brief Construct a numeric JSON value from an integer. @param i Integer value. @author Group 30 */
    Json(int i) : v(static_cast<double>(i)) {}
    /** @brief Construct a string JSON value. @param s String value. @author Group 30 */
    Json(const std::string& s) : v(s) {}
    /** @brief Construct a string JSON value. @param s Null-terminated string. @author Group 30 */
    Json(const char* s) : v(std::string(s)) {}
    /** @brief Construct an array JSON value. @param a Array value. @author Group 30 */
    Json(const array& a) : v(a) {}
    /** @brief Construct an object JSON value. @param o Object value. @author Group 30 */
    Json(const object& o) : v(o) {}

    /**
     * @brief Check whether this value is null.
     * @return True when the stored variant currently holds null.
     * @author Group 30
     */
    bool isNull() const;

    /**
     * @brief Check whether this value is a boolean.
     * @return True when the stored variant currently holds a bool.
     * @author Group 30
     */
    bool isBool() const;

    /**
     * @brief Check whether this value is numeric.
     * @return True when the stored variant currently holds a number.
     * @author Group 30
     */
    bool isNumber() const;

    /**
     * @brief Check whether this value is a string.
     * @return True when the stored variant currently holds text.
     * @author Group 30
     */
    bool isString() const;

    /**
     * @brief Check whether this value is an array.
     * @return True when the stored variant currently holds an array.
     * @author Group 30
     */
    bool isArray() const;

    /**
     * @brief Check whether this value is an object.
     * @return True when the stored variant currently holds an object.
     * @author Group 30
     */
    bool isObject() const;

    /**
     * @brief Access the value as an array.
     * @return Const reference to the array.
     * @author Group 30
     */
    const array& asArray() const;

    /**
     * @brief Access the value as an object.
     * @return Const reference to the object.
     * @author Group 30
     */
    const object& asObject() const;

    /**
     * @brief Access the value as a string.
     * @return Const reference to the string.
     * @author Group 30
     */
    const std::string& asString() const;

    /**
     * @brief Access the value as a number.
     * @return Numeric value as a double.
     * @author Group 30
     */
    double asNumber() const;

    /**
     * @brief Access the value as a boolean.
     * @return Boolean value.
     * @author Group 30
     */
    bool asBool() const;

    /**
     * @brief Access the value as a mutable array.
     * @return Mutable reference to the array.
     * @author Group 30
     */
    array& asArray();

    /**
     * @brief Access the value as a mutable object.
     * @return Mutable reference to the object.
     * @author Group 30
     */
    object& asObject();

    /**
     * @brief Access the value as a mutable string.
     * @return Mutable reference to the string.
     * @author Group 30
     */
    std::string& asString();

    /**
     * @brief Parse JSON text into a Json value.
     *
     * @param text JSON source text.
     * @return Parsed Json tree.
     *
     * @author Group 30
     */
    static Json parse(const std::string& text);

    /**
     * @brief Serialize the value as formatted JSON text.
     *
     * @param indent Number of spaces per indentation level.
     * @return JSON string representation.
     *
     * @author Group 30
     */
    std::string dump(int indent = 2) const;
};

} // namespace studyflow
