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

#include <stdlib.h>
#include <string.h>
#include "gps_time_face.h"
#include "watch_utility.h"

static uint32_t unix_to_gps = 315964800;
static uint32_t secs_per_week = 60 * 60 * 24 * 7;

void _update_seconds(uint32_t seconds, bool significant) {
    char buf[7];
    char submode[3];
    uint32_t short_secs;
    
    watch_display_text_with_fallback(WATCH_POSITION_TOP_LEFT, "GPS", "NT");

    // Show 6 least significant digits
    if (!significant) {
        short_secs = seconds % 1000000;
        sprintf(submode, " L");
    } else { // most significant digits
        short_secs = seconds / 1000000;
        sprintf(submode, " S");
    }    
    sprintf(buf, "%6d", short_secs);
    watch_display_text(WATCH_POSITION_TOP_RIGHT, submode);
    watch_display_text(WATCH_POSITION_BOTTOM, buf);
}

void _update_weeknum(uint32_t seconds) {
    char buf[7];

    uint32_t week = seconds / secs_per_week;
    sprintf(buf, "%6d", week);
    watch_display_text_with_fallback(WATCH_POSITION_TOP_LEFT, "WKN", "WN");
    watch_display_text(WATCH_POSITION_TOP_RIGHT, "  ");
    watch_display_text(WATCH_POSITION_BOTTOM, buf);
}

void _update_time_of_week(uint32_t seconds) {
    char buf[7];

    uint32_t time_of_week = seconds % secs_per_week;
    sprintf(buf, "%6d", time_of_week);
    watch_display_text_with_fallback(WATCH_POSITION_TOP_LEFT, "TOW", "TO");
    watch_display_text(WATCH_POSITION_TOP_RIGHT, "  ");
    watch_display_text(WATCH_POSITION_BOTTOM, buf);
}

void _update_leap_sec(gps_time_state_t *state) {
    char buf[7];

    sprintf(buf, "  %2d  ", state->leap_seconds);
    watch_display_text_with_fallback(WATCH_POSITION_TOP, "LPS", "LE");
    watch_display_text_with_fallback(WATCH_POSITION_TOP_RIGHT, "EC", "  ");
    watch_display_text(WATCH_POSITION_BOTTOM, buf);
}

void _change_leap_sec(gps_time_state_t *state) {
    state->leap_seconds += 1;
    if (state->leap_seconds > 60) {
        state->leap_seconds = 0;
    }
}

static void _update(gps_time_state_t *state) {
    watch_date_time_t date_time = movement_get_utc_date_time();
    uint32_t now = watch_utility_date_time_to_unix_time(date_time, 0);
    uint32_t seconds = now - unix_to_gps + (uint32_t) state->leap_seconds;

    switch (state->current_mode) {
        case GPS_TIME_SECONDS:            
            _update_seconds(seconds, state->significant);
            break;
        case GPS_TIME_WEEKNUM:            
            _update_weeknum(seconds);
            break;
        case GPS_TIME_TIMEOFWEEK:
            _update_time_of_week(seconds);
            break;
        case GPS_TIME_LEAPSEC:
            _update_leap_sec(state);
            break;
        default:
            state->current_mode = GPS_TIME_SECONDS;
    }
}

void gps_time_face_setup(uint8_t watch_face_index, void ** context_ptr) {
    (void) watch_face_index;
    if (*context_ptr == NULL) {
        *context_ptr = malloc(sizeof(gps_time_state_t));
        memset(*context_ptr, 0, sizeof(gps_time_state_t));
        // Do any one-time tasks in here; the inside of this conditional happens only at boot.
        gps_time_state_t *state = (gps_time_state_t *)*context_ptr;
        state->leap_seconds = 18;
    }
    // Do any pin or peripheral setup here; this will be called whenever the watch wakes from deep sleep.
}

void gps_time_face_activate(void *context) {
    gps_time_state_t *state = (gps_time_state_t *)context;
    // Handle any tasks related to your watch face coming on screen.
    state->current_mode = GPS_TIME_SECONDS;
    state->significant = false;

    if (watch_sleep_animation_is_running()) {
        watch_stop_sleep_animation();
        watch_stop_blink();
    }
}

bool gps_time_face_loop(movement_event_t event, void *context) {
    gps_time_state_t *state = (gps_time_state_t *)context;

    switch (event.event_type) {
        case EVENT_ACTIVATE:
        case EVENT_TICK:
            _update(state);
            break;
        case EVENT_ALARM_BUTTON_UP:
            state->current_mode += 1;
            _update(state);
            break;
        case EVENT_TIMEOUT:
            movement_move_to_face(0);
            break;
        case EVENT_LIGHT_BUTTON_DOWN:
            if (state->current_mode == GPS_TIME_LEAPSEC) {
                _change_leap_sec(state);
                _update(state);
                break;
            } else if (state->current_mode == GPS_TIME_SECONDS) {
                state->significant = 1;
                _update(state);
                break;
            }
            return movement_default_loop_handler(event);
        case EVENT_LIGHT_BUTTON_UP:
            if (state->current_mode == GPS_TIME_SECONDS) {
                state->significant = 0;
                _update(state);
                break;
            }
        default:
            return movement_default_loop_handler(event);
    }

    return true;
}

void gps_time_face_resign(void *context) {
    (void) context;
}

