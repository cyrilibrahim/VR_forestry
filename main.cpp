
#include <gl/glut.h> 
#include <osgViewer/Viewer> 
#include <osgText/Text> 
osg::Node* CreateScene()
{
	// Création d'un nœud et désactivation de la lumière pour celui-ci
	osg::Geode* geode = new osg::Geode();
	osg::StateSet* stateset = geode->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	// Création d'une projection orthographique afin d'avoir le texte aligné à l'écran
	osg::Camera* camera = new osg::Camera;
	camera->setProjectionMatrix(osg::Matrix::ortho2D(0, 512, 0, 512));
	camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	camera->setViewMatrix(osg::Matrix::identity());
	camera->addChild(geode);

	// Création d'un texte et ajout au noeud
	osgText::Text* text = new osgText::Text;
	geode->addDrawable(text);

	// Définit les données du texte
	text->setPosition(osg::Vec3(16.f, 16.0f, 0.0f));
	text->setColor(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));
	text->setText("Hello World");

	return camera;
}

int main(void)
{
	// Construit le viewer
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;

	// Créer une fenêtre 512x512 et la positionner en 32, 32
	viewer->setUpViewInWindow(32, 32, 512, 512);

	// Définit les données de scène que le viewer dessinera
	viewer->setSceneData(CreateScene());

	// Exécute la boucle principale
	return viewer->run();
}