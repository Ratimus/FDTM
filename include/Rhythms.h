// ------------------------------------------------------------------------
// Rhythm.h
//
// Nov. 2022
// Ryan "Ratimus" Richardson
// ------------------------------------------------------------------------
// Rhythm patterns transribed from User Manual for Noise Engineering's
// Zularic Repetitor trigger sequencer module for Eurorack. Patterns
// were interpreted as 32-Bit binary values, where a "1" represents a
// gate or trigger and a "0" represents a rest, and then stored here
// as arrays of hex values.
//
// The downbeat of each pattern falls on bit
// 31, so patterns of less than 32 steps are zero-padded on the end.
// The value at index 0 of each array is the number of steps in that
// pattern, and idx 1:4 contain the individual tracks for each pattern.
//
// The array {uint32_t * primes[]} stores pointers to each individual
// pattern.
// ------------------------------------------------------------------------
//   EXAMPLE CODE:
// 
// class TriggerSeq { ...
//    int8_t   loopLen[4];
//    uint32_t bitLane[4]; ... };
//
// const uint8_t NUM_RHYTHMS(30);
//
// void TriggerSeq :: setPrime(uint8_t idx)
// {  
//    idx = constrain(idx, 0, NUM_RHYTHMS);
// 
//    uint32_t * pPrime = primes[idx];
// 
//    // Get PRIME rhythm. First value is length, 1:4 are bitLanes 0:3
//    for(int8_t ch = 0; ch < 4; ++ch)
//    {
//       loopLen[ch] = pPrime[0];
//       bitLane[ch] = bitReverse(pPrime[ch+1]);
//    }
// }
// ------------------------------------------------------------------------
#ifndef Rhythms_h
#define Rhythms_h

#include <Arduino.h>

extern uint8_t NUM_WORLD_RHYTHMS;
extern uint8_t NUM_NW_RHYTHMS;
extern uint8_t NUM_OW_RHYTHMS;

extern uint32_t NW_00 [5]; // Motorik 1
extern uint32_t NW_01 [5]; // Motorik 2
extern uint32_t NW_02 [5]; // Motorik 3
extern uint32_t NW_03 [5]; // Pop 1
extern uint32_t NW_04 [5]; // Pop 2
extern uint32_t NW_05 [5]; // Pop 3
extern uint32_t NW_06 [5]; // Pop 4
extern uint32_t NW_07 [5]; // Funk 1
extern uint32_t NW_08 [5]; // Funk 2
extern uint32_t NW_09 [5]; // Funk 3
extern uint32_t NW_0A [5]; // Funk 4
extern uint32_t NW_0B [5]; // Post
extern uint32_t OW_0C [5]; // King 1
extern uint32_t OW_0D [5]; // King 2
extern uint32_t OW_0E [5]; // Kroboto
extern uint32_t OW_0F [5]; // Vodu 1
extern uint32_t OW_10 [5]; // Vodu 2
extern uint32_t OW_11 [5]; // Vodu 3
extern uint32_t OW_12 [5]; // Gahu
extern uint32_t OW_13 [5]; // Clave
extern uint32_t OW_14 [5]; // Rhumba
extern uint32_t OW_15 [5]; // Jhaptal 1
extern uint32_t OW_16 [5]; // Jhaptal 2
extern uint32_t OW_17 [5]; // Chachar
extern uint32_t OW_18 [5]; // Mata
extern uint32_t OW_19 [5]; // Pashta
extern uint32_t OB_1A [5]; // Prime 232
extern uint32_t OB_1B [5]; // Sequence
extern uint32_t OB_1C [5]; // Prime 2
extern uint32_t OB_1D [5]; // Prime 322
extern uint32_t TK_1E [5]; // Tekno
extern uint32_t TK_1F [5]; // Tekno

/////////////////////////////////////////////////
extern uint32_t * primes[32];


uint8_t pattnSteps(int8_t ptn);


///////////////////////////////////////////////////////////////
//
///////////// "NEW WORLD" /////////////

/*
NEW WORLD:

Motorik 1
X-XX------XX----X-XX------XX----
X---X---X---X---X-X-X----X--X-X-
----X-XX----X-XX----X-XX----X-XX
X---X---X---X---X---X---X---X---

Motorik 2
X-X-X-X-X-X-X-X-X-X-X-X-X-X-X-X-
----X-------X-------X-------X--X
X-X---X-X-X---X-X-X---X-X-X-----
X---X---X---X---X---X---X---X---

Motorik 3
XX------X-X-----XX------X-X-----
----X-------X-------X-------X-X-
X-XXX--XX-XXX--XX-XXX--XX-XXX--X
X-------X-X---X-X---------------

Pop 1
X---X---X---X---X---X---X---X-X-
--X--X--X-XX------X--X--X-XX--X-
----X-------X-------X-------X--X
X-X-X-X-X-X-X-X-X-X-X-X-X-X-X-XX

Pop 2
--X-X-XXX-X---XXX-X---XXX-X---X-
--X-X-XXX-X---XXX-X---XXX-X---X-
----X-------X-------X-------X---
----X-------X-------X-------X---

Pop 3
X-X-XXXXX-X---XXX-X---XXX-X-X---
X-X-XXXXX-X---XXX-X---XXX-X-X---
--------X-X-----X-X-----X-X---X-
--------X-X-----X-X-----X-X---X-

Pop 4
X-XX-XX-----XXXXX-XX-XX-----XXXX
X-XX-XX-----XXXXX-XX-XX-----XXXX
XXXXX-X---X-XXXXX-X---XXX-X-XX--

Funk 1
X--X-X--X--X-X--
--X---X---X---X-
-X-X-X-X-X-X-X-X
-X---X---X---XXX

Funk 2
--X---X---X---X-
X-XX-XXXX-X--XXX
-X-X-X-X-X-X-X-X
-X---X---X---XXX

Funk 3
X-XX-XXXX-X--XXX
X--XX--XX--XX-X-
-X-X-X-X-X-X-X-X
-X---X---X---XXX

Funk 4
X--XX--XX--XX-X-
X--X-X--X--X-X--
-X-X-X-X-X-X-X-X
-X---X---X---XXX

Post
X--X-X-XXX--X-X-X-X-
X--------X----------
---X--------X-X-X-X-
X---X---X---X---X---


OLD WORLD:

King 1
X-X-XX-X-X-X
X-X-XX-X-X-X
X-XX-X--XX--
X-XX-X--XX--

King 2
X-XX-X--XX--
X-XX-X---X--
X-X-XX-X-X-X
X-X-XX-X-X-X

Kroboto
--X-XX--X-XX
--X-XX--X-XX
X-----X--X--
X-X-XX-X-X-X

Vodu 1
X-X-X-XX-X-X
X-X-X-XX-X-X
X-----X--X--
----XX----XX

Vodu 2
-XX-XX-XX-XX
-XX-XX-XX-XX
X-X-X-XX-X-X
----XX----XX

Vodu 3
X-----X--X--
X-----X--X--
-XX-XX-XX-XX
----XX----XX

Gahu
XX-X-X-X-X-X-X--
XX-X-X-X-X-X-X--
X---------------
X--X---X---X-X--

Clave
X--X--X---X-X---
X--X--X---X-X---
X-XX-X-XX-X-XX-X
--XX--XX--X---XX

Rhumba
X--X---X--X-X---
X--X---X--X-X---
X-XX-X-XX-X-XX-X
--XX--XX--X---XX

Jhaptal 1
-X--XXX--X
-X--XXX--X
X-XX---XX-
X----X----

Jhaptal 2
X---------
X-XX---XX-
X-XX---XX-
X----X----

Chachar
X---------------X---------------
X-------X-X-----X-------X-X-----
----X-------X-------X-------X---
------------------------------XX

Mata
X--------------X--
X---X-X-----X--X--
X---X-X-----XX-X-X
--------X-----X--X

Pashta
{forgot to transcribe this one,
 but it's OW_19}

 
Prime 232
X-----------
X-----X-----
X-X-X-X-X-X-
XXXXXXXXXXXX


///////////////////////////////////////////////////////////////

Sequence
X---X---X---X---
-X---X---X---X--
--X---X---X---X-
---X---X---X---X

Prime 2
X-------
X---X---
X-X-X-X-
XXXXXXXX

Prime 322
X-----------
X-----X-----
X--X--X--X--
XXXXXXXXXXXX

RANDOM
clk*(.25P)
clk*(cv1*P)
clk*(cv2*P)
clk*(cv3*P)

DIVIDE
clk/4
clk/(map(cv1,1,32))
clk/(map(cv2,1,32))
clk/(map(cv3,1,32))

NEW RAT RHYTHMS:





*/
#endif