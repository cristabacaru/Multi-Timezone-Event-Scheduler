# Multi Time-Zone Event Schedule

## Overview
The **Multi Time-Zone Event Schedule** project is designed to manage and schedule events across different time zones. It converts timestamps into human-readable date and time formats, handles daylight saving changes, and identifies common free time slots among participants for scheduling events.

## Features
- **Timestamp Conversion**: Converts UNIX timestamps to local time and date, considering time zone differences.
- **Date Calculation**: Calculates the exact date from timestamps, taking into account leap years and different month lengths.
- **Time Zone Adjustments**: Adjusts for local time zones, ensuring accurate scheduling across different geographical locations.
- **Free Time Slot Identification**: Analyzes participants' availability and finds common free time slots for event scheduling.

## Tasks Overview

### Task 1: Timestamp to Time Conversion
Converts a given UNIX timestamp into hours, minutes, and seconds. The timestamp is divided by the number of seconds in an hour to obtain the hour (0-23), then uses the remainder to find the minute (0-59), and finally, the leftover seconds represent the seconds not accounted for.

### Task 2: Timestamp to Date Conversion
Focuses on extracting the date from the timestamp. The time is set to noon (12 PM) to avoid complications. It calculates the total number of days from the timestamp and determines the year, month, and day accordingly, using a pre-defined array of days in each month.

### Task 3: Accurate Date Calculation with Leap Years
Similar to Task 2 but accounts for leap years. The program iterates through years, adjusting the day count based on whether the year is a leap year. It uses the same logic for determining the month and day.

### Task 4: Time Zone Adjusted Time Representation
Utilizes results from Tasks 1 and 3 to adjust the converted time for the specified time zone. If the time zone difference is zero, it returns the previously calculated date. Otherwise, it adjusts the time accordingly, managing any overflow into the next day, month, or year.

### Task 5: Time Zone Adjusted Timestamp Conversion
Reverses the processes from Tasks 1 and 3 by converting a given time and date back into a UNIX timestamp, considering the time zone difference from UTC.

### Task 6: Formatted Output
Displays the calculated date and time in a formatted manner. Ensures that single-digit days and months are prefixed with a "0" for consistent output.

### Task 7: Bitwise Operations for Date Encoding
Implements a bitwise approach to encode and decode date information. It uses masks and shifts to retrieve day, month, and year information from encoded data.

### Task 8: Data Integrity Verification
Reads and verifies encrypted data against control bits to ensure data integrity. Valid entries are stored separately, and a method to decode the data is applied using the logic from Task 7.

### Task 9: Free Time Slot Finder
Collects and processes availability data from participants to identify common free time slots for events. It sorts timestamps for quick access and checks for available time slots that meet the duration requirement of the event, ensuring the minimum number of required participants is met.

## Getting Started
To run the Multi Time-Zone Event Schedule project, follow these steps:

1. **Clone the Repository**:
   ```bash
   git clone https://github.com/cristabacaru/Multi-Timezone-Event-Schedule.git
