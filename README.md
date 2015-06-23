## ESP8266-AT-examples

Several examples for ESP8266 wifi modules with 'ai-thinker.com' AT command firmware version 0.9.2.4

###Notes
* Actual esp8266 library is under `common` folder and it is *almost* hardware indipendent. 
* Key idea for the library is: you have a relatively large *ring buffer* for storing the uart messages coming from module for later analysis. Therefore you can't miss any messages up to certain degree.
* Hardware dependent part of the library is `esp8266_hal.c` file and it can be found under example folders.
* `esp8266_hal_rebootSystem()` method is used to reboot the whole system in case anything fails. It is utterly useful for *fault tolerance*. For example; wifi might go offline and transmisson error happens. In order to maintain functionality, system reboots itself and tries to re-connect to the wifi network and all. This may not be desired in every situation but it is better than nothing.

