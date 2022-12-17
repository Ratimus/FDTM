// ------------------------------------------------------------------------
// SeqAlgo.cpp
//
// Nov. 2022
// Ryan "Ratimus" Richardson
// ------------------------------------------------------------------------
#include "SeqAlgo.h"
#include "RatFuncs.h"
#include "Latchable.h"
#include <bitHelpers.h>


////////////////////////////////////////////////////////////////////////////////
//                       FREE STUFF FOR EVERYBODY
////////////////////////////////////////////////////////////////////////////////

void buffToRowArray(
    uint32_t *pRows,
    uint8_t numRows,
    uint8_t numCols,
    uint32_t *pBuff)
{
  uint8_t bitmask = 0;
  uint32_t buffCopy = *pBuff;
  for (int8_t ii = 0; ii < numCols; ++ii)
  {
    bitmask = bitRotateLeft(bitmask, 1) | 1;
  }

  for (uint8_t ridx = 0; ridx < numRows; ++ridx)
  {
    int8_t offset = ridx * numCols;
    pRows[ridx] = bitRotateRight(buffCopy, offset) & bitmask;
  }
}

// Divides entire 32 bit pattern into sub-loop "cells" of [loopLen] bits. Locally 
// rotates bits within those cells.
void loopCellsLeft(
  uint32_t *pBuff,
  int8_t   bitsLEFT,
  uint8_t  loopLen/*= 0*/)
{
  uint8_t shiftSize = abs(bitsLEFT);

  if (bitsLEFT == 0)
  {
    return;
  }

  if (loopLen == 0)
  {
    if (bitsLEFT < 0) *pBuff = bitRotateRight(*pBuff, shiftSize);
    else              *pBuff = bitRotateLeft (*pBuff, shiftSize);

    return;
  }

  uint32_t carryMask = (uint32_t)1; // MASK FOR LEFT SHIFT
  
  if (bitsLEFT < 0)                  // MASK FOR RIGHT SHIFT
  {
    carryMask = carryMask << (loopLen - 1); 
  }

  for (int8_t ii = 0; (ii * loopLen) < 32; ++ii) // NB: NUMBER OF BITS IN {VAL}
  {
    carryMask |= (carryMask << (loopLen * ii));
  }

  uint32_t shiftMask = ~carryMask;
  uint32_t shiftVal  = *pBuff;
  uint32_t carryVal  = shiftVal;

  if (bitsLEFT < 0)
  {
    for (int8_t ll = 0; ll < shiftSize; ++ll)
    {
    // RIGHT SHIFT
    shiftVal  = shiftVal >> 1;
    shiftVal &= shiftMask;
    carryVal  = carryVal << (loopLen - 1);
    carryVal &= carryMask;
    shiftVal |= carryVal;
    }
  }
  else
  {
    for (int8_t ll = 0; ll < shiftSize; ++ll)
    {
    // LEFT SHIFT
    shiftVal  = shiftVal << 1;
    shiftVal &= shiftMask;
    carryVal  = carryVal >> (loopLen - 1);
    carryVal &= carryMask;
    shiftVal |= carryVal;
    }
  }
  *pBuff = shiftVal;
}


/////////////////////////////////////////////
// rowArrayToColArray
//     aSRC: pointer to an array where each element is a row in a grid with its origin top right
//   lenSRC: number of valid elements in array, corresponding to number of rows in grid
//     pDST: pointer to array in which each element is to be filled with one column
//   lenDST: number of columns; this had darn well be large enough to hold them all
//  ------> !!! *NB* IT IS UP TO THE CALLER TO ENSURE ARRAYS DO NOT OVERRUN !!! <------

void rowArrayToColArray(uint32_t const aSRC[], uint32_t * pDST, uint8_t lenSRC, uint8_t lenDST)
{
  uint8_t dstCOPY[lenDST];
  uint8_t srcCOPY[lenSRC];

  for (int8_t sidx = 0; sidx < lenSRC; ++sidx)
  {
    srcCOPY[sidx] = aSRC[sidx];
  }

  for (int32_t didx = 0; didx < lenDST; ++didx)
  {
    dstCOPY[didx] = 0;

    for (int32_t sidx = 0; sidx < lenSRC; ++sidx)
    {
      bitWrite(dstCOPY [didx], sidx, bitRead(srcCOPY[sidx], 0));
      srcCOPY [sidx] =  srcCOPY[sidx] >> 1;
    }
    pDST[didx] = dstCOPY[didx];
  }
}
