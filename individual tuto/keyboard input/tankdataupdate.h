#pragma once

#include <gl/glut.h> 
#include <osg/Group>
#include <osgDB/ReadFile> 
#include <osg/PositionAttitudeTransform>
#include <osgViewer/Viewer>
#include <osgSim/DOFTransform>
#include <osgSim/MultiSwitch>
#include "findNodeVisitor.h"
#include "CustomEventHandler.h"
#include "tankInputDeviceStateType.h"
#include <osgGA/TrackballManipulator>
#include <iostream>


class tankDataType : public osg::Referenced
{
public:
	//Constructor (tankDataType)
	tankDataType(osg::Node*n) {
		rotation = 0;
		elevation = 0;

		findNodeVisitor findTurret("turret");
		n->accept(findTurret);
		tankTurretNode = dynamic_cast <osgSim::DOFTransform*> (findTurret.getFirst());
		findNodeVisitor findGun("gun");
		n->accept(findGun);
		tankGunNode = dynamic_cast <osgSim::DOFTransform*> (findGun.getFirst());
	}

	void updateGunElevation() {
		elevation += 0.001;
		tankGunNode->setCurrentHPR(osg::Vec3(0, elevation, 0));

		if (elevation > 0.5) {
			elevation = 0.0;
		}
	}

	void updateTurretRotation() {
		rotation += 0.001;
		tankTurretNode->setCurrentHPR(osg::Vec3(rotation, 0, 0));
	}

protected:
	osgSim::DOFTransform* tankTurretNode;
	osgSim::DOFTransform* tankGunNode;
	double rotation;
	double elevation;
};





//Call back to turn Turret and elevate gun
class tankNodeCallBack : public osg::NodeCallback
{
public:
	//We override the operator () wich will be executed at each update
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {

		osg::ref_ptr<tankDataType> tankData = dynamic_cast <tankDataType*> (node->getUserData());
		if (tankData) {
			tankData->updateGunElevation();
			tankData->updateTurretRotation();
		}
		traverse(node, nv);
	}
};

//Callback to make the tank move forward
class updateTankPosCallback : public osg::NodeCallback
{
public:
	updateTankPosCallback(tankInputDeviceStateType* tankIDevState)
	{
		tankInputDeviceState = tankIDevState;
	}
	//We override the operator () wich will be executed at each update
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {

		//We have a transform node who contain the tank
		osg::PositionAttitudeTransform* pat = dynamic_cast<osg::PositionAttitudeTransform*> (node);		
		if (pat) {
			if (tankInputDeviceState->moveFwdRequest) {
				tankPos = tankPos + osg::Vec3(0.0, 0.1, 0.0);
				pat->setPosition(tankPos);
			}
		}
		traverse(node, nv);
	}
protected:
	tankInputDeviceStateType* tankInputDeviceState;
	osg::Vec3 tankPos;
};

