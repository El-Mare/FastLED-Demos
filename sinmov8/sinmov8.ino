/*
#
# sinmov8
#
# By: Andrew Tuline
# www.tuline.com
# atuline@gmail.com
#
# Date: Sep, 2014
#
# Two moving waves of different colours, initially red and blue. In this case, the wave values are either '1' or '0', thus really bars.
# The cool thing about this routine is that there are lot of configurable items without requiring a lot of complex code. It uses
# sine waves to create 'bars' and not lengthy/complex 'marching' code.
#
#
# With a few potentiometers or other input, such as MSGEQ7, you could change several values:
#
# - Change the width of each wave
# - Change the speed
# - Change the direction
# - Change the overall frequency
# - Change the colour of each wave or even change the hue rotation speed.
# - You could even get real funky and have different frequencies and phase changes for the waves. Epilepsy alert!
# 
# This would make for an awesome interactive display for youth. Oh wait, I've already ordered several 10K pots from aliexpress.
#
# I also added some twinkles just for Mark Kriegsman.
#
#
# FastLED 2.1 is available at https://github.com/FastLED/FastLED/tree/FastLED2.1
#
# Note: If you receive compile errors (as I have in the Stino add-on for Sublime Text), set the compiler to 'Full Compilation'.
#
*/

#define qsub(x, b)  ((x>b)?wavebright:0)                      // A digital unsigned subtraction macro. if result <0, then => 0. Otherwise, take on fixed value.
// #define qsub(x, b)  ((x>b)?x-b:0)                          // Unsigned subtraction macro. if result <0, then => 0


#include <FastLED.h>

#define LED_PIN     13
#define NUM_LEDS    24
#define BRIGHTNESS  255                                       // This is the overall brightness
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];


// Most of these variables can be mucked around with. Better yet, add some form of variable input or routine to change them on the fly. 1970's here I come. . 
//
uint8_t wavebright = 128;                                     // You can change the brightness of the waves/bars rolling across the screen. Best to make them not as bright as the sparkles.
uint8_t thishue = 0;                                          // You can change the starting hue value for the first wave.
uint8_t thathue = 140;                                        // You can change the starting hue for other wave.
uint8_t thisrot = 0;                                          // You can change how quickly the hue rotates for this wave. Currently 0.
uint8_t thatrot = 0;                                          // You can change how quickly the hue rotates for the other wave. Currently 0.
uint8_t allsat = 255;                                         // I like 'em fully saturated with colour.
uint8_t alldir = 0;                                           // You can change direction.
int8_t thisspeed = 4;                                         // You can change the speed.
int8_t thatspeed = 4;                                         // You can change the speed.
uint8_t allfreq = 32;                                         // You can change the frequency, thus overall width of bars.
int thisphase = 0;                                            // Phase change value gets calculated.
int thatphase = 0;                                            // Phase change value gets calculated.
uint8_t thiscutoff = 192;                                     // You can change the cutoff value to display this wave. Lower value = longer wave.
uint8_t thatcutoff = 192;                                     // You can change the cutoff value to display that wave. Lower value = longer wave.
int loopdelay = 10;                                           // You can change the delay. Also you can change the allspeed variable above. 
uint8_t twinkrun = 1;                                         // Enable/disable twinkles.

typedef struct {                                              // Define a structure for the twinkles that get overlaid on the moving waves.
      int twinkled;                                           // Supports a long strand of LED's.
      int twinkbright;                                        // Defined as 'int', so that we can trigger change on a negative brightness value.
  }  twinks;

#define numtwinks 4                                           // You can change the number of twinkles.
twinks mytwinks[numtwinks];                                   // The structure is called mytwinks.



void setup() {
  delay(1000);                                                 // Power-up safety delay or something like that.
  Serial.begin(9600);
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  set_max_power_in_volts_and_milliamps(5, 500);               // FastLED 2.1 Power management set at 5V, 500mA
  FastLED.setBrightness( BRIGHTNESS );
} // setup()



void loop() {
  sinmove8();                                                 // Simple call to the routine.
  if(twinkrun == 1) twinkle();                                // You can keep or lose the twinkles.
  show_at_max_brightness_for_power();
  delay_at_max_brightness_for_power(loopdelay*2.5);
} // loop()



// Edit the variables above, and save edits here for last.
void sinmove8() {

  if (alldir == 0) {thisphase+=thisspeed; thatphase+=thatspeed;} else {thisphase-=thisspeed; thatphase-=thatspeed;}   // You can change direction.
                                                                                                                      // You can have individual speeds.

    thishue = thishue + thisrot;                                        // Hue rotation is fun for thiswave.
    thathue = thathue + thatrot;                                        // It's also fun for thatwave.
  
  for (int k=0; k<NUM_LEDS-1; k++) {
    int thisbright = qsub(cubicwave8((k*allfreq)+thisphase), thiscutoff);      // qsub sets a minimum value called thiscutoff. If < thiscutoff, then bright = 0. Otherwise, bright = 128 (as defined in qsub)..
    int thatbright = qsub(cubicwave8((k*allfreq)+128+thatphase), thatcutoff);  // This wave is 180 degrees out of phase (with the value of 128).

    leds[k] = CHSV(thishue, allsat, thisbright);                        // Assigning hues and brightness to the led array.
    leds[k] += CHSV(thathue, allsat, thatbright);                      
  }
} // sinmov8()




void twinkle() {                                                        // This has been added for Mark Kriegsman.
  for (int i = 0; i < numtwinks; i++) {
    if (mytwinks[i].twinkbright <0) {
      mytwinks[i].twinkled = random8(0, NUM_LEDS-1);
      mytwinks[i].twinkbright = random8(220, 255);
    }
    leds[mytwinks[i].twinkled] = CHSV(80, 120, mytwinks[i].twinkbright);   // Trying to make a soft white twinkle
    mytwinks[i].twinkbright -= 8;
  }
} // twinkle()
