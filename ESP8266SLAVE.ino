#include <ESP8266WiFi.h>
byte ledPin = 2;

int b1pin = 15;
int b2pin = 16;
int b3pin = 17;

int b1;
int b2;
int b3;

int ppressed = 1;
int spressed = 1;
int lpressed = 1;

char ssid[] = "ReaperMan";           // SSID of your AP
char pass[] = "DisasterofPuppets80";         // password of your AP

IPAddress server(15,15,15,15);     // IP address of the AP
WiFiClient client;

void setup() {

  pinMode(b1pin, INPUT_PULLUP);
  pinMode(b2pin, INPUT_PULLUP);
  pinMode(b3pin, INPUT_PULLUP);
  
  
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);           // connects to the WiFi AP
  Serial.println();
  Serial.println("Connection to the AP");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println("Connected");
  Serial.println("station_bare_01.ino");
  Serial.print("LocalIP:"); Serial.println(WiFi.localIP());
  Serial.println("MAC:" + WiFi.macAddress());
  Serial.print("Gateway:"); Serial.println(WiFi.gatewayIP());
  Serial.print("AP MAC:"); Serial.println(WiFi.BSSIDstr());
  pinMode(ledPin, OUTPUT);
}

void loop() {
  client.connect(server, 80);
  digitalWrite(ledPin, LOW);
  
  b1 = digitalRead(b1pin);
  b2 = digitalRead(b2pin);
  b3 = digitalRead(b3pin);

Serial.println(client.print(b1));
Serial.println(client.print(b2));
Serial.println(client.print(b3));
  
//  Serial.println("********************************");
//  Serial.print("Byte sent to the AP: ");
  Serial.println(client.print("DoPController\r"));
  String answer = client.readStringUntil('\r');
  Serial.println("From the AP: " + answer);
  client.flush();
  digitalWrite(ledPin, HIGH);
  client.stop();

Serial.println("============");
Serial.print("B1  State : ");
Serial.println(b1);
Serial.print("B2  State : ");
Serial.println(b2);
Serial.print("B3  State : ");
Serial.println(b3);
Serial.println("============");

delay(1000);
}
