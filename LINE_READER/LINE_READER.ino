//sources:
//1. https://www.instructables.com/Running-DC-Motor-With-Arduino-Using-L298N-Motor-Dr/
//2. https://mschoeffler.com/2017/11/27/arduino-tutorial-ir-distance-line-tracing-line-tracking-sensor-mh-sensor-series-ky-033-tcrt5000/
//3. https://www.instructables.com/Simple-Arduino-and-HC-SR04-Example/
#define trigPin 13
#define echoPin 12
#define FWD 0
#define REV 1
#define L 2
#define R 3
#define STOP 4
#include <Wire.h>
const int A_LINE_L = A0;
const int A_LINE_R = A1;
const int THRESHOLD_L = 300;
const int THRESHOLD_R = 300;
int L_SPEED = 164;
int R_SPEED = 160;
int R_RATE = 100;
int L_RATE = 100;
//Note that higher values correspond to more light.
//Digital 1 = lots of light
double value_AL;
double value_AR;
const int readings = 16;
double sensorReadings[readings];
int gamble = 0;

//Motor Pins
int EN_A = 10; //Enable pin for first motor
int IN1 = 9; //control pin for first motor (L)
int IN2 = 8; //control pin for first motor (L)
int IN3 = 7; //control pin for second motor (R)
int IN4 = 6; //control pin for second motor (R)
int EN_B = 11; //Enable pin for second motor
//Initializing variables to store data
int trn_cnter_L = 0;
int trn_cnter_R = 0;
int motor_speed;
int motor_speed1;
//MOTOR SPEED FROM 0-255

const double threshold_L = 500;
const double threshold_R = 500;

//timer system
unsigned long timerStart;
bool timerActive = false;
int timer_len = 1500;
//stores direction last correction L = 0, R = 1
int LAST_CORRECTION = 0;

//

void setup() {
  Serial.begin(9600);
  pinMode(A_LINE_L, INPUT);
  pinMode(A_LINE_R, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  //Initializing the motor pins as output
  pinMode(EN_A, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(EN_B, OUTPUT);
  
  Serial.println("START");
  // Start the serial communication at 9600 baud rate
}

void loop() {
   Serial.println("");
   value_AL = analogRead(A_LINE_L); // reads the analog input from the IR distance sensor
   value_AR = analogRead(A_LINE_R);
   
  Serial.println("ANALOG_L: " + String(value_AL) + "\n"
                  "ANALOG_R: " + String(value_AR) );
  double dist = check_distance();
  if(dist > 10){
    course_correct();
    checkAndTurn();
  }
  else if(dist < 7){
    //MOTOR_CONTROL(REV);
    //delay(100);
  }
  else{
    path();
  }
  
}

void course_correct() {
  if (value_AL > 450) {
    trn_cnter_R = 0;
    LAST_CORRECTION = 0;
    L_SPEED = 159;
    R_SPEED = 155;

    MOTOR_CONTROL(L);
    delay(7 + trn_cnter_L++);
    
    MOTOR_CONTROL(STOP);
    delay(5);
    return;
  }

  if (value_AR > 600) {
    // Right turn
    LAST_CORRECTION = 1;
    trn_cnter_L = 0;
    L_SPEED = 174;
    R_SPEED = 170;
   
    MOTOR_CONTROL(R);
    delay(7 + trn_cnter_R++);
    
    MOTOR_CONTROL(STOP);
    delay(5);
    return;
  }
// Modify this to detect a T junction
// If it detects the T junction, turn right 90, proceed. If we detect no tape, turn back 180, proceed then return to the loop
if (value_AL > 230 && value_AR > 230) {
    L_SPEED = 154;
    R_SPEED = 150;
    Serial.println("run");
    MOTOR_CONTROL(FWD);
    delay(20);
    MOTOR_CONTROL(STOP);
    delay(10);
    value_AL = analogRead(A_LINE_L);
    value_AR = analogRead(A_LINE_R);
    
    // Detected T-Junction, first turn right 90 degrees
    if(LAST_CORRECTION = 0){
    MOTOR_CONTROL(L);
    delay(10); // Adjust this delay to accurately achieve a 90-degree turn
    MOTOR_CONTROL(STOP);
    delay(10);
    MOTOR_CONTROL(L);
    delay(10); // Adjust this delay to accurately achieve a 90-degree turn
    MOTOR_CONTROL(STOP);
    delay(10);
    MOTOR_CONTROL(L);
    delay(10); // Adjust this delay to accurately achieve a 90-degree turn
    MOTOR_CONTROL(STOP);
    delay(10);
    
    // Check for no tape
    value_AL = analogRead(A_LINE_L);
    value_AR = analogRead(A_LINE_R);
     L_SPEED = 144;
     R_SPEED = 140;
    while(value_AR < 350){
          MOTOR_CONTROL(R);
          delay(10); // Adjust this delay to accurately achieve a 90-degree turn
          MOTOR_CONTROL(STOP);
          delay(20);
          value_AL = analogRead(A_LINE_L);
          value_AR = analogRead(A_LINE_R);
    }
    return;
    }
    else{
    MOTOR_CONTROL(R);
    delay(10); // Adjust this delay to accurately achieve a 90-degree turn
    MOTOR_CONTROL(STOP);
    delay(10);
    MOTOR_CONTROL(R);
    delay(10); // Adjust this delay to accurately achieve a 90-degree turn
    MOTOR_CONTROL(STOP);
    delay(10);
    MOTOR_CONTROL(R);
    delay(10); // Adjust this delay to accurately achieve a 90-degree turn
    MOTOR_CONTROL(STOP);
    delay(10);
    
    // Check for no tape
     L_SPEED = 144;
     R_SPEED = 140;
    value_AL = analogRead(A_LINE_L);
    value_AR = analogRead(A_LINE_R);
    while(value_AL < 350){
          MOTOR_CONTROL(L);
          delay(15); // Adjust this delay to accurately achieve a 90-degree turn
          MOTOR_CONTROL(STOP);
          delay(20);
          value_AL = analogRead(A_LINE_L);
          value_AR = analogRead(A_LINE_R);
    }
    return;
    }
}
  L_SPEED = 164;
  R_SPEED = 160;
  MOTOR_CONTROL(FWD);
  delay(10);
  MOTOR_CONTROL(STOP);
  delay(10);
}




void MOTOR_CONTROL (int CMD){
  
  switch(CMD){
    case FWD: 
    //go forward
    //LMOTOR CW
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    //RMOTOR CW
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    //ENABLE MOTORS
    analogWrite(EN_A,L_SPEED);
    analogWrite(EN_B,R_SPEED);
    break;
    case REV:
    //go backwards
    //LMOTOR CCW
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    //RMOTOR CCW
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    //ENABLE MOTORS
    analogWrite(EN_A,L_SPEED);
    analogWrite(EN_B,R_SPEED);
    break;
    case L:
    Serial.println("LEFT");
    //Turn left
    //LMOTOR CCW
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    //RMOTOR CW
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    //ENABLE MOTORS
    analogWrite(EN_A,L_SPEED);
    analogWrite(EN_B,R_SPEED);
    break;
    case R:
    Serial.println("RIGHT");
    //Turn right
    //LMOTOR CW
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    //RMOTOR CCW
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    //ENABLE MOTORS
    analogWrite(EN_A,L_SPEED);
    analogWrite(EN_B,R_SPEED);
    break;
    case STOP:
    //stop
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    //RMOTOR CCW
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    analogWrite(EN_A,0);
    analogWrite(EN_B,0);
    break;
    default:
    //tbd
    break;
  }
  
}

double check_distance(){
   long duration;
   double distance;
   digitalWrite(trigPin, LOW); 
   delayMicroseconds(2);
   digitalWrite(trigPin, HIGH);
   delayMicroseconds(10);
   digitalWrite(trigPin, LOW);
   duration = pulseIn(echoPin, HIGH);
   distance = (duration/2) / 29.1;
   //Serial.print("DISTANCE: ");
   Serial.print(distance, 2); // Print the distance with 2 decimal places
   Serial.println("cm");
   return distance;
}

void scan_area() {
  int index = 0; // Array index
  int arraySize = readings;
  // Turn right and take readings
  MOTOR_CONTROL(R);
  for (int i = 0; i < (readings/2); i++) { // Assuming 200ms at 10ms per reading
    if (index < arraySize) {
      MOTOR_CONTROL(R);
      delay(R_RATE);
      MOTOR_CONTROL(STOP);
      delay(100);
      sensorReadings[index++] = check_distance();
    }
  }
  // Turn left and take readings
  for (int j = 0; j < readings/2; j++){
    MOTOR_CONTROL(L);
    delay(L_RATE);
    MOTOR_CONTROL(STOP);
    delay(100);
  }
  
  for (int i = 0; i < (readings/2); i++) { // Assuming 200ms at 10ms per reading
    if (index < arraySize) {
      MOTOR_CONTROL(STOP);
      delay(100);
      sensorReadings[index++] = check_distance();
      MOTOR_CONTROL(L);
      delay(L_RATE);
    }
  }
  for (int j = 0; j < readings/2; j++){
    MOTOR_CONTROL(R);
    delay(R_RATE);
    MOTOR_CONTROL(STOP);
    delay(100);
  }
  

  // Print the sensor readings to the serial monitor
  for (int i = 0; i < index; i++) {
    Serial.print("Reading ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(sensorReadings[i]);
  }
}

int findIndexOfLargestTriplet(double arr[], int n) {
  // Check if the array has at least 3 elements
  if (n < 3) {
    return -1;
  }

  // Initialize the maximum sum and index
  int maxSum = arr[0] + arr[1] + arr[2];
  int maxIndex = 0;

  // Iterate through the array
  for (int i = 1; i < n - 2; i++) {
    int currentSum = arr[i] + arr[i + 1] + arr[i + 2];
    if (currentSum > maxSum) {
      maxSum = currentSum;
      maxIndex = i;
    }
  }

  return maxIndex;
}

void path() {
  scan_area();
  int i = findIndexOfLargestTriplet(sensorReadings, readings);
  if (i < readings/2){
    for(int j = 0; j < i; j++){
      MOTOR_CONTROL(R);
      delay(R_RATE);
      MOTOR_CONTROL(STOP);
      delay(100);
    }
  }
  else {
        for(int j = 0; j < (i - (readings/2)); j++){
          MOTOR_CONTROL(L);
          delay(L_RATE);
          MOTOR_CONTROL(STOP);
          delay(100);
        }

  }
}

//timer function
void checkAndTurn() {
  // Start the timer if it's not already running
  if (!timerActive) {
    timerStart = millis();
    timerActive = true;
  }

  // Read sensor values
  value_AL = analogRead(A_LINE_L); 
  value_AR = analogRead(A_LINE_R);

  // Check if either sensor value is above 400 or if 200ms have passed
  if (value_AL > 150 || value_AR > 150) {
    // Reset the timer
    timerActive = false;
  } else if (millis() - timerStart > timer_len) {
    // 200ms have passed without detecting the required values, turn the device
    if(LAST_CORRECTION = 1){
    value_AL = analogRead(A_LINE_L);
    value_AR = analogRead(A_LINE_R);
     L_SPEED = 144;
     R_SPEED = 140;
     int flip = 0;
    while(value_AR < 350){
          MOTOR_CONTROL(R);
          delay(20); // Adjust this delay to accurately achieve a 90-degree turn
          MOTOR_CONTROL(STOP);
          delay(15);
          if(flip){
          MOTOR_CONTROL(FWD);
          delay(50);
          flip = 0;
          }
          else{
            flip = 1;
          }
          value_AL = analogRead(A_LINE_L);
          value_AR = analogRead(A_LINE_R);
    }
    return;
    }
    else{
      value_AL = analogRead(A_LINE_L);
      value_AR = analogRead(A_LINE_R);
     L_SPEED = 144;
     R_SPEED = 140;
     int flip = 0;
    while(value_AL < 350){
          MOTOR_CONTROL(L);
          delay(20); // Adjust this delay to accurately achieve a 90-degree turn
          MOTOR_CONTROL(STOP);
          delay(15);
          if(flip){
          MOTOR_CONTROL(FWD);
          delay(50);
          flip = 0;
          }
          else{
            flip = 1;
          }
          value_AL = analogRead(A_LINE_L);
          value_AR = analogRead(A_LINE_R);
          
    }
    }
  }
}
