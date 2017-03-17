#pragma once

#include <GL/glut.h>
#include <osg/PositionAttitudeTransform>
#include "truckInputDeviceStateType.h"
#include <iostream>
// Callback to make the truck tank move forward
class updateTruckPosCallback : public osg::NodeCallback
{
public:
	updateTruckPosCallback(truckInputDeviceStateType* truckIDevState, osg::Node* terrainModel)
	{
		truckInputDeviceState = truckIDevState;
		this->terrainModel = terrainModel;
		angle = 0;
		speed = 0.1;
		rotationSpeed = 0.1;
		direction = osg::Vec3(0,0,0);
	}

	//We override the operator () wich will be executed at each update
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {

		//We have a transform node who contain the tank
		osg::PositionAttitudeTransform* pat = dynamic_cast<osg::PositionAttitudeTransform*> (node);
		
		osg::Quat rotIncr(0.01, osg::Vec3(0, 0, 1));

		//Calculer la direction actuel 
		direction = osg::Vec3(cos(osg::DegreesToRadians(angle + 90)), sin(osg::DegreesToRadians(angle + 90)), 0);
		direction.normalize();

		if (pat) {
			osg::Quat att(pat->getAttitude());
			
			if (truckInputDeviceState->moveFwdRequest)
				pat->setPosition(pat->getPosition() + direction * speed);
			if (truckInputDeviceState->moveBackRequest)
				pat->setPosition(pat->getPosition() - direction * speed);
			if (truckInputDeviceState->turnLeftRequest) 
				angle = angle + 5 * rotationSpeed;
			if (truckInputDeviceState->turnRightRequest)
				angle = angle - 5 * rotationSpeed;

			//Ne laisse pas deborder l'angle
			if (angle > 360)
				angle = 0;
			else if (angle < 0)
				angle = 360;
			
			//MAJ de l'angle
			pat->setAttitude(osg::Quat(osg::DegreesToRadians(angle), osg::Vec3(0, 0, 1)));
		
		
			//Mettre a jour la hauteur du camion
			// Ray-tracing pour planter les arbres
			osg::LineSegment* truckHeightRay = new osg::LineSegment();
			osg::Vec2 camionXY = osg::Vec2(pat->getPosition().x(), pat->getPosition().y());
			truckHeightRay->set(
				osg::Vec3(camionXY, 9999),
				osg::Vec3(camionXY, -999));

			osgUtil::IntersectVisitor truckElevationVisitor;
			truckElevationVisitor.addLineSegment(truckHeightRay);
			terrainModel->accept(truckElevationVisitor);

			osgUtil::IntersectVisitor::HitList heightHits;
			heightHits = truckElevationVisitor.getHitList(truckHeightRay);
			osgUtil::Hit HeightResults;
			osg::Vec3d truckPos(camionXY, 800.);
			if (!heightHits.empty()) {
				HeightResults = heightHits.front();
				truckPos = HeightResults.getWorldIntersectPoint();
			}
			pat->setPosition(truckPos);
			traverse(node, nv);
		}
	}

	//Accesseur pour la direction
	osg::Vec3 getTruckDirection() {
		return direction;
	}

protected:
	truckInputDeviceStateType* truckInputDeviceState;
	osg::Node* terrainModel;
	double angle;
	osg::Vec3 direction;
	double speed;
	double rotationSpeed;
};
