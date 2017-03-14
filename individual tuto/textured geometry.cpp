
#include <gl/glut.h> 
#include <osgViewer/Viewer> 
#include <osgText/Text> 
#include <osg/PositionAttitudeTransform>
#include <osgGA/TrackballManipulator>
#include <osgDB/ReadFile> 
#include <iostream>

//Return a pointer to the created pyramid
osg::Geode* createPyramid(void)
{
	osg::Geode* pyramidGeode = new osg::Geode();
	osg::Geometry* pyramidGeometry = new osg::Geometry();

	//We associate the pyramid geometry and geode
	pyramidGeode->addDrawable(pyramidGeometry);

	//We declare the vertices of the pyramid
	osg::Vec3Array* pyramidVertices = new osg::Vec3Array;
	pyramidVertices->push_back(osg::Vec3(0, 0, 0)); // front left 
	pyramidVertices->push_back(osg::Vec3(2, 0, 0)); // front right 
	pyramidVertices->push_back(osg::Vec3(2, 2, 0)); // back right 
	pyramidVertices->push_back(osg::Vec3(0, 2, 0)); // back left 
	pyramidVertices->push_back(osg::Vec3(1, 1, 2)); // peak

	//We associate the vertices and the geometry
	pyramidGeometry->setVertexArray(pyramidVertices);

	//We now create the faces of the pyramid
	//The base is a QUAD
	osg::DrawElementsUInt* pyramidBase = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
	
	//We add element in counterclockwise order
	pyramidBase->push_back(3);
	pyramidBase->push_back(2);
	pyramidBase->push_back(1);
	pyramidBase->push_back(0);
	pyramidGeometry->addPrimitiveSet(pyramidBase);

	//Four other faces are triangles

	osg::DrawElementsUInt* pyramidFaceOne =
		new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
	pyramidFaceOne->push_back(0);
	pyramidFaceOne->push_back(1);
	pyramidFaceOne->push_back(4);
	pyramidGeometry->addPrimitiveSet(pyramidFaceOne);

	osg::DrawElementsUInt* pyramidFaceTwo =
		new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
	pyramidFaceTwo->push_back(1);
	pyramidFaceTwo->push_back(2);
	pyramidFaceTwo->push_back(4);
	pyramidGeometry->addPrimitiveSet(pyramidFaceTwo);

	osg::DrawElementsUInt* pyramidFaceThree =
		new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
	pyramidFaceThree->push_back(2);
	pyramidFaceThree->push_back(3);
	pyramidFaceThree->push_back(4);
	pyramidGeometry->addPrimitiveSet(pyramidFaceThree);

	osg::DrawElementsUInt* pyramidFaceFour =
		new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
	pyramidFaceFour->push_back(3);
	pyramidFaceFour->push_back(0);
	pyramidFaceFour->push_back(4);
	pyramidGeometry->addPrimitiveSet(pyramidFaceFour);

	//Declare the colors
	osg::Vec4Array* colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f)); //index 0 red
	colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f)); //index 1 green
	colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f)); //index 2 blue
	colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f)); //index 3 white 
	colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f)); //index 4 red

	//We want one color by vertices so we have the same number of vertices and colors.
	pyramidGeometry->setColorArray(colors);
	pyramidGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	// Since the mapping from vertices to texture coordinates is 1:1, 
	// we don't need to use an index array to map vertices to texture
	// coordinates. We can do it directly with the 'setTexCoordArray' 
	// method of the Geometry class. 
	// This method takes a variable that is an array of two dimensional
	// vectors (osg::Vec2). This variable needs to have the same
	// number of elements as our Geometry has vertices. Each array element
	// defines the texture coordinate for the cooresponding vertex in the
	// vertex array.
	osg::Vec2Array* texcoords = new osg::Vec2Array(5);
	(*texcoords)[0].set(0.0f, 0.0f); //Tex coord for vertex 0
	(*texcoords)[1].set(0.25f, 0.0f); // tex coord for vertex 1 
	(*texcoords)[2].set(0.50f, 0.0f); // ""
	(*texcoords)[3].set(0.75f, 0.0f); // "" 
	(*texcoords)[4].set(0.50f, 1.0f); // ""
	pyramidGeometry->setTexCoordArray(0, texcoords);

	return pyramidGeode;

}


int main(void)
{
	//Initialize the scene viewer
	osgViewer::Viewer viewer;

	//Root of the scene graph
	osg::Group* root = new osg::Group();
	osg::Geode* pyramidGeode = createPyramid();

	root->addChild(pyramidGeode);
	
	//We will add the textures
	osg::Texture2D* KLN89FaceTexture = new osg::Texture2D;

	//protect from being optimized away as static state:
	KLN89FaceTexture->setDataVariance(osg::Object::DYNAMIC);

	//load an image by reading a file:
	osg::Image* klnFace = osgDB::readImageFile("Textures/KLN89FaceB.tga");

	if (!klnFace)
	{
		std::cout << " couldn't find texture, quiting." << std::endl;
		return -1;
	}

	//Assign the texture to the image we read from file
	KLN89FaceTexture->setImage(klnFace);

	// Create a new StateSet with default settings: 
	osg::StateSet* stateOne = new osg::StateSet();

	// Assign texture unit 0 of our new StateSet to the texture 
	// we just created and enable the texture.
	stateOne->setTextureAttributeAndModes
	(0, KLN89FaceTexture, osg::StateAttribute::ON);
	// Associate this state set with the Geode that contains
	// the pyramid: 
	pyramidGeode->setStateSet(stateOne);

	//The final step is to set up and enter a simulation loop.
	viewer.setSceneData(root);

	viewer.setCameraManipulator(new osgGA::TrackballManipulator());
	viewer.realize();

	while (!viewer.done()) {
		viewer.frame();
	}

	return 0;

}