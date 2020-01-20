
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
int state = 0;
int layer=0;
int last=-1;
//Function checks whether the linear actuator is in start position
boolean isAtStart() {
  //BOILER PLATE
  return true;
}

//Puts the linear actuator to start position
void calibrate() {
  
}

void windFull() {
  
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
  calibrate();
}

void loop()
{
  //Calibrate
  if (state == 0) {
    while(!isAtStart()){
      linearStepper.setSpeed(linSpeed);
      linearStepper.move(-1);
      linearStepper.runSpeedToPosition();
  }
      linearStepper.setCurrentPosition(0);
      state++;
  }
  if (state == 1) {
    if (last!=state){
    Serial.print("On layer ");
    Serial.println(layer);}
    linearStepper.setSpeed(linSpeed);
    rotateStepper.setSpeed(rotSpeed);
   
    //Moves to either 0 or the numOfTurns
    if (last!=state){
    Serial.print("Linear going to ");
    Serial.println((numOfTurns*linSpeed*dir)/2+(numOfTurns*linSpeed));
    }
    while (!linearStepper.runSpeedToPosition()){
    linearStepper.moveTo((numOfTurns*linSpeed*dir)/2+(numOfTurns*linSpeed));}
        if (last!=state){
    Serial.print("Rotate going to ");
    Serial.println(numOfTurns*rotSpeed);
        }
    while (!rotateStepper.runSpeedToPosition()){
    rotateStepper.moveTo(numOfTurns*rotSpeed);}
    linearStepper.run();
    rotateStepper.run();
    if ((linearStepper.targetPosition()==linearStepper.currentPosition()) &&(rotateStepper.targetPosition()==rotateStepper.currentPosition())){
      Serial.println("Finished Layer");
      layer++;
      dir =dir*-1;
    }
    if (layer==fullLayers) {
      Serial.println("Done ALL layers!");
      state++;
    }
    last = state;
  }
  //TODO: Part of a loop logic
}

    
