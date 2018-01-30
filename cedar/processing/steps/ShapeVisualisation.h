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

    File:        ShapeVisualisation.h

    Maintainer:  Nico Kuerschner
    Email:       Nico.Kuerschner@ini.rub.de
    Date:        2017 04 28

    Description: Header file for the class cedar::proc::steps::ShapeVisualisation.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_SHAPE_VISUALISATION_H
#define CEDAR_PROC_STEPS_SHAPE_VISUALISATION_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// FORWARD DECLARATIONS
#include "cedar/processing/steps/ShapeVisualisation.fwd.h"

// SYSTEM INCLUDES
#include <cedar/processing/Step.h>
#include <cedar/auxiliaries/MatData.h>
#include "cedar/auxiliaries/EnumType.h"
#include "cedar/auxiliaries/EnumParameter.h"
#include "cedar/auxiliaries/BoolParameter.h"
#include <cedar/auxiliaries/DoubleParameter.h>

// includes for visualisation
#include "cedar/auxiliaries/gl/GlobalScene.h"
#include "cedar/auxiliaries/gl/Block.h"
#include "cedar/auxiliaries/gl/Cone.h"
#include "cedar/auxiliaries/gl/Cylinder.h"
#include "cedar/auxiliaries/gl/Prism.h"
#include "cedar/auxiliaries/gl/Pyramid.h"
#include "cedar/auxiliaries/gl/Sphere.h"
#include "cedar/auxiliaries/gl/Torus.h"

class cedar::proc::steps::ShapeVisualisation : public cedar::proc::Step
{
  Q_OBJECT
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  class Colour
  {
    public:

      typedef cedar::aux::EnumId Id;
      typedef boost::shared_ptr<cedar::aux::EnumBase> TypePtr;

    public:
      //! Registers the enum values.
      static void construct()
      {
        mColourType.type()->def(cedar::aux::Enum(red, "red"));
        mColourType.type()->def(cedar::aux::Enum(green, "green"));
        mColourType.type()->def(cedar::aux::Enum(blue, "blue"));
        mColourType.type()->def(cedar::aux::Enum(yellow, "yellow"));
        mColourType.type()->def(cedar::aux::Enum(purple, "purple"));
        mColourType.type()->def(cedar::aux::Enum(cyan, "cyan"));
        mColourType.type()->def(cedar::aux::Enum(black, "black"));
        mColourType.type()->def(cedar::aux::Enum(white, "white"));
      }

      //! Returns the enumeration type.
      static const cedar::aux::EnumBase& type()
      {
        return *mColourType.type();
      }

      //! Returns a pointer to an enumeration type.
      static const TypePtr& typePtr()
      {
        return mColourType.type();
      }

    public:
      static const Id red = 0;
      static const Id green = 1;
      static const Id blue = 2;
      static const Id yellow = 3;
      static const Id purple = 4;
      static const Id cyan = 5;
      static const Id black = 6;
      static const Id white = 7;

    private:
      static cedar::aux::EnumType<Colour> mColourType;
  };

  class Shape
  {
    public:

      typedef cedar::aux::EnumId Id;
      typedef boost::shared_ptr<cedar::aux::EnumBase> TypePtr;

    public:
      //! Registers the enum values.
      static void construct()
      {
        mShapeType.type()->def(cedar::aux::Enum(block, "Block"));
        mShapeType.type()->def(cedar::aux::Enum(cone, "Cone"));
        mShapeType.type()->def(cedar::aux::Enum(cylinder, "Cylinder"));
        mShapeType.type()->def(cedar::aux::Enum(prism, "Prism"));
        mShapeType.type()->def(cedar::aux::Enum(pyramid, "Pyramid"));
        mShapeType.type()->def(cedar::aux::Enum(sphere, "Sphere"));
        mShapeType.type()->def(cedar::aux::Enum(torus, "Torus"));
      }

      //! Returns the enumeration type.
      static const cedar::aux::EnumBase& type()
      {
        return *mShapeType.type();
      }

      //! Returns a pointer to an enumeration type.
      static const TypePtr& typePtr()
      {
        return mShapeType.type();
      }

    public:
      static const Id block = 0;
      static const Id cone = 1;
      static const Id cylinder = 2;
      static const Id prism = 3;
      static const Id pyramid = 4;
      static const Id sphere = 5;
      static const Id torus = 6;

    private:
      static cedar::aux::EnumType<Shape> mShapeType;
  };

public:
  //!@brief The standard constructor.
  ShapeVisualisation();

  ~ShapeVisualisation();

  cedar::proc::DataSlot::VALIDITY determineInputValidity
                                  (
                                    cedar::proc::ConstDataSlotPtr slot,
                                    cedar::aux::ConstDataPtr data
                                  ) const;

private slots:
  void visualisationChanged();

private:
  void compute(const cedar::proc::Arguments&);
  void inputConnectionChanged(const std::string& inputName);

protected:
private:
  cedar::aux::ConstMatDataPtr mpTargetPosition;

  cedar::aux::gl::ObjectVisualizationPtr mVisualisationPtr;

  //! Visualisation colour
  cedar::aux::EnumParameterPtr _mColour;

  //! Visualisation shape
  cedar::aux::EnumParameterPtr _mShape;

  //! Visualisation shape scale
  cedar::aux::DoubleParameterPtr _mScale;

  const std::string mInputName;
}; // class cedar::proc::steps::ShapeVisualisation

#endif // CEDAR_PROC_STEPS_SHAPE_VISUALISATION_H

