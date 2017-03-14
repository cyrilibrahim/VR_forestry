
#include <gl/glut.h> 
#include <osg/Group>
#include <osgDB/ReadFile> 
#include <osg/PositionAttitudeTransform>
#include <osgViewer/Viewer>
#include <osgSim/DOFTransform>
#include <osgSim/MultiSwitch>
#include "findNodeVisitor.h"
#include <osgGA/TrackballManipulator>

#include <osgText/Font>
#include <osgText/Text>

class tankDataType : public osg::Referenced
{
public:
	tankDataType(osg::Node*n);
	void updateTurretRotation();
	void updateGunElevation();
protected:
	osgSim::DOFTransform* tankTurretNode;
	osgSim::DOFTransform* tankGunNode;
	double rotation;
	double elevation;
};

//Constructor (tankDataType)
tankDataType::tankDataType(osg::Node*n) {
	rotation = 0;
	elevation = 0;

	findNodeVisitor findTurret("turret");
	n->accept(findTurret);
	tankTurretNode = dynamic_cast <osgSim::DOFTransform*> (findTurret.getFirst());
	findNodeVisitor findGun("gun");
	n->accept(findGun);
	tankGunNode = dynamic_cast <osgSim::DOFTransform*> (findGun.getFirst());
}

void tankDataType::updateGunElevation() {
	elevation += 0.01;
	tankGunNode->setCurrentHPR(osg::Vec3(0,elevation,0));

	if (elevation > 0.5) {
		elevation = 0.0;
	}
}

void tankDataType::updateTurretRotation() {
	rotation += 0.01;
	tankTurretNode->setCurrentHPR(osg::Vec3(rotation, 0, 0));
}

class tankNodeCallBack : public osg::NodeCallback
{
	public:
		//We override the operator () wich will be executed at each update
		virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {

			osg::ref_ptr<tankDataType> tankData = dynamic_cast <tankDataType*> (node->getUserData());
			
			if (tankData) {
				tankData->updateGunElevation();
				tankData->updateTurretRotation();
			}

			traverse(node, nv);
		}
};

int main()
{
	// Declare a group for the root of our tree and 
	//  three groups to contain individual tank models
	osg::Group* root = new osg::Group();

	osgViewer::Viewer viewer;

	osg::Node* tankNode = NULL;


	tankNode = osgDB::readNodeFile("models/t72-tank/t72-tank_des.flt");
	root->addChild(tankNode);

	tankDataType* tankData = new tankDataType(tankNode);
	tankNode->setUserData(tankData);

	tankNode->setUpdateCallback(new tankNodeCallBack);

	viewer.setSceneData(root);
	viewer.setCameraManipulator(new osgGA::TrackballManipulator());
	viewer.realize();

	while (!viewer.done())
	{
		viewer.frame();
	}
}
