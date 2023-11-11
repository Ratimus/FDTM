#include "CtrlPanel.h"
#include "FDTM_IO.h"
#include "SharedCtrl.h"

#include "LoopSeq.h"
extern LoopSeq TheSeq;
// #include "LoopSeq.h"
// #include "ClockObject.h"
// #include "ClockDivider.h"

// // Use tact switches w/pullups instead of gate inputs
// #ifndef RLR_DEBUG
//   #define RLR_DEBUG 1
// #endif


MCP3204 adc0;
MCP3008 adc1;

void initADC()
{
  adc0.begin(ADC0_CS); // Chip select pin.
  adc0.setSPIspeed(4000000);
}

void initFaders()
{
  long t0(micros());
  while (!pFaders[0]->isReady()) { ; }
  long t1(micros());
  while (!pFaders[1]->isReady()) { ; }
  long t2(micros());
  while (!pFaders[2]->isReady()) { ; }
  long t3(micros());
  while (!pFaders[3]->isReady()) { ; }
  long t4(micros());
  Serial.printf("Fader 0 ready [%d uS]...",t1-t0);
  Serial.printf("Fader 1 ready [%d uS]...",t2-t1);
  Serial.printf("Fader 2 ready [%d uS]...",t3-t1);
  Serial.printf("Fader 3 ready [%d uS]...\n",t4-t3);
  delay(10);
  Serial.printf("ADC 0 ready\n");
}
////////////////////////////////////////////////////////////////
//                        BUTTONS
////////////////////////////////////////////////////////////////
MagicButton modeButtonA(GATEin_PINS[0]);
MagicButton modeButtonB(GATEin_PINS[1]);
MagicButton runButton  (GATEin_PINS[2]);
MagicButton auxButton  (GATEin_PINS[3]);

////////////////////////////////////////////////////////////////
//                      CLICK ENCODER
////////////////////////////////////////////////////////////////
// ClickEncoder          encoder(ENC_A, ENC_B, ENC_SW, STEPS_PER_NOTCH, ACTIVE_LOW);
// ClickEncoderInterface encoderInterface(encoder, SENSITIVITY);

// Menu::RatRotaryEvent  Rotareeeee(                      // Source of "nav events" for ArduinoMenu Library
//     RatRotaryEvent::EventType::BUTTON_CLICKED |        // select
//     RatRotaryEvent::EventType::BUTTON_DOUBLE_CLICKED | // back
//     RatRotaryEvent::EventType::BUTTON_LONG_PRESSED |   // also back
//     RatRotaryEvent::EventType::ROTARY_CCW |            // up
//     RatRotaryEvent::EventType::ROTARY_CW               // down
// );

// Menu::EncoderWrapper  encoderWrapper(encoderInterface, Rotareeeee);

////////////////////////////////////////////////////////////////
//                       FADERS
////////////////////////////////////////////////////////////////
HardwareCtrl Fader1(&adc0, 0, SAMPLE_BUFFER_SIZE);
HardwareCtrl Fader2(&adc0, 1, SAMPLE_BUFFER_SIZE);
HardwareCtrl Fader3(&adc0, 2, SAMPLE_BUFFER_SIZE);
HardwareCtrl Fader4(&adc0, 3, SAMPLE_BUFFER_SIZE);

HardwareCtrl* pFaders[NUM_FADERS] =
{
  &Fader1, &Fader2, &Fader3, &Fader4//, &Fader5, &Fader6, &Fader7, &Fader8
};

int16_t   ZERO(0);
int16_t * VirtualCtrl::sharedZERO(&ZERO);
int16_t * pPTR_HI(&(TheSeq.maxSteps));
int16_t   HI_IDX(TheSeq.maxSteps-1);

size_t  NUM_DIVS(16);
int16_t CLK_DIVS[] = { 64, 32, 16, 8, 7, 6, 5, 4, 3, 2, 1, -2, -3, -4, -6, -8 };

// START POINT
VirtualCtrl start_TEST[4] = {
  VirtualCtrl(pFaders[0], 0, (int16_t*)pPTR_HI),
  VirtualCtrl(pFaders[0], 0, (int16_t*)pPTR_HI),
  VirtualCtrl(pFaders[0], 0, (int16_t*)pPTR_HI),
  VirtualCtrl(pFaders[0], 0, (int16_t*)pPTR_HI)
};
// END POINT
VirtualCtrl endpt_TEST[4] = {
  VirtualCtrl(pFaders[1], HI_IDX, (int16_t*)pPTR_HI),
  VirtualCtrl(pFaders[1], HI_IDX, (int16_t*)pPTR_HI),
  VirtualCtrl(pFaders[1], HI_IDX, (int16_t*)pPTR_HI),
  VirtualCtrl(pFaders[1], HI_IDX, (int16_t*)pPTR_HI)
};
// CLOCK DIVIDE
ArrayCtrl clkdv_TEST[4] = {
  ArrayCtrl(pFaders[2], CLK_DIVS, NUM_DIVS, 10),
  ArrayCtrl(pFaders[2], CLK_DIVS, NUM_DIVS, 10),
  ArrayCtrl(pFaders[2], CLK_DIVS, NUM_DIVS, 10),
  ArrayCtrl(pFaders[2], CLK_DIVS, NUM_DIVS, 10)
};

// CLOCK SKEW
VirtualCtrl cskew_TEST[4] = {
  VirtualCtrl(pFaders[3], 0, 4, -4),
  VirtualCtrl(pFaders[3], 0, 4, -4),
  VirtualCtrl(pFaders[3], 0, 4, -4),
  VirtualCtrl(pFaders[3], 0, 4, -4)
};


VirtualCtrl * STPT_BANK[] = { start_TEST, start_TEST+1, start_TEST+2, start_TEST+3 };
VirtualCtrl * NDPT_BANK[] = { endpt_TEST, endpt_TEST+1, endpt_TEST+2, endpt_TEST+3 };
VirtualCtrl * CDIV_BANK[] = { clkdv_TEST, clkdv_TEST+1, clkdv_TEST+2, clkdv_TEST+3 };
VirtualCtrl * CSKW_BANK[] = { cskew_TEST, cskew_TEST+1, cskew_TEST+2, cskew_TEST+3 };

ModalCtrl TRACK_PARAM_TEST[4] = {
  ModalCtrl(4, STPT_BANK),
  ModalCtrl(4, NDPT_BANK),
  ModalCtrl(4, CDIV_BANK),
  ModalCtrl(4, CSKW_BANK)
};

ModalCtrl (* pCV)(TRACK_PARAM_TEST);
uint8_t      selCH(0);
// MCP3204 adc0;
// MCP3008 adc1;

// const uint8_t NUM_CV_INS(4);
// const uint8_t SAMPLE_BUFFER_SIZE(16);
// const uint8_t NUM_FADERS(4); // RLR_DEBUG - 8 in final version

// int16_t  ZERO(0);
// int16_t* pPTR_HI(&(TheSeq.maxSteps));
// int16_t  HI_IDX(TheSeq.maxSteps-1);

// size_t  NUM_DIVS(16);
// int16_t CLK_DIVS[] = { 64, 32, 16, 8, 7, 6, 5, 4, 3, 2, 1, -2, -3, -4, -6, -8 };

// HardwareCtrl Fader1(&adc0, 0, SAMPLE_BUFFER_SIZE);
// HardwareCtrl Fader2(&adc0, 1, SAMPLE_BUFFER_SIZE);
// HardwareCtrl Fader3(&adc0, 2, SAMPLE_BUFFER_SIZE);
// HardwareCtrl Fader4(&adc0, 3, SAMPLE_BUFFER_SIZE);
//
//// /*extern*/ HardwareCtrl Fader5(&adc0, 0, SAMPLE_BUFFER_SIZE);
//// /*extern*/ HardwareCtrl Fader6(&adc0, 1, SAMPLE_BUFFER_SIZE);
//// /*extern*/ HardwareCtrl Fader7(&adc0, 2, SAMPLE_BUFFER_SIZE);
//// /*extern*/ HardwareCtrl Fader8(&adc0, 3, SAMPLE_BUFFER_SIZE);

// HardwareCtrl* pFaders[NUM_FADERS] =
// {
//   &Fader1, &Fader2, &Fader3, &Fader4//, &Fader5, &Fader6, &Fader7, &Fader8
// };

////////////////////////////////////////////////////////////////
//                          ADCS
////////////////////////////////////////////////////////////////

// void initADCs()
// {
//   adc0.begin(ADC0_CS); // Chip select pin.
//   adc0.setSPIspeed(4000000);
// }
////////////////////////////////////////////////////////////////
//                   TIMER INTERRRUPTS
////////////////////////////////////////////////////////////////
// const uint16_t uS_TO_mS(1000);
// const uint16_t ONE_KHZ_MICROS(uS_TO_mS); // 1000 uS for 1khz timer cycle for encoder
// volatile long rightNOW_micros(0);

// hw_timer_t* timer1 = NULL;               // Timer library takes care of telling this where to point

// void ICACHE_RAM_ATTR onTimer1()
// {
//   int8_t acc(0);
// // RLR_DEBUG gets to line 120 but not 126
//   for (uint8_t fd = 0; fd < 4; ++fd)
//   {
//     pFaders[fd]->service();
//   }

//   encoder.service();

//   switch(acc)
//   {
//     case 0:
//     {
//       cli();
//       rightNOW_micros = micros();
//       sei();
//       break;
//     }
//     case 1:
//     {
//       readGateInputs();
//       break;
//     }
//     case 2:
//     {
//       break;
//     }
//     case 3:
//     {
//       modeButtonA.service();
//       readGateInputs();
//       break;
//     }
//     case 4:
//     {
//       modeButtonB.service();
//       break;
//     }
//     case 5:
//     {
//       readGateInputs();
//       break;
//     }
//     case 6:
//     {
//       runButton.service();
//       break;
//     }
//     case 7:
//     {
//       readGateInputs();
//       break;
//     }
//     case 8:
//     {
//       auxButton.service();
//       break;
//     }
//     case 9:
//     {
//       readGateInputs();
//       break;
//     }
//     default:
//     {
//       break;
//     }
//   }

//   ++acc;
//   acc %= 10;
// }


// //////// TIMER 1 SETUP ////////
// //
// void initTimer1()
// {
//   timer1 = timerBegin(1, 80, true);
//   timerAttachInterrupt(timer1, &onTimer1, true);
// }

// void startTimer1()
// {
//   timerAlarmWrite(timer1, ONE_KHZ_MICROS/10, true);
//   timerAlarmEnable(timer1);
// }

// LoopSeq TheSeq;
// TrkPts LOOP(TheSeq.maxSteps);
// ClockObject TheClock(128, 24);

// ClockDivider DIV_A(0, 'A', TheClock.getPPQN(), ClockSource::INTERNAL_CLK, NULL,   NULL);
// ClockDivider DIV_B(1, 'B', TheClock.getPPQN(), ClockSource::INTERNAL_CLK, &DIV_A, NULL);
// ClockDivider DIV_C(2, 'C', TheClock.getPPQN(), ClockSource::CLONE_A,      &DIV_A, &DIV_B);
// ClockDivider DIV_D(3, 'D', TheClock.getPPQN(), ClockSource::CLONE_B,      &DIV_A, &DIV_B);

// //////// INPUT GATES
// volatile bool newGate_FLAG [NUM_GATES_IN] = {0, 0, 0, 0};
// volatile bool gateIn_VAL   [NUM_GATES_IN] = {0, 0, 0, 0};
// volatile bool gateIn_STATE [NUM_GATES_IN] = {0, 0, 0, 0};

// volatile long dbTimeStamp  [NUM_GATES_IN] = {0, 0, 0, 0};
// const uint8_t dbLockout(25);

// void initDigitalInputs()
// {
//   // Set up I/O pins
//   for (int8_t gg = 0; gg < NUM_GATES_IN; ++gg)
//   {
//     pinMode(GATEin_PINS[gg], INPUT);
//   }
// }


// void initSeq()
// {
//   TheSeq.setCmd(seqCmds::resetNowCmd);
// }

// void readGateInputs()
// {
//   Serial.println(__LINE__);
// #ifdef RLR_DEBUG
//   // Just debounce stuff
//   for (int gg = 1; gg < NUM_GATES_IN; ++gg)
//   {
//     cli();
//     if ((rightNOW_micros - dbTimeStamp[gg]) < dbLockout * uS_TO_mS)
//     {
//       sei();
//       continue;
//     }
//     sei();

//     cli();
//     gateIn_VAL[gg] = !digitalRead(GATEin_PINS[gg]);
//     if (gateIn_VAL[gg] == gateIn_STATE[gg])
//     {
//       sei();
//       continue;
//     }
//     sei();

//     cli();
//     if (newGate_FLAG[gg] == false)
//     {
//       gateIn_STATE[gg] = gateIn_VAL[gg];

//       if (gateIn_STATE[gg])
//       {
//         newGate_FLAG[gg] = true;
//       }
//     }

//     dbTimeStamp[gg] = rightNOW_micros;
//     sei();
//   }
// #else  // IF RLR_DEBUG NOT DEFINED
//   for (int gg = 0; gg < NUM_GATES_IN; ++gg)
//   {
//     gateIn_VAL[gg] = digitalRead(GATEin_PINS[gg]);
//   }
// #endif // RLR_DEBUG DEFINED
//   Serial.println(__LINE__);
// }

//   switch(bnState[0])
//   {
//     case ButtonState::Clicked:
//     {
//       ++selCH;
//       selCH %= 4;
//       Serial.printf("------ select track %d ------\n", selCH);
//       for (uint8_t kn = 0; kn < 4; ++kn)
//       {
//         (pCV + kn)->select(selCH);
//         Serial.printf("control %d: %p\n", kn, (pCV + kn)->pACTIVE);
//       }
//       Serial.printf("----------------------------\n");
//       break;
//     }
//     case ButtonState::DoubleClicked:
//     {
//       for (uint8_t ch = 0; ch < 4; ++ch)
//       {
//         Serial.printf("------- track %d -------\n", ch);
//         Serial.printf("STPT control : %p\n", (pCV    )->ctrlOBJECTS[ch]);
//         Serial.printf("NDPT control : %p\n", (pCV + 1)->ctrlOBJECTS[ch]);
//         Serial.printf("CLDV control : %p\n", (pCV + 2)->ctrlOBJECTS[ch]);
//         Serial.printf("CSKW control : %p\n", (pCV + 3)->ctrlOBJECTS[ch]);
//         Serial.printf("-----------------------\n\n");
//       }
//       break;
//     }
//     default:
//     {
//       break;
//     }
//   }
// }


// void readGates()
// {
//   bool flag(0);
//   for (uint8_t gg = 0; gg < NUM_GATES_IN; ++gg)
//   {
//     cli();
//     flag = newGate_FLAG[gg];
//     newGate_FLAG[gg] = 0;
//     sei();

//     if (!flag) { continue; }

//     else if (gg == 0) { TheSeq.setCmd(seqCmds::clockInCmd); }
//     else if (gg == 1) { TheSeq.setCmd(seqCmds::mod1Cmd);    }
//     else if (gg == 2) { TheSeq.setCmd(seqCmds::mod2Cmd);    }
//     else if (gg == 3) { resetPending = true; }
//   }
// }



// bool forceReset(0);

// void checkResets()
// {
//   // If user-selected reset, wait for a gate event to actually do it
//   if (resetPending)
//   {
//     bool newGate(0);
//     for (uint8_t gg = 0; gg < NUM_GATES_IN; ++gg)
//     {
//       cli();
//       newGate |= newGate_FLAG[gg];
//       sei();
//       if (newGate)
//       {
//         resetPending = false;
//         forceReset = true;
//         break;
//       }
//     }
//   }

//   // SW decided it was a good idea to reset RIGHT NOW and doesn't care what you think
//   if (forceReset)
//   {
//     TheSeq.setCmd(seqCmds::resetNowCmd);
//     TheClock.reset();
//     resetPending = 0;
//     forceReset = 0;
//   }
// }


// // TODO: clean up all these magic numbers once we've got the total HW configuration established
// bool initialized(0);

// void DoInputStuff()
// {
//   if (!initialized)
//   {
//     initialized = true;
//     for (uint8_t kn = 0; kn < 4; ++kn)
//     {
//       (pCV + kn)->select(selCH);
//       Serial.printf("control %d: %p\n", kn, (pCV + kn)->pACTIVE);
//     }
//   }

//   readButtons();
//   readGates();

//   checkResets();

//   // Read internal clock and pulse clock dividers
//   if (TheClock.flagReady()) { TheSeq.setCmd(seqCmds::pulseIntlCmd); }

//   TheSeq.updateTrkBank(pCV, selCH);
//   encoderWrapper.read();
// }

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
// ALL DEAD CODE AFTER THIS
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
// #include "MCP_ADC.h"
// MCP3204 adc0;
// MCP3008 adc1;

// #include "LoopSeq.h"
// extern LoopSeq TheSeq;
// extern TrkPts LOOP;

// #include "SharedCtrl.h"
// const uint8_t SAMPLE_BUFFER_SIZE(128);
// const uint8_t NUM_CV_INS(4);

// int16_t   pointerToZeroPleaseDontAsk(0);
// int16_t   HI_IDX(TheSeq.maxSteps-1);
// int16_t * pPTR_HI(&(TheSeq.maxSteps));
// int16_t * pZZZ(&pointerToZeroPleaseDontAsk);

// HardwareCtrl CV_1(&adc0, 0, SAMPLE_BUFFER_SIZE);
// HardwareCtrl CV_2(&adc0, 1, SAMPLE_BUFFER_SIZE);
// HardwareCtrl CV_3(&adc0, 2, SAMPLE_BUFFER_SIZE);
// HardwareCtrl CV_4(&adc0, 3, SAMPLE_BUFFER_SIZE);

// HardwareCtrl *CV_IN[NUM_CV_INS] = {&CV_1, &CV_2, &CV_3, &CV_4};
// ModalCtrl * pCV(cvModeKnob);

// /////////////////////////////////////////////////////////////////////////
// // FADERS

// // PRIMARY   SLIDER MODE:   LOOP POINTS
// //   1, 3, 5, 7: LOOP START
// //   2, 4, 6, 8: LOOP END -OR- LOOP LENGTH
// //  Trk1 st/offs    ---> fader1                // Repetitor: track offset / Lysiic: start point
// //  Tkr1 end/len    ---> fader2                // Repetitor: loop length  / Lysitic: end point
// //  Trk2 st/offs    ---> fader3                //
// //  Tkr2 end/len    ---> fader4
// //  Trk3 st/offs    ---> fader5                //
// //  Tkr3 end/len    ---> fader6
// //  Trk4 st/offs    ---> fader7                //
// //  Tkr4 end/len    ---> fader8
// //
// // SECONDARY SLIDER MODE 1:
// //   1, 3, 5, 7: CLOCK DIV
// //   2, 4, 6, 8: CLOCK SKEW
// //  Trk1 clk div    ---> fader1 + WR           //
// //  Trk1 clk skew   ---> fader2 + WR           //
// //  Trk2 clk div    ---> fader3 + WR           //
// //  Trk2 clk skew   ---> fader4 + WR           //
// //  Trk3 clk div    ---> fader5 + WR           //
// //  Trk3 clk skew   ---> fader6 + WR           //
// //  Trk4 clk div    ---> fader7 + WR           //
// //  Trk4 clk skew   ---> fader8 + WR           //
// // SECONDARY SLIDER MODE 2: SEQ MOD
// //   1, 3, 5, 7: SEQ STEPS
// //   2, 4, 6, 8: SEQ JITTER
// //  Trk1 seq jump   ---> fader1 + CL           // steps to advance / move back per clock
// //  Trk1 jitter     ---> fader2 + CL           // probability sequence will [advance 2 | fall back 1]
// //  Trk2 seq jump   ---> fader3 + CL           //
// //  Trk2 jitter     ---> fader4 + CL
// //  Trk3 seq jump   ---> fader5 + CL           //
// //  Trk3 jitter     ---> fader6 + CL
// //  Trk4 seq jump   ---> fader7 + CL           //
// //  Trk4 jitter     ---> fader8 + CL
// //
// // TERTIARY  SLIDER MODE 1: MAIN EUCLIDEAN
// // Logic: AND sequence, OR sequence, XOR sequence, NOT sequence, ALL ONES
// //  Trk1 Euc fill   ---> fader1 + 2WR
// //  Trk1 Euc offst  ---> fader2 + 2WR
// //  Trk2 Euc fill   ---> fader3 + 2WR
// // Trk2 Euc offst  ---> fader4 + 2WR
// //  Trk3 Euc fill   ---> fader5 + 2WR
// //  Trk3 Euc offst  ---> fader6 + 2WR
// //  Trk4 Euc fill   ---> fader7 + 2WR
// //  Trk4 Euc offst  ---> fader8 + 2WR
// // TERTIARY  SLIDER MODE 2: AUX. EUCLIDEAN
// //  Trk5 Euc fill   ---> fader1 + 2CL
// //  Trk5 Euc offst  ---> fader2 + 2CL
// //  Trk6 Euc fill   ---> fader3 + 2CL
// //  Trk6 Euc offst  ---> fader4 + 2CL
// //  Trk7 Euc fill   ---> fader5 + 2CL
// //  Trk7 Euc offst  ---> fader6 + 2CL
// //  Trk8 Euc fill   ---> fader7 + 2CL
// //  Trk8 Euc offst  ---> fader8 + 2CL

// const uint8_t NUM_FADERS(8);

// HardwareCtrl Fader1(&adc1, 0);
// HardwareCtrl Fader2(&adc1, 1);
// HardwareCtrl Fader3(&adc1, 2);
// HardwareCtrl Fader4(&adc1, 3);
// HardwareCtrl Fader5(&adc1, 4);
// HardwareCtrl Fader6(&adc1, 5);
// HardwareCtrl Fader7(&adc1, 6);
// HardwareCtrl Fader8(&adc1, 7);

// HardwareCtrl *pFaders[NUM_FADERS] =
// {
//   &Fader1, &Fader2, &Fader3, &Fader4, &Fader5, &Fader6, &Fader7, &Fader8
// };

// // START POINT
// VirtualCtrl start_CTRL[4] = {
//   VirtualCtrl(pFaders[0], 0, pPTR_HI),
//   VirtualCtrl(pFaders[2], 0, pPTR_HI),
//   VirtualCtrl(pFaders[4], 0, pPTR_HI),
//   VirtualCtrl(pFaders[6], 0, pPTR_HI)
// };

// // END POINT
// VirtualCtrl endpt_CTRL[4] = {
//   VirtualCtrl(pFaders[1], HI_IDX, pPTR_HI),
//   VirtualCtrl(pFaders[3], HI_IDX, pPTR_HI),
//   VirtualCtrl(pFaders[5], HI_IDX, pPTR_HI),
//   VirtualCtrl(pFaders[7], HI_IDX, pPTR_HI)
// };

// VirtualCtrl offst_CTRL[4] = {
//   VirtualCtrl(pFaders[0], 0, pPTR_HI),
//   VirtualCtrl(pFaders[2], 0, pPTR_HI),
//   VirtualCtrl(pFaders[4], 0, pPTR_HI),
//   VirtualCtrl(pFaders[6], 0, pPTR_HI)
// };

// // LENGTH
// VirtualCtrl lenth_CTRL[4] = {
//   VirtualCtrl(pFaders[1], HI_IDX+1, pPTR_HI, 0, pZZZ, 1),
//   VirtualCtrl(pFaders[3], HI_IDX+1, pPTR_HI, 0, pZZZ, 1),
//   VirtualCtrl(pFaders[5], HI_IDX+1, pPTR_HI, 0, pZZZ, 1),
//   VirtualCtrl(pFaders[7], HI_IDX+1, pPTR_HI, 0, pZZZ, 1)
// };

// // CLOCK DIVIDE
// // TODO: this might have to become volatile and/or const ptrs should be passed
// // instead of the array itself. We'll see.
// int16_t CLK_DIVISIONS[] = {64, 32, 16, 7, 6, 5, 4, 3, 2, 1, -2, -3, -4, -6, -8};
// size_t  NUM_DIVISIONS(sizeof(CLK_DIVISIONS) / sizeof(CLK_DIVISIONS[0]));

// ArrayCtrl clkdv_CTRL[4] = {
//   ArrayCtrl(pFaders[0], &CLK_DIVISIONS, NUM_DIVISIONS),
//   ArrayCtrl(pFaders[2], &CLK_DIVISIONS, NUM_DIVISIONS),
//   ArrayCtrl(pFaders[6], &CLK_DIVISIONS, NUM_DIVISIONS),
//   ArrayCtrl(pFaders[4], &CLK_DIVISIONS, NUM_DIVISIONS)
// };

// // CLOCK SKEW
// VirtualCtrl cskew_CTRL[4] = {
//   VirtualCtrl(pFaders[1], 0, 4, -4),
//   VirtualCtrl(pFaders[3], 0, 4, -4),
//   VirtualCtrl(pFaders[5], 0, 4, -4),
//   VirtualCtrl(pFaders[7], 0, 4, -4)
// };

// // SEQ JUMP
// VirtualCtrl sqjmp_CTRL[4] = {
//   VirtualCtrl(pFaders[0], 0, 4, -4),
//   VirtualCtrl(pFaders[2], 0, 4, -4),
//   VirtualCtrl(pFaders[4], 0, 4, -4),
//   VirtualCtrl(pFaders[6], 0, 4, -4)
// };

// // JITTER
// VirtualCtrl jtter_CTRL[4] = {
//   VirtualCtrl(pFaders[1], 0, 100),
//   VirtualCtrl(pFaders[3], 0, 100),
//   VirtualCtrl(pFaders[5], 0, 100),
//   VirtualCtrl(pFaders[7], 0, 100)
// };

// // EUC1 FILL
// VirtualCtrl efllA_CTRL[4] = {
//   VirtualCtrl(pFaders[0], HI_IDX+1, pPTR_HI, 0, pZZZ, 1),
//   VirtualCtrl(pFaders[2], HI_IDX+1, pPTR_HI, 0, pZZZ, 1),
//   VirtualCtrl(pFaders[4], HI_IDX+1, pPTR_HI, 0, pZZZ, 1),
//   VirtualCtrl(pFaders[6], HI_IDX+1, pPTR_HI, 0, pZZZ, 1)
// };

// // EUC1 OFFSET
// VirtualCtrl efstA_CTRL[4] = {
//   VirtualCtrl(pFaders[1], 0, pPTR_HI),
//   VirtualCtrl(pFaders[3], 0, pPTR_HI),
//   VirtualCtrl(pFaders[5], 0, pPTR_HI),
//   VirtualCtrl(pFaders[7], 0, pPTR_HI)
// };

// // EUC2 FILL
// VirtualCtrl efllB_CTRL[4] = {
//   VirtualCtrl(pFaders[0], HI_IDX+1, pPTR_HI, 0, pZZZ, 1),
//   VirtualCtrl(pFaders[2], HI_IDX+1, pPTR_HI, 0, pZZZ, 1),
//   VirtualCtrl(pFaders[4], HI_IDX+1, pPTR_HI, 0, pZZZ, 1),
//   VirtualCtrl(pFaders[6], HI_IDX+1, pPTR_HI, 0, pZZZ, 1),
// };

// // EUC2 OFFSET
// VirtualCtrl efstB_CTRL[4] = {
//   VirtualCtrl(pFaders[1], 0, pPTR_HI),
//   VirtualCtrl(pFaders[3], 0, pPTR_HI),
//   VirtualCtrl(pFaders[5], 0, pPTR_HI),
//   VirtualCtrl(pFaders[7], 0, pPTR_HI)
// };

// VirtualCtrl * FADER_0[] =
// { &start_CTRL[0], &offst_CTRL[0], &clkdv_CTRL[0], &sqjmp_CTRL[0], &efllA_CTRL[0], &efllB_CTRL[0] };
// ModalCtrl CTRL_0(4, FADER_0);

// VirtualCtrl * FADER_1[] =
// { &endpt_CTRL[0], &lenth_CTRL[0], &cskew_CTRL[0], &jtter_CTRL[0], &efstA_CTRL[0], &efstB_CTRL[0] };
// ModalCtrl CTRL_1(4, FADER_1);

// VirtualCtrl * FADER_2[] =
// { &start_CTRL[1], &offst_CTRL[1], &clkdv_CTRL[1], &sqjmp_CTRL[1], &efllA_CTRL[1], &efllB_CTRL[1] };
// ModalCtrl CTRL_2(4, FADER_2);

// VirtualCtrl * FADER_3[] =
// { &endpt_CTRL[1], &lenth_CTRL[1], &cskew_CTRL[1], &jtter_CTRL[1], &efstA_CTRL[1], &efstB_CTRL[1] };
// ModalCtrl CTRL_3(4, FADER_3);

// VirtualCtrl * FADER_4[] =
// { &start_CTRL[2], &offst_CTRL[2], &clkdv_CTRL[2], &sqjmp_CTRL[2], &efllA_CTRL[2], &efllB_CTRL[2] };
// ModalCtrl CTRL_4(4, FADER_4);

// VirtualCtrl * FADER_5[] =
// { &endpt_CTRL[2], &lenth_CTRL[2], &cskew_CTRL[2], &jtter_CTRL[2], &efstA_CTRL[2], &efstB_CTRL[2] };
// ModalCtrl CTRL_5(4, FADER_5);

// VirtualCtrl * FADER_6[] =
// { &start_CTRL[3], &offst_CTRL[3], &clkdv_CTRL[3], &sqjmp_CTRL[3], &efllA_CTRL[3], &efllB_CTRL[3] };
// ModalCtrl CTRL_6(4, FADER_6);

// VirtualCtrl * FADER_7[] =
// { &endpt_CTRL[3], &lenth_CTRL[3], &cskew_CTRL[3], &jtter_CTRL[3], &efstA_CTRL[3], &efstB_CTRL[3] };
// ModalCtrl CTRL_7(4, FADER_7);

// #endif