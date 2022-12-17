// ------------------------------------------------------------------------
// Latchable.h
//
// Nov. 2022
// Ryan "Ratimus" Richardson
// ------------------------------------------------------------------------
#ifndef LASH_A_BULL_DOT_AITCH
#define LASH_A_BULL_DOT_AITCH
#include <Arduino.h>


template <typename T>
  class latchable
{
protected:
  T ParamR;
  T ParamQ;
  T ParamS;
  bool enabled;

public:
  const T& Q;
  T& D;

  explicit latchable(T data):
    ParamR(data),
    ParamQ(ParamR),
    ParamS(ParamQ),
    enabled(true),
    Q(ParamQ),
    D(ParamS)
  { ; }

  bool enable(bool en)
  {
    enabled = en;
    return enabled;
  }

  void set(T data)
  {
    if (enabled)
    {
      ParamS = data;
    }
  }

  T clock(void)
  {
    if (enabled) { ParamQ = ParamS; }
    return Q;
  }

  T clockIn(T data)
  {
    set(data);
    return clock();
  }

  void clear()
  {
    set(ParamR);
  }

  void reset()
  {
    clear();
    clock();
  }

  T pending()
  {
    return (D - Q);
  }

  // Change the default value to which element reverts on RESET
  void preEnable(T data)
  {
    ParamR = data;
  }
};


#endif