#include <ESP8266WiFi.h>
#include<Servo.h>
#include<FastLED.h>
#define COLOR_ORDER GRB
#define CHIPSET     WS2812
#define BRIGHTNESS  255

//Eyes (flame effect)
#define EYE_PIN     5 //14 on the ESP
#define EYE_LEDS    14
#define FRAMES_PER_SECOND 60

#define COOLING 55
#define SPARKING 120
  
bool gReverseDirection = false;

CRGB eyeleds[EYE_LEDS];

WiFiServer server(80);
IPAddress IP(15,15,15,15);
IPAddress mask = (255, 255, 255, 0);

byte ledPin = 2;

bool GoPointer = false;
bool GoScythe = false;
bool GoLights = false;

int b1;
int b2;
int b3;

int scythearm = 15;
int pointarm = 16;
int endstop = 18;

int ppressed;
int spressed;
int lpressed;

Servo pservo;
Servo sservo;

void setup() {

  FastLED.addLeds<CHIPSET, EYE_PIN, COLOR_ORDER>(eyeleds, EYE_LEDS).setCorrection( TypicalLEDStrip ); 
  FastLED.setBrightness( BRIGHTNESS );

 Serial.begin(115200);
 WiFi.mode(WIFI_AP);
 WiFi.softAP("ReaperMan", "DisasterofPuppets80");
 WiFi.softAPConfig(IP, IP, mask);
 server.begin();
 pinMode(ledPin, OUTPUT);
 Serial.println();
 Serial.println("accesspoint_bare_01.ino");
 Serial.println("Server started.");
 Serial.print("IP: "); Serial.println(WiFi.softAPIP());
 Serial.print("MAC:"); Serial.println(WiFi.softAPmacAddress());
}

void loop() {
 WiFiClient client = server.available();
 if (!client) {
  return;
  }
 
 digitalWrite(ledPin, LOW);
 String request = client.readStringUntil('\r');
 Serial.println("********************************");
 Serial.println("From the station: " + request);
 client.flush();
 Serial.print("Byte sent to the station: ");
 Serial.println(client.println(request + "ca" + "\r"));
 digitalWrite(ledPin, HIGH);

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
