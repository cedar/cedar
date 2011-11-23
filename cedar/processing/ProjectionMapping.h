/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        ProjectionMapping.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2011 11 18

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_PROJECTION_MAP_H
#define CEDAR_PROC_PROJECTION_MAP_H

// LOCAL INCLUDES
#include "cedar/processing/namespace.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <map>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::proc::ProjectionMapping
{
  typedef std::map<unsigned int, unsigned int>::iterator iterator;
  typedef std::map<unsigned int, unsigned int>::const_iterator const_iterator;

  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ProjectionMapping();

  //!@brief Destructor
  ~ProjectionMapping();

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
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  void addMapping(unsigned int inputIndex, unsigned int outputIndex);
  void dropDimension(unsigned int inputIndex);
  void removeMapping(unsigned int inputIndex);
  unsigned int lookUp(unsigned int inputIndex);
  unsigned int getNumberOfMappings();
  bool isDropped(unsigned int inputIndex);
  void clear();
  bool mappingExists(unsigned int inputIndex);
  cedar::proc::ProjectionMapping::iterator begin();
  cedar::proc::ProjectionMapping::const_iterator begin() const;
  cedar::proc::ProjectionMapping::iterator end();
  cedar::proc::ProjectionMapping::const_iterator end() const;

protected:
  // none yet
private:
  static unsigned int msDropIndex;
  bool isMappingBijective(unsigned int inputIndex, unsigned int outputIndex);

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet
private:
  std::map<unsigned int, unsigned int> mMapping;

}; // class cedar::proc::ProjectionMapping

#endif // CEDAR_PROC_PROJECTION_MAP_H

