# Garage_Kids

Project allows the control of a single servo motor (OPEN/CLOSE) to simulate a garage door.

The Door may be OPEN currently by two methods:
1- ULS distance detected < 20cm (configurable)
2- RFID chip with an hardcoded UID (currently only first byte of chipID is checked)

The Door will automatically close if distance is >=20cm for 2sec (configurable)

Additionally:
LED Matrix is added for visual information
1- Matrix will display smiley face if Door is OPEN
2- Matrix will display sad face if Door is CLOSE (display for 5sec) 
3- Matrix will display a rocket animation each 5 sec if Door is idle
----Rocket annimation taken from Library example, modified to avoid delay() function----