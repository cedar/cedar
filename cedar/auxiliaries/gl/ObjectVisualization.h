/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
    This file is part of cedar.

    cedar is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the
    Free Software Foundation, either version 3 of the License, or (at your
    option) any later version.

    cedar is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
    License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with cedar. If not, see <http://www.gnu.org/licenses/>.

========================================================================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        ObjectVisualization.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 10 27

    Description: Virtual class for a simple object geometry for visualization with OpenGL

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GL_OBJECT_VISUALIZATION_H
#define CEDAR_AUX_GL_OBJECT_VISUALIZATION_H

#define GL_GLEXT_PROTOTYPES // to avoid a problem with finding some GL stuff, apparently caused by Qt
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

// CEDAR INCLUDES
#include "cedar/auxiliaries/gl/ObjectVisualization.fwd.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/LocalCoordinateFrame.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/gl/gl.h"
#include "cedar/auxiliaries/BoolParameter.h"
#include "cedar/auxiliaries/DoubleVectorParameter.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/IntParameter.h"

// SYSTEM INCLUDES
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <QObject>

/*!@brief Base class for simple OpenGL visualizations of geometric objects
 *
 */
class cedar::aux::gl::ObjectVisualization : public QObject, public cedar::aux::Configurable
{    
  //--------------------------------------------------------------------------------------------------------------------
  // structs
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief container for vertex and related data
  typedef struct
  {
    //!@brief position of the vertex
    GLfloat location[3];
    //!@brief texture coordinates
    GLfloat tex[2];
    //!@brief normal
    GLfloat normal[3];
    //!@brief color
    GLfloat colour[4];
    //!@brief pads the struct out to 64 bytes for performance increase
    GLubyte padding[16];
  } Vertex;

private:
  Q_OBJECT
  
public:
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
  /*!@brief standard constructor.
   * @param pLocalCoordinateFrame pointer to the LocalCoordinateFrame of the visualized object
   * @param objectType the type of the object
   * @param colorR red value of RGB
   * @param colorG green value of RGB
   * @param colorB blue value of RGB
   */
  ObjectVisualization
  (
    cedar::aux::LocalCoordinateFramePtr pLocalCoordinateFrame,
    const std::string& objectType = "no type",
    double colorR = 1.0,
    double colorG = 0.0,
    double colorB = 0.0
  );
  
  /*!@brief destructor. */
  virtual ~ObjectVisualization();
  
  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief the visualization initializes resources in the current GL context
  virtual void initializeGl();

  //!@brief draws a visualization of the object in the current GL context
  virtual void draw() = 0;
  
  //!@brief returns visibility state
  bool isVisible() const;

  //!@brief returns type of the object
  const std::string& getObjectVisualizationType() const;
  
  //!@brief returns current resolution
  int getResolution() const;
  
  //!@brief returns R value of main object color in RGB
  double getColorR() const;
  
  //!@brief returns G value of main object color in RGB
  double getColorG() const;
  
  //!@brief returns B value of main object color in RGB
  double getColorB() const;
  
  //!@brief switch between drawing the object with full surfaces or as wire frame only
  void setDrawAsWireFrame(const bool state);

  //!@brief get state of object being drawn with full surfaces or as wire frame only
  bool isDrawnAsWireFrame() const;
  
  //!@brief switch drawing the local coordinate frame of the rigid body
  void setDrawLocalCoordinateFrame(const bool state);

  //!@brief true if the local coordinate frame of the rigid body is being drawn
  bool isDrawingLocalCoordinateFrame() const;

  /*!@brief set the length of the axes in the local coordinate frame visualization
   * @param value    new axis length
   */
  void setAxisLength(double value);

  /*!@brief get the length of the axes in the local coordinate frame visualization
   * @return axis length
   */
  double getAxisLength() const;

  /*!@brief set the general resolution of the object, 10 is a usual value
   * @param value    new resolution value
   */
  void setResolution(int value);
  
  /*!@brief sets the main color of the object, in RGB
   * @param r    value for red channel in RGB color
   * @param g    value for green channel in RGB color
   * @param b    value for blue channel in RGB color
   */
  void setColor(double r, double g, double b);


  inline bool getIsDrawnAsWireFrame() const
  {
    return _mIsDrawnAsWireFrame->getValue();
  }
  
  inline bool getIsDrawingLocalCoordinateFrame() const
  {
    return _mIsDrawingLocalCoordinateFrame->getValue();
  }

  /*!@brief returns a smart pointer to the local coordinate frame of the visualized object
   *
   * @return smart pointer to the LocalCoordinateFrame
   */
  cedar::aux::LocalCoordinateFramePtr getLocalCoordinateFrame();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  //!@brief prepares the drawing by moving to the local coordinate frame of the object
  void prepareDraw();

  //!@brief draws the local coordinate frame of the rigid body if mIsDrawingLocalCoordinateFrame flag is set
  void drawLocalCoordinateFrame();

  /*!@brief loads vertex data from file into a vertex array
   * @param dataFileName specifies the file where the vertex data are stored
   * @param numberOfVertices specifies how many vertices are in the file
   * @param vertices array of vertices the data is stored into
   */
  void loadVertexData(const QString& dataFileName, unsigned int numberOfVertices, Vertex* vertices);

  /*!@brief loads index data from file into a vertex array
   * @param dataFileName specifies the file where the vertex data are stored
   * @param numberOfFaces specifies how many faces are in the file (3 indices for each face)
   * @param indices array where the data is stored into
   */
  void loadIndexData(const QString& dataFileName, unsigned int numberOfFaces, GLushort* indices);

  //!@brief draw an element
  void drawElement
  (
    const GLuint vertexVboId,
    const GLuint indexVboId,
    unsigned int numberOfFaces
  );

  //!@brief set a material specified by an int value
  void setMaterial(int material);

  //--------------------------------------------------------------------------------------------------------------------
  // Qt slots
  //--------------------------------------------------------------------------------------------------------------------

public slots:
  //!@brief turns the visibility of the object on and off
  void setVisibility(bool state = true);
  
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief pointer to the LocalCoordinateFrame of the visualized object
  cedar::aux::LocalCoordinateFramePtr mpLocalCoordinateFrame;

private:
  //!@brief geometric type of the object
  std::string mObjectType;
 
protected:

  //!@brief encodes the different materials
  enum MaterialType {
                      NO_MATERIAL,
                      SEGMENT,
                      POWERCUBE,
                      CHROME,
                      BRONZE,
                      BRASS,
                      BLACK,
                      WHITE
                    };

  //! Static member for no specularity.
  static const float mNoSpecular[3];

  //! Static member for ambient color of segments.
  static const float mSegment_Ambient[3];

  //! Static member for diffuse color of segments.
  static const float mSegment_Diffuse[3];

  //! Static member for specilar color of segments.
  static const float mSegment_Specular[3];

  //! Static member for the shininess factor of segments.
  static const float mSegment_Shininess[1];

  //! Static member for ambient color of power cubes.
  static const float mPowerCube_Ambient[3];

  //! Static member for diffuse color of power cubes.
  static const float mPowerCube_Diffuse[3];

  //! Static member for specilar color of power cubes.
  static const float mPowerCube_Specular[3];

  //! Static member for the shininess factor of power cubes.
  static const float mPowerCube_Shininess[1];

  //! Static member for the ambient color of chrome parts.
  static const float mChrome_Ambient[3];

  //! Static member for the ambient color of chrome parts.
  static const float mChrome_Diffuse[3];

  //! Static member for the specular color of chrome parts.
  static const float mChrome_Specular[3];

  //! Static member for the shininess factor of chrome parts.
  static const float mChrome_Shininess[1];

  //! Static member for the ambient color of bronze parts.
  static const float mBronze_Ambient[3];

  //! Static member for the ambient color of bronze parts.
  static const float mBronze_Diffuse[3];

  //! Static member for the specular color of bronze parts.
  static const float mBronze_Specular[3];

  //! Static member for the shininess factor of bronze parts.
  static const float mBronze_Shininess[1];

  //! Static member for the ambient color of brass parts.
  static const float mBrass_Ambient[3];

  //! Static member for the ambient color of brass parts.
  static const float mBrass_Diffuse[3];

  //! Static member for the specular color of brass parts.
  static const float mBrass_Specular[3];

  //! Static member for the shininess factor of brass parts.
  static const float mBrass_Shininess[1];

  //! Static member for the ambient color of black parts.
  static const float mBlack_Ambient[3];

  //! Static member for the diffuse color of black parts.
  static const float mBlack_Diffuse[3];

  //! Static member for the specular color of black parts.
  static const float mBlack_Specular[3];

  //! Static member for the shininess factor of black parts.
  static const float mBlack_Shininess[1];

  //! Static member for the ambient color of white parts.
  static const float mWhite_Ambient[3];

  //! Static member for the diffuse color of white parts.
  static const float mWhite_Diffuse[3];

  //! Static member for the specular color of white parts.
  static const float mWhite_Specular[3];

  //! Static member for the shininess factor of white parts.
  static const float mWhite_Shininess[1];

private:
  // none yet

// Parameters:

  //!@brief object color in RGB, R channel
  cedar::aux::DoubleParameterPtr _mColorR;
  //!@brief object color in RGB, G channel
  cedar::aux::DoubleParameterPtr _mColorG;
  //!@brief object color in RGB, B channel
  cedar::aux::DoubleParameterPtr _mColorB;

  //!@brief determines how well curves and surfaces are approximated (default 10)
  cedar::aux::IntParameterPtr _mResolution;

  //!@brief length of the local coordinate frame axis arrows
  cedar::aux::DoubleParameterPtr _mAxisLength;

  //!@brief determines if the object is drawn with full surfaces or as wire frame only
  cedar::aux::BoolParameterPtr _mIsDrawnAsWireFrame;
  //!@brief determines whether the local coordinate frame of the rigid body is drawn
  cedar::aux::BoolParameterPtr _mIsDrawingLocalCoordinateFrame;

  //!@brief the object will only be drawn if this is true
  cedar::aux::BoolParameterPtr _mIsVisible;

};

#include "cedar/auxiliaries/FactoryManager.h"
#include "cedar/auxiliaries/Singleton.h"

namespace cedar
{
  namespace aux
  {
    namespace gl
    {
      //!@brief The manager of all sigmoind instances
      typedef cedar::aux::FactoryManager<ObjectVisualizationPtr> ObjectVisualizationManager;

      //!@brief The singleton object of the TransferFunctionFactory.
      typedef cedar::aux::Singleton<ObjectVisualizationManager> ObjectVisualizationManagerSingleton;
    }
  }
}

CEDAR_AUX_EXPORT_SINGLETON(cedar::aux::gl::ObjectVisualizationManager);


#endif  // CEDAR_AUX_GL_OBJECT_VISUALIZATION_H
