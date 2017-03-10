
#include <gl/glut.h> 
#include <osgViewer/Viewer> 
#include <osgText/Text> 
#include <osg/ShapeDrawable>
#include <osgDB/ReadFile>
#include <osg/PositionAttitudeTransform>
#include <osgGA/TrackballManipulator>
#include <string>
#include <iostream>

osg::Node* createHeightField(std::string heightFile, std::string texFile) {

	osg::Image* heightMap = osgDB::readImageFile(heightFile);

	if (!heightMap) {
		std::cout << "Carte des hauteurs introuvable veuillez verifier le nom du ficher.";
		return 0;
	}

	osg::HeightField* heightField = new osg::HeightField();
	heightField->allocate(heightMap->s(), heightMap->t());
	heightField->setOrigin(osg::Vec3(-heightMap->s() / 2, -heightMap->t() / 2, 0));
	heightField->setXInterval(1.0f);
	heightField->setYInterval(1.0f);
	heightField->setSkirtHeight(1.0f);

	for (int r = 0; r < heightField->getNumRows(); r++) {
		for (int c = 0; c < heightField->getNumColumns(); c++) {
			heightField->setHeight(c, r, ((*heightMap->data(c, r)) / 255.0f) * 80.0f);
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


int main(void)
{
	//Initialize the scene viewer
	osgViewer::Viewer viewer;

	//Root of the scene graph
	osg::Group* root = new osg::Group();

	std::string heightMapFile = "client_data/Clip2_512x512.tif";
	std::string texMapFile = "client_data/terrain.jpg";

	osg::Node* terrainModele = createHeightField(heightMapFile, texMapFile);

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