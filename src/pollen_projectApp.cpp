#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class pollen_projectApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
};

void pollen_projectApp::setup()
{
}

void pollen_projectApp::mouseDown( MouseEvent event )
{
}

void pollen_projectApp::update()
{
}

void pollen_projectApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP( pollen_projectApp, RendererGl )
