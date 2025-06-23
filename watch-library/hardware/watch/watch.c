/*
 * MIT License
 *
 * Copyright (c) 2021 Joey Castillo
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

#include "watch.h"

// receives interrupts from MCLK, OSC32KCTRL, OSCCTRL, PAC, PM, SUPC and TAL, whatever that is.
void irq_handler_system(void) {
    if (SUPC->INTFLAG.bit.BOD33DET) {
        // Our system voltage has dipped below 2.6V!
        // Set the voltage regulator to work at low system voltage before we hit 2.5 V
        // This voltage regulator can carry us down to 1.62 volts as the battery drains.
        SUPC->VREG.bit.LPEFF = 0;
        // clear the interrupt condition
        SUPC->INTENCLR.bit.BOD33DET = 1;
        // and disable the brownout detector (TODO: add a second, "power critical" brownout condition?)
        SUPC->INTFLAG.reg &= ~SUPC_INTFLAG_BOD33DET;
    }
}
