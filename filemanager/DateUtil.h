#ifndef DATEUTIL_H
#define DATEUTIL_H

#include <string>
#include <filesystem>

/**
 * Name: Shikha Patel
 * Description: Helper functions for formatting filesystem timestamps.
 * Date: Jan 2026
 */
class DateUtil
{
public:
    /**
     * Function: FormatFileTime
     * Description: Converts filesystem::file_time_type to a readable string.
     * Parameters:
     *   ft: file_time_type timestamp
     * Returns: formatted string like "2026-01-27 14:05"
     */
    static std::string FormatFileTime(const std::filesystem::file_time_type& ft);
};

#endif
