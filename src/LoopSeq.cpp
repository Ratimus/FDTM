// ------------------------------------------------------------------------
// LoopSeq.cpp
//
// Nov. 2022
// Ryan "Ratimus" Richardson
// ------------------------------------------------------------------------
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <bitHelpers.h>
#include <RatFuncs.h>
#include "FDTM_IO.h"
#include "SharedCtrl.h"
#include "ClockObject.h"
#include "ClockDivider.h"
#include "LoopSeq.h"
#include "SeqAlgo.h"
#include "Rhythms.h"

extern Adafruit_SSD1306 display;

const int8_t ELASTIC_MODE(0);
const int8_t WINDOW_MODE(1);

long timeGateHigh   [8] = {0, 0, 0, 0, 0, 0, 0, 0};
static bool allGates[8] = {0, 0, 0, 0, 0, 0, 0, 0};

const int8_t defaultA(27);
const int8_t defaultB(28);

TrkPts::TrkPts(int8_t maxLength):
  S1(0),
  S2(0),
  S3(0),
  S4(0),
  E1(maxLength - 1),
  E2(maxLength - 1),
  E3(maxLength - 1),
  E4(maxLength - 1),
  O1(0),
  O2(0),
  O3(0),
  O4(0),
  P1(maxLength),
  P2(maxLength),
  P3(maxLength),
  P4(maxLength),
  J1(1),
  J2(1),
  J3(1),
  J4(1),
  START {&S1, &S2, &S3, &S4},
  END   {&E1, &E2, &E3, &E4},
  STEPS {&P1, &P2, &P3, &P4},
  JUMP  {&J1, &J2, &J3, &J4}
  { ; }


//////////////////////////////////////////////////////////////
// NOTES/TODO:
//
//////////////////////////////////////////////////////////////
LoopSeq::LoopSeq() :
  MODE(ELASTIC_MODE),
  PW(10),
  ThePattn(defaultA, defaultB),
  pattnA(ThePattn.getNextPattern(1)),
  pattnB(ThePattn.getNextPattern(0)),
  running(false),
  pendingResetFLAG(true),
  probB(0),
  PROB_KNOB(0),
  maxSteps(ThePattn.getMaxSteps())
{
  validatePattns();
  ThePattn.latchInPattern(1, true);
  ThePattn.latchInPattern(0, true);
  updateMaxSteps();
}


void LoopSeq::reset()
{
  if (!pendingResetFLAG)            // Forced reset
  {
    ThePattn.resetPlayheads();      // Playheads to ZERO
    updateMaxSteps();

    for (int8_t trk = 0; trk < TRAX_PER_PATN; ++trk) // Initialize start and end points
    {
      LOOP.START[trk]->clockIn(0);
      LOOP.END  [trk]->clockIn(maxSteps-1);
      LOOP.STEPS[trk]->clockIn(maxSteps);
    }
  }
  else
  {
    ThePattn.restartPlayheads();    // Playheads to START POINTS
    pendingResetFLAG = false;
  }

  patternOled(1);
}


// Fetch pattern length from Pattn 
void LoopSeq::updateMaxSteps()
{
  int16_t oldMax(maxSteps);
  maxSteps = ThePattn.getMaxSteps();

  if (oldMax == maxSteps) { return; }

  for (uint8_t trk = 0; trk < TRAX_PER_PATN; ++trk)
  {
    if (LOOP.STEPS[trk]->Q == oldMax || LOOP.STEPS[trk]->Q > maxSteps)
    {
      LOOP.STEPS[trk]->clockIn(maxSteps);
      int16_t newEnd((LOOP.START[trk]->Q + LOOP.STEPS[trk]->Q - 1) % maxSteps);
      LOOP.END[trk]->clockIn(newEnd);
    }
  }
}


// Make sure the selected patterns follow the changes from our menu
void LoopSeq::validatePattns(bool force)
{
  if (ThePattn.getNextPattern(1) != pattnA)
  {
    ThePattn.setNextPattern(1, pattnA);
  }

  if (ThePattn.getNextPattern(0) != pattnB)
  {
    ThePattn.setNextPattern(0, pattnB);
  }

  if (!force) { return; }

  // Default behavior is to wait for clock pulse before changing patterns,
  // but you can override this
  ThePattn.latchInPattern(1, false);
  ThePattn.latchInPattern(0, false);
  updateMaxSteps();
}


// Print 16 steps from the desired pattern bank's selected trk, starting
// at that track's current playhead position
void LoopSeq::oledPrint16(bool A, int8_t trk)
{
  return; // RLR_DEBUG
  display.setCursor(0, trk * 8);

  int8_t cellSize(LOOP.START[trk]->Q - LOOP.END[trk]->Q);

  int8_t offset(ThePattn.getPlayheadIdx(trk));
  if (cellSize < 0)
  {
    cellSize = -cellSize;
  }
  else
  {
    offset   = -offset;
  }

  if (offset < 0)
  {
    offset += 32;
  }

  cellSize += 1;
  uint32_t cpyBits(ThePattn.grabTrack(A, trk));
  for (int8_t idx = 0; idx < 16; ++idx)
  {
    int8_t lcl = (idx + offset) % cellSize; 
    display.print((cpyBits & (1 << lcl)) ? "X" : "-");
  }
  display.display();
}


void LoopSeq::patternOled(bool A)
{
  display.clearDisplay();
  display.setTextColor(WHITE);   
  for (uint8_t trk = 0; trk < TRAX_PER_PATN; ++trk)
  {
    oledPrint16(A, trk);
  }
  display.display();
}


void LoopSeq::recalcLength(uint8_t trk)
{
  int16_t before(LOOP.STEPS[trk]->Q);
  int16_t after(1 + LOOP.END[trk]->Q - LOOP.START[trk]->Q);

  if (after <= 0)             { after += maxSteps; }
  else if ( after > maxSteps) { after %= maxSteps; }

  LOOP.STEPS[trk]->clockIn(after);
  if (after != before)
    Serial.printf("TRK %d: steps before: %d, steps after: %d\n",trk,before,LOOP.STEPS[trk]->Q);
}


// Start point moved but Length stays consistent: End point moves in tandem
void LoopSeq::recalcEndpt(uint8_t trk)
{
  int16_t before(LOOP.END[trk]->Q);

  int16_t newEnd((LOOP.START[trk]->Q + LOOP.STEPS[trk]->Q - 1) % maxSteps);
  LOOP.END[trk]->clockIn(newEnd);

  if (LOOP.END[trk]->Q != before)
  {
    Serial.printf("TRK %d: end before: %d, end after: %d, start: %d,  steps: %d\n",
    trk,before,LOOP.END[trk]->Q, LOOP.START[trk]->Q, LOOP.STEPS[trk]->Q);
  }
}


void LoopSeq::updateStPts(ModalCtrl * pMC, uint8_t trk)
{
  LOOP.START[trk]->clockIn(pMC->readActiveCtrl());
}


void LoopSeq::updateStPts(ModalCtrl (* arr))
{
  for (uint8_t trk = 0; trk < TRAX_PER_PATN; ++trk)
  {
    updateStPts(arr+trk, trk);
  }
}


void LoopSeq::updateNdPts(ModalCtrl * pMC, uint8_t trk)
{
  LOOP.END[trk]->clockIn(pMC->readActiveCtrl());
}


void LoopSeq::updateNdPts(ModalCtrl (* arr))
{
  for (uint8_t trk = 0; trk < TRAX_PER_PATN; ++trk)
  {
    updateNdPts(arr+trk, trk);
  }
}


void LoopSeq::updateOffst(ModalCtrl * pMC, uint8_t trk)
{
  LOOP.START[trk]->clockIn(pMC->readActiveCtrl());
  recalcEndpt(trk);
}


void LoopSeq::updateOffst(ModalCtrl (* arr))
{
  for (uint8_t trk = 0; trk < TRAX_PER_PATN; ++trk)
  {
    updateOffst(arr+trk, trk);
  }
}


void LoopSeq::updateLngth(ModalCtrl * pMC, uint8_t trk)
{
  int16_t before(LOOP.STEPS[trk]->Q);
  int16_t after(LOOP.STEPS[trk]->clockIn(pMC->readActiveCtrl()));
  if (after != before) { Serial.printf("TRK %d: steps before: %d, steps after: %d\n",trk,before,LOOP.STEPS[trk]->Q); }
  recalcEndpt(trk);
  LOOP.JUMP[trk]->clockIn(pMC->readActiveCtrl());
}


void LoopSeq::updateLngth(ModalCtrl (* arr))
{
  for (uint8_t trk = 0; trk < TRAX_PER_PATN; ++trk)
  {
    updateLngth(arr+trk, trk);
  }
}


void LoopSeq::updateJumps(ModalCtrl * pMC, uint8_t trk)
{
  LOOP.JUMP[trk]->clockIn(pMC->readActiveCtrl());
}


void LoopSeq::updateJumps(ModalCtrl (* arr))
{
  for (uint8_t trk = 0; trk < TRAX_PER_PATN; ++trk)
  {
    updateJumps(arr+trk, trk);
  }
}

ClockDivider * DIV[] = { &DIV_A, &DIV_B, &DIV_C, &DIV_D };

void LoopSeq::updateTrkBank(ModalCtrl (* arr), uint8_t trk)
{
  LOOP.START  [trk]->clockIn((arr    )->readActiveCtrl());
  LOOP.END    [trk]->clockIn((arr + 1)->readActiveCtrl());
  DIV[trk]->DIVISION.clockIn((arr + 2)->readActiveCtrl());
  DIV[trk]->    SKEW.clockIn((arr + 3)->readActiveCtrl());
}


void LoopSeq::latchAll()
{
  validatePattns(true);
  
  PW.clock();
  if (TheClock.BPM.pending())
  {
    TheClock.setBPM(TheClock.BPM.clock());
  }
}


void LoopSeq::pulse(ClockSource pulseSrc)
{
  clearExpiredGates();
  if (!running) { return; }
  // Trigger pulsewidth met/exceeded?
  latchAll();

  // Internal clock paused?
  if (pulseSrc == INTERNAL_CLK && !running)
  {
    return;
  }

  uint8_t pulseByte(0);

  pulseByte |= DIV_A.pulse(pulseSrc);
  pulseByte |= DIV_B.pulse(pulseSrc);
  pulseByte |= DIV_C.pulse(pulseSrc);
  pulseByte |= DIV_D.pulse(pulseSrc);

  if (pendingResetFLAG)
  {
    reset();
  }

  // Get the current state of the seq pattern
  if (pulseByte != 0)
  {
    uint8_t gateByte(0);

    bool A(random(0, 100) > PROB_KNOB);
    // Serial.printf("Using %s pattern [%2d]\n", A ? "LEFT" : "RIGHT", ThePattn.getSeed(A));

    for (uint8_t trk = 0; trk < TRAX_PER_PATN; ++trk)
    {
      bool pulseBit(pulseByte & (1 << trk));

      uint32_t lane(ThePattn.grabTrack(A, trk));
      bool stepBit(ThePattn.readBit(A, trk));
      if (stepBit)
      {
        gateByte |= (pulseByte & (1 << trk));
      }

      if (pulseBit)
      {
        ThePattn.setNextPosn(trk, LOOP.JUMP[trk]->Q);
      }
    }

    // Only output gates on tracks that are active this pulse
    newGatesON(gateByte & pulseByte);
    // patternOled(A);
  }
}

//////////////////////////////////////////////////////////////////
//
//                   GATE AND CV OUTPUTS
//
//////////////////////////////////////////////////////////////////

// Turns on the selected gate and notes the time it fired so we can 
// clear it after the selected pulsewidth.
long LoopSeq ::gateON(uint8_t gg)
{
  long now = millis();
  if (allGates[gg])
  {
    digitalWrite(GATEout_PINS[gg], LOW);
  }

  digitalWrite(GATEout_PINS[gg], HIGH);

  allGates[gg] = 1;
  timeGateHigh[gg] = now;

  return now;
}


// Turns off the selected gate and resets it pulsewidth timer
void LoopSeq::gateOFF(uint8_t gg)
{
  digitalWrite(GATEout_PINS[gg], LOW);
  timeGateHigh[gg] = 0;
  allGates[gg] = 0;
}


// Set all outputs to the same state at the same time
void LoopSeq::writeAllGates(bool ON)
{
  for (uint8_t gg = 0; gg < NUM_GATES_OUT; ++gg)
  {
    if (!ON)
    {
      gateOFF(gg);
    }
    else
    {
      gateON(gg);
    }
  }
}


// Bits in setVal correspond to individual gates' states.
void LoopSeq::newGatesON(const byte setVal)
{
  long now = millis();
  for (uint8_t gg = 0; gg < NUM_GATES_OUT; ++gg)
  {
    bool val(setVal & GATE_MASKS[gg]);
    if (val)
    {
      gateON(gg);
    }
  }
}


// Turns off gates who's on time has exceeded the selected pulsewidth
void LoopSeq::clearExpiredGates()
{
  long expired = millis() - PW.clock();

  for (int8_t gg = 0; gg < NUM_GATES_OUT; ++gg)
  {
    if (allGates[gg])
    {
      if (timeGateHigh[gg] < expired)
      {
        gateOFF(gg);
      }
    }
  }
}


void LoopSeq::setCmd(seqCmds cmd)
{
  switch (cmd)
  {
  case seqCmds::clockInCmd:
    pulse(ClockSource::EXT_CLOCK);
    break;

  case seqCmds::mod1Cmd:
    pulse(ClockSource::MOD1_GATEIN);
    break;

  case seqCmds::mod2Cmd:
    ThePattn.swapTracks();
    // pulse(ClockSource::MOD2_GATEIN);
    break;

  case seqCmds::pulseIntlCmd:
    pulse(ClockSource::INTERNAL_CLK);
    break;

  case seqCmds::runCmd:
    running = !running;
    break;

  case seqCmds::resetNowCmd:
    reset();
    break;

  case seqCmds::resetPendingCmd:
    ThePattn.restartPlayheads();
    // patternOled(1);
    pendingResetFLAG = true;
    break;

  default:
    break;
  }
}


// static const uint8_t * FontsILike [] =
//   Iain5x7,		         // similar to system5x7 but proportional
//   Arial_bold_14,      // Bold proportional font; looks good
//   Verdana_digits_24,  // large proportional font contains [0-9] and :
//   lcdnums12x16,	     // font that looks like LCD digits
//   lcdnums14x24,	     // font that looks like LCD digits
//   fixed_bold10x15     // 4 ROWS, 11.5 COLS; looks good