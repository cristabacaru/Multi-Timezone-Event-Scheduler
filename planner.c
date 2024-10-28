#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timelib.h"
#define NAME_LEN 11
#define TZ_LEN 5
typedef struct {
    unsigned int given_year;
    unsigned char given_month;
    unsigned char given_day;
    unsigned char given_hour;
    unsigned int duration;
    unsigned int timestamp_start;
    unsigned int timestamp_end;
} FreeIntervalInfo;
typedef struct {
    char person_name[NAME_LEN];
    char particular_timezone[TZ_LEN];
    int K;
    FreeIntervalInfo *free_interval;
} PersonInfo;
typedef struct {
    unsigned int timestamp_start;
    unsigned int timestamp_end;
    int good;
    char good_names[NAME_LEN];
    PersonInfo *person;
} TimestampPersonPair;
void swap_dates(TimestampPersonPair *date_i, TimestampPersonPair *date_j) {
    TimestampPersonPair temp = *date_i;
    *date_i = *date_j;
    *date_j = temp;
}
void bubble_sort_timestamps(TimestampPersonPair *timestamp_array, int tmp_index) {
    int is_sorted = 0;
    while (is_sorted == 0) {
        is_sorted = 1;
        for (int i = 0; i + 1 < tmp_index; i++) {
            if (timestamp_array[i].timestamp_start > timestamp_array[i + 1].timestamp_start) {
                is_sorted = 0;
                swap_dates(&timestamp_array[i], &timestamp_array[i+1]);
            }
        }
    }
}
int compareTimestamps(const void *a, const void *b) {
    return strcmp(((TimestampPersonPair*)a)->person->person_name, ((TimestampPersonPair*)b)->person->person_name);
}
int main() {
    const int SEC_IN_HOUR = 3600, HOURS_IN_DAY = 24;
    int T = 0;
    scanf("%d", &T);
    TTimezone *tzs = malloc(T * sizeof(TTimezone));
    for (int i = 0; i < T; i++) {
        scanf("%s %hhd", tzs[i].name, &tzs[i].utc_hour_difference);
    }
    int P = 0;
    scanf("%d", &P);
    PersonInfo *person = malloc(P * sizeof(PersonInfo));
    for (int i = 0; i < P; i++) {
        scanf("%s %s %d", person[i].person_name, person[i].particular_timezone, &person[i].K);
        person[i].free_interval = malloc(person[i].K * sizeof(FreeIntervalInfo));
        for (int j = 0; j < person[i].K; j++) {
            scanf("%u", &person[i].free_interval[j].given_year);
            scanf("%hhu", &person[i].free_interval[j].given_month);
            scanf("%hhu", &person[i].free_interval[j].given_day);
            scanf("%hhu", &person[i].free_interval[j].given_hour);
            scanf("%u", &person[i].free_interval[j].duration);
            TDateTimeTZ given_date;
            given_date.tz = malloc(sizeof(TTimezone));
            given_date.date.year = person[i].free_interval[j].given_year;
            given_date.date.month = person[i].free_interval[j].given_month;
            given_date.date.day = person[i].free_interval[j].given_day;
            given_date.time.hour = person[i].free_interval[j].given_hour;
            given_date.time.min = 0;
            given_date.time.sec = 0;
            // strcpy(given_date.tz->name, person[i].particular_timezone);
            snprintf(given_date.tz->name, sizeof(given_date.tz->name), "%s", person[i].particular_timezone);
            for (int k = 0; k < T; k++) {
                if (strcmp(given_date.tz->name, tzs[k].name) == 0) {
                    given_date.tz->utc_hour_difference = tzs[k].utc_hour_difference;
                    break;
                }
            }
            person[i].free_interval[j].timestamp_start = convertDateTimeTZToUnixTimestamp(given_date);
            person[i].free_interval[j].timestamp_end = person[i].free_interval[j].timestamp_start;
            person[i].free_interval[j].timestamp_end += person[i].free_interval[j].duration * SEC_IN_HOUR;
            free(given_date.tz);
        }
    }
    int F = 0, duration = 0;
    scanf("%d%d", &F, &duration);
    int max_timestamp_array_size = 0;
    for (int i = 0; i < P; i++) {
        max_timestamp_array_size += (person[i].K + 1);
    }
    TimestampPersonPair *timestamp_array = malloc(max_timestamp_array_size * sizeof(TimestampPersonPair));
    int tmp_index = 0;
    for (int i = 0; i < P; i++) {
        if (person[i].K == 0) {
            timestamp_array[tmp_index].timestamp_start = -1;
            timestamp_array[tmp_index].timestamp_end = -1;
            timestamp_array[tmp_index].person = &person[i];
            tmp_index++;
        } else {
            for (int j = 0; j < person[i].K; j++) {
                if (tmp_index > 0) {
                    if (person[i].free_interval[j].timestamp_start == timestamp_array[tmp_index - 1].timestamp_end) {
                        tmp_index--;
                        timestamp_array[tmp_index].timestamp_end = person[i].free_interval[j].timestamp_end;
                        tmp_index++;
                    } else {
                        timestamp_array[tmp_index].timestamp_start = person[i].free_interval[j].timestamp_start;
                        timestamp_array[tmp_index].timestamp_end = person[i].free_interval[j].timestamp_end;
                        timestamp_array[tmp_index].person = &person[i];
                        tmp_index++;
                    }
                } else {
                    timestamp_array[tmp_index].timestamp_start = person[i].free_interval[j].timestamp_start;
                    timestamp_array[tmp_index].timestamp_end = person[i].free_interval[j].timestamp_end;
                    timestamp_array[tmp_index].person = &person[i];
                    tmp_index++;
                }
            }
        }
    }
    bubble_sort_timestamps(timestamp_array, tmp_index);
    int free_people_cnt = 0;
    int found = 0;
    int event_start = 0;
    for (int i = 0; i < tmp_index; i++) {
        for (int k = 0; k < tmp_index; k++) {
            timestamp_array[k].good = 0;
        }
        if (timestamp_array[i].timestamp_end - timestamp_array[i].timestamp_start >= duration * SEC_IN_HOUR) {
            timestamp_array[i].good = 1;
            free_people_cnt = 1;
            event_start = (int)timestamp_array[i].timestamp_start;
            for (int j = 0; j < tmp_index; j++) {
                int curr_elm_start = (int)timestamp_array[j].timestamp_start;
                int curr_elm_end = (int)timestamp_array[j].timestamp_end;
                if (j != i && curr_elm_end - curr_elm_start >= duration) {
                    if (curr_elm_start <= event_start && curr_elm_end >= event_start + duration * SEC_IN_HOUR) {
                        timestamp_array[j].good = 1;
                        free_people_cnt++;
                    }
                }
            }
            if (free_people_cnt >= F) {
                found = 1;
                break;
            }
        }
    }
    qsort(timestamp_array, tmp_index, sizeof(TimestampPersonPair), compareTimestamps);
    char **printed_people = malloc(max_timestamp_array_size * sizeof(char*));
    for (int i = 0; i < max_timestamp_array_size; i++) {
       printed_people[i] = malloc(NAME_LEN * sizeof(char));
    }
    int printed_people_cnt = 0;
    char **available_people = malloc(max_timestamp_array_size * sizeof(char*));
    for (int i = 0; i < max_timestamp_array_size; i++) {
       available_people[i] = malloc(NAME_LEN * sizeof(char));
    }
    int available_people_cnt = 0;
    for (int i = 0; i < tmp_index; i++) {
        if (timestamp_array[i].good == 1) {
            // strcpy(available_people[available_people_cnt++], timestamp_array[i].person->person_name);
            snprintf(available_people[available_people_cnt++], NAME_LEN, "%s", timestamp_array[i].person->person_name);
        }
    }
    if (found == 0) {
        printf("imposibil\n");
    } else {
            TDateTimeTZ datetimetz;
            for (int i = 0; i < tmp_index; i++) {
                if (timestamp_array[i].good == 1) {
                    char free_interval_good[TZ_LEN];
                    // strcpy(free_interval_good, timestamp_array[i].person->particular_timezone);
                    snprintf(free_interval_good, TZ_LEN, "%s", timestamp_array[i].person->particular_timezone);
                    int tzs_index = 0;
                    for (int  t = 0; t < T; t++) {
                        if (strcmp(tzs[t].name, free_interval_good) == 0) {
                            tzs_index = t;
                            datetimetz.tz = &tzs[t];
                            break;
                        }
                    }
                    printf("%s: ", timestamp_array[i].person->person_name);
                    TDateTimeTZ event_date = convertUnixTimestampToDateTimeTZ(event_start, tzs, tzs_index);
                    datetimetz.date.year = event_date.date.year;
                    datetimetz.date.month = event_date.date.month;
                    datetimetz.date.day = event_date.date.day;
                    datetimetz.time.hour = (unsigned char)((event_start /
                                        SEC_IN_HOUR + tzs[tzs_index].utc_hour_difference) % HOURS_IN_DAY);
                    datetimetz.time.min = 0;
                    datetimetz.time.sec = 0;
                    printDateTimeTZ(datetimetz);
                    for (int p = 0; p < printed_people_cnt; p++) {
                        if (strcmp(printed_people[p], timestamp_array[i].person->person_name)) {
                            // strcpy(printed_people[printed_people_cnt++], timestamp_array[i].person->person_name);
                            snprintf(printed_people[printed_people_cnt++], NAME_LEN,
                            "%s", timestamp_array[i].person->person_name);
                            break;
                        }
                    }
                } else {
                    int is_good_name = 0;
                    for (int j = 0; j < printed_people_cnt; j++) {
                        if (strcmp(printed_people[j], timestamp_array[i].person->person_name) == 0) {
                            is_good_name = 1;
                            break;
                        }
                    }
                    for (int k = 0; k < available_people_cnt; k++) {
                        if (strcmp(available_people[k], timestamp_array[i].person->person_name) == 0) {
                            is_good_name = 1;
                            break;
                        }
                    }
                    // strcpy(printed_people[printed_people_cnt++], timestamp_array[i].person->person_name);
                    snprintf(printed_people[printed_people_cnt++], NAME_LEN,
                    "%s", timestamp_array[i].person->person_name);
                    if (is_good_name == 0)
                        printf("%s: invalid\n", timestamp_array[i].person->person_name);
                }
            }
    }
    for (int i = 0; i < max_timestamp_array_size; i++) {
        free(available_people[i]);
    }
    free(available_people);
    for (int i = 0; i < max_timestamp_array_size; i++) {
        free(printed_people[i]);
    }
    free(printed_people);
    free(timestamp_array);
    free(tzs);
    for (int i = 0; i < P; i++) {
        free(person[i].free_interval);
    }
    free(person);

    return 0;
}
