
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

#include "osgTerrain/TerrainTile"


int main(void)
{

	//Initialize the scene viewer
	osgViewer::Viewer viewer;

	//Root of the scene graph
	osg::Group* root = new osg::Group();

	osg::Node* firstTree = NULL;
	firstTree = osgDB::readNodeFile("trees_models/tree2/source/TreeTest01.obj");

	osg::Geode* boxGeode = new osg::Geode;
	osg::Box* box = new osg::Box(osg::Vec3(-68.0 * 2000, 51.05183854 * 2000, 900),3.0f);
	osg::ShapeDrawable* sd = new osg::ShapeDrawable(box);
	boxGeode->addDrawable(sd);

	root->addChild(boxGeode);


	//Declare transform,initialize with defaults.
	osg::PositionAttitudeTransform* treeXForm = new osg::PositionAttitudeTransform();
	treeXForm->addChild(firstTree);
	root->addChild(treeXForm);

	
	treeXForm->setPosition(osg::Vec3(-136029, 102108, 800));

	std::string heightMapFile = "client_data/smooth.tif";
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

