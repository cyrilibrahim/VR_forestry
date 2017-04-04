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
#include <osg/ComputeBoundsVisitor>
#include <osg/MatrixTransform>

#include "tinyxml2.h"
#include "terrainCreator.h"
#include "ClientDataManager.h"
#include "CoordinateConverter.h"
#include "Server.h"
#include "Skybox.h"
#include "truckInputDeviceStateType.h"
#include "CustomEventHandler.h"
#include "updateTruckPosRotCallback.h"

int main(void)
{
	//Entre dans le dossier pour windows seulement
	#ifdef _WIN32
	if (!SetCurrentDirectory("datasets"))
	{
		printf("SetCurrentDirectory failed (%d)\n", GetLastError());
	}
	#endif

	//Initialize the scene viewer
	osgViewer::Viewer viewer;

	//DEBUG PARAMETERS

	//0: vue du camion de haut
	//1: Monde qui tourne autour du terrain
	//2: Vue du camion a la premiere personne
	int pointOfViewChoice = 2;
	bool debugBoundingBox = true;

	//Root of the scene graph
	osg::Group* root = new osg::Group();

	//Ajout des modeles des arbres
	osg::Node* firstTree = osgDB::readNodeFile("trees_models/tree_birch_osg/birch_13m.obj");
	osg::Node* secondTree = osgDB::readNodeFile("trees_models/sapin/fir.obj");
	osg::Node* thirdTree = osgDB::readNodeFile("trees_models/tree_birch_osg/birch_13m_autmn.obj");
	//Chargement du model du camion
	osg::Node* truckModel = osgDB::readNodeFile("models/t72-tank/t72-tank_des.flt");

	

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

	//Recuperation des arbres

	//Groupe regroupant les bounding boxes des arbres
	std::vector <osg::BoundingBox> tree_bouding_boxes;

	std::ifstream  data("client_data/Arbres.csv");

	std::string line;
	osg::Image* heightMap = osgDB::readImageFile(heightMapFile);

	int j = 0;

	// On parcours la liste d'arbre et les ajoutes un a un 
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
			treeXForm->computeBound();

			//Random rotation
			//double randRotation = rand() % (360 - 1 + 1) + 1;
			double randRotation = 0.0;
			treeXForm->setAttitude(osg::Quat(osg::DegreesToRadians(randRotation), osg::Vec3(0, 0, 1)));

			// offset pour corriger la position de la bounding box (collision)
			osg::Vec3 treeCenterOffset(0, 0, 0);

			// taille de la bounding box (padding interne x et y) plus on se rapproche de 0.5 plus c'est petit
			double boundingBoxScale = 0.49;

			if (espece == 'C'){
				boundingBoxScale = 0.495;
				treeCenterOffset.set(-0.1, 0.56, 0);
				treeXForm->addChild(secondTree);
				treeXForm->setScale(osg::Vec3(0.5, 0.5, 0.5) * height);
			}
			else if(espece == 'F') {
				treeCenterOffset.set(0.08, -0.2, 0);
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

			//Calcul du bounding box
			
			osg::BoundingSphere currentTreeBB = treeXForm->getBound();

			//Calculer l'orientation actuel (en vecteur 3D)
			osg::Vec3 orientation = osg::Vec3(cos(osg::DegreesToRadians(randRotation + 90)), sin(osg::DegreesToRadians(randRotation + 90)), 0);
			// La direction perpendiculaire (utile pour la bounding box)
			osg::Vec3 orientation_perp = osg::Vec3(cos(osg::DegreesToRadians(randRotation)), sin(osg::DegreesToRadians(randRotation)), 0);
			
			// On va corriger legerement le centre de chaque arbre
			currentTreeBB.set(currentTreeBB.center() + orientation * treeCenterOffset.x() + orientation_perp * treeCenterOffset.y(), currentTreeBB.radius());

			osg::BoundingBox  bb;
			bb.expandBy(currentTreeBB);

			// On rend les orientations unitaires
			orientation.normalize();
			orientation_perp.normalize();

			double bbXlenght = bb.xMax() - bb.xMin();
			double bbYlenght = bb.yMax() - bb.yMin();

			double internePadding =boundingBoxScale;
			bb.set(bb.xMin()+ bbXlenght* internePadding,bb.yMin() + bbYlenght* internePadding,bb.zMin(), bb.xMax()- bbXlenght* internePadding, bb.yMax()- bbYlenght* internePadding, bb.zMin() + 10);
			
			if (debugBoundingBox) {
				osg::Geode* bbRepresentationGeode = new osg::Geode();

				// Box pour afficher la boite englobante des arbres
				osg::Box* boxDebugTree = new osg::Box();
				osg::PositionAttitudeTransform*  PATDebugTree = new osg::PositionAttitudeTransform();
				PATDebugTree->setPosition(bb.center());
				PATDebugTree->setScale((bb._max - bb._min));

				PATDebugTree->addChild(new osg::ShapeDrawable(boxDebugTree));
				PATDebugTree->addChild(bbRepresentationGeode);
				root->addChild(PATDebugTree);

				PATDebugTree->getOrCreateStateSet()->setAttributeAndModes(
					new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE));
				//std::cout << bb.xMin() << " " << bb.xMax() << " " << bb.yMin() << " " << bb.yMax() << " " << bb.zMin() << " " << bb.zMax() << "\n";
			}
			
			//Ajoute a la liste des bounding box d'arbres
			tree_bouding_boxes.push_back(bb);
			
		}
		j++;
	}

	//Ajout du camion
	osg::PositionAttitudeTransform* truckXForm = new osg::PositionAttitudeTransform();
	truckXForm->setPosition(osg::Vec3(0, 0, 0));
	root->addChild(truckXForm);
	truckXForm->addChild(truckModel);

	//on le met a la position au centre
	truckXForm->setPosition(converter->getMapCenter(heightMap));
	truckXForm->setScale(osg::Vec3(0.4f, 0.4f, 0.4f));
	//Declare instance of class to record state of keyboard
	truckInputDeviceStateType* tIDevState = new truckInputDeviceStateType;

	// Set up the truck update callback
	//  pass the constructor a pointer to our truck input device state
	//  that we declared above.
	updateTruckPosCallback* truckUpdater =  new updateTruckPosCallback(tIDevState, terrainModele, tree_bouding_boxes);
	
	if (debugBoundingBox) {

		osg::Geode* bbTruckRepresentationGeode = new osg::Geode();
		osg::Box* bbTruckRepresentationBox = new osg::Box(osg::Vec3(0,0,0),1.0,1.0,1.0);

		bbTruckRepresentationGeode->addDrawable(new osg::ShapeDrawable(bbTruckRepresentationBox));
		
		bbTruckRepresentationGeode->getOrCreateStateSet()->setAttributeAndModes(
			new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE));
		osg::PositionAttitudeTransform* boxTrans = new osg::PositionAttitudeTransform();

		root->addChild(boxTrans);
		boxTrans->addChild(bbTruckRepresentationGeode);

		truckUpdater->setBbTruckVisual(boxTrans);
	}

	truckXForm->setUpdateCallback(truckUpdater);

	// The constructor for our event handler also gets a pointer to
	//   our truck input device state instance
	CustomEventHandler* myEventHandler = new CustomEventHandler(tIDevState);

	// Add our event handler to the list
	viewer.addEventHandler(myEventHandler);

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


	viewer.getCamera()->setProjectionMatrixAsPerspective(65, 1, 0.1, 300);
	viewer.getCamera()->setCullingMode(osg::CullSettings::FAR_PLANE_CULLING);
	viewer.getCamera()->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);


	//The final step is to set up and enter a simulation loop.
	viewer.setSceneData(root);

	//Camera monde qui tourne
	if (pointOfViewChoice == 1) {
		viewer.setCameraManipulator(new osgGA::TerrainManipulator());
	}



	viewer.realize();
	viewer.getCamera()->getView()->setLightingMode(osg::View::NO_LIGHT); //works
	// Démarrage du serveur web
	//boost::shared_ptr<Server> server(listen("0.0.0.0", "9000"));

	// Boucle d'affichage
	while (!viewer.done()) {
		//3eme personne
		if (pointOfViewChoice == 0) {

			osg::Vec3 offsetZ(0, 0, 1);
			viewer.getCamera()->setViewMatrixAsLookAt(
					truckXForm->getPosition() - truckUpdater->getTruckDirection() * 4 + offsetZ,
					truckXForm->getPosition(),
					osg::Vec3(0, 0, 1));

		}
		//1 ere personne
		else if(pointOfViewChoice == 2) {

			osg::Vec3 offsetZ(0, 0, 1);
			viewer.getCamera()->setViewMatrixAsLookAt(
				truckXForm->getPosition() + truckUpdater->getTruckDirection() + offsetZ,
				truckXForm->getPosition() + truckUpdater->getTruckDirection()*2 + offsetZ,
				osg::Vec3(0, 0, 1));

		}

		//pollSocket();
		viewer.frame();
	}

	return 0;
}
