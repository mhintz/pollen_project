#pragma once

#include "glm/gtx/vector_angle.hpp"
#include "glm/gtx/log_base.hpp"

#include "cinder/TriMesh.h"

#include "Math.h"
#include "GeomEater.h"
#include "Gradient.h"

class Pollen;
typedef std::unique_ptr<Pollen> PollenBox;
typedef std::shared_ptr<Pollen> PollenRef;

class Pollen {
public:
	static PollenRef create() { return std::make_shared<Pollen>(); }

	Pollen();

	ci::TriMeshRef getMeshRef();
	ci::TriMesh const & getMesh();

	std::vector<ci::vec3> pointsOnSphere(int count);
	ci::vec3 posOnSphere(float lat, float lon);
	ci::geom::Scale getSquish();
	ci::vec3 squishPoint(ci::vec3 point);

	void setSpineDistance(float newDist) { mSpineDistance = newDist; }

	void generate();

private:
	ci::GeomEater mMesh;

	float mRadius;
	float mSquish;
	int mNumSpines;
	float mSpineLength;
	float mSpineDistance;
};