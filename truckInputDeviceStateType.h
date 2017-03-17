#pragma once
//class definition to enable communication between the keyboard event handler and the update callback
class truckInputDeviceStateType
{
public:
	truckInputDeviceStateType() {
		moveFwdRequest = false;
		moveBackRequest = false;
		turnLeftRequest = false;
		turnRightRequest = false;
	}

	bool moveFwdRequest;
	bool moveBackRequest;
	bool turnLeftRequest;
	bool turnRightRequest;
};
