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

#ifndef CEDAR_AUX_GL_VIEWER_H
#define CEDAR_AUX_GL_VIEWER_H

// LOCAL INCLUDES
#include "namespace.h"
#include "Scene.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QGLViewer/qglviewer.h>
#include <QList>

class cedar::aux::gl::Viewer : public QGLViewer
{
private:
  
	Q_OBJECT
  
public:
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
  /*!@brief the constructor */
	Viewer(cedar::aux::gl::Scene* scene);

  /*!@brief the constructor */
	~Viewer();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief draws all objects in the scene */
	void draw();

  /*!@brief function being called automatically when a timer is up, usually in a loop */
	void timerEvent(QTimerEvent* pEvent);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*!@brief initialization */
  void init();
  
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
  cedar::aux::gl::Scene* mpScene;

};

#endif  // CEDAR_AUX_GL_VIEWER_H
