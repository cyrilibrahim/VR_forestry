#pragma once
//class definition to enable communication between the keyboard event handler and the update callback
class tankInputDeviceStateType
{
public:
	tankInputDeviceStateType::tankInputDeviceStateType() {
		moveFwdRequest = false;
	}
		
	bool moveFwdRequest;
};