//---------------------------------------------------------------------------------------
// Author  : BugTraker
// File    : tools.c - misc tools functions.
// Date    : 20/11/2009 01:09
// Website : http://www.bugtraker.pl 
//
// BeFree - Copyright (C) 2009 BugTraker (http://www.bugtraker.pl).
//
// BeFree is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// BeFree is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with BeFree; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
//
//                            I N C L U D E D   F I L E S
//
//---------------------------------------------------------------------------------------


#include "./types.h"
#include "tools.h"
#include "stream.h"


//---------------------------------------------------------------------------------------
//
//                               D E F I N I T I O N S
//
//---------------------------------------------------------------------------------------


#define NUM_SECS_PER_DAY                86400UL
#define NUM_SECS_PER_HR                 3600U
#define NUM_SECS_PER_MIN                60U

#define UNIX_START_YEAR                 1970U
#define MAX_YEAR                        2140U
#define LEAP_YEAR_DAYS                  366U
#define YEAR_DAYS                       365U
#define MNTHS_IN_YEAR                   12U
#define MNTH_FEB                        2U


//---------------------------------------------------------------------------------------
//
//                          L O C A L   P R O T O T Y P E S
//
//---------------------------------------------------------------------------------------


static uint16_t get_year (uint16_t *days);
static uint8_t get_mnth (uint16_t *days, uint16_t year);
static uint8_t is_leap_year (uint16_t year);


//---------------------------------------------------------------------------------------
//
//                               L O C A L   D A T A
//
//---------------------------------------------------------------------------------------


static const uint8_t mnth_days_map[MNTHS_IN_YEAR] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};


//---------------------------------------------------------------------------------------
//
//                         G L O B A L   F U N C T I O N S
//
//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
// Converts time in seconds to string.
//
// Arguments:
// char_t buff - string buffer for converted time.
// uint32_t time - time in seconds since 1970-01-01 00:00:00.
// uint8_t fmt - format of time to display.
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
void
time2str (char_t *buff, uint32_t time, uint8_t fmt)
{
  uint32_t rem;
  uint16_t year;
  uint16_t mnth;
  uint16_t days;
  uint8_t hr;
  uint8_t min;

  
  switch (fmt)
    {
      case FMT_UNIX_TIME:
        sprintf(buff, "%d", (uint16_t)time);
        break;

      case FMT_ISO_8601:
        days = time / NUM_SECS_PER_DAY;
        rem  = time - (days * NUM_SECS_PER_DAY);
        hr   = rem / NUM_SECS_PER_HR;
        rem  = rem - (hr * NUM_SECS_PER_HR);
        min  = rem / NUM_SECS_PER_MIN;
        year = get_year(&days);
        mnth = get_mnth(&days, year);
        ++days;
        sprintf(buff, "%04d-%02d-%02d %02d:%02d", year, mnth, days, hr, min);
        break;
        
      default:
        break;
    }
}


//---------------------------------------------------------------------------------------
//
//                           L O C A L   F U N C T I O N S
//
//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
// Calculates year from number of days passed and taking 1970 (UNIX start year) as
// a base.
//
// Arguments:
// uint16_t *days - number of days to calculate year.
//
// Return:
// uint16_t year - calculated year.
//---------------------------------------------------------------------------------------
static uint16_t
get_year (uint16_t *days)
{
  uint16_t year;


  for (year = UNIX_START_YEAR; year < MAX_YEAR; year++)
    {
      if (*days >= LEAP_YEAR_DAYS)                                                        // Check for minimal amount of days.
        {
          if (_TRUE_ == is_leap_year(year))
            {
              *days -= LEAP_YEAR_DAYS;
            }
          else
            {
              *days -= YEAR_DAYS;
            }
        }
      else                                                                                // Year found - stop loop.
        {
          break;
        }
    }
  return (year);
}


//---------------------------------------------------------------------------------------
// Calculate month in year from given number of days. If number of days is greater than
// 366 (number of days in leap year) then function returns 0, otherwise month number.
//
// Arguments:
// uint16_t *days - number of days to calculate month.
// uint16_t year  - year to check for leap year.
//
// Return:
// uint8_t mnth - calculated month.
//---------------------------------------------------------------------------------------
static uint8_t
get_mnth (uint16_t *days, uint16_t year)
{
  uint16_t tmp;
  uint8_t mnth = 0;
  uint8_t curr_mnth_days;
  uint8_t i;


  if (*days <= LEAP_YEAR_DAYS)                                                            // Process for 366 days or less.
    {
      for (i = 0, tmp = 0; i < MNTHS_IN_YEAR; i++)
        {
          curr_mnth_days = mnth_days_map[i];
          if (MNTH_FEB == i)                                                              // Special check for february.
            {
              if (_TRUE_ == is_leap_year(year))                                           // Check for lap year.
                {
                  curr_mnth_days = mnth_days_map[i] + 1;                                  // 29 days in february.
                }
            }
          tmp += curr_mnth_days;                                                          // Add current month days.
          if (*days <= tmp)                                                               // Num days still greater than cumulative days.
            {
              *days -= (tmp - curr_mnth_days);                                            // Substract cumulative number of days
              mnth = i + 1;
              break;
            }
          tmp += curr_mnth_days;
        }
    }
  return (mnth);                                                                          // Return month of year.
}


//---------------------------------------------------------------------------------------
// Leap year is a year that can be divided by 4 and can't be divided by 100, or can
// be divided by 400.
// 
//
// Arguments:
// uint16_t year - year to check.
//
// Return:
// uint8_t ret - true/false
//---------------------------------------------------------------------------------------
static uint8_t
is_leap_year (uint16_t year)
{
  uint8_t ret = _FALSE_;


  if (((0 == year % 4) && (0 != year % 100)) || (0 == year % 400))
    {
      ret = _TRUE_;
    }
  return (ret);
}
