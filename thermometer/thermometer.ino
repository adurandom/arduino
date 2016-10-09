/*
       +--------------------------------------------------------------+
       |                  (c) 2016, Antonio Duran                     |
       |                  antonio.duran@gmail.com                     |
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
 |    Filename          : thermometer.ino
 |    File Kind         : Arduino Source File
 |    Author            : Antonio Duran
 |    Creation Date     : 09/10/2016
 |    Current Version   : 1.0
 +------------------------------------------------------------------------------
 | 2. Purpose:
 |    Implemets a digital thermometer. Uses a LM35 temperature sensor and a 8
 |    segment 4-digit LCD (3641BS) to display temperature. The limits are that 
 |    is not possible to dispaly negative temperatures.
 +------------------------------------------------------------------------------
 | 3. Revision History:
 |    Ver   When     Who      Why
 |    ----- -------- -------- --------------------------------------------------
 |    1.0   20161009 ADD      Initial implementation.
 +------------------------------------------------------------------------------
*/

/*
 +-----------------------------------------------------------------------------
 | Constants
 +-----------------------------------------------------------------------------
 */

/*  SEGMENTS 
 *  Number of LCD segments in each digit of the display (excluding the decimal 
 *  point segment).
 */
 
const int   SEGMENTS            = 7;

/*  SEG_PINS
 *  Array containig the Arduino pins connected to each display segment (A - G).
 */
 
const int   SEG_PINS[SEGMENTS]  = {2, 3, 4, 5, 6, 7, 8};

/*  DP_PIN
 *  Arduino pin connected to the decimal point segment of the display.
 */
 
const int   DP_PIN              = 9;

/*  DIGITS
 *  Number of digits in the LCD display.
 */
 
const int   DIGITS              = 4;

/*  DIGIT_PINS
 *  Arduino pins connected to each digit line in the display.
 */
 
const int   DIG_PINS[DIGITS]    = {10, 11 , 12, 13};

/*  TEMP_SENSOR
 *  Arduino pin connected to LM35 Vout pin.
 */
 
const int   TEMP_SENSOR         = A0;

/*  SYMBOL_COUNT
 *  Number of symbols to display (10 digits + the C letter).
 */
 
const int   SYMBOL_COUNT        = 11;

/*  C_LETTER
 *  Index of the C letter symbol.
 */
 
const int   C_LETTER            = 10;

/*  SYMBOLS
 *  The symbol table, each array in the table contains the pattern for
 *  representing a symbol 0 means the segment should be ON (voltage for
 *  corresponding pin shold be set to LOW) and 1 means that the segment
 *  should be off (pin voltage set to HIGH). The display is common 
 *  anode.
 */
const int   SYMBOLS[SYMBOL_COUNT][SEGMENTS] = {
  {0, 0, 0, 0, 0, 0, 1},              //| 0
  {1, 0, 0, 1, 1, 1, 1},              //| 1
  {0, 0, 1, 0, 0, 1, 0},              //| 2
  {0, 0, 0, 0, 1, 1, 0},              //| 3
  {1, 0, 0, 1, 1, 0, 0},              //| 4
  {0, 1, 0, 0, 1, 0, 0},              //| 5
  {0, 1, 0, 0, 0, 0, 0},              //| 6
  {0, 0, 0, 1, 1, 1, 1},              //| 7
  {0, 0, 0, 0, 0, 0, 0},              //| 8
  {0, 0, 0, 0, 1, 0, 0},              //| 9
  {0, 1, 1, 0, 0, 0, 1}               //| C
};

/*  ITERATIONS
 *  Number of iterations to perform in the display loop.
 */
 
const int   ITERATIONS          = 50;

/*  DELAY
 *  Delay after displaying each LCD digit.
 */
 
const int   DELAY               = 5;

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

  //| Initialize pins for display segments.
  
  for (int i = 0; i < SEGMENTS; i++) {
    pinMode(SEG_PINS[i], OUTPUT);
  }

  //| Initialize pin for Decimal Point segment.
  
  pinMode(DP_PIN, OUTPUT);

  //| Initialize pins for display digits.
  
  for (int i = 0; i < DIGITS; i++) {
    pinMode(DIG_PINS[i], OUTPUT);
  }
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
  
  //| Display the temperature obtained from the sensor.
  
  displayTemperature(getTemperature());
}

/*  getTemperature
 *  Reads from the LM35 sensor and obtains and returns the 
 *  temperature in Celsius degrees.
 *  
 *  Arguments
 *  None (void)
 *  
 *  Returns
 *  Float value with the temperature in celsius degrees.
 */

float getTemperature() {
  
  //| Read sensor.

  int sensorRead  = analogRead(TEMP_SENSOR);

  //| Convert reading to voltage. analogRead returns a value in the 
  //| 0 - 1023 range that corresponds to 0 - 5 volts range.

  float volts = (sensorRead / 1024.0) * 5.0;

  //| Convert volts to temperature. According to LM35 specs, the 
  //| Output voltage of the sensor is a lineal function of the
  //| temperature: T = mV * 10.
  
  float temp = volts * 100;

  //| Return the computed temperature.
  
  return temp;
}

/*  displayTemperature
 *  Displays the temperature.
 *  
 *  Arguments
 *  temp          Float value with the temperaute to display.
 *  
 *  Return value
 *  None (void).
 */

void displayTemperature(float temp) {

  //| Separate the integer and decimal parts into integer values.
  //| Get two digits for each part.
  
  int int_part  = int(temp);
  int dec_part  = int(temp * 100) % 100;

  //| Get the four individual digits of the temperature.
  
  int digits[4];
  digits[0] = int_part / 10;
  digits[1] = int_part % 10;
  digits[2] = dec_part / 10;
  digits[3] = dec_part % 10;

  //| Approximate to the 0.1 celsius degree.
  
  if (digits[3] >= 5) {
    digits[3] = 0;
    digits[2]++;

    if (digits[2] == 10) {
      digits[2] = 0;
      digits[1]++;

      if (digits[1] == 10) {
        digits[1] = 0;
        digits[0]++;
      }
    }
  }

  //| Display loop. This loop will display in turn each 
  //| temperature digit. Since segments are multiplexed, each
  //| segment shall be displayed in turn. Some flickering could
  //| happen.
  //|
  //| The constants ITERATIONS and DELAY must be adjusted so 
  //| to minimize the flickering and to spaciate the temperature
  //| sensor readings.
  
  for (int i = 0; i < ITERATIONS; i++) {
    displaySymbol(0, digits[0], 0);
    delay(DELAY);
    displaySymbol(1, digits[1], 1);
    delay(DELAY);
    displaySymbol(2, digits[2], 0);
    delay(DELAY);
    displaySymbol(3, C_LETTER, 0);
    delay(DELAY);
  }
}

/*  displaySymbol
 *  Displays the specified symbol into a specific digit of the
 *  display.
 *  
 *  Arguments
 *  digit         Integer. The digit to display the symbol in.
 *  symbol        Integer. The index to the symbol table of the 
 *                symbol to display.
 *  dp            Integer. Flag to indicate whether or not the decimal
 *                point shall be OFF (0) or ON (any other value).
 *  
 *  Returns
 *  None (void)
 */
 
void displaySymbol(int digit, int symbol, int dp) {

  //| Check arguments are between ranges.
  
  if (digit < 0 || digit >= DIGITS) {
    return;
  }

  if (symbol < 0 || symbol > SYMBOL_COUNT) {
    return;
  }

  //| Setup display digits. Selected digit pin output shall be
  //| set to HIGH, all other digit pins are set to LOW.
  
  for (int i = 0; i < DIGITS; i++) {
    if (i == digit) {
      digitalWrite(DIG_PINS[i], HIGH);      
    }
    else {
      digitalWrite(DIG_PINS[i], LOW);            
    }
  }

  //| Set segment pints according to symbol template.
  
  for (int i = 0; i < SEGMENTS; i++) {
    digitalWrite(SEG_PINS[i], SYMBOLS[symbol][i] ? HIGH : LOW); 
  }

  //| Set decimal point as instructed.
  
  if (dp) {
    digitalWrite(DP_PIN, LOW);
  }
  else {
    digitalWrite(DP_PIN, HIGH);
  }
}

