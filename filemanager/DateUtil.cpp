#include "DateUtil.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

/**
 * Name: Shikha Patel
 * Description: Implementation of filesystem time formatting.
 * Date: jan 2026
 */

/**
 * Function: FormatFileTime
 * Description: Converts a std::filesystem::file_time_type to a readable local timestamp string.
 *              non-trivial because file_time_type uses a clock that may differ from system_clock.
 * Parameters:
 *   ft: filesystem timestamp (typically from last_write_time)
 * Returns: formatted local time string in "YYYY-MM-DD HH:MM" format.
 */
std::string DateUtil::FormatFileTime(const std::filesystem::file_time_type& ft)
{
    using namespace std::chrono;

    // Convert from file_time_type to system_clock time
    // This formula accounts for the difference between the two clocks
    auto sctp = time_point_cast<system_clock::duration>(
        ft - std::filesystem::file_time_type::clock::now() + system_clock::now()
    );

    // Converting to time_t so we can use standard C time functions
    std::time_t tt = system_clock::to_time_t(sctp);
    
    // Converting to local time struct
    std::tm tm{};

#if defined(_WIN32)
    // Windows uses localtime_s
    localtime_s(&tm, &tt);
#else
    // Unix/Linux/Mac use localtime_r 
    localtime_r(&tt, &tm);
#endif

    // Format as "YYYY-MM-DD HH:MM"
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M");
    return oss.str();
}
