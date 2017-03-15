#include "ClientDataManager.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

ClientDataManager::ClientDataManager()
{

	//Chargement du fichier XML
	xmlDoc = new tinyxml2::XMLDocument();
	xmlDoc->LoadFile("client_data/Clip2.tif.aux.xml");

	if (xmlDoc->Error()) {
		std::cout << "ERROR IN PARSING THE FILE";
		std::cout << xmlDoc->GetErrorStr1();
		exit(0);
	}

	//Recuperation des parametres du "World File"
	std::ifstream infile("client_data/Clip2-modif.wld");

	long double currentParameter;
	int i = 0;

	while (infile >> currentParameter) {
		worldFileParameters[i] = currentParameter;
		i++;
	}

	//Recuperation des arbres
	std::ifstream  data("client_data/Arbres.csv");

	std::string line;

	int j = 1;

	while (std::getline(data, line))
	{
		int k = 0;
		std::stringstream  lineStream(line);
		std::string        cell;

		double lon, lat;

		while (std::getline(lineStream, cell, ','))
		{
			if (k == 0) {
				lon = atof(cell.c_str());
			}
			else if(k == 1){
				lat = atof(cell.c_str());
			}

			k++;
		}
		//std::cout << "Longitude " << lon << " Latitude" << lat << "\n";

		j++;
	}
}


ClientDataManager::~ClientDataManager()
{
}

float ClientDataManager::getMaxHeight()
{
	//Hauteur maximum du terrain ( en metre)
	const char* maxHeightstr = xmlDoc->FirstChildElement("PAMDataset")->FirstChildElement("PAMRasterBand")->FirstChildElement("Histograms")->FirstChildElement("HistItem")->FirstChildElement("HistMax")->GetText();

	//Cast en float de la hauteur max
	float maxHeight = std::stod(maxHeightstr);
	return maxHeight;
}

float ClientDataManager::getMinHeight()
{
	//Hauteur minimum du terrain ( en metre)
	const char* minHeightstr = xmlDoc->FirstChildElement("PAMDataset")->FirstChildElement("PAMRasterBand")->FirstChildElement("Histograms")->FirstChildElement("HistItem")->FirstChildElement("HistMin")->GetText();

	//Cast en float de la hauteur min
	float minHeight = std::stod(minHeightstr);

	return minHeight;
}

double * ClientDataManager::getWorldFileParameters()
{
	return worldFileParameters;
}

float ** ClientDataManager::getTreeData()
{
	return treeData;
}
