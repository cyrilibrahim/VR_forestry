
#include <gl/glut.h> 
#include <osgViewer/Viewer> 
#include <osgText/Text> 
#include <osg/PositionAttitudeTransform>
#include <osgGA/TrackballManipulator>
#include <osgDB/ReadFile> 
#include <osg/ShapeDrawable> 
#include <iostream>


int main(void)
{
	//Initialize the scene viewer
	osgViewer::Viewer viewer;

	//Root of the scene graph
	osg::Group* root = new osg::Group();

	// Declare a box class (derived from shape class) instance
	// This constructor takes an osg::Vec3 to define the center
	// and a float to define the height, width and depth.
	// (an overloaded constructor allows you to specify unique
	// height, width and height values.)
	osg::Box* unitCube = new osg::Box(osg::Vec3(0, 0, 0), 1.0f);

	// Declare an instance of the shape drawable class and initialize 
	// it with the unitCube shape we created above.
	// This class is derived from 'drawable' so instances of this
	// class can be added to Geode instances.
	osg::ShapeDrawable* unitCubeDrawable = new osg::ShapeDrawable(unitCube);

	//Declare an instance of the geode class:
	osg::Geode* basicShapesGeode = new osg::Geode();

	//Add the unit cube drawable to the geode: 
	basicShapesGeode->addDrawable(unitCubeDrawable);

	//Add the geode to the scene
	root->addChild(basicShapesGeode);

	//Create a sphere centered at the origin, unit radius:
	osg::Sphere* unitSphere = new osg::Sphere(osg::Vec3(0, 0, 0), 1.0);
	osg::ShapeDrawable* unitSphereDrawable = new osg::ShapeDrawable(unitSphere);

	//We need a transformation to move it awat from the cube
	osg::PositionAttitudeTransform* sphereXForm =
		new osg::PositionAttitudeTransform();
	sphereXForm->setPosition(osg::Vec3(2.5, 0, 0));
	
	osg::Geode* unitSphereGeode = new osg::Geode();
	root->addChild(sphereXForm);

	sphereXForm->addChild(unitSphereGeode);
	unitSphereGeode->addDrawable(unitSphereDrawable);

	//Set states

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
	// Declare a state for 'BLEND' texture mode
	osg::StateSet* blendStateSet = new osg::StateSet();

	// Declare a TexEnv instance, set to the mode to 'BLEND'
	osg::TexEnv* blendTexEnv = new osg::TexEnv;
	blendTexEnv->setMode(osg::TexEnv::BLEND);

	// Turn the attribute of texture 0 - the texture we loaded above - 'ON'
	blendStateSet->setTextureAttributeAndModes
	(0, KLN89FaceTexture, osg::StateAttribute::ON);

	//Set the texture environment for texture 0 to the texture env 
	//we declared above:
	blendStateSet->setTextureAttribute(0, blendTexEnv);

	//We create a second state for DECAL texture mode.
	osg::StateSet* decalStateSet = new osg::StateSet();

	osg::TexEnv* decalTexEnv = new osg::TexEnv();
	decalTexEnv->setMode(osg::TexEnv::DECAL);

	decalStateSet->setTextureAttributeAndModes
	(0, KLN89FaceTexture, osg::StateAttribute::ON);
	decalStateSet->setTextureAttribute(0, decalTexEnv);

	//Propagate to the leaf
	root->setStateSet(blendStateSet);
	//Change the sphere State to decale
	unitSphereGeode->setStateSet(decalStateSet);


	//The final step is to set up and enter a simulation loop.
	viewer.setSceneData(root);

	viewer.setCameraManipulator(new osgGA::TrackballManipulator());
	viewer.realize();

	while (!viewer.done()) {
		viewer.frame();
	}

	return 0;

}