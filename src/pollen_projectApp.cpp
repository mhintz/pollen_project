#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"
#include "cinder/CameraUi.h"
#include "cinder/TriMesh.h"

#include "GeomEater.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class pollen_projectApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;

	void keyDown(KeyEvent event) override;

	CameraPersp mCamera;
	CameraUi mUiCamera;

	TriMeshRef mPollenMesh;
	gl::GlslProgRef mShader;
	GeomEater mMeshEater;
};

void pollen_projectApp::setup()
{
	mCamera.lookAt(vec3(0, 0, 30), vec3(0), vec3(0, 1, 0));
	mUiCamera = CameraUi(&mCamera, getWindow());

	auto ico = geom::Icosahedron() >> geom::Translate(10, 0, 0) >> geom::Constant(geom::Attrib::COLOR, vec4(0, 0.2, 0.8, 1.0));
	auto cube = geom::Cube() >> geom::Translate(-10, 0, 0) >> geom::Constant(geom::Attrib::COLOR, vec4(0, 0.8, 0.2, 1.0));

	mPollenMesh = TriMesh::create();

	mMeshEater = GeomEater(TriMesh::Format().positions().normals().colors());

	mMeshEater.eat(ico);
	mMeshEater.eat(cube);

	mShader = gl::getStockShader(gl::ShaderDef().lambert().color());

	gl::enable(GL_DEPTH_TEST);
}

void pollen_projectApp::mouseDown( MouseEvent event )
{
}

void pollen_projectApp::keyDown(KeyEvent event) {
	if (event.getCode() == KeyEvent::KEY_ESCAPE) {
		quit();
	}
}

void pollen_projectApp::update()
{
}

void pollen_projectApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 

	gl::setMatrices(mCamera);

	mShader->bind();

	gl::draw(mMeshEater.getMesh());
}

CINDER_APP( pollen_projectApp, RendererGl )
