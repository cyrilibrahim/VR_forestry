
#include <gl/glut.h> 
#include <osgViewer/Viewer> 
#include <osgText/Text> 
#include <osg/PositionAttitudeTransform>
#include <osgGA/TrackballManipulator>
osg::Node* CreateScene()
{

}

int main(void)
{
	//Initialize the scene viewer
	osgViewer::Viewer viewer;

	//Root of the scene graph
	osg::Group* root = new osg::Group();
	
	osg::Geode* pyramidGeode = new osg::Geode();
	osg::Geometry* pyramidGeometry = new osg::Geometry();

	//We associate the pyramid geometry and geode
	pyramidGeode->addDrawable(pyramidGeometry);
	//We add the pyramidGeode to the root node
	root->addChild(pyramidGeode);

	//We declare the vertices of the pyramid
	osg::Vec3Array* pyramidVertices = new osg::Vec3Array;
	pyramidVertices->push_back(osg::Vec3(0, 0, 0)); // front left
	pyramidVertices->push_back(osg::Vec3(10, 0, 0)); // front right
	pyramidVertices->push_back(osg::Vec3(10, 10, 0)); // back right 
	pyramidVertices->push_back(osg::Vec3(0, 10, 0)); // back left 
	pyramidVertices->push_back(osg::Vec3(5, 5, 10)); // peak

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

	//Add a second pyramid from the first one

	//Declare and initialize a transform node.
	osg::PositionAttitudeTransform* pyramidTwoXForm = new osg::PositionAttitudeTransform();

	//Use the 'addChild' of the root to add the transform as a child of the root node and the pyramid node
	//as a child of the transform. 
	root->addChild(pyramidTwoXForm);
	pyramidTwoXForm->addChild(pyramidGeode);

	//Declare and initialize a vec3 instance to change the position of the second pyramid model in the scene
	pyramidTwoXForm->setPosition(osg::Vec3(15,0,0));

	//The final step is to set up and enter a simulation loop.
	viewer.setSceneData(root);

	viewer.setCameraManipulator(new osgGA::TrackballManipulator());
	viewer.realize();

	while (!viewer.done()) {
		viewer.frame();
	}

	return 0;

}