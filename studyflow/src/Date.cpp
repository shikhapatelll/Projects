#include "studyflow/Date.h"

#include <cctype>
#include <cstdio>

namespace studyflow {

Date::Date(int y, int m, int d) : year(y), month(m), day(d) {
    if (m < 1 || m > 12) throw std::invalid_argument("Invalid month");
    if (d < 1 || d > daysInMonth(y, m)) throw std::invalid_argument("Invalid day");
}

bool Date::isLeap(int y) {
    return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
}

int Date::daysInMonth(int y, int m) {
    static const int mdays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (m == 2) return mdays[m - 1] + (isLeap(y) ? 1 : 0);
    return mdays[m - 1];
}

Date Date::parse(const std::string& iso) {
    if (iso.size() != 10 || iso[4] != '-' || iso[7] != '-') {
        throw std::invalid_argument("Date must be YYYY-MM-DD");
    }

    for (int i : {0, 1, 2, 3, 5, 6, 8, 9}) {
        if (!std::isdigit(static_cast<unsigned char>(iso[i]))) {
            throw std::invalid_argument("Date must be numeric YYYY-MM-DD");
        }
    }

    const int y = std::stoi(iso.substr(0, 4));
    const int m = std::stoi(iso.substr(5, 2));
    const int d = std::stoi(iso.substr(8, 2));
    return Date(y, m, d);
}

std::string Date::toString() const {
    char buf[11];
    std::snprintf(buf, sizeof(buf), "%04d-%02d-%02d", year, month, day);
    return std::string(buf);
}

int Date::daysFromCivil(int y, unsigned m, unsigned d) {
    y -= m <= 2;
    const int era = (y >= 0 ? y : y - 399) / 400;
    const unsigned yoe = static_cast<unsigned>(y - era * 400);
    const unsigned doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
    const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + static_cast<int>(doe) - 719468;
}

Date Date::civilFromDays(int z) {
    z += 719468;
    const int era = (z >= 0 ? z : z - 146096) / 146097;
    const unsigned doe = static_cast<unsigned>(z - era * 146097);
    const unsigned yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    int y = static_cast<int>(yoe) + era * 400;
    const unsigned doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    const unsigned mp = (5 * doy + 2) / 153;
    const unsigned d = doy - (153 * mp + 2) / 5 + 1;
    const unsigned m = mp + (mp < 10 ? 3 : -9);
    y += (m <= 2);
    return Date(y, static_cast<int>(m), static_cast<int>(d));
}

Date Date::addDays(int delta) const {
    int z = daysFromCivil(year, static_cast<unsigned>(month), static_cast<unsigned>(day));
    z += delta;
    return civilFromDays(z);
}

int Date::daysUntil(const Date& other) const {
    const int a = daysFromCivil(year, static_cast<unsigned>(month), static_cast<unsigned>(day));
    const int b = daysFromCivil(other.year, static_cast<unsigned>(other.month), static_cast<unsigned>(other.day));
    return b - a;
}

bool Date::operator<(const Date& rhs) const {
    if (year != rhs.year) return year < rhs.year;
    if (month != rhs.month) return month < rhs.month;
    return day < rhs.day;
}

bool Date::operator==(const Date& rhs) const {
    return year == rhs.year && month == rhs.month && day == rhs.day;
}

bool Date::isWeekend() const {
    const int z = daysFromCivil(year, static_cast<unsigned>(month), static_cast<unsigned>(day));
    const int dow = (z + 4) % 7;
    return dow == 0 || dow == 6;
}

} // namespace studyflow
