/*---------------------------------------------------------------------------------------------  
  There are many parts to my code, and I needed to look at, and adapt many different smaller
  examples. There were the standard Arduino library examples that came with each of my sensors,
  but then there were others that I needed to find as I started to troubleshoot.
  TCS34725 changing the colour of the LEDs example:
  https://hackaday.io/project/2582/logs?sort=oldest
  
  16 channel multiplexer wireless example: https://github.com/nuxeh/esp-osc-udp-examples
  
  Adafruit LIS3DH with the ESP8266 example:
  https://www.collin.edu/hr/benefits/cMorgan_sabbaticalSummary_Spr2018b.pdf
  
  multiplexor: EN to (ESP)GND, SIG to A0, VCC to 3v3 and GND to GND
  the multiplexer's other pins can connect to any digital pin of the Arduino or ESP, as long as
  they are assigned. I have them as:
  S0 = 0
  S1 = 15
  S2 = 13
  S3 = 12 

  Kevin Mitchell added the ability to control LED brightness with Max MSP. He also made it so the ESP sends its IP address to Max, that way we know what IP to send LED brightness data to. 
  The send IP can be turned on by Max sending a 1 and off by Max sending a 0. This way it's not constantly printing the IP address needlessly.
  --------------------------------------------------------------------------------------------- */
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include "SparkFunLIS3DH.h"
#include "Adafruit_TCS34725.h"
#include "Wire.h"
#include <Adafruit_NeoPixel.h>
#define PINA 2
#define TPIXELA 29 //The total number of LEDs in your connected strip/stick (Default is 60)
//I named the strip, strip_a in case I wanted to connect multiple strips. In that case, I would include a strip_b.

Adafruit_NeoPixel strip_a = Adafruit_NeoPixel(TPIXELA, PINA, NEO_GRB + NEO_KHZ800);

WiFiUDP Udp; // A UDP instance to let us send and receive packets over UDP

// our RGB -> eye-recognized gamma color
byte gammatable[256];
char ssid[] = "*******"; // the network SSID (name)
char pass[] = "***************"; // the network password

// Network
const IPAddress outIp(***, ***, **, ***); // remote IP of the computer => need to manually change the IP of the computer so it is the same every time
const unsigned int outPort = ****; // remote port to send OSC messages to the computer
const unsigned int localPort = ****; // local port to listen for OSC messages from the computer

//the mux pins to their assigned digital pins. Change these if you want to change the wiring.
int MUXPinS0 = 0;
int MUXPinS1 = 15;
int MUXPinS2 = 13;
int MUXPinS3 = 12;

//I2C address and name for the LIS3DH
LIS3DH lis( I2C_MODE, 0x19 );

//integration time, gain, and name for the TCS34725. If you want to change the integration time of gain, see the "Adafruit_TCS34725.h" file.
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_4X);

bool outputAll = true;

bool sendIPOnSerial = true;

float storeRed = 0;
float storeGreen = 0;
float storeBlue = 0;

//previously was always 100, now reads from max
int LEDBrightness = 100;

void printLineWrapper(std::string output)
{
  if (true == outputAll)
  {
    Serial.println(output.c_str());
  }
}

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
  if ( lis.begin() != 0 )
  {
    printLineWrapper("Problem starting the sensor at 0x19.");
    //Serial.println("Problem starting the sensor at 0x19.");
  }
  else 
  {
    if (outputAll == true) {
      Serial.println("Sensor at 0x19 started.");
    }
  }
  
  //start the TCS34725
  if ( tcs.begin() != 0 )
  {
    Serial.println("Problem starting the sensor at 0x29.");
  }
  else
  {
    Serial.println("Sensor at 0x29 started.");
  }
  
  // Connect to WiFi network
  Serial.println("\r\n");
  //Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    }
  
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

//https://forum.arduino.cc/t/char-array-to-string/531812
void charToString(const char S[], String&D)
{
  byte at = 0;
  const char *p = S;
  D = "";

  while (*p++)
  {
    D.concat(S[at++]);
  }
}

void BuildOSCForMultiplexer ()
{
  //Create an OSC message for the multiplexer
  OSCMessage msgMux("/mux");
  //Get all analogue values in turn from the multiplexer
  for (char i = 0; i < 16; i++)
  {
    int value = getAnalog(i); // Get the current analogue value of control i
    //Also print the sensor data through serial, just in case
    Serial.print(value);
    Serial.print(" ");
    //Add a parameter for this control to the message, as 32-bit int
    msgMux.add((int32_t)value);
  }

  
  // Send the message over UDP
  Udp.beginPacket(outIp, outPort); // Start of UDP packet
  msgMux.send(Udp); // send the OSC frame over UDP
  Udp.endPacket(); // End of UDP packet
  msgMux.empty();

  
  //Create an OSC message for the LIS3DH
  OSCMessage msglis("/xyz");
  Serial.print(lis.readFloatAccelX());
  Serial.print(" ");
  Serial.print(lis.readFloatAccelY());
  Serial.print(" ");
  Serial.print(lis.readFloatAccelZ());
  Serial.print(" ");

  //1 of the 3 analog pins that are on the LIS3DH, for the IR sensor. If you need the other 2 pins, then copy these two lines and add a "ADC2" and "ADC3".
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
}

void BuildOSCForTCS (uint16_t* clear, uint16_t* red, uint16_t* green, uint16_t *blue)
{
    //Create an OSC message for the TCS34725
  OSCMessage msgtcs("/tcs");
  tcs.setInterrupt(false);
  delay(50); // takes 50ms to read
  tcs.getRawData(red, green, blue, clear);
  tcs.setInterrupt(true);
  Serial.print(*red, DEC);
  Serial.print(" ");
  Serial.print(*green, DEC);
  Serial.print(" ");
  Serial.print(*blue, DEC);
  Serial.print(" ");
  Serial.print(*clear, DEC);
  //very last serial print needs to be a Serial.println. That way it prints properly in Max.
  Serial.println(" ");
  msgtcs.add((int32_t)*red);
  msgtcs.add((int32_t)*green);
  msgtcs.add((int32_t)*blue);
  msgtcs.add((int32_t)*clear);
  Udp.beginPacket(outIp, outPort);
  msgtcs.send(Udp);
  Udp.endPacket();

  msgtcs.empty();
}

void ReadOSC ()
{
  // from https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/udp-examples.html
  char incomingPacket[128];
  
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    //Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    int len = Udp.read(incomingPacket, 255);

    String incomingPacketAsString;
    charToString(incomingPacket, incomingPacketAsString); //convert from array of numbers into 'String' type

    if (incomingPacketAsString == "/brightness") //does the packet start with /brightness?
    {
      //read brightness;
      LEDBrightness = incomingPacket[len - 1];
    }
    else if (incomingPacketAsString == "/ESP_IP")
    {
      sendIPOnSerial = incomingPacket[len - 1];
    }
    else if (incomingPacketAsString == "/example") //When adding a new setting, add a new else if case, and use this condition for the identifier
    {
      //This assumes that the data you are sending is a number between 0/255. If
      //greater, will need to bitshift multiple bytes into single int.
      int example = incomingPacket[len - 1];
    }
  }
}

void loop()
//sends the ESPs IP address to Max so that we know what it is. Then we can send the board a 1 to make it send its IP or 0 to make it stop sending its IP
{
  if (sendIPOnSerial)
  {
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    OSCMessage msgGlobeIP("/GLOBE_IP");

    msgGlobeIP.add(WiFi.localIP()[0]);
    msgGlobeIP.add(".");
    msgGlobeIP.add(WiFi.localIP()[1]);
    msgGlobeIP.add(".");
    msgGlobeIP.add(WiFi.localIP()[2]);
    msgGlobeIP.add(".");
    msgGlobeIP.add(WiFi.localIP()[3]);
    
    Udp.beginPacket(outIp, outPort);
    msgGlobeIP.send(Udp);
    Udp.endPacket();
  }

  uint16_t clear, red, green, blue;
  
  BuildOSCForMultiplexer();

  BuildOSCForTCS(&clear, &red, &green, &blue);

  ReadOSC();

  //it helps convert RGB colors to what humans see
  for (int i = 0; i < 256; i++) {
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
  //if the TCS34725 senses ambient light, then do function1, which makes the LEDs sustain the
  //last sensed colour

  int lightThreshold = 1000;
  
  SetLEDBrightness();
  
  if (clear <= lightThreshold) {
    SetLEDBrightness();
    //SustainLEDColourFunction();
    colorWipe(strip_a.Color(gammatable[(int)storeRed], gammatable[(int)storeGreen], gammatable[(int)storeBlue]), 0);
    //Serial.printf("red %d, blue %d, green %d\n", storeRed, storeBlue, storeGreen);
  }
  
  //if the clear of the TCS34725 is above 1000, then a colour is being pressed close to it, so then do
  //function2. This makes the LEDs re-sample their colour.
  else {
    SetLEDBrightness();
    //Serial.println();
    colorWipe(strip_a.Color(gammatable[(int)r], gammatable[(int)g], gammatable[(int)b]), 0);
    storeRed = r;
    storeGreen = g;
    storeBlue = b;
  }

  while (0 != Udp.parsePacket()) //throw away all the extra plates
  {
    
  }
}


// Get a 10-bit integer for the mux input
int getAnalog(int MUXyPin)
{
  digitalWrite(MUXPinS3, HIGH && (MUXyPin & B00001000));
  digitalWrite(MUXPinS2, HIGH && (MUXyPin & B00000100));
  digitalWrite(MUXPinS1, HIGH && (MUXyPin & B00000010));
  digitalWrite(MUXPinS0, HIGH && (MUXyPin & B00000001));
  return analogRead(A0);
}


// Fill the LEDs one after the other with a color
void colorWipe(uint32_t c, uint8_t wait)
{
  for (uint16_t i = 0; i < strip_a.numPixels(); i++) {
    strip_a.setPixelColor(i, c);
  }
  strip_a.show(); //adafruit support said to put this line out of the "for" loop to ensure the data prints quickly, and it works!
}


//SustainLEDColourFunction function (func1). Keep the last sensed color when the sensor reading is ambient light.
void SustainLEDColourFunction() {
  uint32_t color;
  strip_a.getPixelColor(0);
}


//SetLEDBrightness function (func2).
void SetLEDBrightness()
{
  //Serial.println(LEDBrightness);
  strip_a.setBrightness(LEDBrightness); //adjust brightness here
  strip_a.show();
}
