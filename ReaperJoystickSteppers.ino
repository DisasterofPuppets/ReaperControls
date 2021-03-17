/* this should* allow control of the below with a joystick
Everything on no activity - 660mA (running totals)
+ torch 1.67 A
+ LEDS 1.9 A
+ Pointer 1.95 A
+ Scythe not yet working.


Relay and joystick draws 70mA
+ torch connected via a relay when on - 1.30 A  
+ leds lights with FastLED 300mA
+ Stepper motor for arm movement 390mA max (leds turned off)
stepper motor for scythe rotation 28byj-48 stepper motors ULN2003 driver 
*/

#include <Stepper.h>
#include <FastLED.h>

//INPUT PINS
#define xaxis A0
#define yaxis A1
#define joybutton 3
#define endstop  2

#define COLOR_ORDER GRB
#define CHIPSET     WS2812
#define BRIGHTNESS  255

//Eyes (flame effect)
#define EYE_PIN     12
#define EYE_LEDS   14 
#define FRAMES_PER_SECOND 60

#define COOLING 55
#define SPARKING 120
  
bool gReverseDirection = false;

CRGB eyeleds[EYE_LEDS];

// Input pins

// Joystick

//output pins
int torchout = 4;

//variables
int buttonstate = 1;
int torchon = 1;
int pointeron = 1;
int scytheon = 1;
int sisrunning = 1;
bool ledstate = false;
int isrunning = 1;


// Steps per revolution
const int spr = 2038;
//Using 28byj-48 stepper motors ULN2003 driver (geared max 15rpm or 18 without torque.)

//Initalise Steppers
//pointer
Stepper pstep = Stepper(spr, 8, 10, 9, 11);
//scythe
Stepper sstep = Stepper(spr, A2, A3, A4, A5);

void setup() {

  //Eyes
  FastLED.addLeds<CHIPSET, EYE_PIN, COLOR_ORDER>(eyeleds, EYE_LEDS).setCorrection( TypicalLEDStrip ); 
  FastLED.setBrightness( BRIGHTNESS );

pstep.setSpeed(5); // anything higher doesn't have enough torque and makes noise
sstep.setSpeed(5);

//OUTPUTS
pinMode(torchout,OUTPUT);

//INPUTS

pinMode (joybutton, INPUT_PULLUP);
pinMode (endstop, INPUT_PULLUP);

Serial.begin(115200);
}

void loop() {

// Testing

/*
Serial.print("Endstop State: ");
Serial.println(digitalRead(endstop));
Serial.print("X : ");
Serial.println(analogRead(xaxis));
Serial.print("Y : ");
Serial.println(analogRead(yaxis));
Serial.print("Button : ");
Serial.println(digitalRead(joybutton));
Serial.print("Torch State : ");
Serial.println(torchon);
Serial.print("Scythe State : ");
Serial.println(scytheon);
Serial.print("Point State : ");
Serial.println(pointeron);
Serial.print("Button State: ");
Serial.println(buttonstate);
*/
//delay(300);


if (digitalRead(joybutton) == 0 && buttonstate == 1){
  buttonstate = 0;
}
if (digitalRead(joybutton) == 0 && buttonstate == 0){
  buttonstate = 1;
}


// Torch 
// Hard left
if (analogRead(xaxis) <= 10 && torchon == 1){
  // turn on torch
  Serial.println("TORCH ON");
  digitalWrite(torchout,HIGH);
  torchon = 0;
  } 

else if (analogRead(xaxis) <= 10 && torchon == 0){

  // turn torch off
  Serial.println("TORCH OFF");
  digitalWrite(torchout,LOW);
  torchon = 1;

  }


// Scythe
//Hard right

// this is a pain in the ass. Let's revist

    if (analogRead(xaxis) > 1000 && scytheon == 1){
      Serial.println("SCYTHE ON");
      sstep.step(1); // we need to be able to break here
      scytheon = 0;
     }
 
    else if (analogRead(xaxis) > 1000 && scytheon == 0){
      Serial.println("SCYTHE OFF");
      scytheon = 1;
      
       }
  
 



// Pointer

if (analogRead(yaxis) < 10 && pointeron == 1){
  pointeron = 0;
}

if (pointeron == 0){
  // enable pointer motion
  Serial.println("POINTER RAISE");
  pstep.step(-spr/3); //go backward
  delay(5000); // keep hand in air for a little
    
  Serial.println("POINTER LOWER");
  pstep.step(spr/3); //go forward
  pointeron = 1;
}


//LEDS
//Up

if (analogRead(yaxis) > 1000 && ledstate == true){
    ledstate = false;
}
else if (analogRead(yaxis) > 1000 && ledstate == false){
    ledstate = true;
}
 

if (analogRead(yaxis) > 1000 && ledstate == false){
  Serial.println("LEDS OFF");
  FastLED.clear(true);  
 }

else if (ledstate == true){
  Serial.println("LEDS ON");
  Fire2012();
  FastLED.show(); //Update LED display
 }

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
