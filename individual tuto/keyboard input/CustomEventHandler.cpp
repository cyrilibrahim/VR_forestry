#include "CustomEventHandler.h"



bool CustomEventHandler::handle(const osgGA::GUIEventAdapter & ea, osgGA::GUIActionAdapter &)
{
	switch (ea.getEventType()) {
		case(osgGA::GUIEventAdapter::KEYDOWN):
		{
			switch (ea.getKey()) {
				case 'w':
					tankInputDeviceState->moveFwdRequest = true;
					return false;
					break;
			}
		}
		default: {
			return false;
		}
	}
}
