
#include <gl/glut.h> 
#include <osgViewer/Viewer> 
#include <osg/PositionAttitudeTransform>
#include <osgGA/TrackballManipulator>
#include <osgDB/ReadFile> 
#include <osg/Node>
#include <iostream>


int main(void)
{
	//Initialize the scene viewer
	osgViewer::Viewer viewer;
	
	//Create the node of our models
	osg::Node* cessnaNode = NULL;
	osg::Node* tankNode = NULL;

	cessnaNode = osgDB::readNodeFile("cessna.osg");
	tankNode = osgDB::readNodeFile("models/T72-tank/t72-tank_des.flt");
	
	// Declare a node which will serve as the root node
	// for the scene graph. Since we will be adding nodes
	// as 'children' of this node we need to make it a 'group'
	// instance.
	// The 'node' class represents the most generic version of nodes.
	// This includes nodes that do not have children (leaf nodes.)
	// The 'group' class is a specialized version of the node class. 
	// It adds functions associated with adding and manipulating
	// children.
	osg::Group* root = new osg::Group();
	root->addChild(cessnaNode);

	//Declare transform,initialize with defaults.
	osg::PositionAttitudeTransform* tankXform = new osg::PositionAttitudeTransform();

	// Use the 'addChild' method of the osg::Group class to
	// add the transform as a child of the root node and the
	// tank node as a child of the transform.
	root->addChild(tankXform);
	tankXform->addChild(tankNode);

	// Declare and initialize a Vec3 instance to change the
	// position of the tank model in the scene
	osg::Vec3 tankPosit(5, 0, 0);
	tankXform->setPosition(tankPosit);

	// Next we will need to assign the scene graph we created 
	// above to this viewer:
	viewer.setSceneData(root);

	// attach a trackball manipulator to all user control of the view
	viewer.setCameraManipulator(new osgGA::TrackballManipulator);

	// create the windows and start the required threads.
	viewer.realize();

	// Enter the simulation loop. viewer.done() returns false
	// until the user presses the 'esc' key. 
	// (This can be changed by adding your own keyboard/mouse
	// event handler or by changing the settings of the default 
	// keyboard/mouse event handler)

	while (!viewer.done())
	{
		// dispatch the new frame, this wraps up the follow Viewer operations:
		//   advance() to the new frame
		//   eventTraversal() that collects events and passes them on to the event handlers and event callbacks
		//   updateTraversal() to calls the update callbacks
		//   renderingTraversals() that runs syncronizes all the rendering threads (if any) and dispatch cull, draw and swap buffers
		viewer.frame();
	}

	return 0;

}