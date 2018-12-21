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

    File:        ElementDeclaration.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 11 18

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_ELEMENT_DECLARATION_H
#define CEDAR_PROC_ELEMENT_DECLARATION_H

// CEDAR INCLUDES
#include "cedar/processing/DataRole.h"
#include "cedar/processing/PlotData.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/gui/DefaultConnectableIconView.h"
#include "cedar/processing/gui/ConnectableIconView.h"
#include "cedar/auxiliaries/utilities.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/FactoryDerived.h"
#include "cedar/auxiliaries/PluginDeclaration.h"
#include "cedar/auxiliaries/PluginDeclarationTemplate.h"

// FORWARD DECLARATIONS
#include "cedar/processing/Element.fwd.h"
#include "cedar/processing/ElementDeclaration.fwd.h"
#include "cedar/processing/ElementDeclarationTemplate.fwd.h"
#include "cedar/auxiliaries/Singleton.fwd.h"

// SYSTEM INCLUDES
#include <vector>
#include <string>


/*!@brief A StepDeclaration contains the relation of a unique class id (as string) and the corresponding factory to
 * create a step of this id. It is a concretization of DeclarationBase.
 */
class cedar::proc::ElementDeclaration : public cedar::aux::PluginDeclarationBaseTemplate<cedar::proc::ElementPtr>
{
public:
  //!@brief list that pairs a data role with the desired plot
  typedef std::vector<cedar::proc::PlotDataPtr> DataList;

  /*!@brief Structure that holds information about custom plots defined for a processing step.
   */
  struct PlotDefinition
  {
    /*! Constructor
     *
     * @param name Name of the plot to be displayed in the "defined plots" menu.
     * @param icon (Optional) path to the icon to be used for the plot in the "defined plots" menu.
     */
    PlotDefinition(const std::string& name, const std::string& icon = std::string())
    :
    mName(name),
    mIcon(icon)
    {
    }

    //! Appends data to the list of items to plot.
    void appendData(cedar::proc::DataRole::Id id, const std::string& dataName, bool ignoreIfMissing = false)
    {
      this->mData.push_back(cedar::proc::PlotDataPtr(new cedar::proc::PlotData(id, dataName, ignoreIfMissing)));
    }

    //! Name of the plot.
    std::string mName;

    //! Icon of the plot.
    std::string mIcon;

    //! Data to be plotted.
    DataList mData;
  };

  //!@brief list of plot definitions
  typedef std::vector<PlotDefinition> PlotDefinitionList;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Constructor that takes a category and an (optional) class name to use for the element.
  ElementDeclaration(const std::string& category, const std::string& className = "")
  :
  cedar::aux::PluginDeclarationBaseTemplate<cedar::proc::ElementPtr>(category, className)
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  std::string getPluginType() const
  {
    return "processing step";
  }

  //!@brief Method for setting the description of the element.
  void setDescription(const std::string& description)
  {
    this->mDescription = description;
  }

  //!@brief Returns the description of the element.
  const std::string& getDescription() const
  {
    return this->mDescription;
  }

  std::string getShortDescription() const
  {
    //This is just a heuristic that assumes that descriptions contain a simple sentence as their first line
    //TODO: Should be replaced with a proper field for a short description
    std::size_t pos = this->mDescription.find(".");// position of "." in str
    if(pos!=std::string::npos)
      return this->mDescription.substr(0,pos);
    else
      return this->mDescription;
  }

  /*!@brief Defines a new plot for this type of element
   *
   * @param plotDefinition Definition of the plot.
   */
  void definePlot(const PlotDefinition& plotDefinition)
  {
    this->mPlots.push_back(plotDefinition);
  }

  //!@brief Returns the defined plots for this declaration.
  const PlotDefinitionList& definedPlots() const
  {
    return this->mPlots;
  }

  //! Overriden in the templated version by the super class implementation.
  virtual void declare() const = 0;

  //! Set the default plot. When the plot is an empty string, "plot all" will be the default.
  void setDefaultPlot(const std::string& plotName)
  {
    this->mDefaultPlot = plotName;
  }

  //! Returns the name of the default plot. Empty if none is set.
  const std::string& getDefaultPlot() const
  {
    return this->mDefaultPlot;
  }

  //! creates the icon view of an element
  virtual cedar::proc::gui::ConnectableIconView* createIconView() const = 0;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void customRead(const cedar::aux::ConfigurationNode& node)
  {
    if (node.find("icon") != node.not_found())
    {
      this->setIconPath(node.get_child("icon").get_value<std::string>());
    }

    if (node.find("description") != node.not_found())
    {
      this->setDescription(node.get_child("description").get_value<std::string>());
    }
  }

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief path to icon included in the graphical representation of this step
  std::string mIconPath;

  //!@brief The description of the class.
  std::string mDescription;

  //!@brief Declarations of plots.
  PlotDefinitionList mPlots;

  //! Default plot to open (if empty, all data is plotted).
  std::string mDefaultPlot;
};


/*!@brief This is a template class for comfortably generating element declarations.
 *
 * @todo This class should get its own header file
 */
template <class DerivedClass, class IconView = cedar::proc::gui::DefaultConnectableIconView>
class cedar::proc::ElementDeclarationTemplate
    :
    public cedar::aux::PluginDeclarationTemplate
    <
      cedar::proc::ElementPtr,
      boost::shared_ptr<DerivedClass>,
      cedar::proc::ElementDeclaration
    >
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  typedef
      cedar::aux::PluginDeclarationTemplate<cedar::proc::ElementPtr, boost::shared_ptr<DerivedClass>, cedar::proc::ElementDeclaration>
      DeclarationSuper;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief The constructor.
   *
   * @param category Category for the element.
   * @param classId  Identifier of the class. If this is left empty, the name will be determined automatically. For
   *                 example, a class test::namespaceName::ClassName will result in the name
   *                 test.namespaceName.ClassName.
   */
  ElementDeclarationTemplate(const std::string& category, const std::string& classId = "")
  :
  DeclarationSuper
  (
    category,
    classId
  )
  {
  }

  //! creates an icon view of the element
  cedar::proc::gui::ConnectableIconView* createIconView() const
  {
    return new IconView();
  }

  void declare() const
  {
    DeclarationSuper::declare();
  }
}; // class cedar::proc::ElementDeclarationTemplate

#endif // CEDAR_PROC_ELEMENT_DECLARATION_H
