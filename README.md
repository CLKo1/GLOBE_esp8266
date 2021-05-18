# GLOBE_esp8266
This repository is for the ESP8266 code for the GLOBE project. It has for a 16 channel mux, TCS34725, LIS3DH, IR sensor, and colour changing LEDs. The sensor data is sent to Max MSP through OSD messaging.
You can learn more about the GLOBE here: https://www.chantelleko.com/masters-thesis.html 
The first code posted was what I used for my thesis defence. I may make some minor changes to the names of functions soon, but the overall code for how it works likely will not change in the near future. 
However, I do have an idea to modify the code so not only does the ESP8266 send its data out to Max MSP, but it also listens to OSC messages from Max. I may not get around to seeing if I can effectively add this feature for a while still. 
