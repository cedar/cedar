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

    File:        SpatialTemplate.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2013 10 30

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_SOURCES_SPATIAL_TEMPLATE_H
#define CEDAR_PROC_SOURCES_SPATIAL_TEMPLATE_H

// CEDAR INCLUDES
#include <cedar/processing/sources/SpatialTemplate.fwd.h>
#include <cedar/processing/Step.h>
#include <cedar/auxiliaries/ObjectParameterTemplate.fwd.h>
#include <cedar/auxiliaries/math/TransferFunction.h>
#include <cedar/auxiliaries/UIntParameter.fwd.h>
#include <cedar/auxiliaries/DoubleParameter.fwd.h>
#include <cedar/auxiliaries/MatData.fwd.h>

// PROJECT INCLUDES

// SYSTEM INCLUDES


/*!@brief A processing step that generates spatial patterns for "left", "right", "above", and "below".
 */
class cedar::proc::sources::SpatialTemplate : public cedar::proc::Step
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
  typedef cedar::aux::ObjectParameterTemplate<cedar::aux::math::TransferFunction> SigmoidParameter;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  SpatialTemplate();

  //!@brief Destructor
  ~SpatialTemplate();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void compute(const cedar::proc::Arguments&);

public slots:
  void recompute();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // inputs
  // none

  // outputs
  cedar::aux::MatDataPtr mPattern;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
private:
  cedar::aux::UIntParameterPtr _mSizeX;
  cedar::aux::UIntParameterPtr _mSizeY;
  cedar::aux::DoubleParameterPtr _mMuTh;
  cedar::aux::DoubleParameterPtr _mSigmaTh;
  cedar::aux::DoubleParameterPtr _mMuR;
  cedar::aux::DoubleParameterPtr _mSigmaR;
}; // class cedar::proc::sources::SpatialTemplate

#endif // CEDAR_PROC_SOURCES_SPATIAL_TEMPLATE_H
