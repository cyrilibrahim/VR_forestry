#pragma once
#include <GL/glut.h>
#include <iostream>
#include <osgGA/GUIEventHandler>
#include "truckInputDeviceStateType.h"
class CustomEventHandler :
	public osgGA::GUIEventHandler
{
public:
	CustomEventHandler(truckInputDeviceStateType* tids) {
		truckInputDeviceState = tids;
	}
	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter&);
	~CustomEventHandler();
protected:
	truckInputDeviceStateType* truckInputDeviceState;
};

