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
cedar::aux::gui::PlotManager::PlotManager()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::PlotManager::getPlotClassesFor
     (
       cedar::aux::ConstDataPtr data,
       std::set<cedar::aux::gui::ConstPlotDeclarationPtr>& declarations
     )
{
  typedef cedar::aux::gui::PlotDeclarationManager::ConstNodePtr ConstPlotNodePtr;

  // find all base classes of the data type
  std::set<ConstPlotNodePtr> bases;
  cedar::aux::gui::PlotDeclarationManagerSingleton::getInstance()->findBases(data, bases);

  std::set<std::string> plots_already_added;
  for(std::set<ConstPlotNodePtr>::iterator iter = bases.begin(); iter != bases.end(); ++iter)
  {
    ConstPlotNodePtr node = *iter;
    auto node_declarations = node->getData();
    for (size_t i = 0; i < node_declarations.size(); ++i)
    {
      cedar::aux::gui::ConstPlotDeclarationPtr declaration = node_declarations.at(i);
      if (plots_already_added.find(declaration->getClassName()) == plots_already_added.end())
      {
        plots_already_added.insert(declaration->getClassName());
        declarations.insert(declaration);
      }
    }
  }
}

void cedar::aux::gui::PlotManager::declare(cedar::aux::gui::ConstPlotDeclarationPtr declaration)
{
  mPlotTypeDeclarations[getNormalizedTypeName(declaration->getClassName())] = declaration;
}

cedar::aux::gui::ConstPlotDeclarationPtr cedar::aux::gui::PlotManager::getDeclaration(const std::string& plotClassName) const
{
  std::string normalized_name = getNormalizedTypeName(plotClassName);
  auto iter = this->mPlotTypeDeclarations.find(normalized_name);
  if (iter == this->mPlotTypeDeclarations.end())
  {
    CEDAR_THROW(cedar::aux::NotFoundException, "Plot declaration for \"" + plotClassName + "\" (a.k.a. \"" + normalized_name + "\") not found.");
  }
  return iter->second;
}

cedar::aux::gui::ConstPlotDeclarationPtr cedar::aux::gui::PlotManager::getDefaultDeclarationFor
                                         (
                                           cedar::aux::ConstDataPtr data
                                         )
{
  // check if a default is defined for the given type
  std::string data_type;

  std::map<std::string, std::string>::const_iterator iter;

  // first, try to find a default value for the direct type
  // (defaults may be specified for types that aren't in the hierarchy)
  data_type = cedar::aux::objectTypeToString(data);
  cedar::aux::gui::PlotManager::normalizeTypeName(data_type);
  iter = this->mDefaultPlots.find(data_type);

  // if that wasn't found, try to find one for the closest type known in the plot hierarchy
  cedar::aux::gui::PlotDeclarationManager::ConstNodePtr closest_node
    = cedar::aux::gui::PlotDeclarationManagerSingleton::getInstance()->getClosestNode(data);

  // if this assertion fails, there was no node for the given data in the plot manager; that shouldn't happen
  CEDAR_DEBUG_ASSERT(closest_node);

  if (iter == this->mDefaultPlots.end())
  {
    data_type = closest_node->getTypeString();
    cedar::aux::gui::PlotManager::normalizeTypeName(data_type);
    iter = this->mDefaultPlots.find(data_type);
  }

  // if one of the methods above yielded a result, use that
  if (iter != this->mDefaultPlots.end())
  {
    const std::string& default_plot_type = iter->second;
    auto decl_iter = this->mPlotTypeDeclarations.find(default_plot_type);
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

  // otherwise, automatically determine a default plot (the first one in the vector)
  auto decls = closest_node->getData();
  if (decls.size() == 0)
  {
    CEDAR_THROW(cedar::aux::NotFoundException, "Could not find a plot declaration for this type of data.")
  }
  return decls.at(0);
}
