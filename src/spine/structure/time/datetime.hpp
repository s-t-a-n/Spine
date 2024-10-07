/*********************************************************************************************
 * Datetime
 *
 * This library was originally created by JeeLabs / Adafruit.
 * It provides core functionality for handling datetime.
 *
 * Downloaded on 6-08-2024 from https://github.com/adafruit/TinyRTCLib/blob/master/TinyRTClib.h
 *
 * Modifications by Stan:
 * - Removed unneeded functionality
 *********************************************************************************************/

#include "spine/structure/units/si.hpp"

#include <climits>
#include <cstdint>

// DateTime class restructured by using standardized time functions
class DateTime {
public:
    DateTime(time_t unix_timestamp = 0);

    DateTime(int year, int month, int mday, int hour = 0, int min = 0, int sec = 0, int wday = 0, int yday = 0,
             int dst = -1);

    DateTime(const char* date, const char* time);

    int getYear() const { return _tm.tm_year + 1900; }
    int getMonth() const { return _tm.tm_mon + 1; }
    int getDay() const { return _tm.tm_mday; }
    int getHour() const { return _tm.tm_hour; }
    int getMinute() const { return _tm.tm_min; }
    int getSecond() const { return _tm.tm_sec; }
    int getWeekDay() const { return _tm.tm_wday; }
    int getYearDay() const { return _tm.tm_yday; }
    int getDST() const { return _tm.tm_isdst; }
    size_t strf_DateTime(char* buffer, size_t buffersize, const char* formatSpec = "%a %h %d %T %Y");

    // time_t value as seconds since 1/1/2000
    time_t getY2kTime() const { return _y2k_timestamp; }

    // time_t value as seconds since 1/1/1970
    // THE ABOVE COMMENT IS CORRECT FOR LOCAL TIME; TO USE THIS COMMAND TO
    // OBTAIN TRUE UNIX TIME SINCE EPOCH, YOU MUST CALL THIS COMMAND AFTER
    // SETTING YOUR CLOCK TO UTC
    time_t getUnixTime() const { return _unix_timestamp; }

private:
    void set_timestamps();

protected:
    time_t _unix_timestamp;
    time_t _y2k_timestamp;
    tm _tm;
};