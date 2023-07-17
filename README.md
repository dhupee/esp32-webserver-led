# esp32-webserver-led

Credit always goes to the original blog of [LastMinuteEngineer](https://lastminuteengineers.com/creating-esp32-web-server-arduino-ide/) on Create A Simple ESP32 Web Server In Arduino IDE with some editing to make me comfortable to read it.

His code does have two version, for In Station Mode(STA Mode) where ESP32 actually connects with WiFi Router, and Access Point Mode(AP Mode) where the ESP32 actually is the WiFi Router, also add mDNS since i will need it at some point idk.

What I do merely adjust the code format to suit PlatformIO, which is the IDE I use, and combine two versions so I dont have 2 seperate code running the same thing.

and i added additional comments, so i dont forget.

This code is gonna be useful for local webserver or API-like control.

personal note: Check your broadband, mDNS might be suck depend on it
