#include "CoordinateConverter.h"

//Constantes
const float CoordinateConverter::earth_radius = 6371000;



CoordinateConverter::CoordinateConverter(float * worldParams, float maxH, float minH, float heightScale)
{
	worldParameters = worldParams;
	maxHeight = maxH;
	minHeight = minH;
	this->heightScale = heightScale;
	//
	aspect_ratio = (worldParameters[0] * 0 + worldParameters[2] * 0 + worldParameters[4] +
		worldParameters[0] * 512 + worldParameters[2] *0 + worldParameters[4]) / 2;
}

CoordinateConverter::~CoordinateConverter()
{
}

float CoordinateConverter::pixelToHeightMeters(int pixelValue)
{
	float normalized = minHeight + (maxHeight - minHeight) * (pixelValue) / 255;
	//Car unite de base cm
	return normalized * heightScale;
}

osg::Vec2 CoordinateConverter::getLonLatFromPixel(osg::Vec2 pixelPos)
{
	osg::Vec2 longLat;
	//Transformation grace au world file
	longLat.set(worldParameters[0] * pixelPos.x() + worldParameters[2] * pixelPos.y() + worldParameters[4],
			worldParameters[1] * pixelPos.x() + worldParameters[3] * pixelPos.y() + worldParameters[5]);
	return longLat;
}

//Ne marche que sans rotation
osg::Vec2 CoordinateConverter::LonLatToPixel(osg::Vec2 lonLat) {
	float pixelX = (lonLat.x() - worldParameters[4]) / worldParameters[0];
	float pixelY = (lonLat.y() - worldParameters[5]) / worldParameters[3];

	return osg::Vec2(pixelX, pixelY);
}

osg::Vec3 CoordinateConverter::pixelToLonLatAtt(osg::Vec3 lonLatAtt)
{
	float altitude = pixelToHeightMeters(lonLatAtt.z());
	osg::Vec2 longLat = getLonLatFromPixel(osg::Vec2(lonLatAtt.x(), lonLatAtt.y()));
	return osg::Vec3(longLat,altitude);
}



//Approximation (fait la deformation necessaire)
//http://stackoverflow.com/questions/16266809/convert-from-latitude-longitude-to-x-y
osg::Vec2 CoordinateConverter::lonLatToXY(osg::Vec2 lonLatAtt)
{
	float lonInRad = osg::DegreesToRadians(lonLatAtt.x());
	float latInRad = osg::DegreesToRadians(lonLatAtt.y());

	float ratio = cos(osg::DegreesToRadians(aspect_ratio));

	float x = earth_radius * lonInRad * ratio;
	float y = earth_radius * latInRad;

	return osg::Vec2(x, y);
}

osg::Vec3 CoordinateConverter::pixelToXYZ(osg::Vec3 pixel)
{
	osg::Vec3 lonLatAtt = pixelToLonLatAtt(pixel);

	//Approximation
	osg::Vec2 XY = lonLatToXY(osg::Vec2(lonLatAtt.x(), lonLatAtt.y()));
	osg::Vec3 result(XY, lonLatAtt.z());
	
	//Real
	//std::cout << result.x() << " " << result.y() << "\n";
	//return lonLatAttToXYZ(lonLatAtt);
	
	return result;
}

//NON UTILISÉ
osg::Vec3 CoordinateConverter::lonLatAttToXYZ(osg::Vec3 lonLatAtt)
{
	float lonInRad = osg::DegreesToRadians(lonLatAtt.x());
	float latInRad = osg::DegreesToRadians(lonLatAtt.y());

	double x;
	double y;
	double z;

	std::cout << lonLatAtt.z() << "\n";

	osgConverter.convertLatLongHeightToXYZ(latInRad, lonInRad, 0, x, y, z);



	return osg::Vec3(x, y, z);
}