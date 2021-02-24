#include<Servo.h>
#include<FastLED.h>
#define COLOR_ORDER GRB
#define CHIPSET     WS2812
#define BRIGHTNESS  255

//Eyes (flame effect)
#define EYE_PIN     5
#define EYE_LEDS    14
#define FRAMES_PER_SECOND 60

#define COOLING 55
#define SPARKING 120
  
bool gReverseDirection = false;

CRGB eyeleds[EYE_LEDS];

int b1pin = 2;
int b2pin = 3;
int b3pin = 4;
int endstop = 6;
int scythearm = 9;
int pointarm = 10;

int b1;
int b2;
int b3;

bool GoPointer = false;
bool GoScythe = false;
bool GoLights = false;


int ppressed = 1;
int spressed = 1;
int lpressed = 1;

Servo pservo;
Servo sservo;


void setup() {

  pinMode(b1pin, INPUT_PULLUP);
  pinMode(b2pin, INPUT_PULLUP);
  pinMode(b3pin, INPUT_PULLUP);


  //Eyes
  FastLED.addLeds<CHIPSET, EYE_PIN, COLOR_ORDER>(eyeleds, EYE_LEDS).setCorrection( TypicalLEDStrip ); 
  FastLED.setBrightness( BRIGHTNESS );
  

  
  Serial.begin(115200);
}

void loop() {

  b1 = digitalRead(b1pin);
  b2 = digitalRead(b2pin);
  b3 = digitalRead(b3pin);


Serial.println("============");
Serial.print("B1  State : ");
Serial.println(b1);
Serial.print("B2  State : ");
Serial.println(b2);
Serial.print("B3  State : ");
Serial.println(b3);
Serial.print("ppressed : ");
Serial.println(ppressed);
Serial.print("spressed : ");
Serial.println(spressed);
Serial.print("lpressed : ");
Serial.println(lpressed);
Serial.println("============");


if (b2 == 0 && spressed == 0){
  spressed = 1;
}
else if (b2 == 0 && spressed == 1){
    spressed = 0;
}

if (b3 == 0 && lpressed == 0){
  lpressed = 1;
}
else if (b3 == 0 && lpressed == 1){
    lpressed = 0;
}

if (b1 == 0 or GoPointer == true){
Serial.println("Pointer is now running");
pservo.attach(pointarm);
pservo.write(115); //clockwise rotation
delay(1500); //rotation duration in ms
pservo.write(90);
delay(3000);
while(digitalRead(endstop) != 1){
  pservo.write(65); //counterclockwise rotation
}
pservo.detach(); //detach servo to prevent “creeping” effect
}


if (spressed == 0 or GoScythe == true){ 
sservo.attach(scythearm); //reattach servo to pin x
sservo.write(80); //counterclockwise rotation //anything below is too slow
Serial.println("Scythe is now running");
}
else if (spressed == 1 or GoScythe == false){
sservo.write(90);  
sservo.detach();
}

if (lpressed == 0 or GoLights == true){ 
Serial.println("Lights are now running");
Fire2012(); // run simulation frame for eyes
FastLED.show(); // display this frame
FastLED.delay(1000 / FRAMES_PER_SECOND);

  }
else if (lpressed == 1 or GoLights == false){
  FastLED.clear();
  FastLED.show();
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

 
