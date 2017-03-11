#pragma once
#include "tinyxml2.h"

//Gere de parser les bons fichier et acceder aux données essentiels assez facilement

class ClientDataManager
{
public:
	ClientDataManager();
	~ClientDataManager();
	//Hauteur maximal pour le terrain
	double getMaxHeight();
	//Hauteur minimum pour le terrain
	double getMinHeight();

private:
	tinyxml2::XMLDocument* xmlDoc;

};

