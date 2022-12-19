// ------------------------------------------------------------------------
// menusystem.cpp
//
// Nov. 2022
// Ryan "Ratimus" Richardson
// ------------------------------------------------------------------------
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 
// See http://creativecommons.org/licenses/MIT/ for more information.
//
// -----------------------------------------------------------------------------
//
// definitions for Arduino Menu System
// https://github.com/neu-rah/ArduinoMenu/tree/master/src/menuIO

#ifndef INCLUSION_GUARD_FOR_MENUSYSTEM_H
#define INCLUSION_GUARD_FOR_MENUSYSTEM_H
#include <Arduino.h>
#include "FDTM_IO.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//#include "CtrlPanel.h"
#include "SeqAlgo.h"
#include "LoopSeq.h"
#include "SeqPattern.h"
#include "ClockObject.h"
#include "Rhythms.h"
#include <EncoderWrapper.h>
#include <RatRotaryEvent.h>
// #include "SharedCtrl.h"

extern Menu::EncoderWrapper encoderWrapper;
#include "ClockDivider.h"
extern ClockDivider DIV_A;
extern ClockDivider DIV_B;
extern ClockDivider DIV_C;
extern ClockDivider DIV_D;
// extern const int8_t ELASTIC_MODE;
// extern const int8_t WINDOW_MODE;

#include <menu.h>
#include <menuIO/keyIn.h>
#include <menuIO/chainStream.h>
#include <menuIO/serialOut.h>
#include <menuIO/adafruitGfxOut.h>
#include <menuIO/serialIn.h>

#define gfxWidth 128
#define gfxHeight 32
#define fontX 6          // font width in pixels
#define fontY 10         // vertical text spacing
#define charRows gfxHeight/fontY
#define charCols gfxWidth/fontX
#define MAX_DEPTH 5      // max menu depth
#define textScale 1
#define NUM_SAMPLES 5
#define OLED_RESET -1    // Unused port - to keep the compiler happy

extern Adafruit_SSD1306 display;


///////////////////////////////////////////////////////////////////////////////
// MENU SYSTEM UTILITIES
///////////////////////////////////////////////////////////////////////////////
void showPath(navRoot& root)
{
  Serial.print("nav level:");
  Serial.print(root.level);
  Serial.print(" path:[");
  for(int n=0;n<=root.level;n++)
  {
    Serial.print(n?",":"");
    Serial.print(root.path[n].sel);
  }
  Serial.println("]");
}

result snoop(eventMask e,navNode& nav,prompt& item)
{
  Serial.println();
  Serial.println("========");
  Serial.print("Event for target: 0x");
  Serial.println((long)nav.target,HEX);
  showPath(*nav.root);
  Serial.print(e);
  Serial.printf("\nitem: %s\n",item.getText());
  switch(e) {
    case noEvent://just ignore all stuff
      Serial.println(" noEvent");break;
    case activateEvent://this item is about to be active (system event)
      Serial.println(" activateEvent");break;
    case enterEvent://entering navigation level (this menu is now active)
      Serial.println(" enterEvent");break;
    case exitEvent://leaving navigation level
      Serial.println(" exitEvent");break;
    case returnEvent://TODO:entering previous level (return)
      Serial.println(" returnEvent");break;
    case focusEvent://element just gained focus
      Serial.println(" focusEvent");break;
    case blurEvent://element about to lose focus
      Serial.println(" blurEvent");break;
    case selFocusEvent://TODO:child just gained focus
      Serial.println(" selFocusEvent");break;
    case selBlurEvent://TODO:child about to lose focus
      Serial.println(" selBlurEvent");break;
    case updateEvent://Field value has been updated
      Serial.println(" updateEvent");break;
    case anyEvent:
      Serial.println(" anyEvent");break;
  }
  return proceed;
}


///////////////////////////////////////////////////////////////////////////////////////
// LOOP POINTS
///////////////////////////////////////////////////////////////////////////////////////
extern TrkPts LOOP;
MENU(indexMenu," Loop Points",doNothing,noEvent,noStyle,
  FIELD(LOOP.S1.D, " 1 Start...", "",0,31,1,0,doNothing,noEvent,wrapStyle),
  FIELD(LOOP.E1.D, " 1 End.....", "",0,31,1,0,doNothing,noEvent,wrapStyle),
  FIELD(LOOP.P1.D, " 1 Length..", "",0,31,1,0,doNothing,noEvent,wrapStyle),
  FIELD(LOOP.S2.D, " 2 Start...", "",0,31,1,0,doNothing,noEvent,wrapStyle),
  FIELD(LOOP.E2.D, " 2 End.....", "",0,31,1,0,doNothing,noEvent,wrapStyle),
  FIELD(LOOP.P2.D, " 2 Length..", "",0,31,1,0,doNothing,noEvent,wrapStyle),
  FIELD(LOOP.S3.D, " 3 Start...", "",0,31,1,0,doNothing,noEvent,wrapStyle),
  FIELD(LOOP.E3.D, " 3 End.....", "",0,31,1,0,doNothing,noEvent,wrapStyle),
  FIELD(LOOP.P3.D, " 3 Length..", "",0,31,1,0,doNothing,noEvent,wrapStyle),
  FIELD(LOOP.S4.D, " 4 Start...", "",0,31,1,0,doNothing,noEvent,wrapStyle),
  FIELD(LOOP.E4.D, " 4 End.....", "",0,31,1,0,doNothing,noEvent,wrapStyle),
  FIELD(LOOP.P4.D, " 4 Length..", "",0,31,1,0,doNothing,noEvent,wrapStyle),
  EXIT("< Back"));


///////////////////////////////////////////////////////////////////////////////////////
// CLOCK SOURCES
///////////////////////////////////////////////////////////////////////////////////////
TOGGLE(TheSeq.running,seqRunPause_Submenu," Internal Clk",doNothing,noEvent,noStyle,
  VALUE("    Run",1,doNothing,updateEvent),
  VALUE("  Pause",0,doNothing,updateEvent));

SELECT(DIV_A.SOURCE.D,subMenu_aCLKSRC," A Clock",doNothing,noEvent,noStyle,
 VALUE("  Ext Clk",EXT_CLOCK,      doNothing, noEvent),
 VALUE("   Mod. 1",MOD1_GATEIN,    doNothing, noEvent),
 VALUE("   Mod. 2",MOD2_GATEIN,    doNothing, noEvent),
 VALUE(" Intl Clk",INTERNAL_CLK,   doNothing, noEvent));
SELECT(DIV_B.SOURCE.D,subMenu_bCLKSRC," B Clock",doNothing,noEvent,noStyle,
 VALUE("  Ext Clk",EXT_CLOCK,      doNothing, noEvent),
 VALUE("   Mod. 1",MOD1_GATEIN,    doNothing, noEvent),
 VALUE("   Mod. 2",MOD2_GATEIN,    doNothing, noEvent),
 VALUE(" Use A in",CH_A_INPUT,     doNothing, noEvent),
 VALUE("Use A out",CH_A_OUTPUT,    doNothing, noEvent),
 VALUE("  Clone A",CLONE_A,        doNothing, noEvent),
 VALUE(" Intl Clk",INTERNAL_CLK,   doNothing, noEvent));
SELECT(DIV_C.SOURCE.D,subMenu_cCLKSRC," C Clock",doNothing,noEvent,noStyle,
 VALUE("  Ext Clk",EXT_CLOCK,      doNothing, noEvent),
 VALUE("   Mod. 1",MOD1_GATEIN,    doNothing, noEvent),
 VALUE("   Mod. 2",MOD2_GATEIN,    doNothing, noEvent),
 VALUE(" Use A in",CH_A_INPUT,     doNothing, noEvent),
 VALUE("Use A out",CH_A_OUTPUT,    doNothing, noEvent),
 VALUE("  Clone A",CLONE_A,        doNothing, noEvent),
 VALUE(" Use B in",CH_B_INPUT,     doNothing, noEvent),
 VALUE("Use B out",CH_B_OUTPUT,    doNothing, noEvent),
 VALUE("  Clone B",CLONE_B,        doNothing, noEvent),
 VALUE(" Intl Clk",INTERNAL_CLK,   doNothing, noEvent));
SELECT(DIV_D.SOURCE.D,subMenu_dCLKSRC," D Clock",doNothing,noEvent,noStyle,
 VALUE("  Ext Clk",EXT_CLOCK,      doNothing, noEvent),
 VALUE("   Mod. 1",MOD1_GATEIN,    doNothing, noEvent),
 VALUE("   Mod. 2",MOD2_GATEIN,    doNothing, noEvent),
 VALUE(" Use A in",CH_A_INPUT,     doNothing, noEvent),
 VALUE("Use A out",CH_A_OUTPUT,    doNothing, noEvent),
 VALUE("  Clone A",CLONE_A,        doNothing, noEvent),
 VALUE(" Use B in",CH_B_INPUT,     doNothing, noEvent),
 VALUE("  Clone B",CLONE_B,        doNothing, noEvent),
 VALUE(" Intl Clk",INTERNAL_CLK,   doNothing, noEvent));

MENU(clkSrcMenu, " Clock Srcs",doNothing,noEvent,noStyle,
  SUBMENU(subMenu_aCLKSRC),
  SUBMENU(subMenu_bCLKSRC),
  SUBMENU(subMenu_cCLKSRC),
  SUBMENU(subMenu_dCLKSRC),
  EXIT("< Back"));


///////////////////////////////////////////////////////////////////////////////////////
// CLOCK DIVIDERS
///////////////////////////////////////////////////////////////////////////////////////
#define CLOCK_DIVS\
 VALUE("1/64",64,doNothing,noEvent),\
 VALUE("1/32",32,doNothing,noEvent),\
 VALUE("1/16",16,doNothing,noEvent),\
 VALUE("1/8",8,doNothing,noEvent),\
 VALUE("1/7",7,doNothing,noEvent),\
 VALUE("1/6",6,doNothing,noEvent),\
 VALUE("1/5",5,doNothing,noEvent),\
 VALUE("1/4",4,doNothing,noEvent),\
 VALUE("1/3",3,doNothing,noEvent),\
 VALUE("1/2",2,doNothing,noEvent),\
 VALUE("1",  1,doNothing,noEvent),\
 VALUE("x2",-2,doNothing,noEvent),\
 VALUE("x3",-3,doNothing,noEvent),\
 VALUE("x4",-4,doNothing,noEvent),\
 VALUE("x6",-6,doNothing,noEvent),\
 VALUE("x8",-8,doNothing,noEvent)

SELECT(DIV_A.DIVISION.D,divA_submenu," Div. A",doNothing,noEvent,noStyle,CLOCK_DIVS);
SELECT(DIV_B.DIVISION.D,divB_submenu," Div. B",doNothing,noEvent,noStyle,CLOCK_DIVS);
SELECT(DIV_C.DIVISION.D,divC_submenu," Div. C",doNothing,noEvent,noStyle,CLOCK_DIVS);
SELECT(DIV_D.DIVISION.D,divD_submenu," Div. D",doNothing,noEvent,noStyle,CLOCK_DIVS);
MENU(clkDivMenu, " Clock Divs",doNothing,noEvent,noStyle,
  SUBMENU(divA_submenu),
  SUBMENU(divB_submenu),
  SUBMENU(divC_submenu),
  SUBMENU(divD_submenu),
  EXIT("< Back"));


///////////////////////////////////////////////////////////////////////////////////////
// CLOCK DELAYS/OFFSETS
///////////////////////////////////////////////////////////////////////////////////////
MENU(clkDlyMenu,   " Clock Delays",doNothing,noEvent,noStyle,
  FIELD(DIV_A.SKEW.D, " A Dly","", 0,7,1,0,doNothing,noEvent,noStyle),
  FIELD(DIV_B.SKEW.D, " B Dly","", 0,7,1,0,doNothing,noEvent,noStyle),
  FIELD(DIV_C.SKEW.D, " C Dly","", 0,7,1,0,doNothing,noEvent,noStyle),
  FIELD(DIV_D.SKEW.D, " D Dly","", 0,7,1,0,doNothing,noEvent,noStyle),
  EXIT("< Back"));


///////////////////////////////////////////////////////////////////////////////////////
// CLOCK SETTINGS SUBMENU
///////////////////////////////////////////////////////////////////////////////////////
MENU(clockSubMenu," Clock Settings",doNothing,noEvent,noStyle,
  SUBMENU(seqRunPause_Submenu),
  SUBMENU(clkSrcMenu),
  SUBMENU(clkDivMenu),
  SUBMENU(clkDlyMenu),
  EXIT("< Back"));


///////////////////////////////////////////////////////////////////////////////////////
// SEQUENCER STEPS PER CLOCK
///////////////////////////////////////////////////////////////////////////////////////
MENU(seqTrackMenu, " Steps per Clock",doNothing,noEvent,noStyle,
  FIELD(LOOP.J1.D,  " Trk1   ", "", -4,4,1,0,doNothing,noEvent,noStyle),
  FIELD(LOOP.J2.D,  " Trk2   ", "", -4,4,1,0,doNothing,noEvent,noStyle),
  FIELD(LOOP.J3.D,  " Trk3   ", "", -4,4,1,0,doNothing,noEvent,noStyle),
  FIELD(LOOP.J4.D,  " Trk4   ", "", -4,4,1,0,doNothing,noEvent,noStyle),
  EXIT("< Back"));


///////////////////////////////////////////////////////////////////////////////////////
// LOOP POINT MODE
///////////////////////////////////////////////////////////////////////////////////////
TOGGLE(TheSeq.MODE.D, seqMode_Submenu," Loop Mode",doNothing,noEvent,noStyle,
  VALUE("   Elastic",ELASTIC_MODE,doNothing,updateEvent),
  VALUE("    Window",WINDOW_MODE, doNothing,updateEvent));


result patternSel(FUNC_PARAMS)
{
  TheSeq.validatePattns();
  return proceed;
}

#define KNOBULATE updateEvent|focusEvent|selFocusEvent|blurEvent|selBlurEvent

int8_t lastPrime(NUM_WORLD_RHYTHMS-1);
MENU(mainMenu," --0- SETTINGS -----",doNothing,noEvent,noStyle,
  FIELD(TheSeq.PROB_KNOB,   " MYSTERY!!!   ","", 0,100,      1,0,doNothing,noEvent,noStyle),
  FIELD(TheSeq.pattnA, " Left  Pattern ","",-1,lastPrime,1,0,patternSel,KNOBULATE,wrapStyle),
  FIELD(TheSeq.pattnB, " Right Pattern ","",-1,lastPrime,1,0,patternSel,KNOBULATE,wrapStyle),
  SUBMENU(seqMode_Submenu),
  SUBMENU(indexMenu),     // Loop pts/offset  ^^Ditto
  SUBMENU(clockSubMenu),
  SUBMENU(seqTrackMenu),  // FWD/REV; #steps (Should be GUI-free)
  FIELD(TheClock.BPM.D," BPM","",  30,300,10,1,doNothing,noEvent,wrapStyle),
  FIELD(TheSeq.PW.D," Trigger length","",1,10,1,0,doNothing,noEvent,noStyle),
  EXIT("< Back"));


// define menu colors --------------------------------------------------------
//  {{disabled normal,disabled selected},{enabled normal,enabled selected, enabled editing}}
// Monochromatic color table
const colorDef<uint16_t> colors[] MEMMODE =
{
  {{WHITE,BLACK},{WHITE,BLACK,BLACK}}, // bgColor
  {{BLACK,WHITE},{BLACK,WHITE,WHITE}}, // fgColor
  {{BLACK,WHITE},{BLACK,WHITE,WHITE}}, // valColor
  {{BLACK,WHITE},{BLACK,WHITE,WHITE}}, // unitColor
  {{BLACK,WHITE},{WHITE,WHITE,WHITE}}, // cursorColor
  {{BLACK,WHITE},{WHITE,BLACK,BLACK}}, // titleColor
};

extern RatRotaryEvent Rotareeeee;

MENU_INPUTS(in,&Rotareeeee)


MENU_OUTPUTS(out,MAX_DEPTH
  ,ADAGFX_OUT(display,colors,fontX,fontY,{0,0,gfxWidth/fontX,gfxHeight/fontY})
  ,NONE                       // Must have 2 items at least if using this macro
)

NAVROOT(nav,mainMenu,MAX_DEPTH,in,out)


#endif
