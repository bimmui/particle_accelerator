/**************************************************************
 *
 *                     beambreak_manager.ino
 *
 *     Assignment: Comp Phys Final Project (Phys 68)
 *     Author(s):  Daniel Opara
 *     Date:       4/16/2023
 *
 *     Overview: Driver code for the particle acclerator. Activates electromagnets
 *                to accelerate steel ball based on the beam break sensors it passes 
 *
 *
 **************************************************************/



#include "Queue.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - -
//   Defining digital pins and constants
// - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define LEDPIN 13 // Pin 13: Arduino has an LED connected on pin 13
#define SENSORPIN1 4
#define SENSORPIN2 5
const int BEAM_DISTANCE = 35; // millimeters
// distance from the beam compartment to the beginning of the magnetic field
const int MAGNETICE_FIELD_DISTANCE = 35; // millimeters
// distance from the beam compartment to the center of the magnetic field
const int MAGNETICE_FIELD_CENTER_DISTANCE = 35; // millimeters



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//   Necessary data structures
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef struct doubleBeamCompartment {
   int sensorpin1;
   int sensorpin2;
} doubleBeamCompartment;

Queue<doubleBeamCompartment> sensor_queue(4);



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//   Variables that will change throughout the program's lifetime
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// defines the first set of sensors the ball encounters
int curr_sensor1; // entering beam compartment
int curr_sensorState1 = 0, curr_lastState1 = 0;

// defines the second set of sensors the ball encounters
int curr_sensor2; // exiting beam compartment
int curr_sensorState2 = 0, curr_lastState2 = 0;

// used for calcaluting the time at which the ball will first 
// enter the magnetic field and when it will be at the center of it
unsigned long first_timestamp = 0, second_timestamp = 0;
unsigned long time_interval = 0;
float velocity = 0;
bool is_velocity_calculated = false;

// the times at which the ball will first enter the mag field
// and when it will be in the center of it
float t3 = 0, t4 = 0; 



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//   Helper functions
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/*
* calculateVelocity
* Parameters: The recorded timestamps for each sensor in a compartment
* Purpose: Calculates velocity of the ball
* Returns: Nothing
* Effects: Modifies the value stored insider the "velocity" variable
*/
bool calculateVelocity() {
  time_interval = first_timestamp - second_timestamp;
  velocity = BEAM_DISTANCE / (float) time_interval;
  return true;
}

/*
* calculateElectromagnetActivationTimes
* Parameters: The current velocity of the ball
* Purpose: Calculates the time at which the program should turn on the elctromagnet
* Returns: Nothing
* Effects: Modifies the value stored inside "t3" and "t4"
*/
void calculateElectromagnetActivationTimes() {
  t3 = MAGNETICE_FIELD_DISTANCE / velocity;
  t4 = MAGNETICE_FIELD_CENTER_DISTANCE / velocity;
}

/*
* checkTime
* Parameters: None
* Purpose: Calculates the time at which the program should turn on the elctromagnet
* Returns: Nothing
* Effects: Modifies the value stored inside "t3" and "t4"
*/
float checkTime() {
  return millis() - second_timestamp;
}

/*
* activateElectromagnet
* Parameters: None
* Purpose: Activates an electromagnet based on time interval from
*             second_timestamp to now  
* Returns: Nothing
* Effects: Once the time interval from second_timestamp to now is 
*             equal to t3, the electromagnet corresponding to the 
*             pair of beam break sensors is activated
*/
bool activateElectromagnet() {
  if (checkTime() < t3) {
    return false;
  } else {
    Serial.println("Activated electromagnet");
  }
  return true;
}

/*
* deactivateElectromagnet
* Parameters: None
* Purpose: Deactivates an electromagnet based on time interval from
*             second_timestamp to now  
* Returns: Nothing
* Effects: Once the time interval from second_timestamp to now is 
*             equal to t4, the electromagnet corresponding to the 
*             pair of beam break sensors is deactivated
*/
bool deactivateElectromagnet() {
  if (checkTime() < t4) {
    return true;
  } else {
    Serial.println("Deactivated electromagnet");
  }
  return false;
}





/*************************************************************************/
/*                              MAIN PROGRAM                             */
/*************************************************************************/

void setup() {
  Serial.begin(9600);

  Serial.print("Setting up pin connections...");
  // initialize the LED pin as an output:
  pinMode(LEDPIN, OUTPUT);      
  // initialize the sensor pin as an input:
  pinMode(SENSORPIN1, INPUT);     
  digitalWrite(SENSORPIN1, HIGH); // turn on the pullup
  
  pinMode(SENSORPIN2, INPUT);     
  digitalWrite(SENSORPIN2, HIGH); // turn on the pullup
  Serial.println("Complete!");

  Serial.print("Setting up sensor data structures...");

  sensor_queue.push(doubleBeamCompartment{SENSORPIN1, SENSORPIN2});

  doubleBeamCompartment temp = sensor_queue.pop();
  curr_sensor1 = temp.sensorpin1;
  curr_sensor2 = temp.sensorpin2;
  Serial.println("Complete!");
  Serial.println("Program set up complete! Beginning program...");
  
  
}

void loop(){
  // read the state of the pushbutton value:
  curr_sensorState1 = digitalRead(curr_sensor1);
  curr_sensorState2 = digitalRead(curr_sensor2);

  // check if the sensor beam is broken
  // if it is, the sensorState is LOW:
  if (curr_sensorState1 == LOW) {     
    // turn LED on:
    digitalWrite(LEDPIN, HIGH);  
  } 
  else {
    // turn LED off:
    digitalWrite(LEDPIN, LOW); 
  }
  
  if (curr_sensorState1 && !curr_lastState1) {
    Serial.println("Unbroken");
  } 
  if (!curr_sensorState1 && curr_lastState1) {
    Serial.println("Broken");
    first_timestamp = millis();
  }
  curr_lastState1 = curr_sensorState1;
  

  if (curr_sensorState2 == LOW) {     
    // turn LED on:
    digitalWrite(LEDPIN, HIGH);  
  } 
  else {
    // turn LED off:
    digitalWrite(LEDPIN, LOW); 
  }
  
  if (curr_sensorState2 && !curr_lastState2) {
    Serial.println("Unbroken2");
  } 
  if (!curr_sensorState2 && curr_lastState2) {
    Serial.println("Broken2");
    second_timestamp = millis();
    is_velocity_calculated = calculateVelocity();
    Serial.println("set to true");
  }
  curr_lastState2 = curr_sensorState2;

  if (is_velocity_calculated == true) {    
    // grabbing the next set of sensor to monitor and pushing the current sensors
    // to the back of the queue
    sensor_queue.push(doubleBeamCompartment{curr_sensor1, curr_sensor2});
    doubleBeamCompartment temp = sensor_queue.pop();
    curr_sensor1 = temp.sensorpin1;
    curr_sensor2 = temp.sensorpin2;

    // reseting the sensor state variables to prep for the next pair of beams
    curr_sensorState1 = 0;
    curr_lastState1 = 0;
    curr_sensorState2 = 0;
    curr_lastState2 = 0;

    Serial.println("This is it: ");
    Serial.println(velocity);
    is_velocity_calculated = false;
    
    bool activated_electromagnet = false;
    bool deactivated_electromagnet = true;
    // setting this timestamp to more recent time to calculate t3 and t4
    second_timestamp = millis();
    calculateElectromagnetActivationTimes();
    while (activated_electromagnet != true) {
      activated_electromagnet = activateElectromagnet();
    }
    while (deactivated_electromagnet != false) {
      deactivated_electromagnet = deactivateElectromagnet(); 
    }
  }
}
