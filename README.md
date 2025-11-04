
### Project Description
This project enables the control of a single servo motor (OPEN/CLOSE) to simulate a garage door.

### Door Opening Methods
The door can be opened through two methods:
* ULS distance detected < 20cm (configurable)
* RFID chip with a hardcoded UID (currently only first byte of chipID is checked)

### Automatic Closure
The door will automatically close if distance is >=20cm for 2 seconds (configurable)

### LED Matrix Features
The LED Matrix provides visual information:
* Matrix displays smiley face when door is OPEN
* Matrix displays sad face when door is CLOSED (displays for 5 seconds)
* Matrix displays rocket animation every 5 seconds when door is idle
    > *Note: Rocket animation adapted from library example, modified to avoid delay() function*
