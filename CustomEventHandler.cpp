#include "CustomEventHandler.h"


bool CustomEventHandler::handle(const osgGA::GUIEventAdapter & ea, osgGA::GUIActionAdapter &)
{
	switch (ea.getEventType()) {
	case(osgGA::GUIEventAdapter::KEYDOWN):
	{
		switch (ea.getKey()) {
		case 'w':
			truckInputDeviceState->moveFwdRequest = true;
			return false;
			break;
		
		case 's':
			truckInputDeviceState->moveBackRequest = true;
			return false;
			break;
		}
	}
	case(osgGA::GUIEventAdapter::KEYUP):
	{
		switch (ea.getKey()) {
		case 'w':
			truckInputDeviceState->moveFwdRequest = false;
			return false;
			break;
		
		case 's':
			truckInputDeviceState->moveBackRequest = false;
			return false;
			break;
		}
	}
	default: {
		return false;
	}
	}
}

CustomEventHandler::~CustomEventHandler()
{
}
