/*
4D-Turing Machine / Turing Machine On Crack
Nov. 2022
Ryan "Ratimus" Richardson
 _______________ _______________ _______
|o o o o o o o o| _             |o_     |
|               |(-)            |(-) o_ |
|       _       |               |o_  (-)|
|     ((!))     |  --[]-------  |(-) o_ |
|          ___  |  -----[]----  |o_  (-)|
|  _       \ /  |  ----[]-----  |(-)    |
| (!)      (V)  |  -------[]--  |o_     |
|               |  --[]-------  |(-)    |
|               |  ---[]------  |o_     |
|        o_   _ |  -[]--------  |(-) o_ |
|        (-) (!)|  ----[]-----  |o_  (-)|
|               |               |(-) o_ |
|o_   _   _   _ | _   _   _   _ |o_  (-)|
|(-) (-) (-) (-)|(!) (-) (-) (!)|(-)    |
|_______________|_______________|_______|
*/

////////////////////////////////////////////////////////////
// TODO:
//  *remove start and end point shifts from menu -> put them
//   under control of faders
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
//              HARDWARE LIBRARY STUFF
//
////////////////////////////////////////////////////////////

#include <Arduino.h>
#include <pgmspace.h>

// I/O definitions
#include "FDTM_IO.h"

// An instance of our sequencer
#include "LoopSeq.h"
LoopSeq TheSeq;
TrkPts LOOP(TheSeq.maxSteps);


// Interrupt-driven timer for internal clock
#include "ClockObject.h"
ClockObject TheClock(128, 24);

// Four freely configurable clock dividers - one to drive each track
#include "ClockDivider.h"
ClockDivider DIV_A(0, 'A', TheClock.getPPQN(), ClockSource::INTERNAL_CLK, NULL,   NULL);
ClockDivider DIV_B(1, 'B', TheClock.getPPQN(), ClockSource::INTERNAL_CLK, &DIV_A, NULL);
ClockDivider DIV_C(2, 'C', TheClock.getPPQN(), ClockSource::CLONE_A,      &DIV_A, &DIV_B);
ClockDivider DIV_D(3, 'D', TheClock.getPPQN(), ClockSource::CLONE_B,      &DIV_A, &DIV_B);

////////////////////////////////////////////////////////////////
//                          OLED
////////////////////////////////////////////////////////////////

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET -1    // Unused but has to be defined for library

// Create OLED display device
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

////////////////////////////////////////////////////////////////
//                      CLICK ENCODER
////////////////////////////////////////////////////////////////

// The lower-level HW driver
#include <ClickEncoder.h>
const uint8_t STEPS_PER_NOTCH(2);
const bool ACTIVE_LOW(true);
ClickEncoder encoder(ENC_A, ENC_B, ENC_SW, STEPS_PER_NOTCH, ACTIVE_LOW);

// Wrapper for the encoder - gives us nice, pretty events to read
#include <ClickEncoderInterface.h>
const uint8_t SENSITIVITY(2);
ClickEncoderInterface encoderInterface(encoder, SENSITIVITY);

// Source of "nav events" for ArduinoMenu Library
#include <RatRotaryEvent.h>
RatRotaryEvent reeeee(
    RatRotaryEvent::EventType::BUTTON_CLICKED |        // select
    RatRotaryEvent::EventType::BUTTON_DOUBLE_CLICKED | // back
    RatRotaryEvent::EventType::BUTTON_LONG_PRESSED |   // also back
    RatRotaryEvent::EventType::ROTARY_CCW |            // up
    RatRotaryEvent::EventType::ROTARY_CW               // down
);

// Wrap the encoder interface such that it can drive our menu system
#include <EncoderWrapper.h>
Menu::EncoderWrapper encoderWrapper(encoderInterface, reeeee);

#include <menu.h>
#include <menuIO/keyIn.h>
#include <menuIO/chainStream.h>
#include <menuIO/serialOut.h>
#include <menuIO/adafruitGfxOut.h>
#include <menuIO/serialIn.h>
/////////////////////////////////////////////////////////////
// +-----------------------------------------------------+ //
// | /////////////////////////////////////////////////// | //
// | // WARNING: MACROS!!! This directive has to come // | //
// | // *AFTER* the stuff you're menuing around with  // | //
/* | //   * * * */ #include "menusystem.h" //* * * *  // | //
// | // WARNING: MACROS!!! This directive has to come // | //
// | // *AFTER* the stuff you're menuing around with  // | //
// | /////////////////////////////////////////////////// | //
// +-----------------------------------------------------+ //
/////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////
//                       ADCs
////////////////////////////////////////////////////////////////
// enum FaderMode
// {
//   LOOP_POINTS = 0,
//   CLOCK_MOD,
//   SEQ_MOD,
//   EUC_MAIN,
//   EUC_SECONDARY,
//   NUM_MODES
// };

const uint8_t NUM_CV_INS(4);
const uint8_t NUM_FADERS(4); // RLR_DEBUG

#include "MCP_ADC.h"
MCP3204 adc0;
MCP3008 adc1;

#include "SharedCtrl.h"
const uint8_t SAMPLE_BUFFER_SIZE(1);

/*
HardwareCtrl Fader1(&adc1, 0, SAMPLE_BUFFER_SIZE);
HardwareCtrl Fader2(&adc1, 1, SAMPLE_BUFFER_SIZE);
HardwareCtrl Fader3(&adc1, 2, SAMPLE_BUFFER_SIZE);
HardwareCtrl Fader4(&adc1, 3, SAMPLE_BUFFER_SIZE);
HardwareCtrl Fader5(&adc1, 4, SAMPLE_BUFFER_SIZE);
HardwareCtrl Fader6(&adc1, 5, SAMPLE_BUFFER_SIZE);
HardwareCtrl Fader7(&adc1, 6, SAMPLE_BUFFER_SIZE);
HardwareCtrl Fader8(&adc1, 7, SAMPLE_BUFFER_SIZE);
*/

HardwareCtrl Fader1(&adc0, 0, SAMPLE_BUFFER_SIZE);
HardwareCtrl Fader2(&adc0, 1, SAMPLE_BUFFER_SIZE);
HardwareCtrl Fader3(&adc0, 2, SAMPLE_BUFFER_SIZE);
HardwareCtrl Fader4(&adc0, 3, SAMPLE_BUFFER_SIZE);
// HardwareCtrl Fader5(&adc0, 0, SAMPLE_BUFFER_SIZE);
// HardwareCtrl Fader6(&adc0, 1, SAMPLE_BUFFER_SIZE);
// HardwareCtrl Fader7(&adc0, 2, SAMPLE_BUFFER_SIZE);
// HardwareCtrl Fader8(&adc0, 3, SAMPLE_BUFFER_SIZE);

HardwareCtrl* pFaders[NUM_FADERS] =
{
  &Fader1, &Fader2, &Fader3, &Fader4//, &Fader5, &Fader6, &Fader7, &Fader8
};

////////////////////////////////////////////////////////////////
//                  VARIABLES AND SUCH
////////////////////////////////////////////////////////////////

// Single-bit masks to isolate idividual gate statuses etc.
extern const byte GATE_MASKS[];

//////// INPUT GATES
volatile bool newGate_FLAG [NUM_GATES_IN] = {0, 0, 0, 0};
volatile bool gateIn_VAL   [NUM_GATES_IN] = {0, 0, 0, 0};
volatile bool gateIn_STATE [NUM_GATES_IN] = {0, 0, 0, 0};
volatile long dbTimeStamp  [NUM_GATES_IN] = {0, 0, 0, 0};
const uint8_t dbLockout(25);

//////// OUTPUT GATES
void hwInit();

////////////////////////////////////////////////////////////////
//                   TIMER INTERRRUPTS
////////////////////////////////////////////////////////////////
const uint16_t uS_TO_mS(1000);
#ifndef RLR_DEBUG // Use tact switches w/pullups instead of gate inputs
#define RLR_DEBUG 1
#endif

volatile long rightNOW_micros(0);

#define ONE_KHZ_MICROS uS_TO_mS // 1000 uS for 1khz timer cycle for encoder
hw_timer_t *timer1 = NULL;      // Timer library takes care of telling this where to point

MagicButton modeButtonA(GATEin_PINS[0]);
MagicButton modeButtonB(GATEin_PINS[1]);
MagicButton runButton  (GATEin_PINS[2]);
MagicButton auxButton  (GATEin_PINS[3]);

// ISR for Timer 1
int8_t acc(0);
void ICACHE_RAM_ATTR onTimer1()
{  
  for (uint8_t fd = 0; fd < 4; ++fd)
  {
    pFaders[fd]->service();
  }
  if (acc == 0)
  {
    cli();
    rightNOW_micros = micros();
    sei();
    ++acc;
  }
  else
  {
    ++acc;
    acc %= 10;
    return;
  }


      encoder.service();
  modeButtonA.service();
  modeButtonB.service();
    runButton.service();
    auxButton.service();
  
#ifdef RLR_DEBUG
  // Just debounce stuff
  for (int gg = 1; gg < NUM_GATES_IN; ++gg)
  {
    cli();
    if ((rightNOW_micros - dbTimeStamp[gg]) < dbLockout * uS_TO_mS)
    {
      sei();
      continue;
    }
    sei();

    cli();
    gateIn_VAL[gg] = !digitalRead(GATEin_PINS[gg]);
    if (gateIn_VAL[gg] == gateIn_STATE[gg])
    {
      sei();
      continue;
    }
    sei();

    cli();
    if (newGate_FLAG[gg] == false)
    {
      gateIn_STATE[gg] = gateIn_VAL[gg];

      if (gateIn_STATE[gg])
      {
        newGate_FLAG[gg] = true;
      }
    }

    dbTimeStamp[gg] = rightNOW_micros;
    sei();
  }
#else  // IF RLR_DEBUG NOT DEFINED
  for (int gg = 0; gg < NUM_GATES_IN; ++gg)
  {
    gateIn_VAL[gg] = digitalRead(GATEin_PINS[gg]);
  }
#endif // RLR_DEBUG DEFINED
}


int16_t   ZERO(0);
int16_t * VirtualCtrl::sharedZERO(&ZERO);
int16_t * pPTR_HI(&(TheSeq.maxSteps));
int16_t   HI_IDX(TheSeq.maxSteps-1);

size_t  NUM_DIVS(15);
int16_t CLK_DIVS[] = { 64, 32, 16, 12, 8, 7, 6, 5, 4, 3, 2, 1, -2, -3, -4, -6, -8 };

// START POINT
VirtualCtrl start_TEST[4] = {
  VirtualCtrl(pFaders[0], 0, pPTR_HI),
  VirtualCtrl(pFaders[0], 0, pPTR_HI),
  VirtualCtrl(pFaders[0], 0, pPTR_HI),
  VirtualCtrl(pFaders[0], 0, pPTR_HI)
};
// END POINT
VirtualCtrl endpt_TEST[4] = {
  VirtualCtrl(pFaders[1], HI_IDX, pPTR_HI),
  VirtualCtrl(pFaders[1], HI_IDX, pPTR_HI),
  VirtualCtrl(pFaders[1], HI_IDX, pPTR_HI),
  VirtualCtrl(pFaders[1], HI_IDX, pPTR_HI)
};
// CLOCK DIVIDE
ArrayCtrl clkdv_TEST[4] = {
  ArrayCtrl(pFaders[2], CLK_DIVS, NUM_DIVS, 11),
  ArrayCtrl(pFaders[2], CLK_DIVS, NUM_DIVS, 11),
  ArrayCtrl(pFaders[2], CLK_DIVS, NUM_DIVS, 11),
  ArrayCtrl(pFaders[2], CLK_DIVS, NUM_DIVS, 11)
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

ButtonState bnState[] { ButtonState::Open, ButtonState::Open };

#include "bitHelpers.h"

// TODO: clean up all these magic numbers once we've got the total HW configuration established
bool initialized(0);
void ReadCtrls()
{
  if (!initialized)
  {
    initialized = true;
    for (uint8_t kn = 0; kn < 4; ++kn)
    {
      (pCV + kn)->select(selCH);
      Serial.printf("control %d: %p\n", kn, (pCV + kn)->pACTIVE);
    }
  }
  bnState[0] = modeButtonA.readAndFree();
  bnState[1] = modeButtonB.readAndFree();
  ButtonState bn2(runButton.readAndFree());

  if (bn2 == ButtonState::DoubleClicked)
  {
    TheSeq.setCmd(seqCmds::resetPendingCmd);
  }
  else if (bn2 == ButtonState::Clicked)
  {
    TheSeq.setCmd(seqCmds::runCmd);
  }

  switch(bnState[0])
  {
    case ButtonState::Clicked:
    {
      ++selCH;
      selCH %= 4;
      Serial.printf("------ select track %d ------\n", selCH);
      for (uint8_t kn = 0; kn < 4; ++kn)
      {
        (pCV + kn)->select(selCH);
        Serial.printf("control %d: %p\n", kn, (pCV + kn)->pACTIVE);
      }
      Serial.printf("----------------------------\n");
      break;
    }
    case ButtonState::DoubleClicked:
    {
      for (uint8_t ch = 0; ch < 4; ++ch)
      {
        Serial.printf("------- track %d -------\n", ch);
        Serial.printf("STPT control : %p\n", (pCV    )->ctrlOBJECTS[ch]);
        Serial.printf("NDPT control : %p\n", (pCV + 1)->ctrlOBJECTS[ch]);
        Serial.printf("CLDV control : %p\n", (pCV + 2)->ctrlOBJECTS[ch]);
        Serial.printf("CSKW control : %p\n", (pCV + 3)->ctrlOBJECTS[ch]);


        for (uint8_t kn = 0; kn < 4; ++kn)
        {
        }
        Serial.printf("-----------------------\n\n");
      }
      break;
    }
    default:
    {
      break;
    }
  }
  
  TheSeq.updateTrkBank(pCV, selCH);
}


// When menu is suspended
result idle(menuOut &o, idleEvent e)
{
  return proceed; // Not much goin' on here, TBH
}


void setup()
{
  //////// TIMER 1 SETUP ////////
  //
  Serial.begin(115200);

  while (!Serial)
  {
    ;
  }

  Serial.println(__FILE__);

  // Start up the OLED display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.clearDisplay();
  display.display();

  adc0.begin(ADC0_CS); // Chip select pin.
  adc0.setSPIspeed(4000000);
  // Set up the menu system
  nav.idleTask = idle;       // Function to ping when menu is suspended
  nav.idleOn();              // Start up in idle state
  nav.navRoot::timeOut = 30; // Inactivity timeout
  nav.navRoot::useUpdateEvent = true;

  hwInit();

  timer1 = timerBegin(1, 80, true);
  timerAttachInterrupt(timer1, &onTimer1, true);

  timerAlarmWrite(timer1, ONE_KHZ_MICROS/10, true);
  timerAlarmEnable(timer1);

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
  Serial.printf("ADC 0 initialized...\n");
  delay(20);
  TheSeq.setCmd(seqCmds::resetNowCmd);
  for(int mode = 0; mode <= 4; ++mode)
  {
    Serial.printf("mode: %d\n", mode % 4);
    for (int trk = 0; trk < 4; ++trk)
    {
      // cvModeKnob[trk].select(mode % 4);
      Serial.printf("st %d: %d\n",trk,LOOP.START[trk]->Q);
      Serial.printf("nd %d: %d\n",trk,LOOP.END[trk]->Q);
      Serial.printf("ps %d: %d\n",trk,LOOP.STEPS[trk]->Q);
    }
  }

  // ////////  TIMER 0 SETUP ////////
  // //
  // // Set 80 divider for prescaler (see ESP32 Technical Reference Manual for more info).
  // timer0 = timerBegin(0, 80, true);
  // timerAttachInterrupt(timer0, &onTimer0, true);

  // // Param 2: interval (uS); Param 3: recurring?
  // timerAlarmWrite(timer0, DAC_TIMER_MICROS, true);
  // timerAlarmEnable(timer0);

  Serial.println("\nDone...");
}

////////////////////////////////////////////////////////////////
//  LOOP
bool resetPending(0);
bool forceReset(0);
uint16_t rd[] = {0, 0, 0, 0};
void loop()
{
  ReadCtrls();


  // If user-selected reset, wait for a gate event to actually do it
  if (resetPending)
  {
    bool newGate(0);
    for (uint8_t gg = 0; gg < NUM_GATES_IN; ++gg)
    {
      cli();
      newGate |= newGate_FLAG[gg];
      sei();
      if (newGate)
      {
        resetPending = false;
        forceReset = true;
        display.setTextColor(WHITE); // TODO: reset function for main loop
        break;
      }
    }
  }

  // SW decided it was a good idea to reset RIGHT NOW and doesn't care what you think
  if (forceReset)
  {
    TheSeq.setCmd(seqCmds::resetNowCmd);
    TheClock.reset();
    resetPending = 0;
    forceReset = 0;
  }

  // Here's where we read our gates to see if we should pulse our clock dividers
  // for (uint8_t gg = 0; gg < NUM_GATES_IN; ++gg)
  // {
  // bool resetReq;
  // bool runReq;
  // cli();
  // resetReq = newGate_FLAG[3];
  // runReq   = newGate_FLAG[2];
  // newGate_FLAG[3] = 0;
  // newGate_FLAG[2] = 0;
  // sei();

  // if (!flag)
  // {
  //   continue;
  // }

  // cli();
  // newGate_FLAG[gg] = 0;
  // sei();
  // if (gg == 0)
  //   TheSeq.setCmd(seqCmds::clockInCmd);
  // else if (gg == 1)
  //   TheSeq.setCmd(seqCmds::mod1Cmd);
  // else if (gg == 2)
  //   TheSeq.setCmd(seqCmds::mod2Cmd);
  // if (runReq)
  //   TheSeq.setCmd(seqCmds::runCmd);
  // if (resetReq)
  //   TheSeq.setCmd(seqCmds::resetPendingCmd);
  // }

  // Read internal clock and pulse clock dividers
  if (TheClock.flagReady())
  {
    TheSeq.setCmd(seqCmds::pulseIntlCmd);
  }

  // Check for new events from our encoder and clear the "ready" flag if there are
  encoderWrapper.read();

  // Store state of nav object - this is a hack so we can tell if we've exited the menu
  // by checking if "lastChanged" is the same. This tells us we need to clear the display.
  // There's probably a better way to do this directly using the API, but it works.
  long lastChanged(nav.lastChanged);

  nav.doInput();
  if (nav.changed(0))
  {
    nav.doOutput();
    display.display();

    if (lastChanged == nav.lastChanged)
    {
      display.clearDisplay();
      display.display();
    }
  }
}

////////////////////////////////////////////////////////////////
//  HW INIT
void hwInit()
{
  // Set up I/O pins
  for (int8_t gg = 0; gg < NUM_GATES_IN; ++gg)
  {
    pinMode(GATEin_PINS[gg], INPUT);
  }

  for (int8_t gg = 0; gg < 4; ++gg)
  {
    pinMode(GATEout_PINS[gg], OUTPUT);
  }

  int8_t jj(0);
  bool up(1);
  for (int8_t gg = 0; gg < NUM_GATES_OUT; ++gg)
  {
    digitalWrite(GATEout_PINS[gg], LOW);
  }

  // Blinkenlichts
  for (int8_t ii = 0; ii < 19; ++ii)
  {
    digitalWrite(GATEout_PINS[jj], HIGH);
    delay(30);
    digitalWrite(GATEout_PINS[jj], LOW);
    if (jj == 3)
      up = false;
    else if (jj == 0)
      up = true;
    jj += up ? 1 : -1;
  }

  for (int8_t gg = 0; gg < NUM_GATES_OUT; ++gg)
  {
    digitalWrite(GATEout_PINS[gg], LOW);
  }

  randomSeed(micros());
  // // Set up SPI pins for soft SPI
  // pinMode(S_C_L_K, OUTPUT);
  // pinMode(SDO, OUTPUT);

  // pinMode(ADC0_CS,  OUTPUT);

  // pinMode(DAC0_CS, OUTPUT);
  // pinMode(DAC1_CS, OUTPUT);

  // digitalWrite(DAC0_CS, HIGH); // deassert chip selects
  // digitalWrite(DAC1_CS, HIGH);
  // digitalWrite(ADC0_CS,  HIGH);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
const uint8_t NUM_TRAX(4);