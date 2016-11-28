#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"
#include "cinder/CameraUi.h"
#include "cinder/TriMesh.h"
#include "cinder/params/Params.h"
#include "cinder/Rand.h"

#include "Node.hpp"

#include "Pollen.h"

using namespace ci;
using namespace ci::app;
using namespace std;

struct Blob {
	Blob(vec3 pos, vec3 vel, gl::BatchRef b) : mNode(pos, quat(), vec3(1)), mVel(vel), mBatch(b) {}

	Node mNode;
	vec3 mVel;
	gl::BatchRef mBatch;

	void update() {
		mNode.translate(mVel);
	}

	void draw() {
		gl::ScopedModelMatrix scpMat;
		gl::multModelMatrix(mNode.matrix());
		mBatch->draw();
	}
};

class PollenProjectApp : public App {
public:
	static void prepSettings(Settings * settings);

	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;

	void keyDown(KeyEvent event) override;

	void regeneratePollen();
	void addPollen();

	CameraPersp mCamera;
	CameraUi mUiCamera;

	gl::GlslProgRef mShader;

	std::vector<Blob> mPollenBalls;

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
}

void PollenProjectApp::addPollen() {
	Pollen theBlob;
	theBlob.setSpineDistance(mSpineRadius);
	theBlob.generate();

	mPollenBalls.push_back(Blob(vec3(0), randVec3() * 0.08f, gl::Batch::create(theBlob.getMesh(), mShader)));
}

void PollenProjectApp::mouseDown( MouseEvent event )
{
}

void PollenProjectApp::keyDown(KeyEvent event) {
	if (event.getCode() == KeyEvent::KEY_ESCAPE) {
		quit();
	}
}

void PollenProjectApp::update()
{
	for (auto & blob : mPollenBalls) {
		blob.update();
	}

	if (getElapsedFrames() % 120 == 0) {
		addPollen();
	}

	mPollenBalls.erase(std::remove_if(mPollenBalls.begin(), mPollenBalls.end(), [] (Blob & ball) { return length2(ball.mNode.position()) >= 10000.f; }), mPollenBalls.end());
}

void PollenProjectApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 

	gl::setMatrices(mCamera);

	mShader->bind();

	for (auto & blob : mPollenBalls) {
		blob.draw();
	}

	mParams->draw();
}

CINDER_APP( PollenProjectApp, RendererGl(RendererGl::Options().msaa(4)), & PollenProjectApp::prepSettings )
