#include "Rhythms.h"

// ------------------------------------------------------------------------
// Rhythm.cpp
//
// Nov. 2022
// Ryan "Ratimus" Richardson
// ------------------------------------------------------------------------

uint8_t NUM_WORLD_RHYTHMS(0x20);
uint8_t NUM_NW_RHYTHMS(0x0B);
uint8_t NUM_OW_RHYTHMS(0x0E);

// Motorik 1
uint32_t NW_00 [5] = { 32,  // Steps in pattern
  0xB030B030,
  0x8888A84A,
  0x0B0B0B0B,
  0x88888888
};

// Motorik 2
uint32_t NW_01 [5] = { 32,  // Steps in pattern
  0xAAAAAAAA,
  0x08080809,
  0xA2A2A2A0,
  0x88888888
};

// Motorik 3
uint32_t NW_02 [5] = { 32,  // Steps in pattern
  0xC0A0C0A0,
  0x0808080A,
  0xB9B9B9B9,
  0x80A28000
};

// Pop 1
uint32_t NW_03 [5] = { 32,  // Steps in pattern
  0x8888888A,
  0x24B024B2,
  0x08080809,
  0xAAAAAAAB
};

// Pop 2
uint32_t NW_04 [5] = { 32,  // Steps in pattern
  0x2BA3A3A2,
  0x2BA3A3A2,
  0x08080808,
  0x08080808
};

// Pop 3
uint32_t NW_05 [5] = { 32,  // Steps in pattern
  0xAFA3A3A8,
  0xAFA3A3A8,
  0x00A0A0A2,
  0x00A0A0A2
};

// Pop 4
uint32_t NW_06 [5] = { 32,  // Steps in pattern
  0xB60FB60F,
  0xB60FB60F,
  0xFA2FA3AC,
  0xFA2FA3AC
};

// Funk 1
uint32_t NW_07 [5] = { 16,  // Steps in pattern
  0x94940000,
  0x22220000,
  0x55550000,
  0x44470000
};

// Funk 2
uint32_t NW_08 [5] = { 16,  // Steps in pattern
  0x22220000,
  0xB7A70000,
  0x55550000,
  0x44470000
};

// Funk 3
uint32_t NW_09 [5] = { 16,  // Steps in pattern
  0xB7A70000,
  0x999A0000,
  0x55550000,
  0x44470000
};

// Funk 4
uint32_t NW_0A [5] = { 16,  // Steps in pattern
  0x999A0000,
  0x94940000,
  0x55550000,
  0x44470000
};

// Post
uint32_t NW_0B [5] = { 16,  // Steps in pattern
  0x95CAA000,
  0x80400000,
  0x100AA000,
  0x88888000
};

// King 1
uint32_t OW_0C [5] = { 12,  // Steps in pattern
  0xAD500000,
  0xAD500000,
  0xB4C00000,
  0xB4C00000
};

// King 2
uint32_t OW_0D [5] = { 12,  // Steps in pattern
  0xB4C00000,
  0xB4400000,
  0xAD500000,
  0xAD500000
};

// Kroboto
uint32_t OW_0E [5] = { 12,  // Steps in pattern
  0x2CB00000,
  0x2CB00000,
  0x82400000,
  0xAD500000
};

// Vodu 1
uint32_t OW_0F [5] = { 12,  // Steps in pattern
  0xAB500000,
  0xAB500000,
  0x82400000,
  0x0C300000
};

// Vodu 2
uint32_t OW_10 [5] = { 12,  // Steps in pattern
  0x6DB00000,
  0x6DB00000,
  0xAB500000,
  0x0C300000
};

// Vodu 3
uint32_t OW_11 [5] = { 12,  // Steps in pattern
  0x82400000,
  0x82400000,
  0x6DB00000,
  0x0C300000
};

// Gahu
uint32_t OW_12 [5] = { 16,  // Steps in pattern
  0xD5540000,
  0xD5540000,
  0x80000000,
  0x91140000
};

// Clave
uint32_t OW_13 [5] = { 16,  // Steps in pattern
  0x92280000,
  0x92280000,
  0xB5AD0000,
  0x33230000
};

// Rhumba
uint32_t OW_14 [5] = { 16,  // Steps in pattern
  0x91280000,
  0x91280000,
  0xB5AD0000,
  0x33230000
};

// Jhaptal 1
uint32_t OW_15 [5] = { 10,  // Steps in pattern
  0x4E400000,
  0x4E400000,
  0xB1800000,
  0x84000000
};

// Jhaptal 2
uint32_t OW_16 [5] = { 10,  // Steps in pattern
  0x80000000,
  0xB1800000,
  0xB1800000,
  0x84000000
};

// Chachar
uint32_t OW_17 [5] = { 32,  // Steps in pattern
  0x80008000,
  0x80A080A0,
  0x08080808,
  0x00000003
};

// Mata
uint32_t OW_18 [5] = { 18,  // Steps in pattern
  0x80010000,
  0x8A090000,
  0x8A0D4000,
  0x00824000
};

// Pashta
uint32_t OW_19 [5] = { 14,  // Steps in pattern
  0x80000000,
  0x0C080000,
  0x0C080000,
  0x82200000
};

// Prime 232
uint32_t OB_1A [5] = { 16,  // Steps in pattern
  0x88880000,
  0x44440000,
  0x22220000,
  0x11110000
};

// Sequence
uint32_t OB_1B [5] = { 8,  // Steps in pattern
  0x80000000,
  0x88000000,
  0xAA000000,
  0xFF000000
};

// Prime 2
uint32_t OB_1C [5] = { 12,  // Steps in pattern
  0x80000000,
  0x82000000,
  0x92400000,
  0xFFF00000
};

// Prime 322
uint32_t OB_1D [5] = { 12,  // Steps in pattern
  0x80000000,
  0x82000000,
  0xAAA00000,
  0xFFF00000
};

uint32_t TK_1E [5] = { 32,
  0b10001000100010001000100010001000,
  0b00001000000010000000100000001000,
  0b11011101110111011101110111011101,
  0b00100010001000100010001000100010
};

uint32_t TK_1F [5] = { 32,
  0b11111111111111111111111111111111,
  0b10010010010010101001001001001010,
  0b10010100100101001010100101010010,
  0b10010100101001010010100101001010
};


// Bank1 TECHNO
const static uint32_t bnk1_ptn[8][6] = {
 { 0x88888888,  0x080888AF, 0xDDDDDDDD, 0x22222222, 0x10001000, 0x00220022},
 { 0x88888888,  0x080888AF, 0xFFFFFFFF, 0x22222222, 0x10001000, 0x00220022},
 { 0x88888888,  0x080888AF, 0xCCCCCCCC, 0x22222222, 0x10001000, 0x00220022},
 { 0x888888AF,  0x08080808, 0x45454545, 0x22222222, 0x10001000, 0x00220022},
 { 0x888888AF,  0x08080808, 0xFFFFFFFF, 0x22222222, 0x10001000, 0x00220022},
 { 0x88880000,  0x08090809, 0xDDDDDDDD, 0x22222222, 0x10001000, 0x00220022},
 { 0x88880000,  0x08490849, 0xDDDDDDDD, 0x22222222, 0x10001000, 0x00220022},
 { 0x88888896,  0x08020869, 0xDDDDDDDD, 0x22222222, 0x10001000, 0x00220022}
};

// Bank2 DUBTECHNO
const static uint32_t bnk2_ptn[8][6] = {
 { 0x88888888,  0x08080809, 0xDDDDDDDD, 0x22222222, 0x12401240, 0x00220022},
 { 0x8888000A,  0x08080849, 0xFFFFDDDD, 0x22222222, 0x12401000, 0x00220022},
 { 0x88898888,  0x08080000, 0xCCCCCCCC, 0x22222222, 0x12401240, 0x00220022},
 { 0x88898888,  0x08080809, 0x45454545, 0x22222222, 0x12401240, 0x00220022},
 { 0x888C8888,  0x08088888, 0xFFFFFFFF, 0x22222222, 0x12401240, 0x00220022},
 { 0x888C999F,  0x08090000, 0xDDDDDDDD, 0x22222222, 0x12401240, 0x00220022},
 { 0x0000000A,  0x08490849, 0xDDDDDDDD, 0x22222222, 0x12401000, 0x00220022},
 { 0x0000000A,  0x08020802, 0xDDDDDDDD, 0x22222222, 0x12401000, 0x00220022}
};

// Bank3 HOUSE
const static uint32_t bnk3_ptn[8][6] = {
 { 0x88888888, 0x080888AF, 0x22222222, 0x00000000, 0x00400040, 0x01010101},
 { 0x888A8888, 0x080888AF, 0x23232323, 0x00000000, 0x00400040, 0x01010101},
 { 0x88888888, 0x080888AF, 0xCCCCCCCC, 0x22222222, 0x00400040, 0x01010101},
 { 0x888A8888, 0x080888AF, 0xCCCCCCCC, 0x22222222, 0x00400040, 0x01010101},
 { 0x88880000, 0x08080808, 0xFFFFFFFF, 0x22222222, 0x00400040, 0x01010101},
 { 0x888A0000, 0x08080808, 0xFFFFFFFF, 0x22222222, 0x00400040, 0x01010101},
 { 0x888A0000, 0x08080808, 0xFFFFFFFF, 0x22222222, 0x00400040, 0x41124112},
 { 0x888888AF, 0x08080808, 0xCCCCCCCC, 0x22222222, 0x00400040, 0x41124112}
};

/////////////////////////////////////////////////
uint32_t * primes[32] = 
{
  NW_00, NW_01, NW_02, NW_03,  // "New World"
  NW_04, NW_05, NW_06, NW_07,
  NW_08, NW_09, NW_0A, NW_0B,
  OW_0C, OW_0D, OW_0E, OW_0F,  // "Old World"
  OW_10, OW_11, OW_12, OW_13,
  OW_14, OW_15, OW_16, OW_17,
  OW_18, OW_19,
  OB_1A, OB_1B, OB_1C, OB_1D,   // "Oddball"
  TK_1E, TK_1F
};


// Returns the total number of steps in the pattern (e.g. starts at ONE)
uint8_t pattnSteps(int8_t ptn)
{
  uint8_t shteppes(32);
  if (ptn > 0 && ptn < NUM_WORLD_RHYTHMS)
  {
    shteppes = (uint8_t)(primes[ptn][0] & 0xFF);
  }

  Serial.printf("ptn %d: %d steps\n", ptn, shteppes);
  return shteppes;
}
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