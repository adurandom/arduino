/*
       +--------------------------------------------------------------+
       |                (c) 2016, TCantos Software                    |
       |                     aduran@tcantos.com                       |
       +--------------------------------------------------------------+
       | This  program  is  free  software:  you  can redistribute it |
       | and/or  modify it under the terms of the  GNU General Public |
       | License as published by the Free Software Foundation, either |
       | version  3  of  the License,  or (at your option)  any later |
       | version.                                                     |
       |                                                              |
       | This  program  is  distributed in  the hope  that it will be |
       | useful,  but WITHOUT ANY WARRANTY;  without even the implied |
       | warranty  of  MERCHANTABILITY  or  FITNESS FOR A  PARTICULAR |
       | PURPOSE.   See  the  GNU  General  Public  License for  more |
       | details.                                                     |
       |                                                              |
       | You  should  have received  a copy of the GNU General Public |
       | License    along    with    this    program.    If not,  see |
       | <http://www.gnu.org/licenses/>.                              |
       +--------------------------------------------------------------+

 +------------------------------------------------------------------------------
 | 1. Identification:
 |    Filename          : ESP8266-0.ino
 |    File Kind         : Arduino Source
 |    Author            : A. Duran
 |    Creation Date     : December, 7th 2016
 |    Current Version   : 1.0
 +------------------------------------------------------------------------------
 | 2. Purpose:
 |    Simple sketch to test Arduino WiFi connectivity through ESP8266 module.
 +------------------------------------------------------------------------------
 | 3. Revision History:
 |    Ver   When     Who      Why
 |    ----- -------- -------- --------------------------------------------------
 |    1.0   20161207 ADD      Initial implmentation.
 +------------------------------------------------------------------------------
*/

#include <SoftwareSerial.h>

/*
 * Arduino transmission and reception pins. TX_PIN shall be connected to 
 * ESP8266 RXD pin and RX_PIN to TXD pin of ESP8266. Connections are made
 * through a logical level converter since ESP8266 works @ 3.3V and 
 * Arduino uses 5V.
 */
 
#define TX_PIN    2
#define RX_PIN    3

/*
 * Baud rates for console and software serial communication with ESP8266.
 * Initially we'll use 9600 baud.
 */
 
#define SERIAL_BAUD_RATE    9600L
#define ESP8266_BAUD_RATE   9600L

/*
 * Object that represent the serial communication channel with ESP8266.
 */
 
SoftwareSerial esp8266(RX_PIN, TX_PIN); 

/*  setup
 *  Sets up Arduino.
 *  
 *  Arguments:
 *  None.
 *  
 *  Returned value:
 *  None.
 */
 
void setup() {
  
  // Initialize serial console.
  
  Serial.begin(SERIAL_BAUD_RATE);

  while (!Serial) {
    ;
  }

  Serial.println("Serial console initialized");

  // Initialize ESP8266 software serial communication.
  
  esp8266.begin(ESP8266_BAUD_RATE);
}

/*  loop
 *  Main program loop.
 *  
 *  Arguments:
 *  None.
 *  
 *  Returned value:
 *  None.
 */
 

void loop() {
  
  // Create a string object to hold data read.
  
  String data = "";

  // If there is any input available at ESP8266 read it and send it
  // to console. Get individual characters and append them to the string
  // object until no more data is available. Then print the string to 
  // console.
  
  if (esp8266.available()) {
    while (esp8266.available()) {
      char c = esp8266.read();
      data = data + c;
    }

    Serial.print(data);
    data = "";        
  }

  // If there is input available in console read it and send it
  // to ESP8266. Get individual chars and append to the string object.
  // When sending to ESP8266, Serial takes care of \r\n.
  
  if (Serial.available()) {
    while (Serial.available()) {
      char c = Serial.read();
      data = data + c;
    }

    esp8266.print(data);
    data = "";
  }
}
