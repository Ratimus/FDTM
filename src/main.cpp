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
//
//              HARDWARE LIBRARY STUFF
//
////////////////////////////////////////////////////////////

#include <Arduino.h>
#include <pgmspace.h>
#include "Rhythms.h"
#include "FDTM_IO.h"
// #include "Outputs.h"
#include "CtrlPanel.h"
#include "LoopSeq.h"
#include "ClockObject.h"
#include "ClockDivider.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ClickEncoder.h>
#include <ClickEncoderInterface.h>
#include <RatRotaryEvent.h>
#include <EncoderWrapper.h>
#include "MCP_ADC.h"
#include "SharedCtrl.h"


////////////////////////////////////////////////////////////////
//                      SW COMPONENTS
////////////////////////////////////////////////////////////////

LoopSeq TheSeq;
TrkPts LOOP(TheSeq.maxSteps);

ClockObject TheClock(128, 24);

ClockDivider DIV_A(0, 'A', TheClock.getPPQN(), ClockSource::INTERNAL_CLK, NULL,   NULL);
ClockDivider DIV_B(1, 'B', TheClock.getPPQN(), ClockSource::INTERNAL_CLK, &DIV_A, NULL);
ClockDivider DIV_C(2, 'C', TheClock.getPPQN(), ClockSource::CLONE_A,      &DIV_A, &DIV_B);
ClockDivider DIV_D(3, 'D', TheClock.getPPQN(), ClockSource::CLONE_B,      &DIV_A, &DIV_B);


////////////////////////////////////////////////////////////////
//                          OLED
////////////////////////////////////////////////////////////////

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET -1    // Unused but has to be defined for library

// SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void initDisplay()
{
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.clearDisplay();
  display.display();
}


////////////////////////////////////////////////////////////////
//                      CLICK ENCODER
////////////////////////////////////////////////////////////////

// The lower-level HW driver
const uint8_t STEPS_PER_NOTCH(2);
const bool    ACTIVE_LOW(true);

ClickEncoder encoder(ENC_A, ENC_B, ENC_SW, STEPS_PER_NOTCH, ACTIVE_LOW);

// Wrapper for the encoder - gives us nice, pretty events to read
const uint8_t SENSITIVITY(2);
ClickEncoderInterface encoderInterface(encoder, SENSITIVITY);

// Source of "nav events" for ArduinoMenu Library
RatRotaryEvent Rotareeeee(
    RatRotaryEvent::EventType::BUTTON_CLICKED |        // select
    RatRotaryEvent::EventType::BUTTON_DOUBLE_CLICKED | // back
    RatRotaryEvent::EventType::BUTTON_LONG_PRESSED |   // also back
    RatRotaryEvent::EventType::ROTARY_CCW |            // up
    RatRotaryEvent::EventType::ROTARY_CW               // down
);

Menu::EncoderWrapper encoderWrapper(encoderInterface, Rotareeeee);


////////////////////////////////////////////////////////////////
//                      MENU SYSTEM
////////////////////////////////////////////////////////////////
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


//////// INPUT GATES
volatile bool newGate_FLAG [NUM_GATES_IN] = {0, 0, 0, 0};
volatile bool gateIn_VAL   [NUM_GATES_IN] = {0, 0, 0, 0};
volatile bool gateIn_STATE [NUM_GATES_IN] = {0, 0, 0, 0};
volatile long dbTimeStamp  [NUM_GATES_IN] = {0, 0, 0, 0};
const uint8_t dbLockout(25);

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

int8_t acc(0);
////////////////////////////garbage above
// ISR for Timer 1
void ICACHE_RAM_ATTR onTimer1()
{
 // static int8_t acc(0);

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

  if (bnState[1] == ButtonState::DoubleClicked)
  {
    TheSeq.setCmd(seqCmds::mod2Cmd);
  }

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

  while (!Serial) { ; }

  Serial.println(__FILE__);

  initDisplay();
  initADC();

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

  initFaders();

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

  // // Read internal clock and pulse clock dividers
  // if (TheClock.flagReady())
  // {
  //   TheSeq.setCmd(seqCmds::pulseIntlCmd);
  // }

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



//////////////////////garbage above

/*
// When menu is suspended
result idle(menuOut &o, idleEvent e)
{
  return proceed; // Not much goin' on here, TBH
}

void initNav()
{
  // Set up the menu system
  nav.idleTask = idle;       // Function to ping when menu is suspended
  nav.idleOn();              // Start up in idle state
  nav.navRoot::timeOut = 30; // Inactivity timeout
  nav.navRoot::useUpdateEvent = true;
}

void setup()
{
//  Serial.begin(115200);
//
//  while (!Serial)
//  { ; }
//
//  Serial.println(__FILE__);
//
//  // Start up the OLED display
//  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
//  display.setTextSize(1);
//  display.setTextColor(WHITE);
//  display.setCursor(0, 0);
//  display.clearDisplay();
//  display.display();
//
//  initADCs();
//
  Serial.begin(115200);
  while (!Serial)
  {
    ;
  }
  Serial.println(__FILE__);
  initDisplay();
  Serial.println("Display ready...");
  initADCs();
  Serial.println("ADCs initialized...");
  initNav();
  initDigitalInputs();
  initDigitalOutputs();
  blinkenLichts();
  initTimer1();
  Serial.println("Timer1 initialized...");
  startTimer1();
  Serial.println("Timer1 running");
  startADCs();
  Serial.println("ADCs running");


  for (int mode = 0; mode <= 4; ++mode)
  {
    Serial.printf("mode: %d\n", mode % 4);
    for (int trk = 0; trk < 4; ++trk)
    {
      // cvModeKnob[trk].select(mode % 4);
      Serial.printf("st %d: %d\n", trk, LOOP.START[trk]->Q);
      Serial.printf("nd %d: %d\n", trk, LOOP.END[trk]->Q);
      Serial.printf("ps %d: %d\n", trk, LOOP.STEPS[trk]->Q);
    }
  }

  initSeq();

  // pinMode(DAC0_CS, OUTPUT);
  // pinMode(DAC1_CS, OUTPUT);

  // digitalWrite(DAC0_CS, HIGH); // deassert chip selects
  // digitalWrite(DAC1_CS, HIGH);

  Serial.println("\nSetup complete...");
}

void DoMenuStuff()
{
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
//  LOOP
void loop()
{
  DoInputStuff();
  DoMenuStuff();
}

////////////////////////////////////////////////////////////////
//  HW INIT
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////
const uint8_t NUM_TRAX(4);