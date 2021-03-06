# GLOBE_esp8266
* This repository is for the ESP8266 code for the GLOBE project. It has for a 16 channel mux, TCS34725, LIS3DH, IR sensor, and colour changing LEDs. The sensor data is sent to Max MSP through OSD messaging.

* You can learn more about the GLOBE here: https://www.chantelleko.com/masters-thesis.html 

* The first code posted was what I used for my thesis defence. I may make some minor changes to the names of functions soon, but the overall code for how it works likely will not change in the near future. 

* However, I do have an idea to modify the code so not only does the ESP8266 send its data out to Max MSP, but it also listens to OSC messages from Max. I may not get around to seeing if I can effectively add this feature for a while still.  

* *************************************************************************************************

* Oct 2 2021 Update: 
I have been working with my life partner, Kevin Mitchell, on improvements. He has made the GLOBE send its IP address to Max MSP. This way we know what IP to send data to in order to control the LEDs. This can be turned on/off by sending the ESP a 1/0 from Max. Kevin also added the ability to accept messages from Max to control LED brightness. 

*  Dec 19, 2021 Update: 
There was a bug that Kevin fixed. In the Oct 2021 version, the ESP was refusing to print any data through serial unless it firsts connects to the wireless network. Now it behaves the way it did before where you can easily use wither wifi or serial. Kevin also worked on optimization. Now the loop prints much faster than previously, which is great for musical instruments. The Max Patch that is in the Oct 19 folder should still be used with this updated Arduino Sketch. 

* Jan 2, 2022 Update: We updated both the Arduino Sketch and the Max MSP patch so that if the wireless connection fails, or if the battery dies, the brightness messages being sent from Max MSP can be received by the ESP via serial. This was the last thing that needed updating so that the GLOBE can be used entirely wirelessly, or plugged in, without having to re-upload a new Arduino sketch. It can also be easily modified if we want the RGB messages to be sent from Max as well.  

* Jan 17, 2022: In the previous versions the colour data was sent directly from the TCS34725 colour sensor to the LEDs. This meant no matter what, as long as some sort of colour is pressed against the sensor, you will see it. However, in Max MSP, it is a bit more sensitive to the exact shade of colour and angle the colour is pressed against the sensor. Therefore, if lets say I am using a green sample, the LEDs will appear green right away, but it might not quite be the green that the Max Patch is programmed to trigger the "green" message. It would take slight adjustments for the performer to do to get it accurate. It is the equivalent of have a violin that no matter what played perfect pitch even if the fingers are technically out of tune, and everything else is following the out of tune fingers. 

  Long story short, we changed the Arduino sketch so that the RGB and brightness values are all sent from Max MSP to control the LEDs. This way everything is in  sync and the LEDs truly reflect what the performer is both hearing and seeing. This makes it easier for the performer to use this feedback to adjust and correct their own movements in real time. This opens up more possibilities for the LEDs to be audio reactive as well. I included a screenshot example for how this is hooked up in Max MSP. 

  In previous versions the RGB values outputted from the TCS34725 colour sensor were also well over 500. In the past I never questioned it because it was copied from an example sketch and as long as I was receiving something I thought it was fine. As I have been working more deeply with colour lately, I realized that can't be right - typically RGB values are only between 0-255. Kevin and I went over the code and we found where the issue was. So now the values sent from the sensor to Max are correct. I had updated my colour detection Max patch to reflect this. 

If you experience any issues or have any questions, please let me know: https://www.chantelleko.com/contact.html
