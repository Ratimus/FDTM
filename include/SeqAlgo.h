// ------------------------------------------------------------------------
// SeqAlgo.h
//
// Nov. 2022
// Ryan "Ratimus" Richardson
// ------------------------------------------------------------------------
#ifndef SeqAlgo_h
#define SeqAlgo_h

#include <Arduino.h>
////////////////////////////////////////////////////////////////////////////////
//                         SEQ ALGO [PARENT CLASS]
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//                            HANDY HELPERS
///////////////////////////////////////////////////////////////////////////////

void buffToRowArray(uint32_t *pRows, uint8_t numRows, uint8_t numCols, uint32_t *pBuff);
void rowArrayToColArray(uint32_t const aSRC[], uint32_t * pDST, uint8_t lenSRC, uint8_t lenDST);
void loopCellsLeft(uint32_t *pBuff, int8_t bitsFwd, uint8_t loopLen = 0);

////////////////////////////////////////////////////////////////////////////////
//                         SEQ ALGO [PARENT CLASS]
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//                             MT/TM SEQUENCER
////////////////////////////////////////////////////////////////////////////////

// void SeqAlgo :: pulse(byte &retByte)
// {
//   Serial.println("Nyghm Pulse");
//   ui2vars();
  
//   bool clockx(retByte & MASK0);
//   bool clocky(retByte & MASK1);

//   byte meByte(0);
//   for (int i = 0; i < 4; i++)
//   {
//     nextJumpX = (int8_t)clockx * TheSeq.XSTEPS.Q;
//     bool fwdX(TheSeq.XDIR.Q > 0);

//     uint32_t carryMask((uint32_t)1);

//     if (!fwdX) // Rotate right
//     {
//       carryMask = carryMask << ENDPOINT.Q;
//     }

//     for (int8_t ii = 0; (ii * (1 + ENDPOINT.Q)) < 32; ++ii) // NB: NUMBER OF BITS IN {VAL}
//     {
//       carryMask |= (carryMask << ((1 + ENDPOINT.Q) * ii));
//     }

//     uint32_t shiftMask = ~carryMask;
//     uint32_t xSHIFT    = ThePattern.seqTracks[0];
//     uint32_t xCARRY    = xSHIFT;

//     for (int8_t step = 0; step < nextJumpX; ++step)
//     {
//       // Rotate sub-loops
//       if (!fwdX)
//       {
//         // RIGHT SHIFT
//         xSHIFT  = xSHIFT >> 1;
//         xSHIFT &= shiftMask;
//         xCARRY  = xCARRY << ENDPOINT.Q;
//       }
//       else
//       {
//         // LEFT SHIFT
//         xSHIFT  = xSHIFT << 1;
//         xSHIFT &= shiftMask;
//         xCARRY  = xCARRY >> ENDPOINT.Q;
//       }
//     }

//     xCARRY &= carryMask;
//     xSHIFT |= xCARRY;

//     ThePattern.seqTracks[i] = xSHIFT;
//     meByte = meByte << 1;
//     meByte |= (xSHIFT & 0b1);
//   }
//   retByte = meByte;
//   return;
//   // Ok, now do the Y axis
//   nextJumpY = (int8_t)clocky * TheSeq.YSTEPS.Q;
//   bool fwdY(TheSeq.YDIR.Q > 0);

//   uint16_t lclreg(ThePattern.seqTracks[0] & 0xFFFF);

//   for (int8_t steps = 0; steps < nextJumpY; ++ steps)
//   {
//     if (fwdY)
//     {
//       uint16_t T1 = bitRotateLeft(lclreg, 4) & 0xFFF0;
//       uint16_t T2 = bitRotateLeft(lclreg, 5) & 0x000E;
//       uint16_t T3 = bitRotateLeft(lclreg, 1) & 0x0001;
//       lclreg = T1 | T2 | T3;
//     }
//     else
//     {
//       uint16_t T1 = bitRotateRight(lclreg, 4) & 0x0FFF;
//       uint16_t T2 = bitRotateRight(lclreg, 3) & 0xE000;
//       uint16_t T3 = bitRotateRight(lclreg, 8) & 0x1000;
//       lclreg = T1 | T2 | T3;
//     }
//   }
//   ThePattern.loadTrack((uint32_t(lclreg) << 16) | uint32_t(lclreg), 0);
// }


////////////////////////////////////////////////////////////////////////////////
//                       FREE STUFF FOR EVERYBODY
////////////////////////////////////////////////////////////////////////////////


void buffToRowArray(
    uint32_t *pRows,
    uint8_t numRows,
    uint8_t numCols,
    uint32_t *pBuff);

// Divides entire 32 bit pattern into sub-loop "cells" of [loopLen] bits. Locally 
// rotates bits within those cells.
void loopCellsLeft(
  uint32_t *pBuff,
  int8_t   bitsLEFT,
  uint8_t  loopLen/*= 0*/);


/////////////////////////////////////////////
// rowArrayToColArray
//     aSRC: pointer to an array where each element is a row in a grid with its origin top right
//   lenSRC: number of valid elements in array, corresponding to number of rows in grid
//     pDST: pointer to array in which each element is to be filled with one column
//   lenDST: number of columns; this had darn well be large enough to hold them all
//  ------> !!! *NB* IT IS UP TO THE CALLER TO ENSURE ARRAYS DO NOT OVERRUN !!! <------

void rowArrayToColArray(uint32_t const aSRC[], uint32_t * pDST, uint8_t lenSRC, uint8_t lenDST);

#endif

/*
SEQ Controls:
  Clk A           ---> Gate A
  Clk B           ---> Gate B
  Warp Step       ---> Gate C
  Reset           ---> Gate D

  Steps           ---> Rotary Encoder        // Shift+Click for MODE (tm/pt) or to shift banks

  X-fade          ---> Loop Input * Loop VR  // Shift from one seed to another
  Mod CV          ---> CV1  Input + CV1 VR
  Seed 1          ---> CV2  Input + CV2 VR
  Seed 2          ---> CV3  Input + CV3 VR

  Trk2 offset     ---> fader1                // Repetitor: track offset
  Trk2 clk div    ---> fader1 + WR           //            master clock div
                  ---> fader1 + 2WR
  Trk2 clk offs   ---> fader1 + CL           //            clock delay
                  ---> fader1 + 2CL

  Trk3 offset     ---> fader2                // Repetitor: track offset
  Trk3 clk div    ---> fader2 + WR           //            master clock div
                  ---> fader2 + 2WR
  Trk3 clk offs   ---> fader2 + CL           //            clock delay
                  ---> fader2 + 2CL

  Trk4 offset     ---> fader3                // Repetitor: track offset
  Trk4 clk div    ---> fader3 + WR           //            master clock div
                  ---> fader3 + 2WR
  Trk4 clk offs   ---> fader3 + CL           //            clock delay
                  ---> fader3 + 2CL

  Trk5 Fill       ---> fader4                // Euclidean: active steps
  Trk5 Steps      ---> fader4 + WR           //            total steps
                  ---> fader4 + 2WR
  Trk5 Offset     ---> fader4 + CL           //            offset steps
                  ---> fader4 + 2CL

                  ---> fader5
                  ---> fader5 + WR
                  ---> fader5 + 2WR
                  ---> fader5 + CL
                  ---> fader5 + 2CL

                  ---> fader6
                  ---> fader6 + WR
                  ---> fader6 + 2WR
                  ---> fader6 + CL
                  ---> fader6 + 2CL

  Choke1 decay    ---> fader7
                  ---> fader7 + WR
                  ---> fader7 + 2WR
                  ---> fader7 + CL
                  ---> fader7 + 2CL

  Choke2 decay    ---> fader8
                  ---> fader8 + WR
                  ---> fader8 + 2WR
                  ---> fader8 + CL
                  ---> fader8 + 2CL

  Trk1 output     ---> Pulses 1
  Trk2 output     ---> Pulses 2
  Trk3 output     ---> Pulses 3
  Trk4 output     ---> Pulses 4
  Trk5 output     ---> Pulses 5
                  ---> Pulses 6
                  ---> Pulses 7

                  ---> Pulses 1&2
                  ---> Pulses 2&4
                  ---> Pulses 4&7
                  ---> Pulses 1&2&4&7

*/