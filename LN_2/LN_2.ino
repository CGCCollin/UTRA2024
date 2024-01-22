enum State {
  NORM,
  SEARCH,
  JUNCTION
};

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
// Global variables
unsigned long lostTimerStart = 0;
bool isTimerRunning = false;
int lastTurnDirection = L; // Initialize with a default direction
//stores direction last correction L = 0, R = 1
int LAST_CORRECTION = 0;


State currentState = NORM;

// Global variables
unsigned long searchPatternStart = 0;
bool isSearchTimerRunning = false;
enum SearchPhase {
  TURNING,
  STOPPING,
  MOVING_FORWARD
};

SearchPhase currentSearchPhase = TURNING;
unsigned long searchPatternTimer = 0;

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
  value_AL = analogRead(A_LINE_L); // reads the analog input from the IR distance sensor
  value_AR = analogRead(A_LINE_R);

  switch (currentState) {
    case NORM:
      handleNormState();
      break;
    case SEARCH:
      handleSearchState();
      break;
  }
}

void handleNormState() {
  if (isJunctionDetected()) {
    enterJunctionState();
  } else if (needsCourseCorrection()) {
    courseCorrect();
  } else if (isLost()) {
    enterSearchState();
  } else {
    maintainCourse();
  }
}

bool isJunctionDetected() {
  return value_AL > 250 && value_AR > 250;
}


void enterJunctionState() {
  currentState = JUNCTION;
  // Move forward a bit more to ensure it's fully over the junction
  L_SPEED = 180;
  R_SPEED = 180;
  MOTOR_CONTROL(FWD);
  unsigned long moveStartTime = millis();
  while (millis() - moveStartTime < 5) {
    // Wait for 5ms
  }
  startJunctionTurn();
}

void startJunctionTurn() {
  // Set turning speed
  L_SPEED = 184; // Adjust speed as necessary
  R_SPEED = 180; // Adjust speed as necessary

  // Start turning in the last direction
  continueTurn();

  // Now, keep turning until a line is intersected
  while (!isLineIntersected()) {
    // Just keep turning, no forward burst
    continueTurn();
  }

  currentState = NORM; // Return to normal operation
}

void continueTurn() {
  if (lastTurnDirection == L) {
    R_SPEED = 190;
    MOTOR_CONTROL(L);
  } else {
    MOTOR_CONTROL(R);
  }
}


bool isLineIntersected() {
  // Implement logic to determine if either sensor intersects with the line
  // Placeholder implementation
  value_AL = analogRead(A_LINE_L);
  value_AR = analogRead(A_LINE_R);
  return (value_AL > THRESHOLD_L || value_AR > THRESHOLD_R);
}


void handleSearchState() {
  if (findsTrack()) {
    exitSearchState();
  } else {
    searchPattern();
  }
}

bool findsTrack() {
  // Implement logic to detect the track
  // Placeholder implementation - modify according to your sensor setup
  value_AL = analogRead(A_LINE_L);
  value_AR = analogRead(A_LINE_R);
  return (value_AL > THRESHOLD_L || value_AR > THRESHOLD_R);
}

bool needsCourseCorrection() {
  return value_AL > 250 || value_AR > 300;
}

void courseCorrect() {
  if (value_AL > 300) {
    // Course correct left
    turnLeft();
  } else if (value_AR > 350) {
    // Course correct right
    turnRight();
  }
}

void turnLeft() {
  // Implement left turning logic, adjust speeds or angles as needed
  lastTurnDirection = L;
  L_SPEED = 174;
  R_SPEED = 190;
  MOTOR_CONTROL(L);
  // Use non-blocking delay alternative here
  // ...
}

void turnRight() {
  // Implement right turning logic, adjust speeds or angles as needed
  lastTurnDirection = R;
  L_SPEED = 184;
  R_SPEED = 180;
  MOTOR_CONTROL(R);
  // Use non-blocking delay alternative here
  // ...
}

bool isLost() {
  if (!isTimerRunning) {
    lostTimerStart = millis();
    isTimerRunning = true;
  }

  if (value_AL > 300 || value_AR > 300) {
    lostTimerStart = millis(); // Reset timer if either sensor detects the line
    return false;
  } else if (millis() - lostTimerStart > 1000) {
    isTimerRunning = false; // Stop the timer as the robot is now considered lost
    return true;
  }
  return false;
}

unsigned long lastMoveTime = 0;
bool isMoving = true;
const int moveDuration = 10; // Time to move before stopping (in milliseconds)
const int stopDuration = 20; // Time to stop for better detection (in milliseconds)

void maintainCourse() {
  unsigned long currentTime = millis();

  if (isMoving && currentTime - lastMoveTime > moveDuration) {
    MOTOR_CONTROL(STOP); // Stop motors
    isMoving = false;
    lastMoveTime = currentTime;
  } else if (!isMoving && currentTime - lastMoveTime > stopDuration) {
    L_SPEED = 184;
    R_SPEED = 180;
    MOTOR_CONTROL(FWD); // Move forward
    isMoving = true;
    lastMoveTime = currentTime;
  }
}

void enterSearchState() {
  currentState = SEARCH;
  // Implement any initialization for search state
}

unsigned long searchStartTime = 0;
const unsigned long searchTimeout = 4000; // 4 seconds in milliseconds

void searchPattern() {
  unsigned long currentTime = millis();

  // Start the search timer if it's not already running
  if (!isSearchTimerRunning) {
    isSearchTimerRunning = true;
    searchPatternTimer = currentTime;
    searchStartTime = currentTime; // Record the start time of the search
    currentSearchPhase = TURNING;
  }

  // Check if search time has exceeded the timeout
  if (currentTime - searchStartTime >= searchTimeout) {
    // Timeout reached, stop searching
    MOTOR_CONTROL(STOP);
    currentState = NORM; // Return to normal operation or handle timeout as needed
    isSearchTimerRunning = false;
    return; // Exit the function
  }




  switch (currentSearchPhase) {
    case TURNING:
      if (currentTime - searchPatternTimer < 20) {
        // Turn for 20ms
        if (lastTurnDirection == L) {
          MOTOR_CONTROL(L);
        } else {
          MOTOR_CONTROL(R);
        }
      } else {
        // Next, stop for 40ms
        MOTOR_CONTROL(STOP);
        currentSearchPhase = STOPPING;
        searchPatternTimer = currentTime;
      }
      break;

    case STOPPING:
      if (currentTime - searchPatternTimer >= 40) {
        // After stopping, move forward for 20ms
        currentSearchPhase = MOVING_FORWARD;
        searchPatternTimer = currentTime;
      }
      break;

    case MOVING_FORWARD:
      if (currentTime - searchPatternTimer < 20) {
        // Move forward for 20ms
        MOTOR_CONTROL(FWD);
      } else {
        // Cycle complete, return to turning
        MOTOR_CONTROL(STOP);
        currentSearchPhase = TURNING;
        searchPatternTimer = currentTime;
        isSearchTimerRunning = false; // Reset for next cycle
      }
      break;
  }
}


void exitSearchState() {
  currentState = NORM;
  // Implement any cleanup or re-initialization for normal state
}

void MOTOR_CONTROL (int CMD){
  
  switch(CMD){
    case FWD: 
    //go forward
    //LMOTOR CW
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    //RMOTOR CW
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
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
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
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
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
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
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    //ENABLE MOTORS
    analogWrite(EN_A,L_SPEED);
    analogWrite(EN_B,R_SPEED);
    break;
    case STOP:
    
    //stop
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    //RMOTOR CCW
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, HIGH);
    analogWrite(EN_A,0);
    analogWrite(EN_B,0);
    break;
    default:
    //tbd
    break;
  }
  
}
