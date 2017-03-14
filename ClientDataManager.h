#pragma once
#include "tinyxml2.h"
#include <fstream>

//Gere de parser les bons fichier et acceder aux données essentiels assez facilement

class ClientDataManager
{
public:
	ClientDataManager();
	~ClientDataManager();
	//Hauteur maximal pour le terrain
	float getMaxHeight();
	//Hauteur minimum pour le terrain
	float getMinHeight();
	//Retourne les parametres du "World file"
	float* getWorldFileParameters();

	float ** getTreeData();

private:
	tinyxml2::XMLDocument* xmlDoc;
	float worldFileParameters[6];
	float** treeData;
};

