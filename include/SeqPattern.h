// ------------------------------------------------------------------------
// SeqPattern.h
//
// Nov. 2022
// Ryan "Ratimus" Richardson
// ------------------------------------------------------------------------
#ifndef GENERAL_PATTERN_H
#define GENERAL_PATTERN_H

#include <Arduino.h>
#include <bitHelpers.h>
#include "Latchable.h"
#include "FDTM_IO.h"

const uint8_t  TRAX_PER_PATN(4);
const uint8_t  NUM_MASK_STEPS(16);

const uint16_t MASK_16ths(0b1111111111111111);
const uint16_t MASK_8ths (0b1010101010101010);
const uint16_t MASK_4ths (0b1000100010001000);
const uint16_t MASK_sncpt(0b1001001001001001);

class BeatMask
{
  uint16_t mod_1;
  uint16_t mod_2;

  latchable<uint16_t> mask;
  uint8_t  offset;
public:

  enum LogicType
  {
    TYPE_AND,
    TYPE_OR,
    TYPE_XOR,
    TYPE_STOMP,
    TYPE_AND_NOT,
    TYPE_INVERT
  } Logic;

  enum ModeType
  {
    SIXTEENTHS,
    EIGHTHS,
    QUARTERS,
    SYNCOPATED,
    EUCLIDEAN,
    TM
  } Modus;

  BeatMask(LogicType inType = TYPE_AND, ModeType inMode = SIXTEENTHS):
    Logic(inType),
    offset(0),
    mask(MASK_16ths)
  {
    setMode(inMode);
  }


  void setOffset(int8_t inOffs) // TODO: get control flow working, then constrain to loop length on per-track basis
  {
    while(inOffs < 0) { inOffs += NUM_MASK_STEPS; }
    inOffs %= NUM_MASK_STEPS;
    int8_t delta(inOffs - offset);
    if (delta < 0)
    {
      mask.clockIn(bitRotateRight(mask.Q, -delta));
    }
    else if (delta > 0)
    {
      mask.clockIn(bitRotateLeft(mask.Q, delta));
    }

    offset = inOffs;
  }

  void hardReset()
  {
    offset = 0;
    mask.reset();
  }


  void setMask(uint16_t inMask)
  {
    mask.preEnable(inMask);
    mask.reset();
  }


  void setMode(ModeType inMode)
  {
    Modus = inMode;
    for (uint8_t trk = 0; trk < TRAX_PER_PATN; ++trk)
    {
      switch(Modus)
      {
        case SIXTEENTHS:
        {
          mask.clockIn(MASK_16ths);
          break;
        }
        case EIGHTHS:
        {
          mask.clockIn(MASK_8ths);
          break;
        }
        case QUARTERS:
        {
          mask.clockIn(MASK_4ths);
          break;
        }
        case SYNCOPATED:
        {
          mask.clockIn(MASK_sncpt);
          break;
        }
        case EUCLIDEAN:
        case TM:
        default:
        {
          mask.clockIn(MASK_16ths);
          break;
        }
      }
    }
  }


  void setLogic(LogicType inType)
  {
    Logic = inType;
  }

  bool readBit(uint8_t idx, bool inBit = true)
  {
    uint16_t lclMask(BITMASK_32[NUM_MASK_STEPS - 1 - ((idx + offset) % NUM_MASK_STEPS)]);
    bool     lclBit(mask.clock() & lclMask);

    switch(Logic)
    {
      case TYPE_AND:
      {
        return lclBit && inBit;
      }
      case TYPE_OR:
      {
        return lclBit || inBit;
      }
      case TYPE_XOR:
      {
        return lclBit != inBit;
      }
      case TYPE_STOMP:
      {
        return lclBit;
      }
      case TYPE_AND_NOT:
      {
        return inBit && !lclBit;
      }
      case TYPE_INVERT:
      {
        return !inBit;
      }
      default:
      {
        return inBit;
      }
    }
  }
  

  void euclideanFill(uint8_t trk, uint8_t fill)
  {
    ;
  }

  void euclideanOffset(uint8_t trk, uint8_t offset)
  {
    ;
  }

};

class LoopSeq;

class SeqPattern
{
  latchable<int8_t> seedA;
  latchable<int8_t> stepsA;
  latchable<int8_t> seedB;
  latchable<int8_t> stepsB;

  int8_t   laneShift;
  BeatMask trkMask  [TRAX_PER_PATN];

public:

  int8_t   playHeads[TRAX_PER_PATN];
  int8_t   offsets  [TRAX_PER_PATN];

  bool     pendingA;
  bool     pendingB;

  int8_t   getSeed        (bool A)    { return (A ? seedA.Q : seedB.Q); }
  int8_t   getNextPattern (bool A)    { return (A ? seedA.D : seedB.D); }
  int8_t   getPlayheadIdx(int8_t trk) { return playHeads[trk]; }
  int8_t   getMaxSteps()              { return min(stepsA.Q, stepsB.Q); }

  uint32_t grabTrack      (bool A, int8_t trk);
  bool     readBit        (bool A, int8_t trk);
  uint8_t  readBits       (bool *A);
 
  void     setNextPosn    (int8_t track, int8_t steps);

  void     setNextPattern (bool A, int8_t pattern);
  int8_t   latchInPattern (bool A, bool force);

  void resetPlayheads();
  void restartPlayheads();

  SeqPattern (int8_t pattnA, int8_t pattnB);
  void swapTracks(bool fwd = true);
};



#endif