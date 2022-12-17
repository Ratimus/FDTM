// ------------------------------------------------------------------------
// LoopSeq.h
//
// Nov. 2022
// Ryan "Ratimus" Richardson
// ------------------------------------------------------------------------
#ifndef LoopSeq_h
#define LoopSeq_h

#include <Arduino.h>
#include "FDTM_IO.h"
#include "Latchable.h"
#include "ClockDivider.h"
#include "SeqPattern.h"

class ModalCtrl;

extern const int8_t  ELASTIC_MODE;
extern const int8_t  WINDOW_MODE;

class TrkPts
{
public:

  latchable<int8_t> S1;
  latchable<int8_t> S2;
  latchable<int8_t> S3;
  latchable<int8_t> S4;

  latchable<int8_t> E1;
  latchable<int8_t> E2;
  latchable<int8_t> E3;
  latchable<int8_t> E4;

  latchable<int8_t> O1;
  latchable<int8_t> O2;
  latchable<int8_t> O3;
  latchable<int8_t> O4;

  latchable<int8_t> P1;
  latchable<int8_t> P2;
  latchable<int8_t> P3;
  latchable<int8_t> P4;

  latchable<int8_t> J1;
  latchable<int8_t> J2;
  latchable<int8_t> J3;
  latchable<int8_t> J4;

  latchable<int8_t> * START [4];
  latchable<int8_t> * END   [4];
  latchable<int8_t> * STEPS [4];
  latchable<int8_t> * JUMP  [4];

  explicit TrkPts(int8_t maxLength);
};
extern TrkPts LOOP;

enum seqCmds : int8_t
{
  runCmd = 0,
  clockInCmd,
  mod1Cmd,
  mod2Cmd,
  pulseIntlCmd,
  resetNowCmd,
  resetPendingCmd,
  NUM_SEQ_CMDS
};

class LoopSeq
{
// Set start points to 0, end points to 31.

public:
  latchable<int8_t>  MODE;
  latchable<uint8_t> PW;

  SeqPattern ThePattn;

  int8_t  pattnA;
  int8_t  pattnB;

  bool    running;
  bool    pendingResetFLAG;

  uint16_t probB;
  uint16_t PROB_KNOB;
  int16_t  maxSteps; // Total number of valid steps in pattern (e.g. count begins at ONE)
  
  void reset();

  void validatePattns(bool force = false);
  
  void setCmd(seqCmds cmd);
  void pulse(ClockSource pulseSrc);

  void oledPrint16(bool A, int8_t trk);
  void patternOled(bool A);

  void latchAll();
  void clearExpiredGates();
  long gateON(uint8_t gg);
  void gateOFF(uint8_t gg);
  void writeAllGates(bool ON);
  void newGatesON(const byte setVal);
  void updateMaxSteps();
  void recalcLength(uint8_t trk);
  void recalcEndpt(uint8_t trk);
  void updateStPts(ModalCtrl (* arr));
  void updateNdPts(ModalCtrl (* arr));
  void updateOffst(ModalCtrl (* arr));
  void updateLngth(ModalCtrl (* arr));
  void updateJumps(ModalCtrl (* arr));
  void updateTrkBank(ModalCtrl (* arr), uint8_t trk);
  LoopSeq();
};

extern LoopSeq TheSeq;

#endif