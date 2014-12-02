/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    Maintainer:  jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        2014 10 21

    Description: Header file for the class cedar::proc::steps::ObjectVisualization.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_OBJECT_VISUALIZATION_H
#define CEDAR_PROC_STEPS_OBJECT_VISUALIZATION_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/Step.h"
#include "cedar/processing/ExternalData.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/ObjectParameterTemplate.h"
#include "cedar/auxiliaries/ObjectListParameterTemplate.h"
#include "cedar/auxiliaries/gl/ObjectVisualization.h"

// FORWARD DECLARATIONS
#include "cedar/processing/steps/ObjectVisualization.fwd.h"

// SYSTEM INCLUDES


/*!@todo describe.
 *
 * @todo describe more.
 */
class cedar::proc::steps::ObjectVisualization : public cedar::proc::Step
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
  //!@brief a parameter for the object visualizations
  typedef cedar::aux::ObjectParameterTemplate<cedar::aux::gl::ObjectVisualization> ObjectVisualizationParameter;

  //!@cond SKIPPED_DOCUMENTATION
  CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(ObjectVisualizationParameter);
  //!@endcond


  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ObjectVisualization();

  //!@brief Destructor
  virtual ~ObjectVisualization();

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
  //!@brief Updates the output matrix.
  void compute(const cedar::proc::Arguments& arguments);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //!@brief The input slot containing all the terms.
  cedar::proc::ExternalDataPtr mInputs;

  //!@brief The data containing the output.
  cedar::aux::MatDataPtr mOutput;

  //!@brief: object visualizations we are representing
  ObjectVisualizationParameterPtr _mVisualizedObject;

}; // class cedar::proc::steps::ObjectVisualization

#endif // CEDAR_PROC_STEPS_OBJECT_VISUALIZATION_H

