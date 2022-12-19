// #include "Outputs.h"
// #include "FDTM_IO.h"

// ////////////////////////////////////////////////////////////////
// //                          OLED
// ////////////////////////////////////////////////////////////////

// // Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// #define SCREEN_WIDTH 128 // OLED display width, in pixels
// #define SCREEN_HEIGHT 32 // OLED display height, in pixels
// #define OLED_RESET -1    // Unused but has to be defined for library

// // Create OLED display device
// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h>
// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// void initDisplay()
// {
//   // Start up the OLED display
//   display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
//   display.setTextSize(1);
//   display.setTextColor(WHITE);
//   display.setCursor(0, 0);
//   display.clearDisplay();
//   display.display();
// }

// void displayWhite()
// {
//   display.setTextColor(WHITE); // TODO: reset function for main loop
// }

// void initDigitalOutputs()
// {
//   for (int8_t gg = 0; gg < 4; ++gg)
//   {
//     pinMode(GATEout_PINS[gg], OUTPUT);
//   }

//   for (int8_t gg = 0; gg < NUM_GATES_OUT; ++gg)
//   {
//     digitalWrite(GATEout_PINS[gg], LOW);
//   }
// }

// void blinkenLichts()
// {
//   int8_t jj(0);
//   bool up(1);

//   // Blinkenlichts
//   for (int8_t ii = 0; ii < 19; ++ii)
//   {
//     digitalWrite(GATEout_PINS[jj], HIGH);
//     delay(30);
//     digitalWrite(GATEout_PINS[jj], LOW);

//     if (jj == 3)      { up = false; }
//     else if (jj == 0) { up = true;  }

//     jj += up ? 1 : -1;
//   }

//   for (int8_t gg = 0; gg < NUM_GATES_OUT; ++gg)
//   {
//     digitalWrite(GATEout_PINS[gg], LOW);
//   }
// }