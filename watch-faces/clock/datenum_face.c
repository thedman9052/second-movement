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
#include "datenum_face.h"
#include "watch_utility.h"

void _update_doy(watch_date_time_t date_time) {
    char buf[7];
    uint16_t day_of_year;
    uint16_t year = (uint16_t) date_time.unit.year;
    uint8_t month = (uint8_t) date_time.unit.month;
    uint8_t day = (uint8_t) date_time.unit.day;

    day_of_year = watch_utility_days_since_new_year(year,month,day);

    watch_display_text_with_fallback(WATCH_POSITION_TOP_LEFT, "DOY", "DO");
   
    sprintf(buf, " %3d  ", day_of_year);
    watch_display_text(WATCH_POSITION_BOTTOM, buf);
}

void _update_woy(watch_date_time_t date_time) {
    char buf[7];
    uint16_t week_of_year;
    uint16_t year = (uint16_t) date_time.unit.year;
    uint8_t month = (uint8_t) date_time.unit.month;
    uint8_t day = (uint8_t) date_time.unit.day;

    week_of_year = watch_utility_get_weeknumber(year,month,day);

    watch_display_text_with_fallback(WATCH_POSITION_TOP_LEFT, "WOY", "WO");
   
    sprintf(buf, "  %2d  ", week_of_year);
    watch_display_text(WATCH_POSITION_BOTTOM, buf);
}

static void _update(datenum_state_t *state) {
    watch_date_time_t date_time = movement_get_local_date_time();

    switch (state->current_mode) {
        case DATENUM_DOY:            
            _update_doy(date_time);
            break;
        case DATENUM_WOY:            
            _update_woy(date_time);
            break;
        default:
            state->current_mode = DATENUM_DOY;
    }
}

void datenum_face_setup(uint8_t watch_face_index, void ** context_ptr) {
    (void) watch_face_index;
    if (*context_ptr == NULL) {
        *context_ptr = malloc(sizeof(datenum_state_t));
        memset(*context_ptr, 0, sizeof(datenum_state_t));
        // Do any one-time tasks in here; the inside of this conditional happens only at boot.
    }
    // Do any pin or peripheral setup here; this will be called whenever the watch wakes from deep sleep.
}

void datenum_face_activate(void *context) {
    datenum_state_t *state = (datenum_state_t *)context;
    // Handle any tasks related to your watch face coming on screen.
    state->current_mode = DATENUM_DOY;

    if (watch_sleep_animation_is_running()) {
        watch_stop_sleep_animation();
        watch_stop_blink();
    }
}

bool datenum_face_loop(movement_event_t event, void *context) {
    datenum_state_t *state = (datenum_state_t *)context;

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
        default:
            return movement_default_loop_handler(event);
    }

    return true;
}

void datenum_face_resign(void *context) {
    (void) context;
}

