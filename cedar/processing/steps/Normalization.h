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

    File:        Normalization.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 10 18

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_NORMALIZATION_H
#define CEDAR_PROC_STEPS_NORMALIZATION_H

// CEDAR INCLUDES
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/EnumParameter.h"
#include "cedar/auxiliaries/BoolVectorParameter.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/MatData.fwd.h"
#include "cedar/processing/steps/Normalization.fwd.h"

// SYSTEM INCLUDES


/*!@brief A step that normalizes its input according to a user-selected method.
 */
class cedar::proc::steps::Normalization : public cedar::proc::Step
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Normalization();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void compute(const cedar::proc::Arguments&);

  void inputConnectionChanged(const std::string& inputName);

  unsigned int getNumberOfNormalizedDimensions() const;

  void normalizeAlongOneDimension(int dimension);

  void normalizeAlongAllDimensions();

  // Returns 1/norm and makes sure the result is neither inf nor 0.
  double safeNormInverse(double norm) const;

private slots:
  void recompute();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // inputs
  //! The input matrix
  cedar::aux::ConstMatDataPtr mImage;

  // outputs
  //! The normalized input.
  cedar::aux::MatDataPtr mNormalizedImage;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //! The normalization type.
  cedar::aux::EnumParameterPtr _mNormalizationType;

  //! Dimensions along which normalization takes place.
  cedar::aux::BoolVectorParameterPtr _mNormalizedDimensions;

}; // class cedar::proc::steps::Normalization

#endif // CEDAR_PROC_STEPS_NORMALIZATION_H
