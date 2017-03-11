#include "ClientDataManager.h"
#include <string>
#include <iostream>

ClientDataManager::ClientDataManager()
{

	//Chargement du fichier XML
	xmlDoc = new tinyxml2::XMLDocument();
	xmlDoc->LoadFile("C:/OpenSceneGraph/datasets/client_data/Clip2.tif.aux.xml");

	if (xmlDoc->Error()) {
		std::cout << "ERROR IN PARSING THE FILE";
		std::cout << xmlDoc->GetErrorStr1();
		exit(0);
	}

	//Recuperation des parametres du "World File"
	std::ifstream infile("C:/OpenSceneGraph/datasets/client_data/Clip2.wld");

	double currentParameter;
	int i = 0;

	while (infile >> currentParameter) {
		worldFileParameters[i] = currentParameter;
		i++;
	}
	
}


ClientDataManager::~ClientDataManager()
{
}

double ClientDataManager::getMaxHeight()
{
	//Hauteur maximum du terrain ( en metre)
	const char* maxHeightstr = xmlDoc->FirstChildElement("PAMDataset")->FirstChildElement("PAMRasterBand")->FirstChildElement("Histograms")->FirstChildElement("HistItem")->FirstChildElement("HistMax")->GetText();

	//Cast en float de la hauteur max
	double maxHeight = std::stod(maxHeightstr);
	return maxHeight;
}

double ClientDataManager::getMinHeight()
{
	//Hauteur minimum du terrain ( en metre)
	const char* minHeightstr = xmlDoc->FirstChildElement("PAMDataset")->FirstChildElement("PAMRasterBand")->FirstChildElement("Histograms")->FirstChildElement("HistItem")->FirstChildElement("HistMin")->GetText();

	//Cast en float de la hauteur min
	double minHeight = std::stod(minHeightstr);

	return minHeight;
}

double * ClientDataManager::getWorldFileParameters()
{
	return worldFileParameters;
}
