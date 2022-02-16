{
	"patcher" : 	{
		"fileversion" : 1,
		"appversion" : 		{
			"major" : 8,
			"minor" : 2,
			"revision" : 1,
			"architecture" : "x64",
			"modernui" : 1
		}
,
		"classnamespace" : "box",
		"rect" : [ 529.0, 250.0, 859.0, 504.0 ],
		"bglocked" : 0,
		"openinpresentation" : 0,
		"default_fontsize" : 12.0,
		"default_fontface" : 0,
		"default_fontname" : "Arial",
		"gridonopen" : 1,
		"gridsize" : [ 15.0, 15.0 ],
		"gridsnaponopen" : 1,
		"objectsnaponopen" : 1,
		"statusbarvisible" : 2,
		"toolbarvisible" : 1,
		"lefttoolbarpinned" : 0,
		"toptoolbarpinned" : 0,
		"righttoolbarpinned" : 0,
		"bottomtoolbarpinned" : 0,
		"toolbars_unpinned_last_save" : 15,
		"tallnewobj" : 0,
		"boxanimatetime" : 200,
		"enablehscroll" : 1,
		"enablevscroll" : 1,
		"devicewidth" : 0.0,
		"description" : "",
		"digest" : "",
		"tags" : "",
		"style" : "",
		"subpatcher_template" : "",
		"assistshowspatchername" : 0,
		"boxes" : [ 			{
				"box" : 				{
					"fontsize" : 10.0,
					"id" : "obj-7",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 30.0, 438.0, 506.0, 30.0 ],
					"text" : "This Bpachter was made by Chantelle Ko. www.chantelleko.com \n"
				}

			}
, 			{
				"box" : 				{
					"comment" : "",
					"hidden" : 1,
					"id" : "obj-4",
					"index" : 0,
					"maxclass" : "inlet",
					"numinlets" : 0,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 30.0, 19.0, 30.0, 30.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-2",
					"linecount" : 25,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 30.0, 57.666666626930237, 519.0, 342.0 ],
					"text" : "This bpatcher is used to transfer sensor data from an ESP8266 to Max MSP. There are 24 sensor values being transfered. It has both a UDP option and through the Serial port option. Select which option you would like to use with the blue and purple LED buttons. \n\nSerial: \nWhen the purple LED is selected, it should automatically start the serial port. If it does not work follow these steps: \n-check the cable is plugged in correctly\n-click the black bang button above the umenu, it will print the available serial ports in the Max console. Make sure the port letter name for the serial object and the port name in the umenu are corect, then turn the toggle off then on again. \n\nThe slider next to the umenu controls the sampling rate. It is an added feature in case if it is needed, but otherwise is not necessary to adjust. \n\nUDP:\nWhen the blue LED is selected, it should automatically start reading the sensor values through UDP. If it does not work follow these steps: \n-check the WiFi connection \n-check that your computer has the proper IP address that the ESP8266 is assigned to \n-check that the port number is correct, if not type it into the Port textedit object. \n\nNote, when the IR sensor starts to be more unstable than usual, that is a sign that the battery needs charging. For example, when nothing is in its view, it should rest at 4. And if the battery is low it flickers between 4 and a higher number. "
				}

			}
 ],
		"lines" : [  ],
		"dependency_cache" : [  ],
		"autosave" : 0
	}

}
