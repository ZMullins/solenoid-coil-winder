
#include <Wire.h>
#include <AccelStepper.h>
#include <Adafruit_MotorShield.h>
#include <MultiStepper.h>
#define dirCW 1
#define dirCCW -1

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

//Initial Configuration Parameters
int dir = dirCW;
int state = -1;
int layer = 0;
int last = -1;

//Parameters of Magnetorquer
//All measurements in mm
int wireLength = 84184;
int coreDiameter = 6;
int coreLength = 38;
double linearPitch = 8;
double wireDiameter = 0.19812;
long numOfCoilsFullLayer;
long lengthOfLayer;
long numOfFullLayers;
long linStepsPerLayer;
long rotStepsPerLayer;
//Rotspeed is configured,
long rotSpeed = 235;
long linSpeed;
//Percent Error is a speed increase % for the rotational stepper
float linErrorPercent = 2.402;
//The Serial Input is stored here
int inputNum = -1;
//When system is stopped, this will track the previous speed
long tempSpeed = 0;

//This is the homing function used to check in linear actuator is in the start position
boolean isAtStart() {
  if (digitalRead(8) == LOW) {
    return true;
  }
  return false;
}

void setup()
{
  //Start bottom shield
  AFMS.begin();

  //Set up Serial
  Serial.begin(9600);

  //Set up laser alignment
  pinMode(8, INPUT);

  //Setup Motors
  steppers.addStepper(linearStepper);
  steppers.addStepper(rotateStepper);

  //Perform calculations to determine stepper movement
  //Based on input parameters of the magnetorquer
  numOfCoilsFullLayer = coreLength / wireDiameter;
  double coreCircumference = coreDiameter * 3.14159;
  lengthOfLayer = coreCircumference * numOfCoilsFullLayer;
  numOfFullLayers = wireLength / lengthOfLayer;

  linStepsPerLayer = numOfCoilsFullLayer * (wireDiameter / (linearPitch / 200.0)) * 16.0;
  rotStepsPerLayer = numOfCoilsFullLayer * 200 * 16.0;

  //Rotational speed is user set, linear is calculated as a proportion of this
  linSpeed = ((double)linStepsPerLayer / (double)rotStepsPerLayer) * rotSpeed + 1;
  rotSpeed = rotSpeed + (rotSpeed * linErrorPercent / 100);

  //Output initial conditions
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
  Serial.print(" rotSpeed-");
  Serial.println(rotSpeed);


  linearStepper.setMaxSpeed(1000);
  rotateStepper.setMaxSpeed(1000);
  rotateStepper.setCurrentPosition(0);
}

void loop()
{
  //Wait for user input to start
  checkSerial();
  long positions[2];
  //Calibrate the linear stepper to be in the home position
  if (state == 0) {
    while (!isAtStart()) {
      linearStepper.move(-1);
      linearStepper.setSpeed(linSpeed * 20);
      linearStepper.runSpeedToPosition();
    }
  //Once the linear stepper is in home position, move past to create additional tension
    linearStepper.setCurrentPosition(0);
    linearStepper.moveTo(400);
    while (linearStepper.distanceToGo() != 0) {
      linearStepper.setSpeed(linSpeed * dir * 5);
      rotateStepper.setSpeed(0);
      steppers.run();
    }
   //Set this new position to be the baseline
    linearStepper.setCurrentPosition(0);
    state++;
  }
  //Begin winding the magnetorquer
  if (state == 1) {
    //When it moves on a new payer, output
    if (last != layer) {
      Serial.print("On layer ");
      Serial.println(layer);
      Serial.print("Direction: ");
      Serial.println(dir);
      last = layer;
    }
    //Every odd layer, move to the left
    if (!(layer % 2 == 0)) {
      positions[0] = ((-linStepsPerLayer * dir) / 2 + (-linStepsPerLayer) / 2);
      positions[1] = (rotStepsPerLayer + long(layer) * rotStepsPerLayer);
      linearStepper.moveTo(positions[0]);
      rotateStepper.moveTo(100000000);

      while (linearStepper.distanceToGo() != 0) {
        //Check for any speed adjustments
        checkSerial();
        linearStepper.setSpeed(linSpeed * dir * -1);
        rotateStepper.setSpeed(rotSpeed);
        steppers.run();
      }
      //Ensure the position its at is the "start position"
      while (!isAtStart()) {
        linearStepper.move(-1);
        linearStepper.setSpeed(linSpeed * dir * -3);
        rotateStepper.setSpeed(0);
        steppers.run();
      }
      //Output resulant position
      Serial.println(linearStepper.currentPosition());
      Serial.println(rotateStepper.currentPosition());

      //Move further past to create tension
      linearStepper.moveTo(800);
      while (linearStepper.distanceToGo() != 0) {
        linearStepper.setSpeed(linSpeed * dir * -5);
        rotateStepper.setSpeed(0);
        steppers.run();
      }
      //Set this to be the new baseline position
      linearStepper.setCurrentPosition(0);
    }
    //On odd layers, move to the right
    else {
      positions[0] = ((-linStepsPerLayer * dir) / 2 + (-linStepsPerLayer) / 2);
      positions[1] = (rotStepsPerLayer + long(layer) * rotStepsPerLayer);
      Serial.println("Position target linear:");
      Serial.println(positions[0]);
      Serial.println("Position target rotating:");
      Serial.println(positions[1]);

      linearStepper.moveTo(positions[0]);
      rotateStepper.moveTo(100000000);
      while (linearStepper.distanceToGo() != 0) {
        //Check for user input
        checkSerial();
        linearStepper.setSpeed(linSpeed * dir * -1);
        rotateStepper.setSpeed(rotSpeed);
        steppers.run();
      }
      //Output resultant position
      Serial.println(linearStepper.currentPosition());
      Serial.println(rotateStepper.currentPosition());
      //Move past target position to create tension
      linearStepper.moveTo(positions[0] - 800);
      while (linearStepper.distanceToGo() != 0) {
        linearStepper.setSpeed(linSpeed * dir * -5);
        rotateStepper.setSpeed(0);
        steppers.run();
      }
      linearStepper.setCurrentPosition(positions[0]);
    }
    //Announce finish layer, change direction
    Serial.println(linearStepper.currentPosition());
    Serial.println(rotateStepper.currentPosition());
    Serial.println("Finished Layer");
    layer++;
    dir = dir * -1;
    if (layer == numOfFullLayers) {
      Serial.println("Done ALL layers!");
      state++;
    }
  }
  //This is a state to unwind the wire if needed
  if (state == 5) {
    positions[0] = ((-linStepsPerLayer * dir) / 2 + (-linStepsPerLayer) / 2);
    positions[1] = (rotStepsPerLayer + long(layer) * rotStepsPerLayer);
    steppers.moveTo(positions);
    rotateStepper.setSpeed(-300);
    linearStepper.setSpeed(000);
    steppers.runSpeedToPosition();
  }
}

//Check for user input
//At startup: set state to input #
//While running (state 1):
//  //1 - increase speed, 0 - decrease speed
//  //9 - stop rotation 2 - resume rotation
//  //5 - pause for 15 seconds
void checkSerial() {
  //This essentially acts as an interrupt, it just checks the serial.in flag
  if ( Serial.available() ) {
    inputNum = Serial.parseInt();
    Serial.read();

    if (state == 1) {
      if (inputNum == 1) {
        rotSpeed += 5;
        Serial.print("Rot speed adjusted: ");
        Serial.println(rotSpeed);
      }
      if (inputNum == 0) {
        rotSpeed -= 5;
        Serial.print("Rot speed adjusted: ");
        Serial.println(rotSpeed);
      }
      if (inputNum == 2) {
        rotSpeed = tempSpeed;
      }
      if (inputNum == 9) {
        tempSpeed = rotSpeed;
        rotSpeed = 0;
      }
      if (inputNum == 5) {
        delay(15000);
      }
    }
    else {
      state = inputNum;
      Serial.print("State set to: ");
      Serial.println(state);
    }
  }
}
