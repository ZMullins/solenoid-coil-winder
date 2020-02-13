
#include <Wire.h>
#include <AccelStepper.h>
#include <Adafruit_MotorShield.h>
#include <MultiStepper.h>
#define dirCW 1
#define dirCCW -1
//#define fullLayers 22
//may need to be looked at
//#define linSpeed 20
//#define numOfTurns 203

#define wireLength 84184
#define coreDiameter 6
#define coreLength 37.2
#define MICROSTEPS 16

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
AccelStepper linearStepper(forwardstep1, backwardstep1);
AccelStepper rotateStepper(forwardstep2, backwardstep2);
MultiStepper steppers;

int dir = dirCW;
int state = 0;
int layer=0;
int last=-1;
//Function checks whether the linear actuator is in start position

//NEW CODE
double linearPitch = 8;
//double wireDiameter = 0.28448;
double wireDiameter = 9.3;
long numOfCoilsFullLayer;
long lengthOfLayer;
long numOfFullLayers;
long linStepsPerLayer;
long rotStepsPerLayer;
long rotSpeed = 50;
long linSpeed;
boolean isAtStart() {
  
  if (digitalRead(8)==LOW) {
    return true;
  }
  
  Serial.println(digitalRead(8));
  return false;
}

void setup()
{  
  //Start bottom shield
  AFMS.begin();

  //Set up Serial
  Serial.begin(9600);

  pinMode(8,INPUT);
  //Setup Motors


 

  steppers.addStepper(linearStepper);
  steppers.addStepper(rotateStepper);

  numOfCoilsFullLayer = coreLength/wireDiameter;
  double coreCircumference = coreDiameter*3.14159;
  lengthOfLayer = coreCircumference*numOfCoilsFullLayer;
  numOfFullLayers=wireLength/lengthOfLayer;

  linStepsPerLayer=numOfCoilsFullLayer*(wireDiameter/(linearPitch/200.0))*16.0;
  rotStepsPerLayer=numOfCoilsFullLayer*200*16.0;

  //rotSpeed=((double)rotStepsPerLayer/(double)linStepsPerLayer)*linSpeed;
  linSpeed=((double)linStepsPerLayer/(double)rotStepsPerLayer)*rotSpeed+1;
  double realSpeed=((double)linStepsPerLayer/(double)rotStepsPerLayer)*rotSpeed;
  
  Serial.print("numOfCoilsFullLayer-");
  Serial.println(numOfCoilsFullLayer);
  
  Serial.print(" lengthOfLayer-");
  Serial.println(lengthOfLayer);
  
  Serial.print(" numOfFullLayers-");
  Serial.println(numOfFullLayers);
  
  Serial.print(" linStepsPerLayer-");
  Serial.println(linStepsPerLayer);

    Serial.print(" rotStepsPerLayer-");
  Serial.println(rotStepsPerLayer);
    
    Serial.print(" linSpeed-");
  Serial.println(linSpeed);
  Serial.println(realSpeed);
    Serial.print(" rotSpeed-");
  Serial.println(rotSpeed);

    linearStepper.setMaxSpeed(1000);
    rotateStepper.setMaxSpeed(1000);
    rotateStepper.setCurrentPosition(0);

}

void loop()
{

  long positions[2];
  //Calibrate
  if (state == 0) {
    while(!isAtStart()){
      linearStepper.setSpeed(linSpeed*20);
      linearStepper.move(-1);
      linearStepper.runSpeedToPosition();
  }
      linearStepper.setCurrentPosition(0);
      state++;
  }
  if (state == 1) {
    if (last!=layer){
      Serial.print("On layer ");
      Serial.println(layer);
      Serial.print("Direction: ");
      Serial.println(dir);
      last=layer;
    }
    
      positions[0] = ((-linStepsPerLayer*dir)/2+(-linStepsPerLayer)/2);
      positions[1] = (rotStepsPerLayer+long(layer)*rotStepsPerLayer);
Serial.println("Position target linear:");
Serial.println(positions[0]);
Serial.println("Position target rotating:");
Serial.println(positions[1]);
    steppers.moveTo(positions);
    linearStepper.setSpeed(linSpeed*dir*-1);

   rotateStepper.setSpeed(rotSpeed);
   
   //rotateStepper.setSpeed(0);
//while (linearStepper.distanceToGo() != 0 && rotateStepper.distanceToGo() != 0){
//steppers.run();
Serial.println(linearStepper.currentPosition());
Serial.println(rotateStepper.currentPosition());
//}
    steppers.runSpeedToPosition();
      Serial.println(linearStepper.currentPosition());
      Serial.println(rotateStepper.currentPosition());
      Serial.println("Finished Layer");
      layer++;
      dir =dir*-1;
      if (layer==numOfFullLayers) {
        Serial.println("Done ALL layers!");
        state++;
    }
  }
      if (state == 5) {
                Serial.println("DONE!");
        delay(15000);
             positions[0] = ((-linStepsPerLayer*dir)/2+(-linStepsPerLayer)/2);
      positions[1] = (rotStepsPerLayer+long(layer)*rotStepsPerLayer);
            steppers.moveTo(positions);
            linearStepper.setSpeed(linSpeed*dir*-100);
    steppers.runSpeedToPosition();

      }
      if (state==6) {
        Serial.println("We're done here");
      }
  //TODO: Part of a loop logic
}

    
