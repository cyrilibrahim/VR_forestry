#pragma once

#include <gl/glut.h> 
#include <osg/PositionAttitudeTransform>
#include "truckInputDeviceStateType.h"
#include <iostream>
// Callback to make the truck tank move forward
class updateTruckPosCallback : public osg::NodeCallback
{
public:
	updateTruckPosCallback(truckInputDeviceStateType* truckIDevState)
	{
		truckInputDeviceState = truckIDevState;
	}
	//We override the operator () wich will be executed at each update
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {

		//We have a transform node who contain the tank
		osg::PositionAttitudeTransform* pat = dynamic_cast<osg::PositionAttitudeTransform*> (node);
		if (pat) {
			if (truckInputDeviceState->moveFwdRequest) {
				pat->setPosition(pat->getPosition() + osg::Vec3(0.0, 0.1, 0.0));
			}
			if (truckInputDeviceState->moveBackRequest) {
				pat->setPosition(pat->getPosition() - osg::Vec3(0.0, 0.1, 0.0));
			}
		}
		traverse(node, nv);
	}
protected:
	truckInputDeviceStateType* truckInputDeviceState;
};