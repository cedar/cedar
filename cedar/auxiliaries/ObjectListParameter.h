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

    File:        ObjectListParameter.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 03 09

    Description: Parameter for a list of arbitrary length of dynamically allocatable, configurable objects. Ha!

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_OBJECT_LIST_PARAMETER_H
#define CEDAR_AUX_OBJECT_LIST_PARAMETER_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/Parameter.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/ObjectListParameter.fwd.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/signals2.hpp>
#endif
#include <vector>


/*!@brief Base class for a parameter that reads a list of configurable objects from a file.
 */
class cedar::aux::ObjectListParameter : public cedar::aux::Parameter
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ObjectListParameter(cedar::aux::Configurable *pOwner, const std::string& name, bool hasDefault);

  ~ObjectListParameter();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  //!@brief return the size of the vector
  virtual size_t size() const = 0;

  //!@brief clear vector
  virtual void clear() = 0;

  /*!@brief Fills a vector with the types that can be added to this object list
   */
  virtual void listTypes(std::vector<std::string>& types) const = 0;

  /*!@brief Returns the type id of the parameter with the given index.
   */
  virtual const std::string& getTypeOfObject(cedar::aux::ConfigurablePtr object) const = 0;

  /*!@brief Adds an instance of the given type to the end of the object list.
   */
  virtual void pushBack(const std::string& typeId) = 0;

  /*!@brief Removes the object at the given index.
   */
  virtual void removeObject(size_t index) = 0;

  /*!@brief Connects to the object added signal.
   */
  inline boost::signals2::connection connectToObjectAddedSignal(boost::function<void(int)> slot)
  {
    return this->mObjectAdded.connect(slot);
  }

  /*!@brief Connects to the object removed signal.
   */
  inline boost::signals2::connection connectToObjectRemovedSignal(boost::function<void(int)> slot)
  {
    return this->mObjectRemoved.connect(slot);
  }

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
protected:
  //! Signal that is sent when a new object is added to the parameter.
  boost::signals2::signal<void (int)> mObjectAdded;

  //! Signal that is sent when an object is removed.
  boost::signals2::signal<void (int)> mObjectRemoved;

private:
  // none yet

}; // class cedar::aux::ObjectListParameter

#endif // CEDAR_AUX_OBJECT_LIST_PARAMETER_H

