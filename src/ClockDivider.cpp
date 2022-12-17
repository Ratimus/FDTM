// ------------------------------------------------------------------------
// ClockDivider.cpp
//
// Nov. 2022
// Ryan "Ratimus" Richardson
// ------------------------------------------------------------------------
#include "ClockDivider.h"
#include "ClockObject.h"
#include "FDTM_IO.h"


void ClockDivider::reset()
{
  STATE.reset();
  pulseAccumulator  = 0;
  offsetAccumulator = EXPIRED;
}


void ClockDivider::settShittUpp()
{
  int16_t scale  = (SOURCE.Q == ClockSource::INTERNAL_CLK) ? ppqn : 1;
  int16_t tmpdiv = DIVISION.Q;
  offsetter      = SKEW.Q;
  if (tmpdiv < 0)
  {
    if (ppqn > 1)
    {
      int16_t tmp(scale / abs(tmpdiv));
      divider = (tmp > 0) ? tmp : 1;
    }
    else
    {
      divider = 1;
    }
  }
  else
  {
    divider = tmpdiv * scale;
  }

  offsetter *= scale;

  if (offsetter >= divider)
  {
    offsetter /= divider;
  }
}


uint8_t ClockDivider::pulse(ClockSource pulseSrc)
{
  STATE.clockIn(false);
  latchAll();
  ClockSource tmpSrc(SOURCE.Q);
  bool pulseThis(0);

  // Clone ch. A: (pulse out = A pulse out)
  if (tmpSrc & CLONE_A)
  {
    if ( (pCH_A != 0)
     &&  (pCH_A->SOURCE.Q & pulseSrc)
     &&  (pCH_A->STATE.Q  & pCH_A->trackMask) )
    {
      STATE.set(this->trackMask);
    }
    return STATE.clock();
  }

  // Clone ch. B: (pulse out = B pulse out)
  if (tmpSrc & CLONE_B)
  {
    if (pCH_B != 0)
    {
      // Clone B && B was pulsed by source && B output state HIGH
      if ( (pCH_B->SOURCE.Q & pulseSrc)
        && (pCH_B->STATE.Q  & pCH_B->trackMask) )
      {
        STATE.set(this->trackMask);
      }
      else if (pCH_A->SOURCE.Q & pulseSrc)
      {
        // Clone B, but [B = clone A]: (pulse out = A pulse out)
        if ( (pCH_B->SOURCE.Q & CLONE_A)
          && (pCH_A->STATE.Q  & pCH_A->trackMask) )
        {
          STATE.set(this->trackMask);
        }
        // Clone B, but [B = A input] : (pulse out = B pulse out IF A pulse in)
        else if ( (pCH_B->SOURCE.Q & CH_A_INPUT)
               && (pCH_A->SOURCE.Q & pulseSrc)
               && (pCH_B->STATE.Q  & pCH_B->trackMask) )
        {
          STATE.set(this->trackMask);
        }
      }
    }

      return STATE.clock();
  }  

  settShittUpp();

  // Direct pulse
  if (SOURCE.Q & pulseSrc)
  {
    pulseThis = true;
  }
  
  // Follow A's INPUT (apply own clock source, division, and all other settings)
  if ( (tmpSrc & CH_A_INPUT) && (pCH_A->SOURCE.Q & pulseSrc) )
  {
    pulseThis = true;
  }

  // Follow B's INPUT (apply own clock source, division, and all other settings)
  if (tmpSrc & CH_B_INPUT)
  {
    // Following B's INPUT but B is cloning A's OUTPUT: pretend A OUT is our IN
    if (pCH_B->SOURCE.Q & CLONE_A)
    {
      tmpSrc = CH_A_OUTPUT;
    }

    // Following B's INPUT, B is following A's INPUT: follow A's INPUT
    if ( (pCH_B->SOURCE.Q & CH_A_INPUT) && (pCH_A->SOURCE.Q & pulseSrc) )
    {
      pulseThis = true;
    }
  }

  // Use Ch. A's OUTPUT as our INPUT
  if (tmpSrc & CH_A_OUTPUT)
  {
    pulseThis = (pCH_A->STATE.Q  & pCH_A->trackMask);
  }

  // Use Ch. B's OUTPUT as our INPUT
  if (tmpSrc & CH_B_OUTPUT)
  {
    pulseThis = (pCH_B->STATE.Q & pCH_B->trackMask);
  }

  if (!pulseThis)
  {
    return STATE.clock();
  }

  if (pulseAccumulator == 0)
  {
    if (offsetter == 0)
    {
      offsetAccumulator = 0;
    }
    else if(offsetAccumulator < 0)
    {
      offsetAccumulator = offsetter;
    }
  }
  
  if (offsetAccumulator == 0)
  {
    STATE.set(this->trackMask);
    offsetAccumulator = EXPIRED;
  }
  else if (offsetAccumulator > 0)
  {
      --offsetAccumulator;
  }

  ++pulseAccumulator;
  pulseAccumulator %= divider;

  return STATE.clock();
}


const uint8_t NUM_DIVIDERS(4);
