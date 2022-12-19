// ------------------------------------------------------------------------
// ClockObject.h
//
// Nov. 2022
// Ryan "Ratimus" Richardson
// ------------------------------------------------------------------------
#ifndef ClockObject_h
#define ClockObject_h

#include <Arduino.h>
#include "Latchable.h"

//////////////////////////////////////////////////////////////
// NOTES/TODO:
//
//////////////////////////////////////////////////////////////
extern uint16_t TEMPO;

class ClockObject
{
private:
  volatile bool     newClockFLAG;
  volatile bool     newTickFLAG;
  volatile uint16_t tickCounter;

  unsigned long     ticksPerBeat;  // Per quarter note

public:
  latchable<uint16_t> BPM;
  const uint16_t PPQN;

  hw_timer_t * lilTicker;

  // BPM: tempo (in quarter notes)
  // PPQN: pulses per quarter note
  ClockObject(uint16_t inBPM, uint16_t inPPQN);
  ~ClockObject() { ; }

  void init();
  void reset();
  void pause();
  void resume();
  void PULSE();
  void setBPM(uint16_t inBPM);

  uint16_t getPulseCounter();
  uint16_t getPPQN() { return PPQN; }
  bool     flagReady();
};

extern ClockObject TheClock;

#endif
