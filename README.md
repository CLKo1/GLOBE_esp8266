# GLOBE_esp8266
* This repository is for the ESP8266 code for the GLOBE project. It has for a 16 channel mux, TCS34725, LIS3DH, IR sensor, and colour changing LEDs. The sensor data is sent to Max MSP through OSD messaging.

* You can learn more about the GLOBE here: https://www.chantelleko.com/masters-thesis.html 

* The first code posted was what I used for my thesis defence. I may make some minor changes to the names of functions soon, but the overall code for how it works likely will not change in the near future. 

* However, I do have an idea to modify the code so not only does the ESP8266 send its data out to Max MSP, but it also listens to OSC messages from Max. I may not get around to seeing if I can effectively add this feature for a while still.  

* Oct 2 2021 Update: 
I have been working with my life partner, Kevin Mitchell, on improvements. He has made the GLOBE send its IP address to Max MSP. This way we know what IP to send data to in order to control the LEDs. This can be turned on/off by sending the ESP a 1/0 from Max. Kevin also added the ability to accept messages from Max to control LED brightness. 

*  Dec 19, 2021 Update: 
There was a bug that Kevin fixed. In the Oct 2021 version, the ESP was refusing to print any data through serial unless it firsts connects to the wireless network. Now it behaves the way it did before where you can easily use wither wifi or serial. Kevin also worked on optimization. Now the loop prints much faster than previously, which is great for musical instruments. The Max Patch that is in the Oct 19 folder should still be used with this updated Arduino Sketch. 

* Jan 2, 2022 Update: We updated both the Arduino Sketch and the Max MSP patch so that if the wireless connection fails, or if the battery dies, the brightness messages being sent from Max MSP can be received by the ESP via serial. This was the last thing that needs updating so that the GLOBE can be used entirely wirelessly, or plugged in, without having to re-upload a new Arduino sketch. 
