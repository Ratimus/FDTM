// ------------------------------------------------------------------------
// FDTM_IO.H
//
// Nov. 2022
// Ryan "Ratimus" Richardson
// ------------------------------------------------------------------------
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// See http://creativecommons.org/licenses/MIT/ for more information.
//
// -----------------------------------------------------------------------------
//
#ifndef FDTM_IO_H_
#define FDTM_IO_H_

#include <Arduino.h>
#include "MCP_ADC.h"
////////////////////////////////////////////////////////////////
//                    DIGITAL IO PINS
////////////////////////////////////////////////////////////////

// SPI pins
// const uint8_t SDI(12);
// const uint8_t SDO(13);
// const uint8_t S_C_L_K(14);

// I2C pins
const uint8_t S_D_A(21);
const uint8_t S_C_L(22);

// DAC chip select pins
// const uint8_t DAC0_CS(33);
// const uint8_t DAC1_CS(32);

// ADC chip select pins
const uint8_t ADC0_CS(15);
// const uint8_t ADC1_CS();

extern MCP3204 adc0;
extern MCP3008 adc1;

// GATE input pins
const uint8_t GATEin_0(36);
const uint8_t GATEin_1(39);
const uint8_t GATEin_2(34);
const uint8_t GATEin_3(35);

// GATE output pins
const uint8_t GATEout_0(25);
const uint8_t GATEout_1(26);
const uint8_t GATEout_2(4);
const uint8_t GATEout_3(27);

const uint8_t NUM_GATES_OUT(4);
const uint8_t NUM_GATES_IN (4);

const uint8_t GATEin_PINS [4] = {GATEin_0, GATEin_1, GATEin_2, GATEin_3};
const uint8_t GATEout_PINS[4] = {GATEout_0, GATEout_1, GATEout_2, GATEout_3};

// Encoder pins
const uint8_t ENC_A(5);
const uint8_t ENC_B(18);
const uint8_t ENC_SW(19);

// MIDI serial port pins
const uint8_t MIDIRX(16);
const uint8_t MIDITX(17);

const uint32_t BITMASK_32[] =
    {
        1U << 0, 1U << 1, 1U << 2, 1U << 3, 1U << 4, 1U << 5, 1U << 6, 1U << 7,
        1U << 8, 1U << 9, 1U << 10, 1U << 11, 1U << 12, 1U << 13, 1U << 14, 1U << 15,
        1U << 16, 1U << 17, 1U << 18, 1U << 19, 1U << 20, 1U << 21, 1U << 22, 1U << 23,
        1U << 24, 1U << 25, 1U << 26, 1U << 27, 1U << 28, 1U << 29, 1U << 30, 0b10000000000000000000000000000000};

extern const uint8_t NUM_TRAX;
extern const uint16_t LK_MAX;
#endif // MOTIVATION_RADIO_H_
