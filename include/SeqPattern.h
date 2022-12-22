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

const uint8_t TRAX_PER_PATN(4);
const uint16_t MASK_16ths(0b1111111111111111);
const uint16_t  MASK_8ths(0b1010101010101010);
const uint16_t  MASK_4ths(0b1000100010001000);
const uint16_t MASK_sncpt(0b1001001010010101);
const uint8_t  NUM_MASK_STEPS(16);

class BeatMask
{
  uint16_t mod_1;
  uint16_t mod_2;

  uint16_t mask;
  uint8_t  offset;
public:

  enum LogicType
  {
    TYPE_AND,
    TYPE_OR,
    TYPE_XOR,
    TYPE_STOMP,
    TYPE_NOT
  } Logic;

  enum ModeType
  {
    SIXTEENTHS,
    EIGHTHS,
    QUARTERS,
    SYNCOPATED,
    EUCLIDEAN,
    TM
  } Mode;

  BeatMask(LogicType inType = TYPE_AND, ModeType inMode = SIXTEENTHS):
    Logic(inType),
    offset(0)
  {
    setMode(inMode);
  }


  void setOffset(int8_t inOffs)
  {
    while(inOffs < 0) { inOffs += NUM_MASK_STEPS; }
    inOffs %= NUM_MASK_STEPS;
    offset = inOffs;
  }


  void setMask(uint16_t inMask)
  {
    mask = inMask;
  }


  void setMode(ModeType inMode)
  {
    Mode = inMode;
    for (uint8_t trk = 0; trk < TRAX_PER_PATN; ++trk)
    {
      switch(Mode)
      {
        case SIXTEENTHS:
          mask = MASK_16ths;
          break;
        case EIGHTHS:
          mask = MASK_8ths;
          break;
        case QUARTERS:
          mask = MASK_4ths;
          break;
        case SYNCOPATED:
          mask = MASK_sncpt;
          break;
        case EUCLIDEAN:
        case TM:
        default:
          mask = MASK_16ths;
          break;
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
    bool     lclBit(mask & lclMask);

    switch(Logic)
    {
      case TYPE_AND:
        return lclBit && inBit;
      case TYPE_OR:
        return lclBit || inBit;
      case TYPE_XOR:
        return lclBit != inBit;
      case TYPE_STOMP:
        return lclBit;
      case TYPE_NOT:
        return !inBit;
      default:
        return inBit;
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

  int8_t laneShift;
  BeatMask maskies[TRAX_PER_PATN];

public:
  int8_t   playHeads[TRAX_PER_PATN];
  
  bool  pendingA;
  bool  pendingB;

  int8_t   getSeed        (bool A) { return (A ? seedA.Q : seedB.Q); }
  int8_t   getNextPattern (bool A) { return (A ? seedA.D : seedB.D); }
  uint32_t grabTrack      (bool A, int8_t trk);
 
  void     setNextPosn(int8_t track, int8_t steps);
  bool     readBit(bool A, int8_t trk);

  int8_t   getPlayheadIdx(int8_t trk) { return playHeads[trk]; }
  int8_t   getMaxSteps()              { return min(stepsA.Q, stepsB.Q); }
  void     setNextPattern (bool A, int8_t pattern);
  int8_t   latchInPattern (bool A, bool force);

  void resetPlayheads();
  void restartPlayheads();

  SeqPattern (int8_t pattnA, int8_t pattnB);
  void swapTracks(bool fwd = true);
};



#endif