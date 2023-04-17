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
const int DISTANCE = 33;



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//   Necessary data structures
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef struct doubleBeamCompartment {
   int sensorpin1;
   int sensorpin2;
   
   // defines the first set of sensors the ball encounters
   int sensorstate1;
   int laststate1;

  // defines the second set of sensors the ball encounters
   int sensorstate2;
   int laststate2;
} doubleBeamCompartment;

Queue<doubleBeamCompartment> sensor_queue(4);



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//   Variables that will change throughout the program's lifetime
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int curr_sensor1; // entering beam compartment
int curr_sensorState1 = 0, curr_lastState1 = 0;

int curr_sensor2; // exiting beam compartment
int curr_sensorState2 = 0, curr_lastState2 = 0;
   
unsigned long first_timestamp = 0, second_timestamp = 0;
unsigned long time_interval = 0;
float velocity = 0;
bool is_velocity_calculated = false;



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//   Helper functions
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/*
calculateVelocity
Parameters: The recorded timestamps for each sensor in a compartment
Purpose: Calculates velocity of the ball
Returns: Nothing
Effects: Modifies the value stored insider the "velocity" variable
*/
bool calculateVelocity(unsigned long t1, unsigned long t2) {
  time_interval = t2 - t1;
  velocity = DISTANCE / (float) time_interval;
  return true;
}


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

  sensor_queue.push(doubleBeamCompartment{SENSORPIN1, SENSORPIN2, 0, 0, 0, 0});

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
    is_velocity_calculated = calculateVelocity(first_timestamp, second_timestamp);
    Serial.println("set to true");
  }
  curr_lastState2 = curr_sensorState2;

  if (is_velocity_calculated == true) {
    sensor_queue.push(doubleBeamCompartment{curr_sensor1, curr_sensor2, 0, 0, 0, 0});
    doubleBeamCompartment temp = sensor_queue.pop();
    curr_sensor1 = temp.sensorpin1;
    curr_sensor2 = temp.sensorpin2;    
    Serial.println("This is it: ");
    Serial.println(velocity);
    is_velocity_calculated = false;
  }
}
