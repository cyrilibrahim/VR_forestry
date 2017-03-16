#pragma once
//class definition to enable communication between the keyboard event handler and the update callback
class truckInputDeviceStateType
{
public:
	truckInputDeviceStateType::truckInputDeviceStateType() {
		moveFwdRequest = false;
		moveBackRequest = false;
	}

	bool moveFwdRequest;
	bool moveBackRequest;
};