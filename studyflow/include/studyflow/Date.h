#pragma once

#include <stdexcept>
#include <string>

namespace studyflow {

/**
 * @brief Immutable-style calendar date wrapper used across StudyFlow.
 *
 * This class stores valid Gregorian calendar dates in YYYY-MM-DD form and
 * provides comparison, parsing, formatting, day arithmetic, and weekend
 * detection. It is intentionally lightweight so it can be serialized to JSON
 * without relying on external libraries.
 *
 * @author Group 30
 */
class Date {
public:
    int year{1970};   /**< Four-digit calendar year. */
    int month{1};     /**< Calendar month in the range 1..12. */
    int day{1};       /**< Day of the month in the valid range for the month. */

    /**
     * @brief Construct the default sentinel date 1970-01-01.
     *
     * The default date is also used in a few places as a "not provided"
     * sentinel for optional command fields.
     *
     * @author Group 30
     */
    Date() = default;

    /**
     * @brief Construct a validated date.
     *
     * @param y Four-digit year.
     * @param m Month in the range 1..12.
     * @param d Day in the valid range for the given month and year.
     * @throws std::invalid_argument Thrown when the provided values are not a valid date.
     *
     * @author Group 30
     */
    Date(int y, int m, int d);

    /**
     * @brief Parse an ISO-like date string in YYYY-MM-DD format.
     *
     * @param iso Input text.
     * @return Parsed Date object.
     * @throws std::invalid_argument Thrown when the text is not a valid date.
     *
     * @author Group 30
     */
    static Date parse(const std::string& iso);

    /**
     * @brief Format the date as YYYY-MM-DD.
     *
     * @return ISO-style text representation of the date.
     *
     * @author Group 30
     */
    std::string toString() const;

    /**
     * @brief Return a date offset by a number of days.
     *
     * @param delta Number of days to add. Negative values move backward.
     * @return New date after applying the offset.
     *
     * @author Group 30
     */
    Date addDays(int delta) const;

    /**
     * @brief Calculate the number of days from this date to another date.
     *
     * The calculation returns `other - this`, so future dates produce positive
     * values and past dates produce negative values.
     *
     * @param other Target date.
     * @return Signed day difference.
     *
     * @author Group 30
     */
    int daysUntil(const Date& other) const;

    /**
     * @brief Compare two dates chronologically.
     *
     * @param rhs Right-hand side date.
     * @return True if this date is earlier than `rhs`.
     *
     * @author Group 30
     */
    bool operator<(const Date& rhs) const;

    /**
     * @brief Compare two dates for equality.
     *
     * @param rhs Right-hand side date.
     * @return True if both dates represent the same calendar day.
     *
     * @author Group 30
     */
    bool operator==(const Date& rhs) const;

    /**
     * @brief Determine whether the date falls on Saturday or Sunday.
     *
     * @return True if the date is a weekend day.
     *
     * @author Group 30
     */
    bool isWeekend() const;

private:
    /**
     * @brief Check whether a year is a leap year.
     *
     * @param y Year to test.
     * @return True when the year contains February 29.
     *
     * @author Group 30
     */
    static bool isLeap(int y);

    /**
     * @brief Return the number of days in a month.
     *
     * @param y Year used for leap-year handling.
     * @param m Month in the range 1..12.
     * @return Number of days in the month.
     *
     * @author Group 30
     */
    static int daysInMonth(int y, int m);

    /**
     * @brief Convert a civil date to a serial day count.
     *
     * @param y Year.
     * @param m Month.
     * @param d Day of month.
     * @return Serial day number suitable for date arithmetic.
     *
     * @author Group 30
     */
    static int daysFromCivil(int y, unsigned m, unsigned d);

    /**
     * @brief Convert a serial day count back into a civil date.
     *
     * @param z Serial day number.
     * @return Reconstructed calendar date.
     *
     * @author Group 30
     */
    static Date civilFromDays(int z);
};

} // namespace studyflow
