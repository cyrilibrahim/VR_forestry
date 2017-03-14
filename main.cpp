
#include <GL/glut.h>
#include <osgViewer/Viewer>
#include <osgText/Text>
#include <osg/PolygonMode>
#include <osg/TexGen>
#include <osg/ShapeDrawable>
#include <osgDB/ReadFile>
#include <osg/PositionAttitudeTransform>
#include <osgGA/TrackballManipulator>
#include <osgGA/TerrainManipulator>
#include <string>
#include <iostream>
#include "tinyxml2.h"
#include "terrainCreator.h"
#include "ClientDataManager.h"
#include "CoordinateConverter.h"
#include "Skybox.h"
#include <iostream>



int main(void)
{

	//Initialize the scene viewer
	osgViewer::Viewer viewer;

	//Root of the scene graph
	osg::Group* root = new osg::Group();

	osg::Node* firstTree = NULL;
	firstTree = osgDB::readNodeFile("trees_models/tree-birch/birch_13m.obj");


	//Class qui gere les donnes des differents fichier
	ClientDataManager* clientDataManager = new ClientDataManager();

	//Creation d'un convertisseur
	CoordinateConverter* converter = new CoordinateConverter(clientDataManager->getWorldFileParameters(), clientDataManager->getMaxHeight(), clientDataManager->getMinHeight(),100.0);

	//Declare transform,initialize with defaults.
	osg::PositionAttitudeTransform* treeXForm = new osg::PositionAttitudeTransform();
	treeXForm->addChild(firstTree);
	//root->addChild(treeXForm);


	//treeXForm->setPosition(osg::Vec3(-136029, 102108, 800));

	std::string heightMapFile = "client_data/real_height_map.tif";
	std::string texMapFile = "client_data/terrain.jpg";



	//Construction du modele 3d du terrain
	osg::Geode* terrainModele = createTerrain(heightMapFile, texMapFile, clientDataManager->getMinHeight(),
		clientDataManager->getMaxHeight(), clientDataManager->getWorldFileParameters(), converter);

	//On ajoute le terrain au noeud racine
	root->addChild(terrainModele);

	osg::Geode* boxGeode = new osg::Geode;
	osg::Box* box = new osg::Box(osg::Vec3(0,0,0),2.0);

	osg::ShapeDrawable* sd = new osg::ShapeDrawable(box);
	boxGeode->addDrawable(sd);
	boxGeode->getOrCreateStateSet()->setAttributeAndModes(
			new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE));

	//root->addChild(boxGeode);


	//double ** treeData = clientDataManager->getTreeData();

	//Recuperation des arbres
	std::ifstream  data("client_data/Arbres.csv");

	std::string line;
	osg::Image* heightMap = osgDB::readImageFile(heightMapFile);

	int j = 0;

	while (std::getline(data, line))
	{
		int k = 0;
		std::stringstream  lineStream(line);
		std::string        cell;

		double lon, lat, vol;

		while (std::getline(lineStream, cell, ','))
		{
			if (k == 0) {
				lon = atof(cell.c_str());
			}
			else if (k == 1) {
				lat = atof(cell.c_str());
			}
			else if (k == 5) {
				vol = atof(cell.c_str());
			}
			k++;
		}

		if (j != 0) {


			//std::cout << "Longitude " << lon << " Latitude" << lat << "\n";
			osg::PositionAttitudeTransform* boxXForm = new osg::PositionAttitudeTransform();


			root->addChild(boxXForm);
			boxXForm->addChild(firstTree);
			//boxXForm->addChild(boxGeode);

			osg::Vec2 pixelXY = converter->LonLatToPixel(osg::Vec2(lon, lat)) * 100;

			osg::Vec3 coord_XYZ = converter->pixelToXYZ(osg::Vec3(pixelXY, *heightMap->data((int)pixelXY.x(),(int)pixelXY.y())));




			//boxXForm->setPivotPoint(firstTree->getBound().center() + osg::Vec3(0, 0, 0));
			boxXForm->setPosition(coord_XYZ);
			//boxXForm->setScale(osg::Vec3(50 * vol, 50 * vol, 50 * vol));
		}

		j++;
	}

	//Skybox
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(new osg::ShapeDrawable(
		new osg::Sphere(osg::Vec3(), terrainModele->getBound().radius())));
	SkyBox* skybox = new SkyBox();
	skybox->getOrCreateStateSet()->setTextureAttributeAndModes(
		0, new osg::TexGen);
	osg::Image* posX = osgDB::readImageFile("Cubemap_sky/dsleft.jpg");
	osg::Image* negX = osgDB::readImageFile("Cubemap_sky/dsright.jpg");
	osg::Image* posY = osgDB::readImageFile("Cubemap_sky/dsfront.jpg");
	osg::Image* negY = osgDB::readImageFile("Cubemap_sky/dsback.jpg");
	osg::Image* posZ = osgDB::readImageFile("Cubemap_sky/dstop.jpg");
	osg::Image* negZ = osgDB::readImageFile("Cubemap_axis/negz.png");

	if (!posX) {
		std::cout << "Pas de x";
	}

	skybox->setEnvironmentMap(0, posX, negX, posY, negY, posZ, negZ);
	skybox->addChild(geode.get());

	root->addChild(skybox);

	viewer.getCamera()->setProjectionMatrixAsPerspective(30, 1, 1, 300000);
	// viewer.getCamera()->setCullingMode(osg::CullSettings::FAR_PLANE_CULLING);
	// viewer.getCamera()->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);

	//The final step is to set up and enter a simulation loop.
	viewer.setSceneData(root);

	viewer.setCameraManipulator(new osgGA::TerrainManipulator());
	viewer.realize();

	while (!viewer.done()) {
		viewer.frame();
	}

	return 0;

}
