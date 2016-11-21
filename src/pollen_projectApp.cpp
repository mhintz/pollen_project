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
	static void prepSettings(Settings * settings);

	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;

	void keyDown(KeyEvent event) override;

	CameraPersp mCamera;
	CameraUi mUiCamera;

	gl::GlslProgRef mShader;
	GeomEater mMeshEater;
};

void pollen_projectApp::prepSettings(Settings * settings) {
	settings->setHighDensityDisplayEnabled();
}

void pollen_projectApp::setup()
{
	mCamera.lookAt(vec3(0, 0, 30), vec3(0), vec3(0, 1, 0));
	mUiCamera = CameraUi(&mCamera, getWindow());

	mMeshEater = GeomEater(TriMesh::Format().positions().normals().colors());
	auto ico = geom::Icosahedron().colors();

	for (int idx = 0; idx < 5; idx++) {
		float angle = glm::two_pi<float>() / 5 * idx;
		vec3 pos(cos(angle), sin(angle), 0);
		mMeshEater.eat(ico >> geom::Translate(2.f * pos));
	}

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

CINDER_APP( pollen_projectApp, RendererGl(RendererGl::Options().msaa(4)), & pollen_projectApp::prepSettings )
