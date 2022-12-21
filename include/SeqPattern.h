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

const uint8_t TRAX_PER_PATN(4);

class LoopSeq;

class SeqPattern
{
  latchable<int8_t> seedA;
  latchable<int8_t> stepsA;
  latchable<int8_t> seedB;
  latchable<int8_t> stepsB;

  const int8_t & mode;
  int8_t laneShift;
  
public:
  int8_t   playHeads[TRAX_PER_PATN];
  
  bool  pendingA;
  bool  pendingB;

  int8_t   getSeed        (bool A) { return (A ? seedA.Q : seedB.Q); }
  int8_t   getNextPattern (bool A) { return (A ? seedA.D : seedB.D); }
  uint32_t grabTrack      (bool A, int8_t trk);
 
  void     setNextPosn(int8_t track, int8_t steps);
  void     moveToBit  (int8_t track, int8_t idx)
  {
    playHeads[track] = idx % getMaxSteps();
  }
  bool     readBit(bool A, int8_t trk);

  int8_t   getPlayheadIdx(int8_t trk) { return playHeads[trk]; }
  int8_t   getMaxSteps()              { return min(stepsA.Q, stepsB.Q); }
  void     setNextPattern (bool A, int8_t pattern);
  int8_t   latchInPattern (bool A, bool force);

  void resetPlayheads();
  void restartPlayheads();

  SeqPattern (int8_t pattnA, int8_t pattnB, int8_t inMode);
  void swapTracks(bool fwd = true);
};

#endif