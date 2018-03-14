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

    File:        ArchitectureWidgetList.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 09 03

    Description: Source file for the class cedar::proc::gui::ArchitectureWidgetList.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/ArchitectureWidgetList.h"
#include "cedar/processing/gui/Group.h"

// SYSTEM INCLUDES
#include <map>
#include <set>
#include <string>

cedar::proc::gui::detail::ArchitectureWidgetInfo::ArchitectureWidgetInfo(std::string name)
	:
	mpFileParameter(new cedar::aux::FileParameter(this, "widget path", cedar::aux::FileParameter::READ))
{
  mpFileParameter->setPathMode(cedar::aux::FileParameter::PATH_MODE_RELATIVE_TO_CURRENT_ARCHITECTURE_DIR);
  mpFileParameter->setPathModeConstant(true);
	this->setName(name);
}

cedar::aux::Path cedar::proc::gui::detail::ArchitectureWidgetInfo::getArchitectureWidgetPath()
{
	cedar::aux::Path path = this->mpFileParameter->getPath();
	return path;
}

void cedar::proc::gui::detail::ArchitectureWidgetInfo::setArchitectureWidgetPath(cedar::aux::Path path)
{
	this->mpFileParameter->setValue(path);
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::ArchitectureWidgetList::ArchitectureWidgetList(QWidget* pParent, cedar::proc::gui::GroupPtr group)
:
QDialog(pParent),
mGroup(group)
{
  this->setupUi(this);

//  mpSplitter->setStretchFactor(0, 0);
//  mpSplitter->setStretchFactor(1,1);
//  mpTable->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Preferred));
    QList<int> sizeList({150, 410});
    mpSplitter->setSizes(sizeList);


  QObject::connect(this->mpAddButton, SIGNAL(clicked()), this, SLOT(addRowClicked()));
  QObject::connect(this->mpRemoveButton, SIGNAL(clicked()), this, SLOT(removeRowClicked()));
  QObject::connect(this->mpOkButton, SIGNAL(clicked()), this, SLOT(dialogAccepted()));
  QObject::connect(this->mpLineEdit, SIGNAL(textChanged(const QString & )), this, SLOT(checkCurrentName(const QString& )));
  

  const auto& plots = this->mGroup->getArchitectureWidgets();
  for (const auto& name_path_pair : plots)
  {
    const auto& name = name_path_pair.first;
    const auto& path = name_path_pair.second;

    this->addArchitectureWidgetInfo(name, path);
  }

  QObject::connect(this->mpTable, SIGNAL(itemSelectionChanged()), this, SLOT(itemSelected()));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

//void cedar::proc::gui::ArchitectureWidgetList::appendRow(const std::string& name, const cedar::aux::Path& path)
//{
//  int row = this->mpTable->rowCount();
//  this->mpTable->setRowCount(row + 1);
//
//  auto name_item = new QTableWidgetItem(QString::fromStdString(name));
//  auto path_item = new QTableWidgetItem(QString::fromStdString(path.toString(false)));
//
//  this->mpTable->setItem(row, 0, name_item);
//  //this->mpTable->setItem(row, 1, path_item);
//}

void cedar::proc::gui::ArchitectureWidgetList::addArchitectureWidgetInfo(const std::string& name, const cedar::aux::Path& path)
{
	auto architectureWidgetInfo = boost::shared_ptr<cedar::proc::gui::detail::ArchitectureWidgetInfo>(new cedar::proc::gui::detail::ArchitectureWidgetInfo(name));
//  std::cout<<"addArchitectureWidgetInfo! Set Path: " << path << std::endl;
	architectureWidgetInfo->setArchitectureWidgetPath(path);
//  std::cout<<"addArchitectureWidgetInfo! Get Path afterwards: " << architectureWidgetInfo->getArchitectureWidgetPath() << std::endl;

	mArchitectureInfos[name] = architectureWidgetInfo;

	int row = this->mpTable->rowCount();
	this->mpTable->setRowCount(row + 1);

	this->mpTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(name)));

	auto name_parameter = architectureWidgetInfo->getParameter("name");
	this->mUsedParameterNames[name_parameter.get()] = architectureWidgetInfo->getName();
	QObject::connect(name_parameter.get(), SIGNAL(valueChanged()), this, SLOT(architectureWidgetNameChanged()));
	//QObject::connect(script.get(), SIGNAL(statusChanged(QString)), this, SLOT(scriptStatusChanged(QString)));
}

void cedar::proc::gui::ArchitectureWidgetList::architectureWidgetNameChanged()
{
	auto name_parameter = dynamic_cast<cedar::aux::StringParameter*>(QObject::sender());
	CEDAR_DEBUG_ASSERT(name_parameter);
	auto iter = this->mUsedParameterNames.find(name_parameter);
	CEDAR_DEBUG_ASSERT(iter != this->mUsedParameterNames.end());
	std::string old_name = iter->second;
	std::string new_name = name_parameter->getValue();
	auto items = this->mpTable->findItems(QString::fromStdString(old_name), Qt::MatchExactly);

	for (auto item : items)
	{
		if (item->column() == 0)
		{
			item->setText(QString::fromStdString(new_name));
		}
	}

  //Update the ArchitectureInfoMap
  auto searchIter = this->mArchitectureInfos.find(old_name);
  if(searchIter!=this->mArchitectureInfos.end())
  {
    auto widgetInfo = searchIter->second;
    this->mArchitectureInfos[new_name] =widgetInfo;
    this->mArchitectureInfos.erase(old_name);
  }

	this->mUsedParameterNames[name_parameter] = new_name;
}

void cedar::proc::gui::ArchitectureWidgetList::itemSelected()
{
	auto selected_ranges = this->mpTable->selectedRanges();
	if (selected_ranges.count() == 1 && selected_ranges[0].rowCount() == 1)
	{
		int row = selected_ranges[0].topRow();
		auto item = this->mpTable->item(row, 0);
		this->showItemProperties(item);
	}
}


void cedar::proc::gui::ArchitectureWidgetList::showItemProperties(QTableWidgetItem* pItem)
{
	auto widgetInfo = this->getWidgetInfoFromItem(pItem);
	this->mpConfigurationEditor->display(widgetInfo);
}

boost::shared_ptr<cedar::proc::gui::detail::ArchitectureWidgetInfo> cedar::proc::gui::ArchitectureWidgetList::getWidgetInfoFromItem(QTableWidgetItem* pItem) const
{
	std::string name = pItem->text().toStdString();
	auto iter = this->mArchitectureInfos.find(name);
	CEDAR_DEBUG_ASSERT(iter != this->mArchitectureInfos.end());

	return iter->second;
}

void cedar::proc::gui::ArchitectureWidgetList::addRowClicked()
{
  std::string name = this->mpLineEdit->text().toStdString();

  auto iter = this->mArchitectureInfos.find(name);
  if (iter == this->mArchitectureInfos.end() && name != "")
  {
	  this->addArchitectureWidgetInfo(name);
    this->mpLineEdit->setText("");
    this->mpLineEdit->clearFocus();
    this->mpAddButton->clearFocus();
  }
}

void cedar::proc::gui::ArchitectureWidgetList::removeRowClicked()
{
  std::set<int> rows_to_delete;
  auto selected_items = this->mpTable->selectedItems();

  for (auto item : selected_items)
  {
    rows_to_delete.insert(item->row());
  }

  // delete in reverse so that we don't need to care about row indices moving around as a consequence of deleting things
  for (auto row_it = rows_to_delete.rbegin(); row_it != rows_to_delete.rend(); ++row_it)
  {
    int row = *row_it;
	auto delName = this->mpTable->item(row, 0)->text().toStdString();
	this->mArchitectureInfos.erase(delName);
    this->mpTable->removeRow(row);
  }
}

void cedar::proc::gui::ArchitectureWidgetList::dialogAccepted()
{

  std::map<std::string, cedar::aux::Path> new_widgets;

  for (auto widget_iter = mArchitectureInfos.begin(); widget_iter != mArchitectureInfos.end(); ++widget_iter)
  {
	  auto widgetInfo = widget_iter->second;
    std::string name = widgetInfo->getName();
    cedar::aux::Path path = widgetInfo->getArchitectureWidgetPath();
    if (!name.empty())
    {
      new_widgets[name] = path;
    }
  }

  this->mGroup->setArchitectureWidgets(new_widgets);
}

void cedar::proc::gui::ArchitectureWidgetList::checkCurrentName(const QString& text)
{

  auto iter = this->mArchitectureInfos.find(text.toStdString());
  if (iter != this->mArchitectureInfos.end())
  {
    this->mpLineEdit->setStyleSheet("QLineEdit { background: rgb(255, 0, 0); }");
  }
  else
  {
    this->mpLineEdit->setStyleSheet("QLineEdit { background: rgb(255, 255, 255); }");
  }

}
