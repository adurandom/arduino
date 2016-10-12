/*
       +--------------------------------------------------------------+
       |                   (c) 2016, dotorqantico                     |
       |                  alki1963-github@yahoo.com                   |
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
 |    Filename          : meteo.ino
 |    File Kind         : Arduino Source File
 |    Author            : Dotorqantico
 |    Creation Date     : 12/10/2016
 |    Current Version   : 1.0
 +------------------------------------------------------------------------------
 | 2. Purpose:
 |    Prototype simple meteo station that performs temperature, barometric
 |    pressure, and humidity measurements. This meteo will use a DHT11 sensor
 |    for humidity and a BMP180 sensor for barometric pressure and temperature.
 |    Temperature, barometric pressure and humidity are displayed on a
 |    1602 LCD display.
 +------------------------------------------------------------------------------
 | 3. Revision History:
 |    Ver   When     Who      Why
 |    ----- -------- -------- --------------------------------------------------
 |    1.0   20161012 ADD      Initial implementation.
 +------------------------------------------------------------------------------
*/

/*
 +-----------------------------------------------------------------------------
 | Includes
 +-----------------------------------------------------------------------------
 */

#include <Wire.h>
#include <LiquidCrystal.h>
#include <SFE_BMP180.h>
#include <DHT.h>

/*
 +-----------------------------------------------------------------------------
 | Constants
 +-----------------------------------------------------------------------------
 */

//| LCD pins to arduino pins.

const int   PIN_RS              = 7;
const int   PIN_ENABLE          = 8;
const int   PIN_D4              = 9;
const int   PIN_D5              = 10;
const int   PIN_D6              = 11;
const int   PIN_D7              = 12;

//| Pin to connect the DHT11 to.

const int   DHT_PIN             = 5;

//| Push button pin

const int   BUTTON_PIN          = 4;

//| LCD display dimensions.

const int   ROWS                = 2;
const int   COLS                = 16;

//| Degree symbol for the display.

const byte  DEGREE_SYMBOL[8] = {
  B01000,
  B10100,
  B10100,
  B01000,
  B00000,
  B00000,
  B00000,
};

//| Next constants define the display mode.
//| CURRENT_READINGS        Display current readings.
//| MAX_READINGS            Display maximum values.
//| MIN_READINGS            Display minimum values.

const int   CURRENT_READINGS    = 0;
const int   MAX_READINGS        = 1;
const int   MIN_READINGS        = 2;

//| displayTime
//| Time, in ms, to display each page of information. 

const long  displayTime         = 2000;

//| debounceDelay
//| Delay in reading button state to avoid false positives.

const long  debounceDelay       = 50;

/*
 +-----------------------------------------------------------------------------
 | Globals
 +-----------------------------------------------------------------------------
 */

//| lcd
//| Object that represents the LCD display.

LiquidCrystal lcd(PIN_RS, PIN_ENABLE, PIN_D4, PIN_D5, PIN_D6, PIN_D7);

//| bmp180
//| Object that represents the BMP180 temperature and barometric pressure 
//| sensor.

SFE_BMP180 bmp180;

//| dht
//| Object that represents the DHT11 humidity sensor.

DHT dht(DHT_PIN, DHT11);

//| Current meteo values.

double  temperature;
double  pressure;
float   humidity;

//| Maximum and Minimum values.

double  min_temp          = 100.0;
double  max_temp          = -100.0;
double  min_pres          = 2000.0;
double  max_pres          = 0.0;
float   max_humi          = 0.0;
float   min_humi          = 100.0;

//| state
//| The state of display.

int   state               = CURRENT_READINGS;

/*
 +-----------------------------------------------------------------------------
 | Functions
 +-----------------------------------------------------------------------------
 */

/*  setup
 *  Perform initializations. 
 *  
 *  Arguments
 *  None (void)
 *  
 *  Return value
 *  None (void)
 */
 
void setup() {

  //| Initialize LCD.
  
  lcd.begin(COLS, ROWS);
  lcd.createChar(0, DEGREE_SYMBOL);
  lcd.clear();

  //| Initialize button pin.

  pinMode(BUTTON_PIN, INPUT);
  
  //| Initialize pressure sensor. If there is an error, indicate the error
  //| condition and wait forever.

  if (!bmp180.begin()) {
    lcd.print("Error in BMP180");
    while(1);
  }

  //| Initialize humidity sensor.

  dht.begin();
}

/*  loop
 *  Main iterative sketch processing.
 *  
 *  Arguments
 *  None (void).
 *  
 *  Return value
 *  None (void).
 */
 
void loop() {

  //| Read sensors and set the globals with data.

  getMeteoData();

  //| Check if button is pushed.
  
  readButton();
  
  //| Display depending on the state.

  displayMeteoData();
}

/*  getMeteoData
 *  Gets meteorological data from the sensors. Temperature and
 *  pressure come from BMP180, humidity comes from DHT11. 
 *  
 *  Arguments
 *  None (void)
 *  
 *  Returns
 *  Char value that indicates the reading result 0 => NOK, 
 *  1 => OK.
 */

char getMeteoData() {
  char    res;
  double  temp;
  double  pres;
  
  //| Start reading temperature.
  
  res = bmp180.startTemperature();
  
  if (!res) {
    return 0;
  }
  
  //| Wait and rettrieve temperature measurement.
    
  delay(res);
  res = bmp180.getTemperature(temp);

  if (!res) {
    return 0;
  }

  //| Start reading pressure.
    
  res = bmp180.startPressure(3);
      
  if (!res) {
    return 0;
  }

  //| Wait and retrieve pressure measurement.
  
  delay(res);
  res = bmp180.getPressure(pres, temp);

  if (!res) {
    return 0;
  }

  //| Read humidity.

  humidity = dht.readHumidity();

  //| Reading is complete, update globals.

  temperature = temp;

  if (temp < min_temp) {
    min_temp = temp;
  }

  if (temp > max_temp) {
    max_temp = temp;
  }

  pressure = pres;

  if (pres < min_pres) {
    min_pres = pres;
  }

  if (pres > max_pres) {
    max_pres = pres;
  }

  if (humidity < min_humi) {
    min_humi = humidity;
  }

  if (humidity > max_humi) {
    max_humi = humidity;
  }

  return 1;
}

/*  readButton
 *  Reads button state and sets the display state accordingly.
 *  
 *  Arguments:
 *  None (void).
 *  
 *  Returned value:
 *  None (void)
 */

void readButton(void) {

  //| Read push button pin and store the reading in a local variable.
  
  int reading = digitalRead(BUTTON_PIN);

  //| If button is HIGH.

  if (reading == HIGH) {

    //| Wait and perform a second reading thus avoiding noise effects.

    delay(debounceDelay);
    reading = digitalRead(BUTTON_PIN);

    if (reading == HIGH) {
      switch (state) {
        case CURRENT_READINGS:
          state = MAX_READINGS;
          break;

        case MAX_READINGS:
          state = MIN_READINGS;
          break;

        default:
          state = CURRENT_READINGS;
          break;
      }
    }
  }
}

/*  displayMeteoData
 *  Displays data according state.
 *  
 *  Arguments:
 *  None (void).
 *  
 *  Returned value:
 *  None (void).
 */

void displayMeteoData(void) {

  //| Display depending on the state.
  
  switch (state) {
    case CURRENT_READINGS:
      displayData("Current Readings", temperature, pressure, humidity);
      break;

    case MAX_READINGS:
      displayData("Maximum Values", max_temp, max_pres, max_humi);
      break;

    default:
      displayData("Minimum Values", min_temp, min_pres, min_humi);
      break;
  }
}

/*  displayData
 *  Displays meteo data.
 *  
 *  Arguments:
 *  header        Header string.
 *  temp          Temperature
 *  pres          Barometric pressure.
 *  humi          Relative humidity.
 *  
 *  Returned value:
 *  None (void).
 */

void displayData(String header, double temp, double pres, float humi) {

  //| Separate the integer and decimal parts into integer values.
  //| Get two digits for decimal part.
  
  int tint_part   = int(temp);
  int tdec_part   = int(long(temp * 100) % 100);
  int pint_part   = int(pres);
  int pdec_part   = int(long(pres * 100) % 100);
  int hint_part   = int(humi);
  int hdec_part   = int(long(humi * 100) % 100);

  //| Display current values.

  String t;
  String p;
  String h;
  char buffer[50];
  sprintf(buffer, "Temp: %02d.%02d C", tint_part, tdec_part);
  t = String(buffer);
  sprintf(buffer, "Pres: %3d.%02d mb", pint_part, pdec_part);
  p = String(buffer);
  sprintf(buffer, "Humi: %02d.%02d %%", hint_part, hdec_part);
  h = String(buffer);

  String data[4] = {header, t, p, h};
  
  lcd.clear();
  lcd.print(data[0]);
  lcd.setCursor(0, 1);
  lcd.print(data[1]);
  delay(displayTime);
  lcd.clear();
  lcd.print(data[1]);
  lcd.setCursor(0, 1);
  lcd.print(data[2]);
  delay(displayTime);
  lcd.clear();
  lcd.print(data[2]);
  lcd.setCursor(0, 1);
  lcd.print(data[3]);
  delay(displayTime);
}


