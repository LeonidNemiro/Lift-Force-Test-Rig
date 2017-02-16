
#include <Servo.h>


//for servo control
Servo myServo1;
Servo myServo2;
long servo1Value = 50;
long servo2Value = 50;


//for reading load cell
#include "HX711.h"
#define DOUT  16
#define CLK  10
HX711 scale(DOUT, CLK);
float scaleCalFactor1 = 0.00003f; //lbs per count
float scaleCalFactor2 = 1.0f;  // less gain for second differential input
float scaleOffset1 = 0.0f;          //lbs
float scaleOffset2 = 0.0f;
float scaleValue1 = 0.0f;           //lbs
float scaleValue2 = 0.0f;
boolean scale1 = false;


//for serial com
byte inData[6];  //data read
byte inByte = 0;  // byte read
byte counter;
String inputString = "";
boolean stringComplete = false;


void setup() {

  //set pins for servos
  myServo1.attach(5);
  myServo2.attach(6);

  //set pins for scale
  scale.set_scale();
  scale.tare(); //Reset the scale to 0

  Serial.begin(9600);  //baud doesnt matter for pro micro
  //analogReference(INTERNAL);

  //wait for serial port to open
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  inputString.reserve(200);

}//setup




void loop() {

  //wait for serial commands
  checkSerial();
  //delay(300);
  //Serial.println(millis());


  //if scale ADC is ready, read
  if (scale.is_ready())
  { 
    scaleValue1 = scale.read() * scaleCalFactor1 - scaleOffset1;

//check both inputs of scale using hx711
//    if (scale1) {
//      scaleValue1 = scale.read() * scaleCalFactor1 - scaleOffset1;
//      scale.set_gain(128);
//      scale1 = false;
//
//    } 
//    
//    if (!scale1) {
//    
//      scaleValue2 = scale.read() * scaleCalFactor2 - scaleOffset2;
//      scale.set_gain(32);
//      scale1 = true;
//    }
    
  }//scale reading


  //reset string
  if (stringComplete) {
    //Serial.println(inputString);
    // clear the string:
    inputString = "";
    stringComplete = false;
  }

}//loop


void checkSerial() {

  //check for char in buffer
  while (Serial.available()) {
    delay(1);

    //create new string
    char inChar = (char)Serial.read();
    if ((inChar == ' ') || (inChar == '\n') || (inChar == '\r') || (inputString.length() > 200))
    { stringComplete = true;
      //Serial.println("string Completed");
    }
    else {
      inputString += inChar;
      //Serial.println(inChar);
      //Serial.println(inputString);
    }//while to concatonate string

    if (inputString.compareTo("hello") == 0) {

      stringComplete = true;
      goto readSomething;
    }// if hello

    if (inputString.compareTo("setServoOnePos") == 0) {
      long temp = 0;
      //process 3 bytes for first output
      for (int i = 0; i < 4; i++) {
        temp  = temp * 10 + (Serial.read() - 48);
      }

      servo1Value = temp;
      myServo1.writeMicroseconds(temp);
      stringComplete = true;
      goto readSomething;
    }// if setServoOne

    if (inputString.compareTo("setServoTwoPos") == 0) {
      long temp = 0;
      //process 3 bytes for first output
      for (int i = 0; i < 4; i++) {
        temp  = temp * 10 + (Serial.read() - 48);
      }

      servo2Value = temp;
      myServo2.writeMicroseconds(temp);
      stringComplete = true;
      goto readSomething;
    }// if setServoTwo

    if (inputString.compareTo("printVals") == 0) {
      Serial.print(millis());
      Serial.print(",");
      Serial.print(scaleValue1);
      Serial.print(",");
      Serial.print(scaleValue2);
      Serial.print(",");
      Serial.print(servo1Value);
      Serial.print(",");
      Serial.println(servo2Value);
      stringComplete = true;
      goto readSomething;
    }// if hello

    goto readSomething;

    //peak at first byte.first byte not removed.
    inByte = Serial.peek();
    //Serial.println(inByte);


    if (inByte == 'r') {
      Serial.read();  //dump the 'r
      delay(1);
      Serial.print(millis());
      Serial.print(",");
      Serial.print(analogRead(0));
      Serial.print(",");
      Serial.print(analogRead(1));
      Serial.print(",");
      Serial.print(servo1Value);
      Serial.print(",");
      Serial.println(servo2Value);

      goto readSomething;
    }//if read 'r'

    if (inByte == 's') {
      //read 6 bytes
      Serial.read(); // dump the 's
      Serial.readBytes(inData, 6);
      long temp = 0;

      //process 3 bytes for first output
      for (int i = 0; i < 3; i++) {
        temp  = temp * 10 + (inData[i] - 48);
      }

      servo1Value = temp;
      myServo1.write(temp);

      //repeat for servo 2
      temp = 0;

      //process 3 bytes for second output
      for (int i = 3; i < 6; i++) {
        temp  = temp * 10 + (inData[i] - 48);
      }
      servo2Value = temp;
      myServo2.write(temp);

      goto readSomething;
    }//if read 's'


  }//if statement
  Serial.read();//dump a char to look for next one

readSomething:
  delay(1);
}//check serial



/**********************************
 * todo
 * add ADC via ADS1115
 * add setting gain and offset values
 * add setting min and max us for servo
 * check out servo driver board and consider using it.
 * add periodic reads ability
 * refactor writes to reduce clutter
 * add servo power measurement
 * create schematic
 * post on github
 */


/****************************************
revision history
12 Feb 2017
  revised input to take strings for more flexibility
  added compatibility for scale input via HX711 chip
  revised servo input to be in us and not degrees

*********************************/

