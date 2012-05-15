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

    File:        PlotDeclaration.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 02 23

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_PLOT_DECLARATION_H
#define CEDAR_AUX_GUI_PLOT_DECLARATION_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/namespace.h"
#include "cedar/auxiliaries/Singleton.h"
#include "cedar/auxiliaries/TypeHierarchyMap.h"
#include "cedar/auxiliaries/Data.h"

// SYSTEM INCLUDES
#include <boost/enable_shared_from_this.hpp>


/*!@todo describe.
 *
 * @todo describe more.
 */
class cedar::aux::gui::PlotDeclaration : public boost::enable_shared_from_this<cedar::aux::gui::PlotDeclaration>
{
  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::aux::gui::PlotManager;

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.

  //!@brief Destructor

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief returns the plot class name
  virtual std::string getPlotClass() const = 0;

  //!@brief allocates a new plot
  virtual cedar::aux::gui::PlotInterface* createPlot() = 0;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  virtual void declare() = 0;

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  // none yet
}; // class cedar::aux::gui::PlotDeclaration

/*!@todo describe.
 *
 * @todo describe more.
 */
template <class DataType, class PlotType>
class cedar::aux::gui::PlotDeclarationTemplate : public cedar::aux::gui::PlotDeclaration
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.

  //!@brief Destructor

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  std::string getPlotClass() const
  {
    return cedar::aux::typeToString<PlotType>();
  }

  //!@brief allocates a new plot of the template type
  cedar::aux::gui::PlotInterface* createPlot()
  {
    return new PlotType();
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
  void declare()
  {
    try
    {
      std::vector<cedar::aux::gui::PlotDeclarationPtr>& declarations =
        cedar::aux::gui::PlotDeclarationManagerSingleton::getInstance()->find<DataType>()->getData();
      declarations.push_back(this->shared_from_this());
    }
    catch(cedar::aux::UnknownTypeException&)
    {
      std::vector<cedar::aux::gui::PlotDeclarationPtr> declarations;
      declarations.push_back(this->shared_from_this());
      cedar::aux::gui::PlotDeclarationManagerSingleton::getInstance()->insert<DataType>(declarations);
    }
  }

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  // none yet

};

#endif // CEDAR_AUX_GUI_PLOT_DECLARATION_H
