#include <stdio.h>
#include <stdlib.h>
#include "timelib.h"
const int BITS_IN_DAY = 5, BITS_IN_MONTH = 4, BITS_IN_YEAR = 6;
const int YEAR_UNIX = 1970;
#define MONTHS_NAME 12
unsigned int create_mask(int a, int b) {
    unsigned mask = 0;
    for (int i = a; i <= b; i++) {
        mask = mask | (1 << i);
    }
    return mask;
}
TDate bits_iterate(unsigned int number) {
    TDate result;
    result.day = number & create_mask(0, BITS_IN_DAY - 1);
    result.month = (number >> BITS_IN_DAY) & create_mask(0, BITS_IN_MONTH - 1);
    result.year = YEAR_UNIX + ((number >> (BITS_IN_DAY + BITS_IN_MONTH)) & create_mask(0, BITS_IN_YEAR - 1));
    return result;
}
void swap_dates(TDate *date_i, TDate *date_j) {
    TDate temp = *date_i;
    *date_i = *date_j;
    *date_j = temp;
}
void bubble_sort_dates(TDate *dates, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = i+1; j < n; j++) {
            if (dates[i].year > dates[j].year) {
                swap_dates(&dates[i], &dates[j]);
            }
            if (dates[i].year == dates[j].year && dates[i].month > dates[j].month) {
                swap_dates(&dates[i], &dates[j]);
            }
            if (dates[i].year == dates[j].year && dates[i].month == dates[j].month && dates[i].day > dates[j].day) {
                swap_dates(&dates[i], &dates[j]);
            }
        }
    }
}
unsigned int control_bit(unsigned int packed_date) {
    int count = 0;
    while (packed_date) {
        count += (int)packed_date & 1;
        packed_date >>= 1;
    }
    return count % 2;
}
unsigned int count_bits(unsigned int n) {
    int count = 0;
    while (n) {
        count += (int)n & 1;
        n >>= 1;
    }
    return count;
}
typedef struct {
    unsigned int non_corrupted_data;
    unsigned int index;
} TUnCorrData;
int main() {
    const int TASK7 = 7, TASK8 = 8, BITS_IN_DATE = 15;
    int task_number = 0;
    // int size = sizeof(unsigned int) * 8;
    scanf("%d", &task_number);
    if (task_number == TASK7) {
        unsigned int N = 0;
        scanf("%u", &N);
        TDate *dates = malloc(N * sizeof(TDate));
        int i = 0;
        for (i = 0; i < N; i++) {
            unsigned int date_in_bits = 0;
            scanf("%u", &date_in_bits);
            TDate result = bits_iterate(date_in_bits);
            dates[i].year = result.year;
            dates[i].month = result.month;
            dates[i].day = result.day;
        }
        bubble_sort_dates(dates, (int)N);
        for (int i = 0; i < N; i++) {
            char months_in_words[MONTHS_NAME][MONTHS_NAME] = {"ianuarie", "februarie", "martie", "aprilie", "mai",
                                "iunie", "iulie", "august", "septembrie", "octombrie", "noiembrie", "decembrie"};
            printf("%u %s %u\n", dates[i].day, months_in_words[dates[i].month - 1], dates[i].year);
        }
        free(dates);
    } else if (task_number == TASK8) {
        const int BITS_IN_BYTE = 8;
        int N = 0;
        scanf("%d", &N);
        int no_of_integers = (int)((N * BITS_IN_DATE + (sizeof(unsigned int) * BITS_IN_BYTE - 1))
                            / (sizeof(unsigned int) * BITS_IN_BYTE));
        unsigned int *packed_data = malloc(no_of_integers * sizeof(unsigned int));
        for (int i = 0; i < no_of_integers; i++) {
            scanf("%u", &packed_data[i]);
        }
        int no_of_bitpacked_int = (int)((no_of_integers + (sizeof(unsigned int) * BITS_IN_BYTE - 1))
                                / (sizeof(unsigned int) * BITS_IN_BYTE));
        unsigned int *packed_cbits = malloc(no_of_bitpacked_int * sizeof(unsigned int));
        for (int i = 0; i < no_of_bitpacked_int; i++) {
            scanf("%u", &packed_cbits[i]);
        }
        TUnCorrData *non_corrupted_data = calloc(no_of_integers + 1, sizeof(TUnCorrData));
        int non_corrupted_count = 0;
        int j = 0;
        for (int i = 0; i < no_of_integers; i++) {
            if (packed_cbits[j] == 0) {
                j++;
            }
            if ((packed_cbits[j] & 1) == control_bit(packed_data[i])) {
               //  printf("cb = %d\n", control_bit(packed_data[i]));
                packed_cbits[j] >>= 1;
                non_corrupted_data[non_corrupted_count].non_corrupted_data = packed_data[i];
                non_corrupted_data[non_corrupted_count].index = i;
                non_corrupted_count++;
            }
        }
        // for (int i = 0; i < non_corrupted_count; i++) {
        //      printf("%u, %u\n", non_corrupted_data[i].non_corrupted_data, non_corrupted_data[i].index);
        // }
        // printf("ncount = %d\n", non_corrupted_count);
        if (non_corrupted_count == 1) {
            if (non_corrupted_data[0].index != 0) {
                if (count_bits(non_corrupted_data[0].non_corrupted_data) < BITS_IN_DATE) non_corrupted_count = 0;
            }
        }
        unsigned int *converted_data = malloc ((N + 1) * sizeof(unsigned int));
        int converted_data_count = 0;
        TDate *dates = malloc((N + 1) * sizeof(TDate));
        int dates_count = 1;
        int rest1 = BITS_IN_DATE;
        int rest2 = BITS_IN_DATE;
        int rest3 = 2;
        int bits_count = 0;
        for (int i = 0; i < non_corrupted_count; i++) {
            dates_count = 1;
            while (non_corrupted_data[i].non_corrupted_data != 0) {
                if (dates_count % 3 == 1) {
                    if (bits_count != BITS_IN_DATE) {
                        unsigned int r = ((non_corrupted_data[i].non_corrupted_data &
                                        create_mask(0, rest1 - 1)) << bits_count) |
                                        converted_data[converted_data_count];
                        converted_data[converted_data_count] = r;
                        bits_count += rest1;
                        if (bits_count != BITS_IN_DATE) {
                            non_corrupted_data[i].non_corrupted_data >>= rest1;
                        } else {
                            TDate result = bits_iterate(converted_data[converted_data_count]);
                            non_corrupted_data[i].non_corrupted_data >>= rest1;
                            dates[converted_data_count].year = result.year;
                            dates[converted_data_count].month = result.month;
                            dates[converted_data_count].day = result.day;
                            bits_count = 0;
                            converted_data_count++;
                            dates_count++;
                        }
                        if (rest1 > 2) {
                            rest1 -= 2;
                        } else if (rest1 == 1) {
                            rest1 = BITS_IN_DATE - 1;
                        } else if (rest1 == 2) {
                            rest1 = BITS_IN_DATE;
                        }
                    }
                    // if (bits_count == BITS_IN_DATE) {
                    //     TDate result = bits_iterate(converted_data[converted_data_count]);
                    //     non_corrupted_data[i].non_corrupted_data >>= BITS_IN_DATE;
                    //     dates[converted_data_count].year = result.year;
                    //     dates[converted_data_count].month = result.month;
                    //     dates[converted_data_count].day = result.day;
                    //     bits_count = 0;
                    //     converted_data_count++;
                    //     dates_count++;
                    // }
                }
                if (dates_count % 3 == 2) {
                    unsigned int r = ((non_corrupted_data[i].non_corrupted_data
                                    & create_mask(0, rest2 - 1)) << bits_count)
                                    | converted_data[converted_data_count];
                    converted_data[converted_data_count] = r;
                    bits_count += rest2;
                    TDate result = bits_iterate(converted_data[converted_data_count]);
                    non_corrupted_data[i].non_corrupted_data >>= BITS_IN_DATE;
                    dates[converted_data_count].year = result.year;
                    dates[converted_data_count].month = result.month;
                    dates[converted_data_count].day = result.day;
                    // printf("i= %d %hhu\n", i, result.day);
                    bits_count = 0;
                    converted_data_count++;
                    dates_count++;
                }
                if (dates_count % 3 == 0) {
                    if (bits_count != BITS_IN_DATE) {
                        unsigned int r = ((non_corrupted_data[i].non_corrupted_data
                                        & create_mask(0, rest3 - 1)) << bits_count)
                                        | converted_data[converted_data_count];
                        converted_data[converted_data_count] = r;
                        bits_count += rest3;
                        if (bits_count != BITS_IN_DATE) {
                            non_corrupted_data[i].non_corrupted_data >>= rest3;
                        } else {
                            TDate result = bits_iterate(converted_data[converted_data_count]);
                            non_corrupted_data[i].non_corrupted_data >>= rest3;
                            dates[converted_data_count].year = result.year;
                            dates[converted_data_count].month = result.month;
                            dates[converted_data_count].day = result.day;
                            bits_count = 0;
                            converted_data_count++;
                        }
                        if (rest3 < BITS_IN_DATE) {
                            rest3 +=2;
                        } else if (rest3 == (BITS_IN_DATE + 1)) {
                            rest3 = 3;
                        } else if (rest3 == BITS_IN_DATE) {
                            rest3 = 2;
                        }
                    }
                    // if (bits_count == BITS_IN_DATE) {
                    //     TDate result = bits_iterate(converted_data[converted_data_count]);
                    //     non_corrupted_data[i].non_corrupted_data >>= BITS_IN_DATE;
                    //     dates[converted_data_count].year = result.year;
                    //     dates[converted_data_count].month = result.month;
                    //     dates[converted_data_count].day = result.day;
                    //     bits_count = 0;
                    //     converted_data_count++;
                    // }
                }
            }
        }
        bubble_sort_dates(dates, converted_data_count);
        for (int i = 0; i < converted_data_count; i++) {
            char months_in_words[MONTHS_NAME][MONTHS_NAME] = {"ianuarie", "februarie", "martie", "aprilie", "mai",
                                "iunie", "iulie", "august", "septembrie", "octombrie", "noiembrie", "decembrie"};
            printf("%u %s %u\n", dates[i].day, months_in_words[dates[i].month - 1], dates[i].year);
        }
        free(packed_data);
        free(packed_cbits);
        free(non_corrupted_data);
        free(converted_data);
    }

    return 0;
}
