#ifndef EncoderWrapper_H
#define EncoderWrapper_H

#include <Arduino.h>
#include "ClickEncoderInterface.h"
#include <menuDefs.h>
#include <RatRotaryEvent.h>

namespace Menu
{  
class EncoderWrapper : public menuIn
{
private:

  ClickEncoderInterface &EI;
  RatRotaryEvent &REI;
  encEvnts _evt;

public:

  EncoderWrapper(
    ClickEncoderInterface &EncoderInterface,
    RatRotaryEvent &Rotary) :
  EI(EncoderInterface),
  REI(Rotary),
  _evt(encEvnts::NUM_ENC_EVNTS)
  { update(); }

  inline void update()
  {
    _evt = EI.getEvent();
  }

  int available(void)
  {
    return peek() != -1;
  }

  int peek(void)
  {
    switch(_evt)
    {
      case encEvnts::Click:
        REI.registerEvent(RatRotaryEvent::EventType::BUTTON_CLICKED);
        return options->navCodes[enterCmd].ch;

      case encEvnts::DblClick:
        REI.registerEvent(RatRotaryEvent::EventType::BUTTON_DOUBLE_CLICKED);
        return options->navCodes[escCmd].ch;

      case encEvnts::Right:
      case encEvnts::ShiftRight:
        REI.registerEvent(RatRotaryEvent::EventType::ROTARY_CW);
        return options->navCodes[downCmd].ch;
      
      case encEvnts::Left:
      case encEvnts::ShiftLeft:
        REI.registerEvent(RatRotaryEvent::EventType::ROTARY_CCW);
        return options->navCodes[upCmd].ch;
      
      case encEvnts::ClickHold:
        REI.registerEvent(RatRotaryEvent::EventType::BUTTON_DOUBLE_CLICKED);
        return options->navCodes[escCmd].ch;

      case encEvnts::Hold:
      case encEvnts::Press:
        REI.registerEvent(RatRotaryEvent::EventType::BUTTON_LONG_PRESSED);
        return options->navCodes[idxCmd].ch;  // Experiment; I don't even know what this does

      case encEvnts::NUM_ENC_EVNTS:
      default:
        return -1;
    }
  }

  int read()
  {
    update();
    return peek();
  }

  void flush()
  {
    EI.flush();
  }

  size_t write(uint8_t v)
  {
    return 1;
  }
};
}//namespace Menu

#endif /* EncoderWrapper_h */
