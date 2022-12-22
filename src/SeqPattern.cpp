// ------------------------------------------------------------------------
// SeqPattern.cpp
//
// Nov. 2022
// Ryan "Ratimus" Richardson
// ------------------------------------------------------------------------
#include "SeqPattern.h"
#include "Rhythms.h"
#include "FDTM_IO.h"
#include "LoopSeq.h"

SeqPattern::SeqPattern (int8_t pattnA, int8_t pattnB) :
  seedA(pattnA),
  stepsA(pattnSteps(seedA.Q)),
  seedB(pattnB),
  stepsB(pattnSteps(seedB.Q)),
  playHeads {0,0,0,0},
  pendingA(1),
  pendingB(1),
  laneShift(0)
{
  maskies[0].setMode(BeatMask::SYNCOPATED);
  maskies[0].setLogic(BeatMask::TYPE_STOMP);
  maskies[0].setOffset(1);
  maskies[3].setMode(BeatMask::SYNCOPATED);
  latchInPattern(1, true);
  latchInPattern(0, true);
}


// Bake in the new pattern(s)
int8_t SeqPattern::latchInPattern(bool A, bool force)
{
  if (A)
  {
    if (!pendingA && !force)
    {
      return getMaxSteps();
    }

    pendingA = false;

    seedA.clock();
    // TODO: more programatically-generated modes (e.g. Euclidean)
    if (seedA.Q < 0 || seedA.Q >= NUM_WORLD_RHYTHMS)
    {
      stepsA.clockIn(32);
    }
    else
    {
      stepsA.clockIn(pattnSteps(seedA.Q));
    }
  }
  else
  {
    if (!pendingB && !force)
    {
      return getMaxSteps();
    }

    pendingB = false;

    seedB.clock();
    if (seedB.Q < 0 || seedB.Q >= NUM_WORLD_RHYTHMS)
    {
      stepsB.clockIn(32);
    }
    else
    {
      stepsB.clockIn(pattnSteps(seedB.Q));
    }
  }

  return getMaxSteps();
}


// Select next pattern for given side. Changes won't take effect until
// an incoming pulse locks them in
void SeqPattern::setNextPattern(bool A, int8_t pattern)
{
  if (A)
  {
    seedA.set(pattern);
    pendingA = true;
  }
  else
  {
    seedB.set(pattern);
    pendingB = true;
  }
}


// Returns selected track from selected pattern. NB: patterns in "rhythms.h"
// are LSB-first, so a bitReverse is performed before returning the pattern
// (such that the first step corresponds to the lowest bit)
uint32_t SeqPattern::grabTrack(bool A, int8_t trk)
{
  uint32_t prime(0);
  if (A) { prime = primes[seedA.Q][trk+1]; }
  else   { prime = primes[seedB.Q][trk+1]; }

  return bitReverse(prime);
}

// Tell the pattern object which track to advance and by how many
// steps, and it does the rest.
void SeqPattern::setNextPosn(int8_t trk, int8_t J)
{
  int8_t S(LOOP.START[trk]->Q);  // Starting point
  int8_t P(playHeads [trk]);     // Position
  int8_t E(LOOP.END  [trk]->Q);  // End point
  int8_t R(E - S + 1);          // Range
  int8_t M(getMaxSteps());      // Max steps

  if (S == E || J == 0) // Bro, why are you even here?
  {
    P %= M;
    playHeads[trk] = P;
    return;
  }

  // Move backward (i.e. always move from S toward E, regardless of where they are)
  // Swap the start and end points, and tell it to move in the opposite direction
  P += J;

  // The following 6 lines took me an entire day to write.
  if (P < S)  P += M;
  P -= S;
  if (R <= 0) R += M;
  P %= R;
  P += S;
  P %= M;
  
  playHeads[trk] = P;
}


// Set all playheads to zero
void SeqPattern::resetPlayheads()
{
  for (int8_t trk = 0; trk < TRAX_PER_PATN; ++trk)
  {
    playHeads[trk] = 0;
  }
}


// Set all playheads to LoopSeq start points
void SeqPattern::restartPlayheads()
{
  for (int8_t trk = 0; trk < TRAX_PER_PATN; ++trk)
  {
    playHeads[trk] = LOOP.START[trk]->Q % getMaxSteps();
  }
}


void SeqPattern::swapTracks(bool fwd)
{
  if (fwd) { ++laneShift; }
  else     { --laneShift; }

  laneShift %= TRAX_PER_PATN;
}

// Tell me which bank to read and which track, I'll tell you 
// what the pattern looks like at the current playhead position
// for that track. The idea is that the caller needs to know as
// little as possible about the actual patterns, delegating
// most of the step-by-step control of the sequencer to the
// object in direct contact with the pattern data
bool SeqPattern::readBit(bool A, int8_t trk)
{
  int8_t idx((trk + laneShift) % TRAX_PER_PATN);
  bool ret(0);
  uint8_t bitN(playHeads[idx]);

  int8_t seedNum(A ? seedA.Q : seedB.Q);
  int8_t M(A ? stepsA.Q : stepsB.Q);

  int8_t rB(bitN % M);
  int8_t iB(31 - rB);
  bool vB = (bool)(primes[seedNum][idx+1] & BITMASK_32[iB]);
  ret = vB;
  
  return maskies[trk].readBit(rB, vB);
}
