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

    File:        PlotManager.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 03 26

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/PlotManager.h"
#include "cedar/auxiliaries/gui/PlotDeclaration.h"
#include "cedar/auxiliaries/Log.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::PlotManager::getPlotClassesFor
     (
       cedar::aux::ConstDataPtr data,
       std::set<cedar::aux::gui::PlotDeclarationPtr>& declarations
     )
{
  typedef cedar::aux::gui::PlotDeclarationManager::Node PlotNode;
  typedef cedar::aux::gui::PlotDeclarationManager::ConstNodePtr ConstPlotNodePtr;

  // find all base classes of the data type
  std::set<ConstPlotNodePtr> bases;
  cedar::aux::gui::PlotDeclarationManagerSingleton::getInstance()->findBases(data, bases);

  std::set<std::string> plots_already_added;
  for(std::set<ConstPlotNodePtr>::iterator iter = bases.begin(); iter != bases.end(); ++iter)
  {
    ConstPlotNodePtr node = *iter;
    const std::vector<cedar::aux::gui::PlotDeclarationPtr>& node_declarations = node->getData();
    for (size_t i = 0; i < node_declarations.size(); ++i)
    {
      cedar::aux::gui::PlotDeclarationPtr declaration = node_declarations.at(i);
      if (plots_already_added.find(declaration->getPlotClass()) == plots_already_added.end())
      {
        plots_already_added.insert(declaration->getPlotClass());
        declarations.insert(declaration);
      }
    }
  }
}

void cedar::aux::gui::PlotManager::declare(cedar::aux::gui::PlotDeclarationPtr declaration)
{
  declaration->declare();

  mPlotTypeDeclarations[getNormalizedTypeName(declaration->getPlotClass())] = declaration;
}

cedar::aux::gui::PlotDeclarationPtr cedar::aux::gui::PlotManager::getDefaultDeclarationFor
                                    (
                                      cedar::aux::ConstDataPtr data
                                    )
{
  //!@todo Add a configurable option that specifies a default

  // check if a default is defined for the given type
  std::string data_type = cedar::aux::objectTypeToString(data);
  cedar::aux::gui::PlotManager::normalizeTypeName(data_type);
  std::map<std::string, std::string>::const_iterator iter = this->mDefaultPlots.find(data_type);
  if (iter != this->mDefaultPlots.end())
  {
    const std::string& default_plot_type = iter->second;
    std::map<std::string, cedar::aux::gui::PlotDeclarationPtr>::iterator decl_iter;
    decl_iter = this->mPlotTypeDeclarations.find(default_plot_type);
    if (decl_iter != this->mPlotTypeDeclarations.end())
    {
      return decl_iter->second;
    }
    else
    {
      cedar::aux::LogSingleton::getInstance()->warning
      (
        "A default plot type was specified for \"" + data_type
           + "\", but its declaration could not be found. The plot type is: \"" + default_plot_type + "\".",
        "cedar::aux::gui::PlotManager::getDefaultDeclarationFor"
      );
    }
  }

  // automatically determine a default plot (the first one in the vector)
  const std::vector<cedar::aux::gui::PlotDeclarationPtr>& decls
    = cedar::aux::gui::PlotDeclarationManagerSingleton::getInstance()->getClosest(data);

  return decls.at(0);
}
