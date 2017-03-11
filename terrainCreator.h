#pragma once

#include <gl/glut.h> 
#include <osgViewer/Viewer> 
#include <osgText/Text> 
#include <osg/ShapeDrawable>
#include <osgDB/ReadFile>
#include <string>
#include <iostream>
#include <osgUtil/SmoothingVisitor>
#define _USE_MATH_DEFINES
#include <math.h>

//Normalise notre nombre entre deux nombres a et b, a < b
double normalizeBetweenAandB(double number, double a, double b) {
	double normalized = a + (b - a) * (number) / 255;
	return normalized;
}

double getXWorldCoordinate(double oldX, double oldY, double* worldParameters) {
	double newX = worldParameters[0] * oldX + worldParameters[2] * oldY + worldParameters[4];
	//Pour l'instant nous transformons seulement les points lattitude longitude comme point dans l'espace X Y scaler
	//nous les convertirons par la suite. 
	return newX * 2000;
}
double getYWorldCoordinate(double oldX, double oldY, double* worldParameters) {
	double newY = worldParameters[1] * oldX + worldParameters[3] * oldY + worldParameters[5];
	//Pour l'instant nous transformons seulement les points lattitude longitude comme point dans l'espace X Y scaler
	//nous les convertirons par la suite. 
	return newY * 2000;
}


osg::Vec3* lonLatToVector3(double lng, double lat)
{
	osg::Vec3* out =new osg::Vec3();

	//flips the Y axis
	lat = M_PI / 2 - lat;

	//distribute to sphere
	out->set(
		sin(lat) * sin(lng),
		cos(lat),
		sin(lat) * cos(lng)
	);

	return out;

}

//Fichier pour créer le modèle 3D du terrain à partir de toutes les informations

osg::Geode* createHeightField(std::string heightFile, std::string texFile, double heightMin, double heightMax, double* worldParameters) {

	osg::Image* heightMap = osgDB::readImageFile(heightFile);
	
	if (!heightMap) {
		std::cout << "Carte des hauteurs introuvable veuillez verifier le nom du ficher.";
		return 0;
	}

	int imageHeight = heightMap->t();
	int imageWidth = heightMap->s();

	osg::Geode* geode = new osg::Geode();
	osg::HeightField* heightField = new osg::HeightField();

	osg::Vec3 cornerUpperLeft;
	cornerUpperLeft.set(getXWorldCoordinate(0, 0, worldParameters), getYWorldCoordinate(0, 0, worldParameters), 0.0);
	//std::cout << cornerUpperLeft.x() << " " << cornerUpperLeft.y() << "\n";
	
	osg::Vec3 cornerLowerLeft;
	cornerLowerLeft.set(getXWorldCoordinate(0, imageHeight, worldParameters), getYWorldCoordinate(0, imageHeight, worldParameters), 0.0);
	//std::cout << cornerLowerLeft.x() << " " << cornerLowerLeft.y() << "\n";
	
	osg::Vec3 cornerLowerRight;
	cornerLowerRight.set(getXWorldCoordinate(imageWidth, imageHeight, worldParameters), getYWorldCoordinate(imageWidth, imageHeight, worldParameters), 0.0);
	//std::cout << cornerLowerRight.x() << " " << cornerLowerRight.y() << "\n";
	
	osg::Vec3 cornerUpperRight;
	cornerUpperRight.set(getXWorldCoordinate(imageWidth, 0, worldParameters), getYWorldCoordinate(imageWidth, 0, worldParameters), 0.0);
	//std::cout << cornerUpperRight.x() << " " << cornerUpperRight.y() << "\n";

	osg::Vec3 upperVec = cornerUpperRight - cornerUpperLeft;
	double realWidth = upperVec.length();
	osg::Vec3 sideVec = cornerLowerLeft - cornerUpperLeft;
	double realHeight = sideVec.length();

	

	heightField->allocate(imageWidth, imageHeight);
	heightField->setOrigin(cornerUpperLeft);
	heightField->setXInterval(realWidth / imageWidth);
	heightField->setYInterval(realHeight / imageHeight);
	heightField->setSkirtHeight(heightMax - heightMin);



	for (int r = 0; r < heightField->getNumRows(); r++) {
		for (int c = 0; c < heightField->getNumColumns(); c++) {
			
			double normalizedHeight = normalizeBetweenAandB(*heightMap->data(c, r),heightMin,heightMax);
			heightField->setHeight(c, r, normalizedHeight);
		}
	}
	
	geode->addDrawable(new osg::ShapeDrawable(heightField));

	


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

	return geode;
}

