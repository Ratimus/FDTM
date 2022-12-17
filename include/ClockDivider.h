// ------------------------------------------------------------------------
// ClockDivider.h
//
// Nov. 2022
// Ryan "Ratimus" Richardson
// ------------------------------------------------------------------------
#ifndef CLARK_DEFENDER_DOT_AITCH
#define CLARK_DEFENDER_DOT_AITCH

#include <Arduino.h>
#include "RatFuncs.h"
#include "Latchable.h"
#include "FDTM_IO.h"

const int8_t  EXPIRED(-99);  // Clock delay

enum ClockSource
{
  EXT_CLOCK     = 1 << 0,
  MOD1_GATEIN   = 1 << 1,
  MOD2_GATEIN   = 1 << 2,
  INTERNAL_CLK  = 1 << 3,
  CLONE_A       = 1 << 4,
  CLONE_B       = 1 << 5,
  CH_A_INPUT    = 1 << 6,
  CH_B_INPUT    = 1 << 7,
  CH_A_OUTPUT   = 1 << 8,
  CH_B_OUTPUT   = 1 << 9
};


class ClockDivider
{
public:
  latchable<ClockSource>  SOURCE;
  latchable<int8_t>     DIVISION;
  latchable<int8_t>         SKEW;
  latchable<uint8_t>       STATE;

protected:
  const uint8_t trackMask;
  const char id;
  const uint16_t ppqn;
  
  
  const ClockDivider * const pCH_A;
  const ClockDivider * const pCH_B;

  int16_t   pulseAccumulator;
  int16_t  offsetAccumulator;
  int8_t           offsetter;
  int16_t            divider;

public:

  ClockDivider(
    uint8_t maskIDX,
    char setID,
    uint16_t PPQN,
    ClockSource src, 
    ClockDivider * chA,
    ClockDivider * chB):
      SOURCE(src),
      DIVISION(1),
      SKEW(0),
      STATE(0),
      trackMask((uint8_t)BITMASK_32[maskIDX]),
      id(setID),
      ppqn(PPQN),
      pCH_A(chA),
      pCH_B(chB),
      pulseAccumulator(0),
      offsetAccumulator(EXPIRED),
      offsetter(0),
      divider(-4)
  { ; }

  void latchAll()
  {
    SOURCE.clock();
    DIVISION.clock();
    SKEW.clock();
  }

  void reset();
  void printID() { Serial.print(id); }
  void settShittUpp();
  uint8_t pulse(ClockSource pulseSrc);
};

extern ClockDivider DIV_A;
extern ClockDivider DIV_B;
extern ClockDivider DIV_C;
extern ClockDivider DIV_D;

#endif