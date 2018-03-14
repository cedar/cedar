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

    File:        ArchitectureWidgetList.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 09 03

    Description: Header file for the class cedar::proc::gui::ArchitectureWidgetList.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_ARCHITECTURE_WIDGET_LIST_H
#define CEDAR_PROC_GUI_ARCHITECTURE_WIDGET_LIST_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/ui_ArchitectureWidgetEditor.h"

// FORWARD DECLARATIONS
#include "cedar/processing/gui/ArchitectureWidgetList.fwd.h"
#include "cedar/processing/gui/Group.fwd.h"
#include "cedar/auxiliaries/Path.fwd.h"
#include "cedar/auxiliaries/NamedConfigurable.h"
#include "cedar/auxiliaries/FileParameter.h"

// SYSTEM INCLUDES
#include <QDialog>

namespace cedar
{
	namespace proc
	{
		namespace gui
		{
			namespace detail
			{
				/*!@brief A widget for script control.
				*/
				class ArchitectureWidgetInfo : public cedar::aux::NamedConfigurable
				{
					//Q_OBJECT

				public:
					//!@brief constructor
					ArchitectureWidgetInfo(std::string name);

					cedar::aux::Path getArchitectureWidgetPath();
					void setArchitectureWidgetPath(cedar::aux::Path);

				private:
					cedar::aux::FileParameterPtr mpFileParameter;
				};
			}
		}
	}
}




/*!@brief A dialog for editing the architecture widgets in a group.
 */
class cedar::proc::gui::ArchitectureWidgetList : public QDialog, public Ui_ArchitectureWidgetEditor
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ArchitectureWidgetList(QWidget* pParent, cedar::proc::gui::GroupPtr group);

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
//  void appendRow(const std::string& name, const cedar::aux::Path& path);

  void addArchitectureWidgetInfo(const std::string& name,const cedar::aux::Path& path = "default.json");

  void showItemProperties(QTableWidgetItem* pItem);

  boost::shared_ptr<cedar::proc::gui::detail::ArchitectureWidgetInfo> getWidgetInfoFromItem(QTableWidgetItem* pItem) const;

private slots:
  void dialogAccepted();

  void addRowClicked();

  void removeRowClicked();

  void architectureWidgetNameChanged();

  void itemSelected();

	void checkCurrentName(const QString& text);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  cedar::proc::gui::GroupPtr mGroup;

  std::map<cedar::aux::Parameter*, std::string> mUsedParameterNames;

  std::map<std::string, boost::shared_ptr<cedar::proc::gui::detail::ArchitectureWidgetInfo>> mArchitectureInfos;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::gui::ArchitectureWidgetList

#endif // CEDAR_PROC_GUI_ARCHITECTURE_WIDGET_LIST_H

