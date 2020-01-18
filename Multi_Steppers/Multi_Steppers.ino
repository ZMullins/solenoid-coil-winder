
#include <Wire.h>
#include <AccelStepper.h>
#include <Adafruit_MotorShield.h>

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

int dir = -1;

boolean isAtStart() {
  //BOILER PLATE
  return true;
}
void calibrate() {
  while(!isAtStart()){
      linearStepper.setSpeed(11);
      linearStepper.move(-1);
      linearStepper.runSpeedToPosition();
  }
      linearStepper.setCurrentPosition(0);
}
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

  //Calibrate to start
  //calibrate();
}

void loop()
{
        linearStepper.setCurrentPosition(0);
  //for (int layer=0; layer<22; layer++) {
    linearStepper.setSpeed(dir*22);
    while (!linearStepper.runSpeedToPosition()){
    linearStepper.moveTo(-4466);}
    
    //rotateStepper.setSpeed(200);
   // rotateStepper.move(-40600);
   //linearStepper.run();


}

    
