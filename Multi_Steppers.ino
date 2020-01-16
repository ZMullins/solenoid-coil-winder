// Test to Run Both Stepper Motors

#include <Wire.h>
#include <AccelStepper.h>
#include <Adafruit_MotorShield.h>

// Create Motor Shield Object
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 

// Connect a stepper motor with 200 steps per revolution (1.8 degree)
Adafruit_StepperMotor *linStepper = AFMS.getStepper(200, 1);  // Linear, Port 1
Adafruit_StepperMotor *rotStepper = AFMS.getStepper(200, 2); // Rotating, Port 2

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
AccelStepper stepper1(forwardstep1, backwardstep1);
AccelStepper stepper2(forwardstep2, backwardstep2);

void setup()
{  
  AFMS.begin(); // Start the bottom shield
   
  stepper1.setMaxSpeed(20.0);
  stepper1.setAcceleration(100.0);
  stepper1.moveTo(1000000);
    
  stepper2.setMaxSpeed(20.0);
  stepper2.setAcceleration(100.0);
  stepper2.moveTo(50000);

}

void loop()
{
    // Change direction at the limits
    if (stepper1.distanceToGo() == 0)
  stepper1.moveTo(-stepper1.currentPosition());

    if (stepper2.distanceToGo() == 0)
  stepper2.moveTo(-stepper2.currentPosition());

    stepper1.run();
    stepper2.run();
}
