#include "GeomEater.h"

namespace cinder {

GeomEater::GeomEater(TriMesh::Format format)
: mMainMesh(TriMesh::create(format)) {

}

TriMesh const & GeomEater::getMesh() {
	return * mMainMesh;
}

uint8_t GeomEater::getAttribDims(geom::Attrib attr) const {
	return mMainMesh->getAttribDims(attr);
}

void GeomEater::copyAttrib(geom::Attrib attr, uint8_t srcAttrDims, size_t strideBytes, float const * srcData, size_t numVertices) {
	uint8_t destAttrDims = getAttribDims(attr);

	if (destAttrDims == 0) {
		// Do nothing if the base mesh is not interested in this attribute
		return;
	}

	std::vector<float> * floatDataVec;
	std::vector<vec3> * vecDataVec;
	// This is such a hacky switch statement. Automatic drop-through is intended behavior...
	switch (attr) {
		case geom::Attrib::POSITION:
			floatDataVec = & mMainMesh->getBufferPositions();
		case geom::Attrib::COLOR:
			floatDataVec = & mMainMesh->getBufferColors();
		case geom::Attrib::TEX_COORD_0:
			floatDataVec = & mMainMesh->getBufferTexCoords0();
		case geom::Attrib::TEX_COORD_1:
			floatDataVec = & mMainMesh->getBufferTexCoords1();
		case geom::Attrib::TEX_COORD_2:
			floatDataVec = & mMainMesh->getBufferTexCoords2();
		case geom::Attrib::TEX_COORD_3:
			floatDataVec = & mMainMesh->getBufferTexCoords3();

			// So much potential for a segfault lol
			floatDataVec->resize(floatDataVec->size() + destAttrDims * numVertices);
			geom::copyData(srcAttrDims, srcData, numVertices, destAttrDims, 0, & (* floatDataVec->end()));
		break;

		case geom::Attrib::NORMAL:
			vecDataVec = & mMainMesh->getNormals();
		case geom::Attrib::TANGENT:
			vecDataVec = & mMainMesh->getTangents();
		case geom::Attrib::BITANGENT:
			vecDataVec = & mMainMesh->getBitangents();

			// Potential segfault alert!!
			vecDataVec->resize(vecDataVec->size() + numVertices);
			geom::copyData(srcAttrDims, srcData, numVertices, destAttrDims, 0, (float *) (& (* vecDataVec->end())));
		break;

		default:
			throw geom::ExcMissingAttrib();
	}
}

void GeomEater::copyIndices(geom::Primitive primitive, uint32_t const * source, size_t numIndices, uint8_t numBytesPerIndex) {
	size_t targetIndices = numIndices;
	if (primitive == geom::Primitive::TRIANGLE_STRIP || primitive == geom::Primitive::TRIANGLE_FAN) {
		// Own indices need to be for triangles, so there are many more of them
		targetIndices = (numIndices - 2) * 3;
	}

	uint32_t meshNumIndices = mMainMesh->getNumIndices();
	std::unique_ptr<uint32_t[]> offsetIndices(new uint32_t[numIndices]);
	for (size_t idx = 0; idx < numIndices; idx++) {
		offsetIndices[idx] = meshNumIndices + source[idx];
	}

	std::vector<uint32_t> indexBuffer = mMainMesh->getIndices();
	indexBuffer.resize(indexBuffer.size() + targetIndices);
	// Copy indexes into the space starting at 1 past the end of the index buffer
	copyIndexDataForceTriangles(primitive, offsetIndices.get(), targetIndices, 0, & (*indexBuffer.end()));
}

geom::AttribSet GeomEater::getSupportedAttribs() {
	static std::array<geom::Attrib, 9> possibleAttribs = {
		geom::Attrib::POSITION, geom::Attrib::NORMAL, geom::Attrib::TANGENT, geom::Attrib::BITANGENT, geom::Attrib::COLOR, geom::Attrib::TEX_COORD_0, geom::Attrib::TEX_COORD_1, geom::Attrib::TEX_COORD_2, geom::Attrib::TEX_COORD_3
	};

	geom::AttribSet supported;
	for (auto attr : possibleAttribs) {
		if (getAttribDims(attr)) {
			supported.insert(attr);
		}
	}

	return supported;
}

void GeomEater::eat(geom::Source const & src) {
	src.loadInto(this, getSupportedAttribs());
}

} // namespace cinder