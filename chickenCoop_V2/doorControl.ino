/* File: doorControl.ino
 * Short desc: Handles auto/manual controls for the door motor.
 * IN AUTO MODE:
 *	- runs sunrise/sunset state machines
 *	- when opening/closing door, runs motor state machine 
 *	  with fixed motor interval
 *  - user can adjust motor interval in UI
 * IN MANUAL MODE:
 *	- lets user run motor up/down manually
 *		> web client buttons
 *		> physical press buttons
 *	- lets user manually set open/close status if hook was used
 */

void startDoor(bool dir, uint8_t interval) {
  motorInterval = interval;
  motorStartTime = currentTime;
  motorTime = 0;
  if (dir) {
    motorOn = openDoor();
  } else {
    motorOn = closeDoor();
  }
  updateDoorStatus();
}

// start running motor to open door
bool openDoor() {
  motorDir = true;
  motor.drive(OPEN_DR);
  return true;
}

// start closing door
bool closeDoor() {
  motorDir = false;
  motor.drive(CLOSE_DR); 
  return true;
}

bool stopDoor() {
	motor.brake();
	motorTime = 0; // reset motor time counter
	return false;
}

// when done running the motor, update door status
void updateDoorStatus() {
  if (!motorOn) {
    if (motorDir) { 
      doorStatus = true;
    } else {
      doorStatus = false;
    }
  }
  broadcastChange('d');
}

void updateAutoMode() {
  autoMode = !autoMode;
  motorOn = stopDoor();
  broadcastChange('c');
}
