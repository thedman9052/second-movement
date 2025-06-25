/*
 * MIT License
 *
 * Copyright (c) 2025 Donald Gregorich
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include "movement.h"

/*
 * GPS TIME face
 *
 * Displays GPS time in several formats.
 * GPS time is the number of seconds from Jan 6, 1980. It excludes leap seconds,
 * so as of 2025 GPS is ahead of UTC by 18 seconds. GPS time is also expressed
 * in whole weeks since the epoch and the time of that week. The week rolls over
 * on Sunday at 00:00 UTC.
 * 
 * Pressing the ALARM button cycles through different GPS time displays:
 *   GPS - Total GPS seconds
 *   WKN - GPS week number
 *   TOW - Time of week
 *   LPS - Leap seconds
 * 
 * Pressing the LIGHT button on the LPS page will allow you to change the number
 * of leap seconds by pressing the ALARM button, up to 99.
 */

typedef enum {
    GPS_TIME_SECONDS,
    GPS_TIME_WEEKNUM,
    GPS_TIME_TIMEOFWEEK,
    GPS_TIME_LEAPSEC,
    GPS_TIME_NUM_MODES,
} gps_time_mode_t;

typedef struct {
    gps_time_mode_t current_mode;
    uint8_t leap_seconds;
    bool significant;
} gps_time_state_t;

void gps_time_face_setup(uint8_t watch_face_index, void ** context_ptr);
void gps_time_face_activate(void *context);
bool gps_time_face_loop(movement_event_t event, void *context);
void gps_time_face_resign(void *context);

#define gps_time_face ((const watch_face_t){ \
    gps_time_face_setup, \
    gps_time_face_activate, \
    gps_time_face_loop, \
    gps_time_face_resign, \
    NULL, \
})
