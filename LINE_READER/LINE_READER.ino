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
//Note that higher values correspond to more light.
//Digital 1 = lots of light
double value_AL;
double value_AR;
const int readings = 20;
double sensorReadings[readings];

//Motor Pins
int EN_A = 11; //Enable pin for first motor
int IN1 = 9; //control pin for first motor (L)
int IN2 = 8; //control pin for first motor (L)
int IN3 = 7; //control pin for second motor (R)
int IN4 = 6; //control pin for second motor (R)
int EN_B = 10; //Enable pin for second motor
//Initializing variables to store data
int motor_speed;
int motor_speed1;
//MOTOR SPEED FROM 0-255

const double threshold_L = 500;
const double threshold_R = 500;

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
   Serial.println("LOOP");
   value_AL = analogRead(A_LINE_L); // reads the analog input from the IR distance sensor
   value_AR = analogRead(A_LINE_R);
   
  Serial.println("ANALOG_L: " + String(value_AL) + "\n"
                  "ANALOG_R: " + String(value_AR) ); // Print the value to the serial monitor
  //course_correct();
  //if(check_distance() <= 10.0){
  //  MOTOR_CONTROL(STOP);
  //}
  // delay(1000); // Wait for 100 milliseconds before reading again
  if(check_distance() > 20){
    course_correct();
  }
  else{
    path();
  }
  
}

void course_correct(){
  MOTOR_CONTROL(0);
  if(value_AL > 300){
    //TODO: left turn
    MOTOR_CONTROL(STOP);
    delayMicroseconds(2);
    MOTOR_CONTROL(2);
  }

  if(value_AR > 300){
    //TODO: right turn
    MOTOR_CONTROL(STOP);
    delayMicroseconds(2);
    MOTOR_CONTROL(3);
  }

  
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
    analogWrite(EN_A,255);
    analogWrite(EN_B,255);
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
    analogWrite(EN_A,255);
    analogWrite(EN_B,255);
    break;
    case L:
    //Turn left
    //LMOTOR CCW
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    //RMOTOR CW
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    //ENABLE MOTORS
    analogWrite(EN_A,255);
    analogWrite(EN_B,255);
    break;
    case R:
    //Turn right
    //LMOTOR CW
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    //RMOTOR CCW
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    //ENABLE MOTORS
    analogWrite(EN_A,255);
    analogWrite(EN_B,255);
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
   Serial.print("DISTANCE: ");
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
      MOTOR_CONTROL(STOP);
      delay(200);
      sensorReadings[index++] = check_distance();
      MOTOR_CONTROL(R);
      delay(50);
    }
  }
  // Turn left and take readings
  MOTOR_CONTROL(L);
  delay(250);
  for (int i = 0; i < (readings/2); i++) { // Assuming 200ms at 10ms per reading
    if (index < arraySize) {
      MOTOR_CONTROL(STOP);
      delay(200);
      sensorReadings[index++] = check_distance();
      MOTOR_CONTROL(L);
      delay(50);
    }
  }
  MOTOR_CONTROL(R);
  delay(300);
  MOTOR_CONTROL(STOP);
  

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
      delay(50);
      MOTOR_CONTROL(STOP);
      delay(50);
    }
  }
  else {
        for(int j = 0; j < (i - (readings/2)); j++){
          MOTOR_CONTROL(L);
          delay(50);
          MOTOR_CONTROL(STOP);
          delay(50);
        }

  }
}
