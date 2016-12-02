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

	auto modifiedBase = base >> geom::AttribFn<vec3, vec3>(geom::Attrib::POSITION, geom::Attrib::POSITION, [this] (vec3 inPos) {
		float angle = fmax(glm::angle(inPos, vec3(1, 0, 0)), 0.05f);
		// return inPos + vec3(0.5 * cos(angle) * inPos.x, inPos.y, 0.5 * sin(angle) * inPos.z);
		float xfactor = map(cos(4.0 * M_PI * angle), -1, 1, 0.5, 1);
		float zfactor = map(sin(4.0 * M_PI * angle), -1, 1, 0.5, 1);
		// return inPos * vec3(xfactor, 1.0, zfactor);
		// return inPos * xfactor;
		return inPos;
	});

	modifiedBase = modifiedBase >> geom::AttribFn<vec3, vec3>(geom::Attrib::POSITION, geom::Attrib::POSITION, [this, & spikePoints] (vec3 inPos) {
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
		return inPos * (float) (1.0 + mSpineLength * pow(fmin(cos(M_PI * tval / 2.0), 1.0 - fabs(tval)), 2.5));
	});

	Gradient<Color> testGradient = {
	  { 0.0f, Color(0.0, 0.0, 0.0) }, // black
	  { 1.0f, Color(0, 0, 0.515) }, // blue
	  { 1.85f, Color(0.188, 0.835, 0.784) }, // turquoise
	  { 2.0f, Color(1.0, 1.0, 0.702) } // yellow
	};

	modifiedBase = modifiedBase >> geom::ColorFromAttrib(geom::Attrib::POSITION, [this, & testGradient] (vec3 inPos) {
		return testGradient.sample((float) length(inPos));
	});

	mMesh.eat(modifiedBase >> geom::Scale(mRadius, mRadius, mRadius));
}