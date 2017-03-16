#pragma once

#include <GL/glut.h>
#include <osgViewer/Viewer>
#include <osgText/Text>
#include <osg/PolygonMode>
#include <osg/ShapeDrawable>
#include <osgDB/ReadFile>
#include <string>
#include <iostream>
#include <osgUtil/SmoothingVisitor>
#define _USE_MATH_DEFINES
#include <math.h>
#include "CoordinateConverter.h"


//Fichier pour créer le modèle 3D du terrain à partir de toutes les informations


osg::Geode* createTerrain(std::string heightFile, std::string texFile, double heightMin, double heightMax, double* worldParameters, CoordinateConverter* converter) {

	osg::Image* heightMap = osgDB::readImageFile(heightFile);

	if (!heightMap) {
		std::cout << "Carte des hauteurs introuvable veuillez verifier le nom du ficher.";
		return 0;
	}

	int imageHeight = heightMap->t();
	int imageWidth = heightMap->s();

	std::cout << imageWidth;

	osg::Geode* geode = new osg::Geode();
	osg::Geometry* terrainGeometry = new osg::Geometry();

	geode->addDrawable(terrainGeometry);

	osg::Vec3Array* terrainVertices = new osg::Vec3Array;
	osg::Vec2dArray* texcoords = new osg::Vec2dArray;

	//On ajoute les sommets
	for (int r = 0; r < imageHeight; r++ ) {
		for (int c = 0; c < imageWidth; c++) {
			osg::Vec3 XYZ = converter->pixelToXYZ(osg::Vec3(c, r, *heightMap->data(c, r)));
			std::cout << XYZ.x() << "|" << XYZ.y() << "|" << XYZ.z() << std::endl;
			terrainVertices->push_back(XYZ);
			texcoords->push_back(osg::Vec2( 1.0 - ((double) c / imageWidth), 1.0 - ((double) r / imageHeight)) * 40);
		}
	}

	terrainGeometry->setVertexArray(terrainVertices);
	terrainGeometry->setTexCoordArray(0, texcoords);

	//On creer les face
	for (int r = 0; r  < (imageHeight - 1); r++) {
		for (int c = 0; c < (imageWidth - 1); c++) {


			osg::DrawElementsUInt* faceOne = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
			faceOne->push_back(r*(imageWidth)+c);
			faceOne->push_back(r*(imageWidth)+c + 1);
			faceOne->push_back((r + 1) *(imageWidth) + c + 1);


			osg::DrawElementsUInt* faceTwo = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
			faceTwo->push_back((r + 1)*(imageWidth)+c);
			faceTwo->push_back((r + 1) *(imageWidth)+c + 1);
			faceTwo->push_back(r*(imageWidth) + c);



			terrainGeometry->addPrimitiveSet(faceOne);
			terrainGeometry->addPrimitiveSet(faceTwo);

		}
	}

	osg::Texture2D* tex = new osg::Texture2D(osgDB::readImageFile(texFile));

	if (!tex) {
		return 0;
	}

	tex->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR_MIPMAP_LINEAR);
	tex->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
	tex->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
	tex->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
	geode->getOrCreateStateSet()->setTextureAttributeAndModes(0, tex);
	geode->getOrCreateStateSet()->setAttributeAndModes(
		new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL));

	return geode;
}


//Ancienne approche (marche mal) a ne pas utiliser
osg::Geode* createHeightField(std::string heightFile, std::string texFile, float heightMin, float heightMax, float* worldParameters) {

	osg::Image* heightMap = osgDB::readImageFile(heightFile);

	if (!heightMap) {
		std::cout << "Carte des hauteurs introuvable veuillez verifier le nom du ficher.";
		return 0;
	}

	//heightMap->setOrigin(osg::Image::TOP_LEFT);

	int imageHeight = heightMap->t();
	int imageWidth = heightMap->s();

	//std::cout << heightMap->setOrigin();

	osg::Geode* geode = new osg::Geode();
	osg::HeightField* heightField = new osg::HeightField();
	/*
	osg::Vec3 cornerUpperLeft; //LOWER LEFT
	cornerUpperLeft.set(getXWorldCoordinate(0, 0, worldParameters), getYWorldCoordinate(0, 0, worldParameters), 0.0);
	//std::cout << cornerUpperLeft.x() << " " << cornerUpperLeft.y() << "\n";

	osg::Vec3 cornerLowerLeft;//UPPER LEFT
	cornerLowerLeft.set(getXWorldCoordinate(0, imageHeight, worldParameters), getYWorldCoordinate(0, imageHeight, worldParameters), 0.0);
	//std::cout << cornerLowerLeft.x() << " " << cornerLowerLeft.y() << "\n";

	osg::Vec3 cornerLowerRight; //UPPER RIGHT
	cornerLowerRight.set(getXWorldCoordinate(imageWidth, imageHeight, worldParameters), getYWorldCoordinate(imageWidth, imageHeight, worldParameters), 0.0);
	//std::cout << cornerLowerRight.x() << " " << cornerLowerRight.y() << "\n";

	osg::Vec3 cornerUpperRight; //LOWER RIGHT
	cornerUpperRight.set(getXWorldCoordinate(imageWidth, 0, worldParameters), getYWorldCoordinate(imageWidth, 0, worldParameters), 0.0);
	//std::cout << cornerUpperRight.x() << " " << cornerUpperRight.y() << "\n";

	osg::Vec3 upperVec = cornerUpperRight - cornerUpperLeft;
	double realWidth = abs(cornerUpperRight.x() - cornerUpperLeft.x());
	osg::Vec3 sideVec = cornerLowerLeft - cornerUpperLeft;
	double realHeight  = abs(cornerLowerLeft.y() - cornerUpperLeft.y());



	heightField->allocate(imageWidth, imageHeight);
	heightField->setOrigin(cornerLowerLeft);
	heightField->setXInterval(realWidth / (imageWidth));
	heightField->setYInterval(realHeight / (imageHeight));
	heightField->setSkirtHeight(1.0f);

	for (int r = 0; r < heightField->getNumRows(); r++) {
	for (int c = 0; c < heightField->getNumColumns(); c++) {

	double normalizedHeight = normalizeBetweenAandB(*heightMap->data(c, r),heightMin,heightMax);
	heightField->setHeight(c, r, normalizedHeight);
	}
	}




	osg::ShapeDrawable* sd = new osg::ShapeDrawable(heightField);

	geode->addDrawable(sd);

	osg::Texture2D* tex = new osg::Texture2D(osgDB::readImageFile(texFile));

	if (!tex) {
	return 0;
	}

	tex->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR_MIPMAP_LINEAR);
	tex->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
	tex->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
	tex->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
	//geode->getOrCreateStateSet()->setTextureAttributeAndModes(0, tex);
	osgUtil::SmoothingVisitor sm;
	geode->accept(sm);
	sm.apply(*geode);
	*/
	return geode;
}
