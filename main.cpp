
#include <gl/glut.h> 
#include <osgViewer/Viewer> 
#include <osgText/Text> 
#include <osg/ShapeDrawable>
#include <osgDB/ReadFile>
#include <osg/PositionAttitudeTransform>
#include <osgGA/TrackballManipulator>
#include <string>
#include <iostream>
#include "tinyxml2.h"
#include "terrainCreator.h"
#include "clientDataManager.h"



int main(void)
{

	//Initialize the scene viewer
	osgViewer::Viewer viewer;

	//Root of the scene graph
	osg::Group* root = new osg::Group();

	std::string heightMapFile = "client_data/newimg.tif";
	std::string texMapFile = "client_data/terrain2.jpg";

	//Class qui gere les donnes des differents fichier
	ClientDataManager* clientDataManager = new ClientDataManager();

	//Construction du modele 3d du terrain
	osg::Geode* terrainModele = createHeightField(heightMapFile, texMapFile, clientDataManager->getMinHeight(),
		clientDataManager->getMaxHeight(), clientDataManager->getWorldFileParameters());
	
	//On ajoute le terrain au noeud racine
	root->addChild(terrainModele);

	//The final step is to set up and enter a simulation loop.
	viewer.setSceneData(root);

	viewer.setCameraManipulator(new osgGA::TrackballManipulator());
	viewer.realize();

	while (!viewer.done()) {
		viewer.frame();
	}

	return 0;

}