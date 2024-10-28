#include <stdio.h>
#include <stdlib.h>
#include "timelib.h"
const unsigned int IAN = 31, FEB = 28, MAR = 31, APR = 30, MAI = 31, IUN = 30, IUL = 31, AUG = 31, SEP = 30, OCT = 31;
const unsigned int NOV = 30, DEC = 31, MONTHS = 12;
const unsigned int SEC_IN_HOUR = 3600, SEC_IN_MIN = 60, SEC_IN_DAY = 86400, HOURS_IN_DAY = 24, DAYS_IN_YEAR = 365;
const int UNIX_YEAR = 1970, LEAP_YEAR400 = 400, LEAP_YEAR100 = 100, MID_DAY = 12;
#define MONTHS_NAME 12
TTime convertUnixTimestampToTime(unsigned int timestamp) {
    TTime result;
    result.hour = timestamp / SEC_IN_HOUR % HOURS_IN_DAY;
    result.min = timestamp % SEC_IN_HOUR / SEC_IN_MIN;
    result.sec = timestamp % SEC_IN_MIN;
    return result;
}
TDate convertUnixTimestampToDateWithoutLeapYears(unsigned int timestamp) {
    TDate result;
    unsigned int no_days = 0;
    TTime time = convertUnixTimestampToTime(timestamp);
    if (time.hour != MID_DAY) {
        timestamp -= time.hour * SEC_IN_HOUR;
        time.hour = MID_DAY;
        timestamp += time.hour * SEC_IN_HOUR;
    }
    no_days = timestamp / SEC_IN_DAY + 1;
    result.year = UNIX_YEAR + (no_days / DAYS_IN_YEAR);
    no_days %= DAYS_IN_YEAR;
    unsigned int no_of_days_month[] = {IAN, FEB, MAR, APR, MAI, IUN, IUL, AUG, SEP, OCT, NOV, DEC};
    int i = 0;
    while (no_days > no_of_days_month[i]) {
        no_days -= no_of_days_month[i];
        i++;
    }
    result.month = i+1;
    if (no_days == 0) result.day = 1;
    else
        result.day = no_days;
    return result;
}
TDate convertUnixTimestampToDate(unsigned int timestamp) {
    TDate result;
    int no_days = 0;
    TTime time = convertUnixTimestampToTime(timestamp);
    if (time.hour != MID_DAY) {
        timestamp -= time.hour * SEC_IN_HOUR;
        time.hour = MID_DAY;
        timestamp += time.hour * SEC_IN_HOUR;
    }
    no_days = (int)(timestamp / SEC_IN_DAY + 1);
    result.year = UNIX_YEAR;
    while (no_days > DAYS_IN_YEAR + (result.year % 4 == 0
        && (result.year % LEAP_YEAR400 == 0 || result.year % LEAP_YEAR100 != 0))) {
        no_days -= (int)(DAYS_IN_YEAR + (result.year % 4 == 0 &&
                (result.year % LEAP_YEAR400 == 0 || result.year % LEAP_YEAR100 != 0)));
        result.year++;
    }
    unsigned int no_of_days_month[] = {IAN, FEB, MAR, APR, MAI, IUN, IUL, AUG, SEP, OCT, NOV, DEC};
    if (result.year % 4 == 0 && (result.year % LEAP_YEAR400 == 0 || result.year % LEAP_YEAR100 != 0)) {
            no_of_days_month[1] = FEB + 1;
    }
    int i = 0;
    while (no_days > no_of_days_month[i]) {
        no_days -= (int)no_of_days_month[i];
        i++;
    }
    result.month = i+1;
    if (no_days == 0) result.day = 1;
    else
        result.day = no_days;
    return result;
}
TDateTimeTZ convertUnixTimestampToDateTimeTZ(unsigned int timestamp, TTimezone *timezones, int timezone_index) {
    TDateTimeTZ result;
    TTime timestamp_time = convertUnixTimestampToTime(timestamp);
    TDate timestamp_date = convertUnixTimestampToDate(timestamp);
    if (timezones[timezone_index].utc_hour_difference == 0) {
        result.date = timestamp_date;
        result.time = timestamp_time;
        result.tz = &timezones[timezone_index];
        return result;
    }
    unsigned int no_of_days_month[] = {IAN, FEB, MAR, APR, MAI, IUN, IUL, AUG, SEP, OCT, NOV, DEC};
    if (timestamp_date.year % 4 == 0 &&
        (timestamp_date.year % LEAP_YEAR400 == 0 || timestamp_date.year % LEAP_YEAR100 != 0)) {
            no_of_days_month[1] = FEB + 1;
    }
    int new_time = timestamp_time.hour + timezones[timezone_index].utc_hour_difference;
    if (new_time < 0) {
        new_time += (int)HOURS_IN_DAY;
        timestamp_date.day--;
        if (timestamp_date.day == 0) {
            timestamp_date.month--;
            if (timestamp_date.month == 0) {
                timestamp_date.year--;
                timestamp_date.month = MONTHS;
            }
            timestamp_date.day = no_of_days_month[timestamp_date.month - 1];
        }
    } else if (new_time>= HOURS_IN_DAY) {
        new_time -= (int)HOURS_IN_DAY;
        timestamp_date.day++;
        if (timestamp_date.day > no_of_days_month[timestamp_date.month - 1]) {
            timestamp_date.month++;
            if (timestamp_date.month > MONTHS) {
                timestamp_date.year++;
                timestamp_date.month = 1;
            }
            timestamp_date.day = 1;
        }
    }
    timestamp_time.hour = new_time;
    result.date = timestamp_date;
    result.time = timestamp_time;
    result.tz = &timezones[timezone_index];
    return result;
}
unsigned int convertDateTimeTZToUnixTimestamp(TDateTimeTZ datetime) {
    int hour_difference = datetime.tz -> utc_hour_difference;
    unsigned int yr = datetime.date.year;
    unsigned char mth = datetime.date.month;
    unsigned char day = datetime.date.day;
    unsigned char hour = datetime.time.hour;
    unsigned char min = datetime.time.min;
    unsigned char sec = datetime.time.sec;
    int new_time = (int)hour + (-1)*hour_difference;
    unsigned int no_of_days_month[] = {IAN, FEB, MAR, APR, MAI, IUN, IUL, AUG, SEP, OCT, NOV, DEC};
    if (yr % 4 == 0 && (yr % LEAP_YEAR400 == 0 || yr % LEAP_YEAR100 != 0)) {
            no_of_days_month[1] = FEB + 1;
    }
    if (new_time < 0) {
        new_time += (int)HOURS_IN_DAY;
        day--;
        if (day == 0) {
            mth--;
            if (mth == 0) {
                yr--;
                mth = MONTHS;
            }
            day = no_of_days_month[mth - 1];
        }
    } else if (new_time>= HOURS_IN_DAY) {
        new_time -= (int)HOURS_IN_DAY;
        day++;
        if (day > no_of_days_month[mth - 1]) {
            mth++;
            if (mth > MONTHS) {
                yr++;
                mth = 1;
            }
            day = 1;
        }
    }
    hour = new_time;
    unsigned int timestamp = sec + min * SEC_IN_MIN + hour * SEC_IN_HOUR;
    unsigned int year_month_day_in_day = 0;
    for (int i = UNIX_YEAR; i < yr; i++) {
        if (i % 4 == 0 && (i % LEAP_YEAR400 == 0 || i % LEAP_YEAR100 != 0)) {
            year_month_day_in_day += DAYS_IN_YEAR + 1;
        } else {
            year_month_day_in_day += DAYS_IN_YEAR;
        }
    }
    for (int i = 0; i < mth - 1; i++) {
        year_month_day_in_day += no_of_days_month[i];
    }
    year_month_day_in_day += day - 1;  // without the current day
    timestamp += (year_month_day_in_day * SEC_IN_DAY);
    return timestamp;
}
void printDateTimeTZ(TDateTimeTZ datetimetz) {
    const unsigned int DIGITS2_TIME = 10;
    if (datetimetz.date.day < DIGITS2_TIME) {
        printf("0%u ", datetimetz.date.day);
    } else {
        printf("%u ", datetimetz.date.day);
    }
    char months_in_words[MONTHS_NAME][MONTHS_NAME] = {"ianuarie", "februarie", "martie", "aprilie", "mai",
                                "iunie", "iulie", "august", "septembrie", "octombrie", "noiembrie", "decembrie"};
    for (int i = 0; i < MONTHS; i++) {
        if (i == (int)datetimetz.date.month - 1)
            printf("%s ", months_in_words[i]);
    }
    printf("%u, ", datetimetz.date.year);
    if (datetimetz.time.hour < DIGITS2_TIME) {
        printf("0%u:", datetimetz.time.hour);
    } else {
        printf("%u:", datetimetz.time.hour);
    }
    if (datetimetz.time.min < DIGITS2_TIME) {
        printf("0%u:", datetimetz.time.min);
    } else {
        printf("%u:", datetimetz.time.min);
    }
    if (datetimetz.time.sec < DIGITS2_TIME) {
        printf("0%u ", datetimetz.time.sec);
    } else {
        printf("%u ", datetimetz.time.sec);
    }
    printf("%s ", datetimetz.tz->name);
    if (datetimetz.tz->utc_hour_difference >= 0) {
        printf("(UTC+%u)\n", datetimetz.tz->utc_hour_difference);
    } else {
        printf("(UTC%d)\n", datetimetz.tz->utc_hour_difference);
    }
}
