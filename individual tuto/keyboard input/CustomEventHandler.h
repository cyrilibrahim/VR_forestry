#ifndef KEYBOARD_HANDLER_H
#define KEYBOARD_HANDLER_H

#include <gl/glut.h> 
#include <iostream>
#include <osgGA/GUIEventHandler>
#include "tankInputDeviceStateType.h"


class CustomEventHandler : public osgGA::GUIEventHandler
{
	public:
		CustomEventHandler(tankInputDeviceStateType* tids) {
			tankInputDeviceState = tids;
		}
		virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter&);


	protected:
		tankInputDeviceStateType* tankInputDeviceState;
};
#endif