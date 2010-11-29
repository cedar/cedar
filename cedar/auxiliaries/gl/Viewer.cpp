/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet-Bochum
                    Institut fuer Neuroinformatik
 
 ----- File:        Viewer.h
 
 ----- Maintainer:  Hendrik Reimann
 ------Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 10 28
 
 ----- Description: simple viewer for visualizing a Scene of Objects
 
 ----- Credits:     
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "Viewer.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

using namespace cedar::aux::gl;
using namespace std;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

Viewer::Viewer(cedar::aux::gl::Scene* const scene)
:
mpScene(scene)
{

}

Viewer::~Viewer()
{
  
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void Viewer::init()
{
  restoreStateFromFile();
  mpScene->initLighting();
}

void Viewer::draw()
{
  mpScene->draw();
}

void Viewer::timerEvent(QTimerEvent*)
{
  if (isVisible())
  {
    update();
  }
}

