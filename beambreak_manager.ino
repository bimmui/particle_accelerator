#define LEDPIN 13
  // Pin 13: Arduino has an LED connected on pin 13

#define SENSORPIN1 4
#define SENSORPIN2 5

// variables will change:
int sensorState = 0, lastState=0;         // variable for reading the pushbutton status
int sensorState2 = 0, lastState2=0;         // variable for reading the pushbutton status

unsigned long firstsense = 0;
unsigned long secondsense = 0;
unsigned long currentTime = 0;

float distance = 33;
unsigned long velocity = 0;


void setup() {
  // initialize the LED pin as an output:
  pinMode(LEDPIN, OUTPUT);      
  // initialize the sensor pin as an input:
  pinMode(SENSORPIN1, INPUT);     
  digitalWrite(SENSORPIN1, HIGH); // turn on the pullup
  
  pinMode(SENSORPIN2, INPUT);     
  digitalWrite(SENSORPIN2, HIGH); // turn on the pullup
  
  Serial.begin(9600);
}

void loop(){
  // read the state of the pushbutton value:
  sensorState = digitalRead(SENSORPIN1);
  sensorState2 = digitalRead(SENSORPIN2);

  // check if the sensor beam is broken
  // if it is, the sensorState is LOW:
  if (sensorState == LOW) {     
    // turn LED on:
    digitalWrite(LEDPIN, HIGH);  
  } 
  else {
    // turn LED off:
    digitalWrite(LEDPIN, LOW); 
  }
  
  if (sensorState && !lastState) {
    Serial.println("Unbroken");
  } 
  if (!sensorState && lastState) {
    Serial.println("Broken");
    // firstsense = millis();
  }
  lastState = sensorState;
  
  
  // read the state of the pushbutton value:
  sensorState2 = digitalRead(SENSORPIN2);

  // check if the sensor beam is broken
  // if it is, the sensorState is LOW:
  if (sensorState2 == LOW) {     
    // turn LED on:
    digitalWrite(LEDPIN, HIGH);  
  } 
  else {
    // turn LED off:
    digitalWrite(LEDPIN, LOW); 
  }
  
  if (sensorState2 && !lastState2) {
    Serial.println("Unbroken2");
  } 
  if (!sensorState2 && lastState2) {
    Serial.println("Broken2");
    secondsense = millis();
    currentTime = secondsense - firstsense;
    velocity = distance / (float) currentTime
  }
  lastState2 = sensorState2;
}
