#include <Arduino.h>

/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 https://www.arduino.cc/en/Tutorial/LibraryExamples/Sweep
*/

#include <Servo.h>
#include <NewPing.h>
#define TRIGGER_PIN  6  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     7  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define US_ROUNDTRIP_CM 57 // Constant for converting ping time to centimeters.
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

unsigned int pingSpeed = 50; // How frequently are we going to send out a ping (in milliseconds). 50ms would be 20 times a second.
unsigned long pingTimer;     // Holds the next ping time.

Servo myservo;  // create Servo object to control a servo
// twelve Servo objects can be created on most boards

int pos = 0;    // variable to store the servo position
int distance = 0; // variable to store the ping distance
int state = 0;
#define OPEN 1
#define CLOSED 0
void echoCheck(); // Forward declaration of the echoCheck function.
void setup() {
  myservo.attach(9);  // attaches the servo on pin 9 to the Servo object
  pingTimer = millis(); // Start now.
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
  Serial.println("NewPing Ultrasonic Servo Sweep Example");
}

void loop() {

  if (millis() >= pingTimer) {   // pingSpeed milliseconds since last ping, do another ping.
    pingTimer += pingSpeed;      // Set the next ping time.
    sonar.ping_timer(echoCheck); // Send out the ping, calls "echoCheck" function every 24uS where you can check the ping status.
  }
  if (distance < 20){
   // myservo.write(180);
    state = OPEN;
  }
  if (distance >=20 ){
    ///myservo.write(0);
    state = CLOSED;
  }

  // If state == OPEN, keep the door open for 5 seconds without delay function. if distance < 20cm keep open, only close 5seconds after distance is >=20cm
 // Use millis() to track time, not delay(). below code not working as intended as door is kept open.
//update below code so that door only closes 5 seconds after distance is >=20cm
  static unsigned long doorOpenTime = 0;
  if (state == OPEN) {
    myservo.write(180); // Open the door
    doorOpenTime = millis(); // Reset the door open timer
  } else if (state == CLOSED) {
    if (millis() - doorOpenTime >= 5000) { // Check if 5 seconds have passed since the door was opened
      myservo.write(0); // Close the door
    }
  }
 
}
void echoCheck() { // Timer2 interrupt calls this function every 24uS where you can check the ping status.
  // Don't do anything here!
  if (sonar.check_timer()) { // This is how you check to see if the ping was received.
    // Here's where you can add code.
   // Serial.print("Ping: ");
    //Serial.print(sonar.ping_result / US_ROUNDTRIP_CM); // Ping returned, uS result in ping_result, convert to cm with US_ROUNDTRIP_CM.
    distance = sonar.ping_result / US_ROUNDTRIP_CM;
    /*Serial.print(distance);
    Serial.println("cm");*/
  }
}