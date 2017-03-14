
#include <gl/glut.h> 
#include <osg/Group>
#include <osgDB/ReadFile>

#include <osgDB/FileUtils>
#include <osg/PositionAttitudeTransform>
#include <osgViewer/Viewer>
#include <osgGA/TrackballManipulator>
#include <osgText/Text>

int main()
{

	// Declare a group for the root
	osg::Group* root = new osg::Group();

	osg::Node* groundNode = NULL;
	osg::Node* tankNode = NULL;
	tankNode = osgDB::readNodeFile("models/T72-Tank/T72-tank_des.flt");

	osg::StateSet* brickState = tankNode->getOrCreateStateSet();

	osg::Program* bumpMapProgramObject = new osg::Program;

	bumpMapProgramObject->addShader(osg::Shader::readShaderFile(osg::Shader::VERTEX, osgDB::findDataFile("shaders/bumpmap.vert")));
	bumpMapProgramObject->addShader(osg::Shader::readShaderFile(osg::Shader::FRAGMENT, osgDB::findDataFile("shaders/bumpmap.frag")));
	
	osg::Uniform* lightPosU = new osg::Uniform("LightPosition", osg::Vec3(0, 0, 1));
	osg::Uniform* normalMapU = new osg::Uniform("normalMap", 1);
	osg::Uniform* baseTextureU = new osg::Uniform("baseTexture", 0);
	
	brickState->addUniform(lightPosU);
	brickState->addUniform(normalMapU);
	brickState->addUniform(baseTextureU);
	osg::Texture2D* tankBodyNormalMap = new osg::Texture2D;
	tankBodyNormalMap->setDataVariance(osg::Object::DYNAMIC);

	osg::Image* tankBody = osgDB::readImageFile("Images/water_bump.jpg");
	if (!tankBody)
	{
		std::cout << " couldn't find texture, quiting." << std::endl;
		//return -1;
	}
	tankBodyNormalMap->setImage(tankBody);
	brickState->setTextureAttributeAndModes(1, tankBodyNormalMap, osg::StateAttribute::ON);

	brickState->setAttributeAndModes(bumpMapProgramObject, osg::StateAttribute::ON);



	osgViewer::Viewer viewer;
	osg::PositionAttitudeTransform* tankXform;

	groundNode = osgDB::readNodeFile("models/JoeDirt/JoeDirt.flt");
	

	//Create green irish sky
	osg::ClearNode* backdrop = new osg::ClearNode();
	backdrop->setClearColor(osg::Vec4(0.0f, 0.8f, 0.0f, 1.0f));
	root->addChild(backdrop);
	root->addChild(groundNode);

	tankXform = new osg::PositionAttitudeTransform();

	root->addChild(tankXform);
	tankXform->addChild(tankNode);
	tankXform->setPosition(osg::Vec3(10, 10, 8));
	tankXform->setAttitude(
		osg::Quat(osg::DegreesToRadians(-45.0), osg::Vec3(0,0,1))
	);

	//Create the camera matrix manually
	osg::Matrixd myCameraMatrix;
	osg::Matrixd cameraRotation;
	osg::Matrixd cameraTrans;

	cameraRotation.makeRotate(
		osg::DegreesToRadians(-20.0), osg::Vec3(0, 1, 0), // roll
		osg::DegreesToRadians(-15.0), osg::Vec3(1, 0, 0), // pitch
		osg::DegreesToRadians(10.0), osg::Vec3(0, 0, 1)); // heading 
	
	// 60 meters behind and 7 meters above the tank model
	cameraTrans.makeTranslate(10, -50, 15);

	myCameraMatrix = cameraRotation * cameraTrans;

	viewer.setSceneData(root);
	//viewer.setCameraManipulator(new osgGA::TrackballManipulator());
	viewer.realize();

	viewer.getCamera()->setViewMatrixAsLookAt(osg::Vec3(20, 20, 10),tankXform->getPosition(),osg::Vec3(0,0,1));


	while (!viewer.done())
	{
		viewer.frame();
	}
}
