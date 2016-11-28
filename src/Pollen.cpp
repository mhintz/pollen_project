#include "Pollen.h"

using namespace ci;

Pollen::Pollen() {
	mMesh = GeomEater(TriMesh::Format().positions().normals().colors());

	mRadius = 3.f;
	mSquish = 0.7f;
	mNumSpines = 24;
	mSpineLength = 1.f;
	mSpineDistance = 0.45f;
}

TriMeshRef Pollen::getMeshRef() { return mMesh.getMeshRef(); }

TriMesh const & Pollen::getMesh() { return mMesh.getMesh(); }

std::vector<vec3> Pollen::pointsOnSphere(int count) {
	static float const phi = glm::two_pi<float>() / pow(glm::golden_ratio<float>(), 2.f);

	std::vector<vec3> points;

	for (int idx = 0; idx < count; idx++) {
	  float angle = phi * idx;
	  float zDistance = (1.f - 1.f / count) * (1.f - (2.f * idx) / (count - 1.f));
	  float unitRadius = sqrt(1.f - zDistance * zDistance);

	  points.push_back(vec3(unitRadius * cos(angle), zDistance, unitRadius * sin(angle)));
	}

	return points;
}

vec3 Pollen::posOnSphere(float lat, float lon) {
	return vec3(sin(lat) * cos(lon), cos(lat), sin(lat) * sin(lon));
}

geom::Scale Pollen::getSquish() {
	return geom::Scale(1.f, mSquish, 1.f);
}

ci::vec3 Pollen::squishPoint(vec3 point) {
	point.y *= mSquish;
	return point;
}

void Pollen::generate() {
	auto base = geom::Icosphere().subdivisions(5).colors();

	auto spikePoints = pointsOnSphere(mNumSpines);

	auto modifiedBase = base >> geom::AttribFn<vec3, vec3>(geom::Attrib::POSITION, geom::Attrib::POSITION, [this, & spikePoints] (vec3 inPos) {
		float nearestAngle = glm::angle(inPos, spikePoints.at(0));
		float nearestDistance = distance2(inPos, spikePoints.at(0));
		for (auto & spike : spikePoints) {
			float spikeAngle = glm::angle(inPos, spike);
			if (spikeAngle < nearestAngle) {
				nearestAngle = spikeAngle;
				nearestDistance = distance2(inPos, spike);
			}
		}
		float tval = glm::smoothstep(0.0f, mSpineDistance, (float) sqrt(nearestDistance));
		// return inPos * (1.0f + mSpineLength * (1.f / (float) pow(-tval - 1.f, 2.f)));
		return inPos * (float) (1.0 + mSpineLength * pow(math<double>::min(cos(M_PI * tval / 2.0), 1.0 - fabs(tval)), 2.5));
	});

	mMesh.eat(modifiedBase >> geom::Scale(mRadius, mRadius, mRadius));
}