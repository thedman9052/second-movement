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
#include <math.h>
#include "epoch_face.h"
#include "watch_utility.h"

static float unix_to_j2000 = 946727935.816;
static int leap_secs_since_2000 = 6;

void _update_unix(uint32_t seconds, bool significant) {
    char buf[7];
    char submode[3];
    uint32_t short_secs;
    
    watch_display_text_with_fallback(WATCH_POSITION_TOP_LEFT, "UNX", "UX");

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

void _update_j2000(uint32_t seconds, bool significant) {
    char buf[7];
    char submode[3];
    double j2000_days, j2000_fractpart, j2000_intpart;
    uint32_t disp_digits;

    // Calculate J2000 number of days    
    j2000_days = (((double) (seconds + leap_secs_since_2000)) - unix_to_j2000)/ 86400;
    j2000_fractpart = modf(j2000_days, &j2000_intpart);

    watch_display_text_with_fallback(WATCH_POSITION_TOP_LEFT, "MJD", "MJ");

    // Show 6 decimal digits
    if (!significant) {
        disp_digits = (uint32_t) (j2000_fractpart * 1000000);
        sprintf(submode, " F");
    } else { // most significant digits
        disp_digits = (uint32_t) (j2000_intpart);
        sprintf(submode, " I");
    }    
    sprintf(buf, "%6d", disp_digits);
    watch_display_text(WATCH_POSITION_TOP_RIGHT, submode);
    watch_display_text(WATCH_POSITION_BOTTOM, buf);
}

static void _update(epoch_state_t *state) {
    watch_date_time_t date_time = movement_get_utc_date_time();
    uint32_t now_unix = watch_utility_date_time_to_unix_time(date_time, 0);

    switch (state->current_mode) {
        case EPOCH_UNIX:            
            _update_unix(now_unix, state->significant);
            break;
        case EPOCH_J2000:            
            _update_j2000(now_unix, state->significant);
            break;
        default:
            state->current_mode = EPOCH_UNIX;
    }
}

void epoch_face_setup(uint8_t watch_face_index, void ** context_ptr) {
    (void) watch_face_index;
    if (*context_ptr == NULL) {
        *context_ptr = malloc(sizeof(epoch_state_t));
        memset(*context_ptr, 0, sizeof(epoch_state_t));
        // Do any one-time tasks in here; the inside of this conditional happens only at boot.
    }
    // Do any pin or peripheral setup here; this will be called whenever the watch wakes from deep sleep.
}

void epoch_face_activate(void *context) {
    epoch_state_t *state = (epoch_state_t *)context;
    // Handle any tasks related to your watch face coming on screen.
    state->current_mode = EPOCH_UNIX;
    state->significant = false;

    if (watch_sleep_animation_is_running()) {
        watch_stop_sleep_animation();
        watch_stop_blink();
    }
}

bool epoch_face_loop(movement_event_t event, void *context) {
    epoch_state_t *state = (epoch_state_t *)context;

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
            state->significant = 1;
            _update(state);
            break;
        case EVENT_LIGHT_BUTTON_UP:
            state->significant = 0;
            _update(state);
            break;
        default:
            return movement_default_loop_handler(event);
    }

    return true;
}

void epoch_face_resign(void *context) {
    (void) context;
}

