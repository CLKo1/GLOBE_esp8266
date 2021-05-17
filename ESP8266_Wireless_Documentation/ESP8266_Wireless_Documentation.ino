/*---------------------------------------------------------------------------------------------
There are many parts to my code, and I needed to look at, and adapt many different smaller examples. 
There were the standard arduino library examples that came with each of my sensors, but then there were 
others that I needed to find as I started to troubleshoot. 

*TCS34725 changing the colour of the LEDs example: https://hackaday.io/project/2582/logs?sort=oldest 
*16 channel multiplexer wireless example: https://github.com/nuxeh/esp-osc-udp-examples 
*Adafruit LIS3DH with the ESP8266 example: https://www.collin.edu/hr/benefits/cMorgan_sabbaticalSummary_Spr2018b.pdf 

Wiring: 
*multiplexor: EN to (ESP)GND, SIG to A0, VCC to 3v3 and GND to GND 
*the multiplexer's other pins can connect to any digital pin of the Arduino or ESP, as long as they are assigned. I have them as: 
    S0 = 0 
    S1 = 15
    S2 = 13
    S3 = 12
  --------------------------------------------------------------------------------------------- */

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include "SparkFunLIS3DH.h"
#include "Adafruit_TCS34725.h"
#include "Wire.h"
#include <Adafruit_NeoPixel.h>

#define PINA 2
#define TPIXELA 29 //The total number of led's in your connected strip/stick (Default is 60)

//I named the strip, strip_a in case if I wanted to connect multiple strips.In that case, then I would include a strip_b. 
Adafruit_NeoPixel strip_a = Adafruit_NeoPixel(TPIXELA, PINA, NEO_GRB + NEO_KHZ800);  

WiFiUDP Udp; // A UDP instance to let us send and receive packets over UDP

// our RGB -> eye-recognized gamma color
byte gammatable[256];

char ssid[] = "*******"; // the network SSID (name)
char pass[] = "***************";  // the network password

// Network
const IPAddress outIp(***, ***, **, ***); // remote IP of the computer => need to manually change the IP of the computer so it is the same every time
const unsigned int outPort = ****;        // remote port to send OSC messages to the computer
const unsigned int localPort = ****;      // local port to listen for OSC messages from the computer 

//the mux pins to their assigned digital pins. Change these if you want to change the wireing.
int MUXPinS0 = 0; 
int MUXPinS1 = 15;
int MUXPinS2 = 13;
int MUXPinS3 = 12;

//I2C address and name for the LIS3DH
LIS3DH lis( I2C_MODE, 0x19 );

//integration time, gain, and name for the TCS34725. If you want to change the integration time of gain, see the "Adafruit_TCS34725.h" file. 
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_4X); 

void setup() {
  Serial.begin(115200);
  Serial.println("OSC multimessage");

  // Configure mux pins 
  pinMode(MUXPinS0, OUTPUT);
  pinMode(MUXPinS1, OUTPUT);
  pinMode(MUXPinS2, OUTPUT);
  pinMode(MUXPinS3, OUTPUT);

  //Configure the LED strip 
  pinMode(PINA, OUTPUT);
    strip_a.begin();
    strip_a.show(); // Initialize all pixels to 'off'
    strip_a.setBrightness(100); //adjust brightness here, 0-255

  //start the LIS3DH
  if( lis.begin() != 0 ){
    Serial.println("Problem starting the sensor at 0x19.");
    }
  else {
    Serial.println("Sensor at 0x19 started.");
    } 
  //start the TCS34725 
  if( tcs.begin() != 0 ){
    Serial.println("Problem starting the sensor at 0x29.");
  }
  else{
    Serial.println("Sensor at 0x29 started.");
  }

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

/*Typically this part would be left in. However, if a wireless connection cannot be made, then I can't use it wired either, because the 
 * ESP8266 is stuck on this loop of printing "......." and it won't print the sensor data.
 * With it taken out, then I can still use the device wired.
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    }
*/  

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  Serial.println("Starting UDP");
  // Start UDP
  Udp.begin(localPort);
  Serial.print("Local port: ");
#ifdef ESP32
 Serial.println(localPort);
#else
 Serial.println(Udp.localPort());
#endif
}

void loop() {
  //Create an OSC message for the multiplexer 
  OSCMessage msgMux("/mux"); 

  //Get all analogue values in turn from the multiplexer
  for (char i = 0; i < 16; i++) {
    int value = getAnalog(i); // Get the current analogue value of control i

  //Also print the sensor data through serial, just in case
    Serial.print(value);
    Serial.print(" ");

    //Add a parameter for this control to the message, as 32-bit int
    msgMux.add((int32_t)value);
  }

  // Send the message over UDP
  Udp.beginPacket(outIp, outPort); // Start of UDP packet
  msgMux.send(Udp);               // send the OSC frame over UDP
  Udp.endPacket();                 // End of UDP packet
  msgMux.empty();                 

  //Create an OSC message for the LIS3DH 
  OSCMessage msglis("/xyz"); 
    Serial.print(lis.readFloatAccelX());
    Serial.print(" ");
    Serial.print(lis.readFloatAccelY());
    Serial.print(" ");
    Serial.print(lis.readFloatAccelZ());
    Serial.print(" ");
   //1 of the 3 analog pins that are on the LIS3Dh, for the IR sensor. If you need the other 2 pins, then copy these two lines and at a "ADC2" and "ADC3". 
    Serial.print(lis.read10bitADC1());  
    Serial.print(" ");

  msglis.add(lis.readFloatAccelX()); 
  msglis.add(lis.readFloatAccelY());
  msglis.add(lis.readFloatAccelZ());
  msglis.add(lis.read10bitADC1());
    
  Udp.beginPacket(outIp, outPort);
  msglis.send(Udp);
  Udp.endPacket();
  msglis.empty();

  //Create an OSC message for the TCS34725
  OSCMessage msgtcs("/tcs");
    uint16_t clear, red, green, blue;

      tcs.setInterrupt(false); 
      delay(50); // takes 50ms to read
      tcs.getRawData(&red, &green, &blue, &clear);
      tcs.setInterrupt(true); 
  
      Serial.print(red, DEC); 
      Serial.print(" "); 
      Serial.print(green, DEC); 
      Serial.print(" "); 
      Serial.print(blue, DEC); 
      Serial.print(" ");
      Serial.print(clear, DEC); 
      //very last serial print needs to be a Serial.println. That way it prints properly in Max.
      Serial.println(" "); 

    msgtcs.add((int32_t)red);
    msgtcs.add((int32_t)green);
    msgtcs.add((int32_t)blue);
    msgtcs.add((int32_t)clear);
    
  Udp.beginPacket(outIp, outPort);
  msgtcs.send(Udp);
  Udp.endPacket();
  msgtcs.empty();

  //it helps convert RGB colors to what humans see
  for (int i=0; i<256; i++) {
  float x = i;
  x /= 255;
  x = pow(x, 2.5);
  x *= 255;
  gammatable[i] = x;
  }
  
  //hex code for visualization
  uint32_t sum = red;
  sum += green;
  sum += blue;
  sum = clear;

  float r, g, b;
  r = red; r /= sum;
  g = green; g /= sum;
  b = blue; b /= sum;
  r *= 256; g *= 256; b *= 256;

//if the TCS34725 senses ambient light, then do function1, which makes the LEDs sustain the last sensed colour
if(clear <= 1000){ 
  function1();
}

//if the clear of the TCS34725 is above 1000, then a colour is being pressed close to it, so then do function2. This makes the LEDs re-sample their colour.
else{
  function2();
  Serial.println();
  colorWipe(strip_a.Color(gammatable[(int)r], gammatable[(int)g], gammatable[(int)b]), 0); 
  }
} 

  // Get a 10-bit integer for the mux input
int getAnalog(int MUXyPin) {
  digitalWrite(MUXPinS3, HIGH && (MUXyPin & B00001000));
  digitalWrite(MUXPinS2, HIGH && (MUXyPin & B00000100));
  digitalWrite(MUXPinS1, HIGH && (MUXyPin & B00000010));
  digitalWrite(MUXPinS0, HIGH && (MUXyPin & B00000001));

  return analogRead(A0);
}

// Fill the LEDs one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip_a.numPixels(); i++){ 
  strip_a.setPixelColor(i, c); 
  }
  strip_a.show(); //adafruit support said to put this line out of the "for" loop to ensure the data prints quickly, and it works!
}

//function1. Keep the last sensed color when the sensor reading is ambient light. 
void function1(){
  uint32_t color; 
  strip_a.getPixelColor(0); 
}

//function2. When there is a change in colour from the TCS34725, change the colour of the LEDs. 
void function2(){
  strip_a.setBrightness(100); //adjust brightness here
}
