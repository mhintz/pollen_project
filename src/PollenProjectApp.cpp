#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"
#include "cinder/CameraUi.h"
#include "cinder/TriMesh.h"
#include "cinder/params/Params.h"

#include "Pollen.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class PollenProjectApp : public App {
public:
	static void prepSettings(Settings * settings);

	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;

	void keyDown(KeyEvent event) override;

	void regeneratePollen();

	CameraPersp mCamera;
	CameraUi mUiCamera;

	gl::GlslProgRef mShader;
	PollenRef mPollen;

	params::InterfaceGlRef mParams;

	float mSpineRadius = 0.87f;
};

void PollenProjectApp::prepSettings(Settings * settings) {
	settings->setHighDensityDisplayEnabled();
}

void PollenProjectApp::setup()
{
	mParams = params::InterfaceGl::create(getWindow(), "App parameters", toPixels(ivec2(200, 50)));

	mParams->addParam("spineRadius", & mSpineRadius).min(0.0f).max(2.f).precision(2).step(0.01f);

	gl::enable(GL_DEPTH_TEST);

	mCamera.lookAt(vec3(0, 0, 30), vec3(0), vec3(0, 1, 0));
	mUiCamera = CameraUi(&mCamera, getWindow());

	mShader = gl::getStockShader(gl::ShaderDef().lambert().color());

	regeneratePollen();
}

void PollenProjectApp::regeneratePollen() {
	mPollen = Pollen::create();
	mPollen->setSpineDistance(mSpineRadius);
	mPollen->generate();
}

void PollenProjectApp::mouseDown( MouseEvent event )
{
}

void PollenProjectApp::keyDown(KeyEvent event) {
	if (event.getCode() == KeyEvent::KEY_ESCAPE) {
		quit();
	} else if (event.getCode() == KeyEvent::KEY_r) {
		regeneratePollen();
	}
}

void PollenProjectApp::update()
{
}

void PollenProjectApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 

	gl::setMatrices(mCamera);

	mShader->bind();

	gl::draw(mPollen->getMesh());

	mParams->draw();
}

CINDER_APP( PollenProjectApp, RendererGl(RendererGl::Options().msaa(4)), & PollenProjectApp::prepSettings )
