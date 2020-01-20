
#include <Wire.h>
#include <AccelStepper.h>
#include <Adafruit_MotorShield.h>

#define dirCW 1
#define dirCCW -1
#define fullLayers 22
//may need to be looked at
#define linSpeed 22
#define rotSpeed 200
#define numOfTurns 203
//#define linSteps 4466
//#define rotSteps 40600

// Create Motor Shield Object
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 

// Connect a stepper motor with 200 steps per revolution (1.8 degree)
Adafruit_StepperMotor *linStepper = AFMS.getStepper(200, 2);  // Linear, Port 1
Adafruit_StepperMotor *rotStepper = AFMS.getStepper(200, 1); // Rotating, Port 2

// wrappers for the first motor!
void forwardstep1() {  
  linStepper->onestep(FORWARD, MICROSTEP);
}
void backwardstep1() {  
  linStepper->onestep(BACKWARD, MICROSTEP);
}
// wrappers for the second motor!
void forwardstep2() {  
  rotStepper->onestep(FORWARD, MICROSTEP);
}
void backwardstep2() {  
  rotStepper->onestep(BACKWARD, MICROSTEP);
}

// Now we'll wrap the 3 steppers in an AccelStepper object
AccelStepper linearStepper(forwardstep1, backwardstep1);
AccelStepper rotateStepper(forwardstep2, backwardstep2);

int dir = dirCCW;
void setup()
{  
  //Start bottom shield
  AFMS.begin();

  //Set up Serial
  Serial.begin(9600);
  
  //Setup Motors
  linearStepper.setMaxSpeed(600.0);
  linearStepper.setAcceleration(10000.0);
  rotateStepper.setMaxSpeed(600.0);
  rotateStepper.setAcceleration(10000.0);
   linearStepper.setCurrentPosition(0);
   rotateStepper.setCurrentPosition(0);

    //Moves to either 0 or the numOfTurns
   
   // while (!rotateStepper.runSpeedToPosition()){
   // rotateStepper.moveTo(numOfTurns*rotSpeed);}



}

void loop()
{

  linearStepper.setSpeed(100);
      while (!linearStepper.runSpeedToPosition()){
    linearStepper.moveTo(-50000);}
    
    linearStepper.run();
   rotateStepper.setSpeed(100);
      while (!rotateStepper.runSpeedToPosition()){
    rotateStepper.moveTo(-50000);}

    rotateStepper.run();
  
    /*  linearStepper.setSpeed(100);
    while (!linearStepper.runSpeedToPosition()){
    linearStepper.moveTo(-4400);}*/
}

    
