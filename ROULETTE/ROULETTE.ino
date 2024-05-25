#include <Arduino.h>
#include <Servo.h>
#include "PinDefinitionsAndMore.h" // Define macros for input and output pin etc.
#include <IRremote.hpp>
#include <stdlib.h>

#define DECODE_NEC 

#define left 0x8
#define right 0x5A
#define up 0x52
#define down 0x18
#define ok 0x1C
#define hashtag 0xD
#define star 0x16
#define cmd1 0x45
#define cmd2 0x46
#define cmd3 0x47
#define cmd4 0x44
#define cmd5 0x40
#define cmd6 0x43
#define cmd7 0x7
#define cmd8 0x15
#define cmd9 0x9
#define cmd0 0x19

////////////////////////////////////////////////////////ROULETTE VARIABLES//////////////////////////////////////////////////////////////////////////////////////////////////////////

int Max = 6; //Change this number to increase or decrease the chance of getting shot
int chosenNumbers[6] = {0,0,0,0,0,0}; // Add or Remove 0's to correspond with the "Max" value and change the 6 to the same value as Max
int count = 0; // idk why this is here the Mark RoBot told me to put this
int randomValue;
int randomNumber;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Servo yawServo;
Servo pitchServo;
Servo rollServo;

int yawServoVal; //initialize variables to store the current value of each servo
int pitchServoVal = 100;
int rollServoVal;

int pitchMoveSpeed = 7; //this variable is the angle added to the pitch servo to control how quickly the PITCH servo moves - try values between 3 and 10
int yawMoveSpeed = 80; //this variable is the speed controller for the continuous movement of the YAW servo motor. It is added or subtracted from the yawStopSpeed, so 0 would mean full speed rotation in one direction, and 180 means full rotation in the other. Try values between 10 and 90;
int yawStopSpeed = 90; //value to stop the yaw motor - keep this at 90
int rollMoveSpeed = 130; //this variable is the speed controller for the continuous movement of the ROLL servo motor. It is added or subtracted from the rollStopSpeed, so 0 would mean full speed rotation in one direction, and 180 means full rotation in the other. Keep this at 90 for best performance / highest torque from the roll motor when firing.
int rollStopSpeed = 90; //value to stop the roll motor - keep this at 90

int yawPrecision = 70; // this variable represents the time in milliseconds that the YAW motor will remain at it's set movement speed. Try values between 50 and 500 to start (500 milliseconds = 1/2 second)
int rollPrecision = 150; // this variable represents the time in milliseconds that the ROLL motor with remain at it's set movement speed. If this ROLL motor is spinning more or less than 1/6th of a rotation when firing a single dart (one call of the fire(); command) you can try adjusting this value down or up slightly, but it should remain around the stock value (160ish) for best results.

int pitchMax = 175; // this sets the maximum angle of the pitch servo to prevent it from crashing, it should remain below 180, and be greater than the pitchMin
int pitchMin = 10; // this sets the minimum angle of the pitch servo to prevent it from crashing, it should remain above 0, and be less than the pitchMax


void nope(){

Serial.println("NO");
    int startAngle = pitchServoVal; // Current position of the pitch servo
    int lastAngle = pitchServoVal;
    int nodAngle = startAngle + 60; // Angle for nodding motion
    for (int i = 0; i < 2; i++){
     // Repeat nodding motion three times
        // rotate right, stop, then rotate left, stop
        yawServo.write(180);
        delay(190); // Adjust delay for smoother motion
        yawServo.write(yawStopSpeed);
        delay(50);
        yawServo.write(0);
        delay(190); // Adjust delay for smoother motion
        yawServo.write(yawStopSpeed);
        delay(50); // Pause at starting position
    }

}


void upMove(int moves){
  for (int i = 0; i < moves; i++){
      if(pitchServoVal > pitchMin){//make sure the servo is within rotation limits (greater than 10 degrees by default)
        pitchServoVal = pitchServoVal - pitchMoveSpeed; //decrement the current angle and update
        pitchServo.write(pitchServoVal);
        delay(50);
        Serial.println("UP");
      }
  }
}

void downMove(int moves){
  for (int i = 0; i < moves; i++){
    if (pitchServoVal < pitchMax){
      pitchServoVal = pitchServoVal + pitchMoveSpeed;
      pitchServo.write(pitchServoVal);
      delay(50);
    }
  }
}

void leftMove(int moves){
  for (int i = 0; i < moves; i++){
    yawServo.write(yawStopSpeed+yawMoveSpeed);
    delay(yawPrecision);
    yawServo.write(yawStopSpeed);
    delay(5);
  }
}

void rightMove(int moves){
  for (int i = 0; i < moves; i++){
    yawServo.write(yawStopSpeed-yawMoveSpeed);
    delay(yawPrecision);
    yawServo.write(yawStopSpeed);
    delay(5);
  }
}


void fire(int moves){
  for (int i = 0; i < moves; i++){
    rollServo.write(rollStopSpeed-rollMoveSpeed);
    delay(rollPrecision);
    rollServo.write(rollStopSpeed);
    delay(5);
  }
}


void setup() {

  randomSeed(analogRead(0));
  // put your setup code here, to run once:
Serial.begin(9600);

yawServo.attach(10); //attach YAW servo to pin 10
pitchServo.attach(11); //attach PITCH servo to pin 11
rollServo.attach(12);

IrReceiver.begin(9, ENABLE_LED_FEEDBACK);

}


void loop() {

    /*
    * Check if received data is available and if yes, try to decode it.
    */
    if (IrReceiver.decode()) {

        /*
      
        Serial.println();

        /*
        * !!!Important!!! Enable receiving of the next value,
        * since receiving has stopped after the end of the current received data packet.
        */
        IrReceiver.resume(); // Enable receiving of the next value

      

        /*
        * Finally, check the received data and perform actions according to the received command
        */

        switch(IrReceiver.decodedIRData.command){ //this is where the commands are handled

            case up://pitch up
            upMove(1);
            break;

            case down:
            downMove(1);
            break;

            case hashtag:
          
            randomNumber = random(1, Max); // 1 out of "Max" chance to get shot
          
            while(chosenNumbers[randomNumber] == 1){
              randomNumber = random(1, Max);
            } // rerolls if the number has already been chosen

              if (chosenNumbers[randomNumber] == 0) { // Check if the number has not been chosen before
             chosenNumbers[randomNumber] = 1; // Mark the number as chosen
            count++; // Increment the counter

            delay(random(1, 1000)); //adds dramatic effect ;)

              if (randomNumber == 1) { // Check if the number is 1 (AKA YOU LOST)
             fire(1);
            for (int i = 0; i < Max; i++) {
          chosenNumbers[i] = 0; // Reset all chosen numbers
         }
          count = 0; // Reset the counter
         }
          else{
                  nope();
          }
         }  
          delay(800);
            break;


            case left:
            leftMove(1);
        
            break;
            
            case right:
            rightMove(1);
     
            break;

            case ok:
            fire(1);
             delay(200);
            break;
            
            case cmd1:
            rollPrecision = 151; //Sets the ammount of darts shot to 1
        
            break;

            case cmd2:
            rollPrecision = 302; //Sets the ammount of darts shot to 2
         
            break;

            case cmd3:
            rollPrecision = 453; //Sets the ammount of darts shot to 3
            break;

            case cmd4:
            rollPrecision = 604; //Sets the ammount of darts shot to 4
            break;

            case cmd5:
            rollPrecision = 755; //Sets the ammount of darts shot to 5
            break;

            case cmd6:
            rollPrecision = 906; //Sets the ammount of darts shot to 6
            break;

        }
    }
    delay(5);
}