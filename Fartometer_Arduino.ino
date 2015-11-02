/*

Copyright (c) 2015 Ideia do Luiz

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

/*
 *    Fartometer
 *
 *    Using simple chat sketch from RedBearLab, work with the Chat iOS/Android App.
 *    Get the command from the app and handle it.
 *    After that, it sends an answer.
 *
 */

//"RBL_nRF8001.h/spi.h/boards.h" is needed in every new project
#include <SPI.h>
#include <EEPROM.h>
#include <boards.h>
#include <RBL_nRF8001.h>

#define EMPTY_VALUE ""
#define MAX_SIZE_CMD 16
#define METHANE_LIMIT 100
#define SOUND_LIMIT 0

// commands that will come from the external device
#define CMD_REQUEST_READ_SENSORS "checksensors"
#define CMD_REQUEST_STOP_READING_SENSORS "stopchecking"

#define CMD_ANSWER_METHANE "methane:"
#define CMD_ANSWER_SOUND "sound:"

// change it here for your methane sensor pin number
#define METHANE_ANALOG_PIN A0
// change it here for your sound sensor pin number
#define SOUND_ANALOG_PIN A2

// All my messages should be shorther than MAX_SIZE_CMD characters.
// The (MAX_SIZE_CMD-1)th character is the null terminator, always.
char msg[MAX_SIZE_CMD] = {0};
bool canReadSensors = false;

void setup()
{  
  // Set your BLE Shield name here, max. length 10
  ble_set_name("Fartometer");
  
  // Init. and start BLE library.
  ble_begin();
  
  // Enable serial debug
  Serial.begin(57600);
}

void loop()
{ 
  String answer = readData();
  
  if (answer == CMD_REQUEST_READ_SENSORS)
    canReadSensors = true;
  else if (answer == CMD_REQUEST_STOP_READING_SENSORS)
    canReadSensors = false;

  if (canReadSensors)
  {
    // commenting this line for testing purposes
    //if (analogRead(METHANE_ANALOG_PIN) > METHANE_LIMIT)
      writeData(CMD_ANSWER_METHANE + String(analogRead(METHANE_ANALOG_PIN)));
  }
  
  ble_do_events();
}

// read data that comes from the external device
String readData()
{
  if (ble_available())
  {
    int i = 0;
    while (ble_available())
    {
      if (i < MAX_SIZE_CMD)
      {
        msg[i] = ble_read();
        i++;
      }
      else
      {
        // ignores all others characters
        ble_read();
      }
    }

    // assign the char
    // We don't need the '\0', because we already did it
    String currentCommand(msg);
    
    // clear msg buffer
    for (i = 0; i < MAX_SIZE_CMD; i++)
      msg[i] = 0;
    
    return currentCommand;
  }

  return EMPTY_VALUE;
}

// write data that is going to be handled by the external device
void writeData(String data)
{
  if (data == EMPTY_VALUE)
    return;
    
  delay(5);
  byte dataByte[MAX_SIZE_CMD] = {0};
  data.getBytes(dataByte, MAX_SIZE_CMD);
  ble_write_bytes(dataByte, MAX_SIZE_CMD);
}

