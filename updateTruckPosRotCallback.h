#pragma once

#include <GL/glut.h>
#include <osg/PositionAttitudeTransform>
#include "truckInputDeviceStateType.h"
#include <iostream>
// Callback to make the truck tank move forward
class updateTruckPosCallback : public osg::NodeCallback
{
public:
	updateTruckPosCallback(truckInputDeviceStateType* truckIDevState, osg::Node* terrainModel,std::vector <osg::BoundingBox> treesList )
	{
		truckInputDeviceState = truckIDevState;
		this->terrainModel = terrainModel;
		this->treesList = treesList;
		angle = 0;
		speed = 0.1;
		rotationSpeed = 0.1;
		direction = osg::Vec3f(0,0,0);
	}
	//We override the operator () wich will be executed at each update
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {



		//We have a transform node who contain the tank
		osg::PositionAttitudeTransform* pat = dynamic_cast<osg::PositionAttitudeTransform*> (node);

		//Calculer la direction actuel 
		direction = osg::Vec3(cos(osg::DegreesToRadians(angle + 90)), sin(osg::DegreesToRadians(angle + 90)), 0);
		// La direction perpendiculaire (utile pour la bounding box)
		osg::Vec3 direction_perp = osg::Vec3(cos(osg::DegreesToRadians(angle)), sin(osg::DegreesToRadians(angle)), 0);
		direction.normalize();

		if (pat) {

			//Collision avec les arbres

			// L'idee ici est de calculer la transformation suivante, si cette transformation mene a une collision on ne
			// l'execute pas. 
			osg::PositionAttitudeTransform* nextPat = new osg::PositionAttitudeTransform(*pat);
			double new_angle = angle;
			
			osg::Quat att(pat->getAttitude());
			

			if (truckInputDeviceState->moveFwdRequest)
				nextPat->setPosition(pat->getPosition() + direction * speed);
			if (truckInputDeviceState->moveBackRequest)
				nextPat->setPosition(pat->getPosition() - direction * speed);
			if (truckInputDeviceState->turnLeftRequest) 
				new_angle = new_angle + 5 * rotationSpeed;
			if (truckInputDeviceState->turnRightRequest)
				new_angle = new_angle - 5 * rotationSpeed;

			//Ne laisse pas deborder l'angle
			if (new_angle > 360)
				new_angle = 0;
			else if (new_angle < 0)
				new_angle = 360;
			
			//MAJ de l'angle
			nextPat->setAttitude(osg::Quat(osg::DegreesToRadians(new_angle), osg::Vec3(0, 0, 1)));

			//Calcul si la transformation suivant entre en collision
			bool collision = false;

			// Recuperation de la bounding box du camion
			osg::BoundingSphere currentTankBB = nextPat->getBound();
			currentTankBB.set(currentTankBB.center() + direction_perp * 0.4, 0.5);
			osg::BoundingBox bbTank;
			bbTank.expandBy(currentTankBB);

			//Bounding box debug mode
			if (bbTruckVisual) {
				bbTruckVisual->setPosition(bbTank.center());
				bbTruckVisual->setScale((bbTank._max - bbTank._min));
			}

			// On verifie si le camion entre en collision avec un arbre
			for (int i = 0; i <treesList.size(); i++) {
				osg::BoundingBox box = treesList[i];

				if (bbTank.intersects(box)) {
					collision = true;
					continue;
				}
			}

			//Si il n'y a pas de collision on execute l'action sur le camion
			if (!collision) {
				angle = new_angle;
				pat->setAttitude(osg::Quat(osg::DegreesToRadians(angle), osg::Vec3(0, 0, 1)));
				pat->setPosition(nextPat->getPosition());
			}
			// Sinon on execute aucune action
		}
		
		//Mettre a jour la hauteur du camion
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

	//Accesseur pour la direction
	osg::Vec3 getTruckDirection() {
		return direction;
	}

	// bbTruckVisual est une box simple afficher sur la scene pour pouvoir afficher la bounding box du camion
	void setBbTruckVisual(osg::PositionAttitudeTransform* box){
		bbTruckVisual = box;
	}

protected:
	truckInputDeviceStateType* truckInputDeviceState;
	osg::Node* terrainModel;
	double angle;
	osg::Vec3f direction;
	double speed;
	double rotationSpeed;
	std::vector<osg::BoundingBox> treesList;
	osg::PositionAttitudeTransform* bbTruckVisual;
};
