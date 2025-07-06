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
 * EPOCH face
 * 
 * A collection of faces which display the time from alternate epochs.
 * 
 * Pressing the ALARM button cycles through the epochs:
 *    UNX: Unix seconds from midnight Jan 1, 1970
 *    J2K: J2000 seconds from 12 noon Jan 1, 2000
 * 
 * The pages show the six least significant digits of the seconds, indicated
 * by the letter L in the upper right.
 * Holding the LIGHT button will temporarily show the remaining most significant
 * digits, indicated by S. 
 */

typedef enum {
    EPOCH_UNIX,
    EPOCH_J2000,
} epoch_mode_t;

typedef struct {
    epoch_mode_t current_mode;
    bool significant;
} epoch_state_t;

void epoch_face_setup(uint8_t watch_face_index, void ** context_ptr);
void epoch_face_activate(void *context);
bool epoch_face_loop(movement_event_t event, void *context);
void epoch_face_resign(void *context);

#define epoch_face ((const watch_face_t){ \
    epoch_face_setup, \
    epoch_face_activate, \
    epoch_face_loop, \
    epoch_face_resign, \
    NULL, \
})
