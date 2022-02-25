#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_LINE_LEN 200
#define MAX_EVENTS 1000
#define NULL 0

//DECLARE EVENT STRUCT HERE
typedef struct {
    char description[MAX_LINE_LEN];
    char timezone[6];
    char location[50];
    int day;
    int month;
    int year;
    char dweek[10];
    char start_time[6];
    char end_time[6];
} Event;

//need to declare functions at the beginning

int read_file(char*, char*, char*);
void find_between_start_end(Event*, char*, char*, int);
void convertDate(char*, int*, int*, int*);
void sort_events(Event*, int);
void swap(Event*, Event*);
void output_results(Event*, int);
int day_digits(int);
char* get_month(int);
void reset_cur_event(Event*);
void output_last_event(Event*, int, int);

/*
    Function: main
    Description: represents the entry point of the program.
    Inputs: 
        - argc: indicates the number of arguments to be passed to the program.
        - argv: an array of strings containing the arguments passed to the program.
    Output: an integer describing the result of the execution of the program:
        - 0: Successful execution.
        - 1: Erroneous execution.
*/
int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("arguments not valid!");
        exit(1);
    } else {

        char* start = argv[1];
        char* end = argv[2];
        char* filename = argv[3];

        start += 8; //move so the char* starts at the date and discludes the --start=
        end += 6; //move so the char* starts at the date and discluded the --end=
        filename += 7; //move so the char* starts at the filename and discludes the --file=

        read_file(start, end, filename);
    }
    

    exit(0);
}


/*
    Function: read_file
    Description: reads the file and creates a master array containing Events for each event in the file
    Inputs:
        char* start_date: start date threshold to find between
        char* end_date: end date threshold to find between
        char* filename: the name of the xml file to read
*/
int read_file(char* start_date, char* end_date, char* filename) {

    //open the file:
    FILE* file = fopen(filename, "r");
    //need to cover if there is problems opening the file
    if (file == NULL) {
        printf("trouble opening file\n");
        exit(1);
    }

    //initialize cur_event and the main array
    Event cur_event;
    Event main[MAX_EVENTS];
    int main_index = 0;

    //read the file line by line, each line add the data of the line to cur_event and at the end add cur_event to main array
    char line[MAX_LINE_LEN + 24];

    char* fgets_return;
    fgets_return = fgets(line, MAX_LINE_LEN + 24, file);

    while (fgets_return != NULL) {

        if (strstr(line, "<calendar>")) {

            //do nothing

        } else if (strstr(line, "</calendar>")) {

            //break out of the loop
            break;

        } else if (strstr(line, "<event>")) {

            reset_cur_event(&cur_event);

        } else if (strstr(line, "</event>")) {

            main[main_index] = cur_event;
            main_index++;

        } else if (strstr(line, "<description>")) {

            char* discard = strtok(line, ">");
            char* use = strtok(NULL, "<");
            strcpy(cur_event.description, use);

        } else if (strstr(line, "<timezone>")) {

            char* discard = strtok(line, ">");
            char* use = strtok(NULL, "<");
            strcpy(cur_event.timezone, use);

        } else if (strstr(line, "<location>")) {
            
            char* discard = strtok(line, ">");
            char* use = strtok(NULL, "<");
            strcpy(cur_event.location, use);

        } else if (strstr(line, "<day>")) {

            char* discard = strtok(line, ">");
            char* use = strtok(NULL, "<");
            cur_event.day = atoi(use);

        } else if (strstr(line, "<month>")) {

            char* discard = strtok(line, ">");
            char* use = strtok(NULL, "<");
            cur_event.month = atoi(use);

        } else if (strstr(line, "<year>")) {

            char* discard = strtok(line, ">");
            char* use = strtok(NULL, "<");
            cur_event.year = atoi(use);

        } else if (strstr(line, "<dweek>")) {

            char* discard = strtok(line, ">");
            char* use = strtok(NULL, "<");
            strcpy(cur_event.dweek, use);

        } else if (strstr(line, "<start>")) {

            char* discard = strtok(line, ">");
            char* use = strtok(NULL, "<");
            strcpy(cur_event.start_time, use);

        } else if (strstr(line, "<end>")) {

            char* discard = strtok(line, ">");
            char* use = strtok(NULL, "<");
            strcpy(cur_event.end_time, use);

        }


        fgets_return = fgets(line, MAX_LINE_LEN + 24, file); //get the next line of the file
    }    

    //close the file:
    fclose(file);

    //we now have a master aray which consists of Event structs which contain the info for each event
    //now, we need to analyze which of them are within given start and end date

    find_between_start_end(main, start_date, end_date, main_index);


    exit(0);
}


/*
    Function: find_between_start_end
    Description: goes through the main array and creates another array with only the Events from main which are between the given start and end dates
    Inputs:
        Event main[]: an array of Events formatted correctly
        char* filename: the name of the file we are using
        char* start: the start of the range we are condensing to
        char* end: the end of the range we are condensing to
        int size: size of the input array
    Output:
        none
*/
void find_between_start_end(Event* main, char* start, char* end, int size) {

    Event events_in_timeframe[5];
    int new_index = 0;

    int start_year;
    int start_month;
    int start_day;
    int end_year;
    int end_month;
    int end_day;

    convertDate(start, &start_year, &start_day, &start_month);
    convertDate(end, &end_year, &end_day, &end_month);

    for (int i = 0; i < size; i++) {
        Event cur_event = main[i];


        if (cur_event.year > start_year && cur_event.year < end_year) {
            //within the range
            events_in_timeframe[new_index] = cur_event;
            new_index++;
        } else if (cur_event.year == start_year && cur_event.year == end_year) {
            //it is in the same year as the start date and end date, need to check against both months
            if (cur_event.month > start_month && cur_event.month < end_month) {
                //within the range
                events_in_timeframe[new_index] = cur_event;
                new_index++;
            } else if (cur_event.month == start_month && cur_event.month == end_month) {
                //it is in the same month as the start and end date, need to check against both days
                if (cur_event.day > start_day && cur_event.day < end_day) {
                    //in the range
                    events_in_timeframe[new_index] = cur_event;
                    new_index++;
                } else if (cur_event.day == start_day || cur_event.day == end_day) {
                    //in the range
                    events_in_timeframe[new_index] = cur_event;
                    new_index++;
                } else {
                    //definitively not in range
                }
            } else if (cur_event.month == start_month) {
                //in the same month as only the start date, only need to check days against the start date
                if (cur_event.day >= start_day) {
                    //in the range
                    events_in_timeframe[new_index] = cur_event;
                    new_index++;
                } else {
                    //definitively not in the range
                }
            } else if (cur_event.month == end_month) {
                //in the same month as only the end date, only need to check days against the end date
                if (cur_event.day <= end_day) {
                    //in the range
                    events_in_timeframe[new_index] = cur_event;
                    new_index++;
                } else {
                    //definitively not in range
                }
            } else if (cur_event.month < start_month || cur_event.month > end_month) {
                //definitively not in the range
            }

        } else if (cur_event.year == start_year) {
            //it is in the same year as only the start date, only need to check months against the start date
            if (cur_event.month > start_month) {
                //in the range
                events_in_timeframe[new_index] = cur_event;
                new_index++;
            } else if (cur_event.month == start_month) {
                //in the same month as start date, need to check days
                if (cur_event.day >= start_day) {
                    //in the range
                    events_in_timeframe[new_index] = cur_event;
                    new_index++;
                } else {
                    //definitively not in the range
                }
            } else {
                //definitively not in range
            }
        } else if (cur_event.year == end_year) {
            //it is in the same year as only the end date, only need to check months against the start date
            if (cur_event.month < end_month) {
                //in the range
                events_in_timeframe[new_index] = cur_event;
                new_index++;
            } else if (cur_event.month == end_month) {
                //in the same month as the end date, need to check days
                if (cur_event.day <= end_day) {
                    //in the range
                    events_in_timeframe[new_index] = cur_event;
                    new_index++;
                } else {
                    //definitively not in range
                }
            } else {
                //definitively not in range
            }
        } else if (cur_event.year < start_year || cur_event.year > end_year) {
            //definitively not in timeframe
        }
    }
    //at this point we have an Event array, events_in_timeframe, which contains all events in the given timeframe
    //now we need to sort the events in chronological order
    sort_events(events_in_timeframe, new_index);
}






/*
    Function: convertDate
    Description: Takes a string representation of a date and returns a pointer to an int array containing the year (index 0), the month (index 1), and the day (index 2)
    Inputs:
        char* date: a date in the format YYYY/MM/DD, where MM and DD can be 1 digit if applicable
    Output:
        pointer to an int array in the format : [int year, int month, int day]
*/
void convertDate(char* date, int* year, int* day, int* month) {
    int date_decomp[4];

    char* year_str = strtok(date, "/");
    *year = atoi(year_str);
    char* month_str = strtok(NULL, "/");
    *month = atoi(month_str);
    char* day_str = strtok(NULL, "/");
    *day = atoi(day_str);
}



/*
    Function: sort_events
    Description: sorts the events in the events_in_timeframe array in chronological order
    Inputs:
        Event events_in_timeframe[]: array of Events to sort
        int size: size of the array
    Output:
        none
*/
void sort_events(Event* events_in_timeframe, int size) {
    int i, j, min_index;
    for (i = 0; i < size - 1; i++) {
        min_index = i;
        for (j = i + 1; j < size; j++) {
            if (events_in_timeframe[j].year < events_in_timeframe[i].year) {
                min_index = j;
            } else if (events_in_timeframe[j].year == events_in_timeframe[i].year) {
                if (events_in_timeframe[j].month < events_in_timeframe[i].month) {
                    min_index = j;
                } else if (events_in_timeframe[j].month == events_in_timeframe[i].month) {
                    if (events_in_timeframe[j].day == events_in_timeframe[i].day) {
                        if (strcmp(events_in_timeframe[j].start_time, events_in_timeframe[i].start_time) < 0) {
                            min_index = j;
                        }
                    }
                }
            }
        }
        swap(&events_in_timeframe[min_index], &events_in_timeframe[i]);
    }

    //now the array is sorted in chronological order
    //we just need to output
    output_results(events_in_timeframe, size);

}

/* Helper function for sort_events */
void swap (Event* event1p, Event* event2p) {
    Event temp = *event1p;
    *event1p = *event2p;
    *event2p = temp;
}


/*
    Function: output_results
    Description: outputs the contents of to_output to the console according to specifications
    Assumption: to_output is passed in as a sorted array
    Inputs:
        Event to_output[]: an Event array to output
        int size: the size of to_output
    Output:
        outputs the contents of to_output according to specifications below:


    Output format:
    
    first day in format Month(written out) day, year (dweek)
    --- (***dashes must exactly underline up until the end of the year on the line above***)
    event_start_time(12h format) to event_end_time(12h format): event_description {{event_location}} | event_timezone
*/
void output_results (Event* to_output, int size) {
    int i = 0;
    while (i < size) {
        // for each event in the array
        //output the current event's day line and its own line
        char* month_str = get_month(to_output[i].month);
        printf("%s %d, %d (%s)\n", month_str, to_output[i].day, to_output[i].year, to_output[i].dweek);
        int dashes_length = strlen(month_str) + day_digits(to_output[i].day) + 4 + strlen(to_output[i].dweek) + 6;
        for (int dash = 0; dash < dashes_length; dash++) {
            printf("-");
        }
        printf("\n");

/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
        // convert start time to 12h format
        char* to_convert_start = to_output[i].start_time;

        char* start_hour = strtok(to_convert_start, ":");
        char* start_minute = strtok(NULL, ":");

        int start_hour_int = atoi(start_hour);
        if (start_hour_int < 12) {
            //am time
            if (start_hour_int == 0) {
                start_hour_int = 12;
            }
            printf("%02d:%s AM to ", start_hour_int, start_minute);
        } else {
            if (start_hour_int != 12) {
                start_hour_int -= 12;
            }
            printf("%02d:%s PM to ", start_hour_int, start_minute);
        }


        // convert end time to 12h format
        char* to_convert_end = to_output[i].end_time;

        char* end_hour = strtok(to_convert_end, ":");
        char* end_minute = strtok(NULL, ":");

        int end_hour_int = atoi(end_hour);
        if (end_hour_int < 12) {
            //am time
            if (end_hour_int == 0) {
                end_hour_int = 12;
            }
            printf("%02d:%s AM", end_hour_int, end_minute);
        } else {
            if (end_hour_int != 12) {
                end_hour_int -= 12;
            }
            printf("%02d:%s PM", end_hour_int, end_minute);
        }
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
        
        printf(": %s {{%s}} | %s\n", to_output[i].description, to_output[i].location, to_output[i].timezone);
        for (int j = i+1; j < size; j++) {
            if (j == size - 1) {
                if (to_output[j].year == to_output[i].year &&
                    to_output[j].month == to_output[i].month &&
                    to_output[j].day == to_output[i].day) {
                    output_last_event(to_output, size, 1);
                    i = size - 1;
                    break;
                    }
                    output_last_event(to_output, size, 0);
                    i = size - 1;
                    break;
            }
            //checking for consecutive events on the same day
            if (to_output[j].year == to_output[i].year &&
                to_output[j].month == to_output[i].month &&
                to_output[j].day == to_output[i].day) {
                    //event on same day
                    
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
                     // convert start time to 12h format
                    char* to_convert_start = to_output[j].start_time;

                    char* start_hour = strtok(to_convert_start, ":");
                    char* start_minute = strtok(NULL, ":");

                    int start_hour_int = atoi(start_hour);
                    if (start_hour_int < 12) {
                        //am time
                        if (start_hour_int == 0) {
                            start_hour_int = 12;
                        }
                        printf("%02d:%s AM to ", start_hour_int, start_minute);
                    } else {
                        if (start_hour_int != 12) {
                            start_hour_int -= 12;
                        }
                        printf("%02d:%s PM to ", start_hour_int, start_minute);
                    }


                    // convert end time to 12h format
                    char* to_convert_end = to_output[j].end_time;

                    char* end_hour = strtok(to_convert_end, ":");
                    char* end_minute = strtok(NULL, ":");

                    int end_hour_int = atoi(end_hour);
                    if (end_hour_int < 12) {
                        //am time
                        if (end_hour_int == 0) {
                            end_hour_int = 12;
                        }
                        printf("%02d:%s AM", end_hour_int, end_minute);
                    } else {
                        if (end_hour_int != 12) {
                            end_hour_int -= 12;
                        }
                        printf("%02d:%s PM", end_hour_int, end_minute);
                    }
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/


                    printf(": %s {{%s}} | %s\n", to_output[j].description, to_output[j].location, to_output[j].timezone);
                    i++;
                    if (j + 1 == size - 1) {
                        break;
                    } else {
                        continue;
                    }
                }
            break;
        }
        printf("\n");
        i++;
    }

}

/*
    Function: output_last_event
    Description: outputs according to same rules as output_results, but removes the trailing newline for the end of the output
    Inputs:
        Event* to_output: array containing the events in the given timeframe
        int size: the size of to_output
        int new_day: an int to determine whether a day tag should be output or not
            new_day == 0: this event occurs on the same day as the previous event... dont print new day tag
            new_day == 1: this event occurs on a different day than the previous event... print new day tag
    Output:
        none
*/
void output_last_event(Event* to_output, int size, int new_day) {
    if (new_day == 1) {
        char* month_str = get_month(to_output[size - 1].month);
        printf("%s %d, %d (%s)\n", month_str, to_output[size - 1].day, to_output[size - 1].year, to_output[size - 1].dweek);
        int dashes_length = strlen(month_str) + day_digits(to_output[size - 1].day) + 4 + strlen(to_output[size - 1].dweek) + 6;
        for (int i = 0; i < dashes_length; i++) {
            printf("-");
        }
        printf("\n");
    }

/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
    // convert start time to 12h format
    char* to_convert_start = to_output[size - 1].start_time;

    char* start_hour = strtok(to_convert_start, ":");
    char* start_minute = strtok(NULL, ":");

    int start_hour_int = atoi(start_hour);
    if (start_hour_int < 12) {
        //am time
        if (start_hour_int == 0) {
            start_hour_int = 12;
        }
        printf("%02d:%s AM to ", start_hour_int, start_minute);
    } else {
        if (start_hour_int != 12) {
            start_hour_int -= 12;
        }
        printf("%02d:%s PM to ", start_hour_int, start_minute);
    }


    // convert end time to 12h format
    char* to_convert_end = to_output[size - 1].end_time;

    char* end_hour = strtok(to_convert_end, ":");
    char* end_minute = strtok(NULL, ":");

    int end_hour_int = atoi(end_hour);
    if (end_hour_int < 12) {
        //am time
        if (end_hour_int == 0) {
            end_hour_int = 12;
        }
        printf("%02d:%s AM", end_hour_int, end_minute);
    } else {
        if (end_hour_int != 12) {
            end_hour_int -= 12;
        }
        printf("%02d:%s PM", end_hour_int, end_minute);
    }
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
        
    printf(": %s {{%s}} | %s", to_output[size - 1].description, to_output[size - 1].location, to_output[size - 1].timezone);
}



/*
    Function: day_digits
    Description: given a day of the month, determine whether it is one digit or two
    Inputs:
        int day: the day of the month
    Output:
        int representing how many digits are in day
*/
int day_digits(int day) {
    if (day < 10) {
        return 1;
    } else {
        return 2;
    }
}



/*
    Function: get_month
    Description: given the number (1-12) representing a month, converts it to the month name
    Inputs:
        int month_num: number representation of a month
    Output:
        string of the word representation of inout month
*/
char* get_month(int month_num) {
    if (month_num == 1) {
        char* month_name = "January";
        return month_name;
    } else if (month_num == 2) {
        char* month_name = "February";
        return month_name;
    } else if (month_num == 3) {
        char* month_name = "March";
        return month_name;
    } else if (month_num == 4) {
        char* month_name = "April";
        return month_name;
    } else if (month_num == 5) {
        char* month_name = "May";
        return month_name;
    } else if(month_num == 6) {
        char* month_name = "June";
        return month_name;
    } else if (month_num == 7) {
        char* month_name = "July";
        return month_name;
    } else if (month_num == 8) {
        char* month_name = "August";
        return month_name;
    } else if (month_num == 9) {
        char* month_name ="September";
        return month_name;
    } else if (month_num == 10) {
        char* month_name = "October";
        return month_name;
    } else if (month_num == 11) {
        char* month_name = "November";
        return month_name;
    } else {
        char* month_name = "December";
        return month_name;
    }
}



/*
    Function: reset_cur_event
    Description: resets the Event pointed to by cur_event_pointer
    Inputs:
        cur_event_pointer: a pointer to the Event to reset
    Output:
        none
*/
void reset_cur_event(Event* cur_event_pointer) {
    strcpy((*cur_event_pointer).description, "");
    strcpy((*cur_event_pointer).timezone, "");
    strcpy((*cur_event_pointer).location, "");
    (*cur_event_pointer).day = 0;
    (*cur_event_pointer).month = 0;
    (*cur_event_pointer).year = 0;
    strcpy((*cur_event_pointer).dweek, "");
    strcpy((*cur_event_pointer).start_time, "");
    strcpy((*cur_event_pointer).end_time, "");
}