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
#include "cedar/auxiliaries/gui/PlotManager.h"
#include "cedar/auxiliaries/Singleton.h"
#include "cedar/auxiliaries/TypeHierarchyMap.h"
#include "cedar/auxiliaries/Data.h"
#include "cedar/auxiliaries/PluginDeclaration.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/gui/PlotInterface.fwd.h"
#include "cedar/auxiliaries/gui/PlotDeclaration.fwd.h"
#include "cedar/auxiliaries/gui/PlotDeclarationManager.fwd.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/enable_shared_from_this.hpp>
#endif
#include <vector>
#include <string>


/*!@brief A class used for declaring a plot to the plot manager.
 */
class cedar::aux::gui::PlotDeclaration : public cedar::aux::PluginDeclaration,
                                         public boost::enable_shared_from_this<cedar::aux::gui::PlotDeclaration>
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
  virtual std::string getClassName() const = 0;

  //!@brief allocates a new plot
  virtual cedar::aux::gui::PlotInterface* createPlot() const = 0;

  std::string getPluginType() const
  {
    return "plot";
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
  // none yet
private:
  // none yet
}; // class cedar::aux::gui::PlotDeclaration

/*!@brief A templated class used for declaring a plot to the plot manager, provided for convenience.
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
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Returns the class of the plot as a string.
  std::string getClassName() const
  {
    return cedar::aux::typeToString<PlotType>();
  }

  //!@brief allocates a new plot of the template type
  cedar::aux::gui::PlotInterface* createPlot() const
  {
    return new PlotType();
  }

  void declare() const
  {
    if (cedar::aux::gui::PlotDeclarationManagerSingleton::getInstance()->canFind<DataType>())
    {
      auto& declarations = cedar::aux::gui::PlotDeclarationManagerSingleton::getInstance()->find<DataType>()->getData();
      declarations.push_back(this->shared_from_this());
    }
    else
    {
      std::vector<cedar::aux::gui::ConstPlotDeclarationPtr> declarations;
      declarations.push_back(this->shared_from_this());
      cedar::aux::gui::PlotDeclarationManagerSingleton::getInstance()->insert<DataType>(declarations);
    }
    cedar::aux::gui::PlotManagerSingleton::getInstance()->declare(this->shared_from_this());
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
  // none yet
private:
  // none yet

};

#endif // CEDAR_AUX_GUI_PLOT_DECLARATION_H
