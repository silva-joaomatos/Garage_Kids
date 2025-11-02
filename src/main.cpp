#include <Arduino.h>

#include <Servo.h>
#include <NewPing.h>
#include <SPI.h>
#include <MFRC522.h>
#include "LedController.hpp"
#define RST_PIN         5          // Configurable 
#define SS_PIN          10         // Configurable
#define TRIGGER_PIN  6  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     7  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define US_ROUNDTRIP_CM 57 // Constant for converting ping time to centimeters.
#define OPEN 1
#define CLOSED 0



//These are the Pins used for the SPI transfer
//See the usage instructions for their meaning
#define DIN A0
#define CS A1
#define CLK A2

//The total numer of Segments
#define Segments 4

//The delay between movements
#define delayTime 200

//This creates an uninitilized LedController object.
//It will be initilized in the setup function.
LedController<Segments,1> lc = LedController<Segments,1>();

//This is my pixelart of a rocket which will be used in this example
ByteBlock rocket= ByteBlock::reverse({
  B00000000,
  B00001111,
  B00111110,
  B11111101,
  B00111110,
  B00001111,
  B00000000,
  B00000000
});
ByteBlock rocketColumns;


NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
unsigned int pingSpeed = 50; // How frequently are we going to send out a ping (in milliseconds). 50ms would be 20 times a second.
unsigned long pingTimer;     // Holds the next ping time.
Servo myservo;  // create Servo object to control a servo
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

int pos = 0;    // variable to store the servo position
int distance = 0; // variable to store the ping distance
int state = 0;
bool cardpresent = false;

void echoCheck(); // Forward declaration of the echoCheck function.
byte checkCard() {
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return 0x00;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return 0x00;
  }

  // Dump debug info about the card; PICC_HaltA() is automatically called
  //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
 // Serial.println(mfrc522.uid.uidByte[0],HEX);
  cardpresent = true;
  return mfrc522.uid.uidByte[0];
 
} 
void setup() {
 Serial.begin(115200);		// Initialize serial communications with the PC
	while (!Serial);		// Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
	SPI.begin();			// Init SPI bus
	mfrc522.PCD_Init();		// Init MFRC522
	delay(4);				// Optional delay. Some board do need more time after init to be ready, see Readme
	mfrc522.PCD_DumpVersionToSerial();	// Show details of PCD - MFRC522 Card Reader details
	Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));

  myservo.attach(9);  // attaches the servo on pin 9 to the Servo object
  myservo.write(0); //initially close the door
  pingTimer = millis(); // Start now.
  //Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
  //Serial.println("NewPing Ultrasonic Servo Sweep Example");
  //initilizes the LedController without hardware spi.
  lc.init(DIN,CLK,CS);

  //make a array of columns out of the rocket
  //this is needed to shift it in correctly (you can leave this line if you want to)
  rocketColumns = ByteBlock::makeColumns(rocket);
}
void displayRocket(){
  lc.clearMatrix();

  for(int dir = 0; dir < 2; dir++) {
    delay(delayTime);
    for(int i = 0; i < 8*(Segments+1); i++) {
      //blink led for each iteration

      //if rocket not fully inside let it fly in
      auto in = (i<8) ? rocketColumns[i] : 0x00;

      //if dir is 0 move right if not move left
      dir == 0 ? lc.moveRight(in) : lc.moveLeft(in);

      delay(delayTime);

      //decide whether to move up or down
      if(i > 7) {
        if(i % 6 < 3) {
          lc.moveDown();
        } else {
          lc.moveUp();
        }
      }

      delay(delayTime);

    }
  }

}

void loop() {
 
  displayRocket(); //Display the rocket animation
  byte uid = checkCard();
  if (millis() >= pingTimer) {   // pingSpeed milliseconds since last ping, do another ping.
    pingTimer += pingSpeed;      // Set the next ping time.
    sonar.ping_timer(echoCheck); // Send out the ping, calls "echoCheck" function every 24uS where you can check the ping status.
  }
  if ((distance < 20) || uid == 0x5D){ //if distance less than 20cm or card uid is 0x5D open the door
   // myservo.write(180); //this only to test servo working
    state = OPEN;
    Serial.println("Door Open");
    cardpresent = false; //reset cardpresent after opening door
  }
  else if (distance >=20 ){
    ///myservo.write(0); //only test servo working
    state = CLOSED;
  }

  // If state == OPEN, keep the door open for 5 seconds without delay function. if distance < 20cm keep open, only close 5seconds after distance is >=20cm
 // Use millis() to track time, not delay(). 
//update below code so that door only closes 5 seconds after distance is >=20cm
  static unsigned long doorOpenTime = 0;
  if (state == OPEN) {
    myservo.write(180); // Open the door
    doorOpenTime = millis(); // Reset the door open timer
  } else if (state == CLOSED) {
    if (millis() - doorOpenTime >= 2000) { // Check if 2 seconds have passed since the door was opened
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