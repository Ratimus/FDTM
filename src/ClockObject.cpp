// ------------------------------------------------------------------------
// ClockObject.cpp
//
// Nov. 2022
// Ryan "Ratimus" Richardson
// ------------------------------------------------------------------------
#include "ClockObject.h"


void ICACHE_RAM_ATTR countTicks()
{
  TheClock.PULSE();
}


//////////////////////////////////////////////////////////////
// NOTES/TODO:
//
//////////////////////////////////////////////////////////////

ClockObject::ClockObject(uint16_t bpm, uint16_t inPPQN) :
  newClockFLAG(false),
  newTickFLAG(false),
  tickCounter(0),
  ticksPerBeat(-1),
  BPM(bpm),
  PPQN(inPPQN),
  lilTicker(NULL)
{
  init();
  setBPM(bpm);
}


void ClockObject::PULSE()
{
  cli();
  ++tickCounter;
  tickCounter %= PPQN;
  newClockFLAG = true;
  sei();
}


bool ClockObject::flagReady()
{
  bool retVal;
  cli();
  retVal       = newClockFLAG;
  newClockFLAG = false;
  sei();
  return retVal;
}


void ClockObject::reset()
{
  cli();
  tickCounter  = 0;
  newClockFLAG = false;
  timerRestart(lilTicker);
  sei();
}


uint16_t ClockObject::getPulseCounter()
{
  uint16_t retVal;
  cli();
  retVal = tickCounter;
  sei();
  return retVal;
}


void ClockObject::init()
{
  if (lilTicker != 0) { return; }

  lilTicker = timerBegin(2, 80, true);
  timerAttachInterrupt(lilTicker, &countTicks, true);
}


void ClockObject::setBPM(uint16_t inBPM)
{
  BPM.clockIn(inBPM);

  // (micros/beat) = (micros/milli)(millis/sec)(sec/min)(min/beat)
  //               = (    1000    )(   1000   )(   60  )( 1 / BPM)
  uint32_t ticksPerBeat = (60000000 / (BPM.Q * PPQN));
  timerAlarmWrite(lilTicker, ticksPerBeat, true);
  timerAlarmEnable(lilTicker);
}


void ClockObject::pause()
{
  timerStop(lilTicker);
}


void ClockObject::resume()
{
  timerStart(lilTicker);
}
