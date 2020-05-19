#pragma once

#include <string>

namespace Calendar {

    static const std::string WeekDaysLong[] = {
        "Sunday",
        "Monday",
        "Tuesday",
        "Wednesday",
        "Thursday",
        "Friday",
        "Saturday"
    };

    static const std::string WeekDaysShort[] = {
    "SUN",
    "MON",
    "TUE",
    "WED",
    "THU",
    "FRI",
    "SAT",
    };

    static const std::string WeekDays2Letter[] = {
        "SU",
        "MO",
        "TU",
        "WE",
        "TH",
        "FR",
        "SA",
    };

    static const int daysInMonth[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};

    static const std::string MonthsLong[] = {
        "",
        "January",
        "February",
        "March",
        "April",
        "May",
        "June",
        "July",
        "August",
        "September",
        "October",
        "November",
        "December"
    };

    static const std::string MonthsShort[] =
    {
    "",
    "JAN",
    "FEB",
    "MAR",
    "APR",
    "MAY",
    "JUN",
    "JUL",
    "AUG",
    "SEP",
    "OCT",
    "NOV",
    "DEC"
    };


    class USACalendar {
        int fYear;
        int fDayCode;
        int fMonthDays[13];

        // Figure out which day of the week the year starts on
        // taking into account the leap year
        static size_t calcDayCode(size_t year)
        {
            int d1 = (year - 1) / 4;
            int d2 = (year - 1) / 100;
            int d3 = (year - 1) / 400;

            return (year + d1 - d2 + d3) % 7;
        }

        static bool isLeapYear(int year)
        {
            if ((year % 4 == 0) && (year % 100 != 0) || (year % 400 == 0))
                return true;

            return false;
        }

    public:
        USACalendar()
        {
            setYear(2020);
        }

        USACalendar(const int year)
            :fYear(year)
        {
            setYear(year);
        }

        // Allow the year to be changed at runtime
        void setYear(int year)
        {
            fYear = year;
            fDayCode = USACalendar::calcDayCode(year);    // which day does it start on

            // Fill in the number of days per month
            for (size_t i = 0; i < 13; i++) {
                fMonthDays[i] = Calendar::daysInMonth[i];
            }

            // Adjust days in february for leap years
            if (USACalendar::isLeapYear(year))
                fMonthDays[2] = 29;
        }

        int getDaysInMonth(int month) { return fMonthDays[month]; }
        int getYearDayCode() { return fDayCode; }

        // Months run 1..12
        int dayCodeForMonth(const int month) 
        {
            int dayCode = fDayCode;
            for (int i = 1; i < month; i++)
            {
                dayCode = (dayCode + fMonthDays[i]) % 7;
            }

            return dayCode;
        }
    };

    // Calculate a Julian Day Number
    // Julian day gives a base representation of a point
    // in time, outside any particular calendar representation.
    // https://en.wikipedia.org/wiki/Julian_day
    static inline uint64_t julianDayNumber(int Y, int M, int D) {
        uint64_t JDN = (1461 * (Y + 4800 + (M - 14) / 12)) / 4 + (367 * (M - 2 - 12 * ((M - 14) / 12))) / 12 - (3 * ((Y + 4900 + (M - 14) / 12) / 100)) / 4 + D - 32075;
        return JDN;
    }
} // namespace Calendar

