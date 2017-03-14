#include <gl/glut.h> 
#include <osgViewer/Viewer> 
#include <osg/CoordinateSystemNode>
#include <iostream>
#pragma once
class CoordinateConverter
{
public:
	CoordinateConverter(float* worldParams, float maxH, float minH, float heightScale);
	~CoordinateConverter();
	float pixelToHeightMeters(int pixelValue);
	osg::Vec2 getLonLatFromPixel(osg::Vec2 pixelPos);
	osg::Vec2 LonLatToPixel(osg::Vec2 lonLat);
	osg::Vec3 pixelToLonLatAtt(osg::Vec3 lonLatAtt);
	osg::Vec3 lonLatAttToXYZ(osg::Vec3 lonLatAtt);
	osg::Vec2 lonLatToXY(osg::Vec2 lonLat);
	osg::Vec3 pixelToXYZ(osg::Vec3 pixel);
	double meterToFeet(double meter) {
		return meter * 3.28084;
	}

private:
	float* worldParameters;
	float maxHeight;
	float minHeight;
	static const float earth_radius;
	float aspect_ratio;
	float heightScale;
	osg::EllipsoidModel osgConverter;
};

