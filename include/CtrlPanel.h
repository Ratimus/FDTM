#ifndef CTRLPANEL_H
#define CTRLPANEL_H
#include <Arduino.h>
#include "MagicButton.h"
#include "SharedCtrl.h"
#include "MCP_ADC.h"

const uint8_t NUM_CV_INS(4);
const uint8_t NUM_FADERS(4); // RLR_DEBUG

extern MagicButton modeButtonA;
extern MagicButton modeButtonB;
extern MagicButton runButton;
extern MagicButton auxButton;

const uint8_t SAMPLE_BUFFER_SIZE(16);

void initADC();
void initFaders();

extern ModalCtrl * pCV;
extern uint8_t selCH;

extern HardwareCtrl Fader1;
extern HardwareCtrl Fader2;
extern HardwareCtrl Fader3;
extern HardwareCtrl Fader4;

extern HardwareCtrl* pFaders[NUM_FADERS];

/*
#include <ClickEncoder.h>
#include <ClickEncoderInterface.h>
#include <RatRotaryEvent.h>
#include <EncoderWrapper.h>
#include "LoopSeq.h"
#include "ClockObject.h"
#include "ClockDivider.h"

extern Menu::EncoderWrapper encoderWrapper;
extern Menu::RatRotaryEvent Rotareeeee;
extern LoopSeq              TheSeq;
extern TrkPts               LOOP;

extern ClockObject  TheClock;

extern ClockDivider DIV_A;
extern ClockDivider DIV_B;
extern ClockDivider DIV_C;
extern ClockDivider DIV_D;

void initADCs();
void startADCs();
void initTimer1();
void startTimer1();
void initSeq();
void initDigitalInputs();

void DoInputStuff();
void readGateInputs();
*/
////////////////////////////////////////////////////////////////
//                  GATE INPUTS
////////////////////////////////////////////////////////////////


#endif

// HardwareCtrl CV_1(&adc0, 0, SAMPLE_BUFFER_SIZE);
// HardwareCtrl CV_2(&adc0, 1, SAMPLE_BUFFER_SIZE);
// HardwareCtrl CV_3(&adc0, 2, SAMPLE_BUFFER_SIZE);
// HardwareCtrl CV_4(&adc0, 3, SAMPLE_BUFFER_SIZE);

// HardwareCtrl *CV_IN[NUM_CV_INS] = {&CV_1, &CV_2, &CV_3, &CV_4};

// // VirtualCtrl
// // Track 1

// VirtualCtrl CV1A_CTRL(CV_IN[0], 0, pPTR_HI);
// VirtualCtrl CV1B_CTRL(CV_IN[0], 0, pPTR_HI);
// VirtualCtrl CV1C_CTRL(CV_IN[0], 0, pPTR_HI);
// VirtualCtrl CV1D_CTRL(CV_IN[0], 0, pPTR_HI);
// VirtualCtrl CV1E_CTRL(CV_IN[0], 0, pPTR_HI);
// VirtualCtrl * LockCV_1[] = { &CV1A_CTRL, &CV1B_CTRL, &CV1C_CTRL, &CV1D_CTRL, &CV1E_CTRL};

// // Track 2
// VirtualCtrl CV2A_CTRL(CV_IN[1], 0, pPTR_HI);
// VirtualCtrl CV2B_CTRL(CV_IN[1], 0, pPTR_HI);
// VirtualCtrl CV2C_CTRL(CV_IN[1], 0, pPTR_HI);
// VirtualCtrl CV2D_CTRL(CV_IN[1], 0, pPTR_HI);
// VirtualCtrl CV2E_CTRL(CV_IN[1], 1,  4, -4);
// VirtualCtrl * LockCV_2[] = { &CV2A_CTRL, &CV2B_CTRL, &CV2C_CTRL, &CV2D_CTRL };

// // Track 3
// VirtualCtrl CV3A_CTRL(CV_IN[2], 0, pPTR_HI);
// VirtualCtrl CV3B_CTRL(CV_IN[2], 0, pPTR_HI);
// VirtualCtrl CV3C_CTRL(CV_IN[2], 0, pPTR_HI);
// VirtualCtrl CV3D_CTRL(CV_IN[2], 0, pPTR_HI);
// VirtualCtrl CV3E_CTRL(CV_IN[2], 1,  4, -4 );
// VirtualCtrl * LockCV_3[] = { &CV3A_CTRL, &CV3B_CTRL, &CV3C_CTRL, &CV3D_CTRL };

// // Track 4
// VirtualCtrl CV4A_CTRL(CV_IN[3], 0, pPTR_HI);
// VirtualCtrl CV4B_CTRL(CV_IN[3], 0, pPTR_HI);
// VirtualCtrl CV4C_CTRL(CV_IN[3], 0, pPTR_HI);
// VirtualCtrl CV4D_CTRL(CV_IN[3], 0, pPTR_HI);
// VirtualCtrl CV4E_CTRL(CV_IN[3], 1,  4, -4);
// VirtualCtrl * LockCV_4[] = { &CV4A_CTRL, &CV4B_CTRL, &CV4C_CTRL, &CV4D_CTRL };

// ModalCtrl cvModeKnob[] = {
//   ModalCtrl(5, (VirtualCtrl**)(&LockCV_1)),
//   ModalCtrl(5, (VirtualCtrl**)(&LockCV_2)),
//   ModalCtrl(5, (VirtualCtrl**)(&LockCV_3)),
//   ModalCtrl(5, (VirtualCtrl**)(&LockCV_4))
// };

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
