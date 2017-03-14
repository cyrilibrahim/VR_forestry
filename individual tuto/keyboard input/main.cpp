
#include <gl/glut.h> 
#include <osg/Group>
#include <osgDB/ReadFile> 
#include <osg/PositionAttitudeTransform>
#include <osgViewer/Viewer>
#include <osgSim/DOFTransform>
#include <osgSim/MultiSwitch>
#include "findNodeVisitor.h"
#include "CustomEventHandler.h"
#include "tankdataupdate.h"
#include <osgGA/TrackballManipulator>

int main()
{

	osgViewer::Viewer viewer;

	// Declare a group for the root of our tree and 
	//  three groups to contain individual tank models
	osg::Group* root = new osg::Group();

	osg::PositionAttitudeTransform* tankPat = new osg::PositionAttitudeTransform();
	osg::Node* tankNode = osgDB::readNodeFile("models/t72-tank/t72-tank_des.flt");
	
	tankPat->addChild(tankNode);
	root->addChild(tankPat);

	//Declare instance of class to record state of keybord
	tankInputDeviceStateType* tIDevState = new tankInputDeviceStateType;

	// Set up the tank update callback
	//  pass the constructor a pointer to our tank input device state
	//  that we declared above.
	tankPat->setUpdateCallback(new updateTankPosCallback(tIDevState));

	// The constructor for our event handler also gets a pointer to
	//   our tank input device state instance
	CustomEventHandler* myEventHandler = new CustomEventHandler(tIDevState);
	

	// Add our event handler to the list
	viewer.addEventHandler(myEventHandler);

	viewer.setSceneData(root);
	viewer.setCameraManipulator(new osgGA::TrackballManipulator());
	viewer.realize();

	while (!viewer.done())
	{
		viewer.frame();
	}
}
