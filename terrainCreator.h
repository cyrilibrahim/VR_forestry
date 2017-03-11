#pragma once

#include <gl/glut.h> 
#include <osgViewer/Viewer> 
#include <osgText/Text> 
#include <osg/ShapeDrawable>
#include <osgDB/ReadFile>
#include <string>
#include <iostream>

//Normalise notre nombre entre deux nombres a et b, a < b
double normalizeBetweenAandB(double number, double a, double b) {
	double normalized = a + (b - a) * (number) / 255;
	return normalized;
}

//Fichier pour créer le modèle 3D du terrain à partir de toutes les informations

osg::Node* createHeightField(std::string heightFile, std::string texFile, double heightMin, double heightMax) {

	osg::Image* heightMap = osgDB::readImageFile(heightFile);
	
	if (!heightMap) {
		std::cout << "Carte des hauteurs introuvable veuillez verifier le nom du ficher.";
		return 0;
	}

	int imageHeight = heightMap->t();
	int imageWidth = heightMap->s();

	osg::HeightField* heightField = new osg::HeightField();
	heightField->allocate(imageWidth, imageHeight);
	heightField->setOrigin(osg::Vec3(-heightMap->s() / 2, -heightMap->t() / 2, 0));
	heightField->setXInterval(1.0f);
	heightField->setYInterval(1.0f);
	heightField->setSkirtHeight(1.0f);


	for (int r = 0; r < heightField->getNumRows(); r++) {
		for (int c = 0; c < heightField->getNumColumns(); c++) {
			
			double normalizedHeight = normalizeBetweenAandB(*heightMap->data(c, r),heightMin,heightMax);
			heightField->setHeight(c, r, normalizedHeight);
			std::cout << normalizedHeight << "\n";
		}
	}
	
	osg::Geode* geode = new osg::Geode();
	geode->addDrawable(new osg::ShapeDrawable(heightField));

	osg::Texture2D* tex = new osg::Texture2D(osgDB::readImageFile(texFile));

	if (!tex) {
		std::cout << "Texture introuvable veuillez verifier le nom du ficher.";
		return 0;
	}

	tex->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR_MIPMAP_LINEAR);
	tex->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
	tex->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
	tex->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
	geode->getOrCreateStateSet()->setTextureAttributeAndModes(0, tex);

	return geode;
}

