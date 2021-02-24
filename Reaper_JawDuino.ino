/***********************************************************
08-14-2016 Mike North This is a little proof of concept 
to make a servo move in sync with audio.
*******************/
#include <FastLED.h>
#include <Servo.h>
#include "SoftRcPulseOut.h"

#define COLOR_ORDER GRB
#define CHIPSET     WS2812
#define BRIGHTNESS  255

//FrontLights
#define LED_PIN     5
#define NUM_LEDS    9


//Eyes (flame effect)
#define EYE_PIN     9
#define EYE_LEDS    14
#define FRAMES_PER_SECOND 60

#define COOLING 55
#define SPARKING 120
  
bool gReverseDirection = false;

CRGB fleds[NUM_LEDS];
CRGB eyeleds[EYE_LEDS];


#define TEST_PIN 11 //pin 11 set to ground will kick off the servo sweep test
int audio_value = 0;
long lastMsg = 0;
long sleepWindow = 300000; //if 5 minutes go by with no signal, then put the servos to bed
SoftRcPulseOut servo;
volatile boolean servosEnabled = false;
volatile boolean ledsOn = true;
volatile unsigned long currentTime = 0;
volatile unsigned long lastLEDtime = 0;
unsigned long resetWait = 120000; //servos sleep if not changed within this time frame (120 secs)



void setup()
{

  Serial.begin(115200);
	set_minmax();
	pinMode(TEST_PIN,INPUT);  //pin 11 will be 
	digitalWrite(TEST_PIN,HIGH); //assign pull-up resistor

   // Front lights
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(fleds, NUM_LEDS).setCorrection( TypicalLEDStrip );

  //Eyes
  FastLED.addLeds<CHIPSET, EYE_PIN, COLOR_ORDER>(eyeleds, EYE_LEDS).setCorrection( TypicalLEDStrip ); 
  FastLED.setBrightness( BRIGHTNESS );
  
} 

void loop()
{

Fire2012(); // run simulation frame for eyes

  
  //fleds[0] = CRGB::White;// hidden by heat shrink
  fleds[1] = CRGB::White;
  fleds[2] = CRGB::White;
  fleds[3] = CRGB::White;
  fleds[4] = CRGB::White;
  fleds[5] = CRGB::White;
  fleds[6] = CRGB::White;
  fleds[7] = CRGB::White;
  fleds[8] = CRGB::White;
  
FastLED.show(); // display this frame
FastLED.delay(1000 / FRAMES_PER_SECOND);
  
  
  servo_test();
  audio_updates(); //read sample from audio input and apply average to buffer
  if(servosEnabled) 
  {
	action(); //servos are handled in the action loop
	SoftRcPulseOut::refresh();
	if((millis() - lastMsg) > sleepWindow)
	  detach_servos();
  }
} 
void attach_servos()
{   // attach the pin to the servo object
    servo.attach(2);
    servosEnabled = true;
}
void detach_servos()
{   // detach the servo objects
    servo.detach();
    servosEnabled = false;
}
void servo_test()
{
	if(digitalRead(TEST_PIN) == HIGH) return;
	attach_servos();
	SoftRcPulseOut::refresh();
	for(int i = 0; i < 360;i++)
	{
		if(i < 180)
			audio_value = i;
		else
			audio_value = 359 - i;
		action();
		for(int i = 0; i < 10; i++)
		{
		   delay(1); 
		   SoftRcPulseOut::refresh();
		}
	}
	detach_servos();
}
void audio_updates()
{
	audio_value = 0;
	if(analogRead(A0) < 341) audio_value += 60;
	if(analogRead(A1) < 341) audio_value += 60;
	if(analogRead(A2) < 341) audio_value += 60;

	if(audio_value > 0) 
	{
		lastMsg = millis(); //save the time stamp from when we last had some action
		if(!servosEnabled)attach_servos();
	}
}
void action() { 
  if (!servosEnabled) attach_servos();
  servo.write(audio_value);
  SoftRcPulseOut::refresh();
} 
void set_minmax()
{
	//set the first parameter in the following functions to a number between 0 and 180.
	//I used 92 and 72 in my tests to give about 20 degrees of motion.
	//You may swap the large and small numbers to reverse direction.
	//Just play with them, upload the code, then ground pin 11 to run the sweep test.
	// Be sure to only play with these numbers while the jaw linkage is disconnected,
	//  otherwise, you risk hitting mechanical limits and damaging your linkage or servo!
	servo.setMinimumPulse(map(0,0,180,512,2400));
	servo.setMaximumPulse(map(40,0,180,512,2400));
}

void Fire2012()
{
// Array of temperature readings at each simulation cell
  static byte heat[EYE_LEDS];

  // Step 1.  Cool down every cell a little
    for( int i = 1; i < EYE_LEDS; i++) { // starting at 1 instead of 0 as first led covered by heat shrink
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / EYE_LEDS) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= EYE_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 1; j < EYE_LEDS; j++) {
      CRGB color = HeatColor( heat[j]);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (EYE_LEDS-1) - j;
      } else {
        pixelnumber = j;
      }
      eyeleds[pixelnumber] = color;
    }
}
