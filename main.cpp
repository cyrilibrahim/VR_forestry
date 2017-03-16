#include <boost/shared_ptr.hpp>
#include <GL/glut.h>
#include <iostream>
#include <osg/BlendFunc>
#include <osg/Geometry>
#include <osg/LineSegment>
#include <osg/PolygonMode>
#include <osg/PositionAttitudeTransform>
#include <osg/ShapeDrawable>
#include <osg/TexGen>
#include <osgDB/ReadFile>
#include <osgGA/DriveManipulator>
#include <osgGA/FirstPersonManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/TerrainManipulator>
#include <osgGA/TrackballManipulator>
#include <osgText/Text>
#include <osgUtil/IntersectVisitor>
#include <osgViewer/Viewer>
#include <string>

#include "tinyxml2.h"
#include "terrainCreator.h"
#include "ClientDataManager.h"
#include "CoordinateConverter.h"
#include "Server.h"
#include "Skybox.h"


int main(void)
{
	/*
	if (!SetCurrentDirectory("datasets"))
	{
		printf("SetCurrentDirectory failed (%d)\n", GetLastError());
		//return;
	}
	*/

	//Root of the scene graph
	osg::Group* root = new osg::Group();


	osg::Node* firstTree = osgDB::readNodeFile("trees_models/tree_birch_osg/birch_13m.obj");
	osg::Node* secondTree = osgDB::readNodeFile("trees_models/sapin/fir.obj");
	osg::Node* thirdTree = osgDB::readNodeFile("trees_models/tree_birch_osg/birch_13m_autmn.obj");

	/*
	osg::StateSet* stateSet = new osg::StateSet();
	stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
	stateSet->setAttributeAndModes(blendFunc, osg::StateAttribute::ON);
	//stateSet->setAttributeAndModes(blendFunc2, osg::StateAttribute::ON);
	firstTree->setStateSet(stateSet);
	*/

	//Class qui gere les donnes des differents fichier
	ClientDataManager* clientDataManager = new ClientDataManager();

	//Creation d'un convertisseur
	CoordinateConverter* converter = new CoordinateConverter(
			clientDataManager->getWorldFileParameters(),
			clientDataManager->getMaxHeight(),
			clientDataManager->getMinHeight(),
			1.0);


	std::string heightMapFile = "client_data/real_height_map.tif";
	std::string texMapFile = "client_data/terrain.jpg";

	//Construction du modele 3d du terrain
	osg::Geode* terrainModele = createTerrain(
			heightMapFile,
			texMapFile,
			clientDataManager->getMinHeight(),
			clientDataManager->getMaxHeight(),
			clientDataManager->getWorldFileParameters(),
			converter);

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

		double lon, lat, height, vol;
		char espece = 'n';
		while (std::getline(lineStream, cell, ','))
		{
			if (k == 0)
				lon = atof(cell.c_str());
			else if (k == 1)
				lat = atof(cell.c_str());
			else if (k == 2)
				espece = cell[0];
			else if (k == 4)
				height = atof(cell.c_str());
			else if (k == 5)
				vol = atof(cell.c_str());
			k++;
		}

		if (j != 0) {
			osg::Vec2 treeXY = converter->lonLatToXY(osg::Vec2(lon, lat));
			osg::PositionAttitudeTransform* treeXForm = new osg::PositionAttitudeTransform();

			// Ray-tracing pour planter les arbres
			osg::LineSegment* treeHeightRay = new osg::LineSegment();
			treeHeightRay->set(
				osg::Vec3(treeXY, 9999) ,
				osg::Vec3(treeXY, -999) );

			osgUtil::IntersectVisitor treeElevationVisitor;
			treeElevationVisitor.addLineSegment(treeHeightRay);
			terrainModele->accept(treeElevationVisitor);

			osgUtil::IntersectVisitor::HitList heightHits;
			heightHits = treeElevationVisitor.getHitList(treeHeightRay);
			osgUtil::Hit treeHeightResults;
			osg::Vec3d treePos(treeXY, 800.);
			if (!heightHits.empty()) {
				treeHeightResults = heightHits.front();
				treePos = treeHeightResults.getWorldIntersectPoint();
			} else {
				// Arbre en dehors du terrain
				continue;
			}
			root->addChild(treeXForm);

			treeXForm->setPosition(treePos);
			//Random rotation
			double randRotation = rand() % (360 - 1 + 1) + 1;
			treeXForm->setAttitude(osg::Quat(randRotation, osg::Vec3(0, 0, 1)));

			if (espece == 'C') {
				treeXForm->addChild(secondTree);
				treeXForm->setScale(osg::Vec3(0.15, 0.15, 0.15) * height);
			}
			else if(espece == 'F') {
				//Rand between 1 and 2
				int randTree = rand() % (2 - 1 + 1) + 1;
				//Birch
				if (randTree == 1) {
					treeXForm->addChild(firstTree);
					// Hauteur du birch_13 : 33.22m, mais l'unité du mesh n'est pas le mètre :/
					treeXForm->setScale(osg::Vec3(1, 1, 1) * height / 33.22 * 0.02);
				}
				//Sapin
				else if (randTree == 2) {
					treeXForm->addChild(thirdTree);
					treeXForm->setScale(osg::Vec3(1, 1, 1) * height / 33.22 * 0.02);
				}
			}
		}
		j++;
	}

	//Skybox
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(new osg::ShapeDrawable(
			new osg::Sphere(osg::Vec3(), terrainModele->getBound().radius())));
	SkyBox* skybox = new SkyBox();
	skybox->getOrCreateStateSet()->setTextureAttributeAndModes(0, new osg::TexGen);
	osg::Image* posX = osgDB::readImageFile("Cubemap_sky/dsright.jpg");
	osg::Image* negX = osgDB::readImageFile("Cubemap_sky/dsleft.jpg");
	osg::Image* posY = osgDB::readImageFile("Cubemap_sky/dsfront.jpg");
	osg::Image* negY = osgDB::readImageFile("Cubemap_sky/dsback.jpg");
	osg::Image* posZ = osgDB::readImageFile("Cubemap_sky/dstop.jpg");
	osg::Image* negZ = osgDB::readImageFile("Cubemap_sky/dstop.jpg");

	if (!(posX && negX && posY && negY && posZ && negZ)) {
		std::cout << "Texture(s) de skybox manquante(s)" << std::endl;
	}

	skybox->setEnvironmentMap(0, posX, negX, posY, negY, posZ, negZ);
	skybox->addChild(geode.get());

	root->addChild(skybox);

	//Initialize the scene viewer
	osgViewer::Viewer viewer;

	viewer.getCamera()->setProjectionMatrixAsPerspective(65, 1, 1, 3000);
	viewer.getCamera()->setCullingMode(osg::CullSettings::FAR_PLANE_CULLING);
	viewer.getCamera()->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);

	//The final step is to set up and enter a simulation loop.
	viewer.setSceneData(root);

	viewer.setCameraManipulator(new osgGA::TerrainManipulator());
	viewer.realize();
	viewer.getCamera()->getView()->setLightingMode(osg::View::NO_LIGHT); //works
	// Démarrage du serveur web
	boost::shared_ptr<Server> server(listen("0.0.0.0", "9000"));

	// Boucle d'affichage
	while (!viewer.done()) {
		viewer.frame();
		pollSocket();
	}

	return 0;
}
