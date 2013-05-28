/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        Configurable.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 01 16

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/Configurable.h"
#include "cedar/auxiliaries/gui/Parameter.h"
#include "cedar/auxiliaries/TypeBasedFactory.h"
#include "cedar/auxiliaries/utilities.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/Parameter.h"

// SYSTEM INCLUDES
#include <QVBoxLayout>
#include <QHeaderView>
#include <QAbstractItemModel>
#include <QStyledItemDelegate>
#include <QList>
#include <QLabel>

//----------------------------------------------------------------------------------------------------------------------
// delegate that tells the QTreeWidget what widget to open for editing parameters
//----------------------------------------------------------------------------------------------------------------------

class DataDelegate : public QStyledItemDelegate
{
  public:
    DataDelegate(cedar::aux::ConfigurablePtr pConfigurable)
    :
    mpConfigurable(pConfigurable)
    {
    }

    QWidget *createEditor
             (
               QWidget *pParent,
               const QStyleOptionViewItem& /*option*/,
               const QModelIndex &index
             )
             const
    {
      // Get the name of the parameter for which to return the edit widget.
      std::string parameter_path = index.data(Qt::UserRole).toString().toStdString();

      cedar::aux::ParameterPtr parameter = this->mpConfigurable->getParameter(parameter_path);

      QWidget* p_ret = NULL;
      try
      {
        cedar::aux::gui::Parameter *p_widget
          = cedar::aux::gui::ParameterFactorySingleton::getInstance()->get(parameter)->allocateRaw();
        p_widget->setParameter(parameter);
        p_ret = p_widget;
      }
      catch (cedar::aux::UnknownTypeException& e)
      {
        QLabel* p_no_widget_label = new QLabel("(No widget for type)");
        p_no_widget_label->setToolTip(QString::fromStdString(e.exceptionInfo()));
        p_no_widget_label->setAlignment(Qt::AlignCenter);
        p_no_widget_label->setAutoFillBackground(true);
        p_no_widget_label->setStyleSheet("background-color: rgb(255, 200, 137)");
        p_ret = p_no_widget_label;
      }

      p_ret->setParent(pParent);

      return p_ret;
    }

  private:
    cedar::aux::ConfigurablePtr mpConfigurable;
};

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::Configurable::Configurable()
{
  // create layout
  QVBoxLayout* p_layout = new QVBoxLayout();
  this->setLayout(p_layout);

  // insert property tree
  this->mpPropertyTree = new QTreeWidget();
  p_layout->addWidget(mpPropertyTree, 1);
  p_layout->setContentsMargins(0, 0, 0, 0);
  this->mpPropertyTree->setAlternatingRowColors(true);

  // setup header
  this->mpPropertyTree->setColumnCount(2);
  QStringList header_labels;
  header_labels << "Property" << "Value";
  this->mpPropertyTree->setHeaderLabels(header_labels);
}


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::Configurable::display(cedar::aux::ConfigurablePtr configurable)
{
  this->clear();
  
  this->mpPropertyTree->setItemDelegateForColumn(1, new DataDelegate(configurable));

  std::string type_name = cedar::aux::objectTypeToString(configurable);
  QStringList text;
  text << QString::fromStdString(type_name);
  QTreeWidgetItem* p_item = new QTreeWidgetItem(text);
  this->mpPropertyTree->addTopLevelItem(p_item);
  this->mpPropertyTree->setFirstItemColumnSpanned(p_item, true);

  this->append(configurable, p_item, std::string());
}

void cedar::aux::gui::Configurable::append(cedar::aux::ConfigurablePtr configurable, QTreeWidgetItem* pItem, const std::string& pathSoFar)
{
  auto parameters = configurable->getParameters();
  for (auto param_iter = parameters.begin(); param_iter != parameters.end(); ++param_iter)
  {
    cedar::aux::ParameterPtr parameter = *param_iter;

    this->append(parameter, pItem, pathSoFar);
  }

  auto children = configurable->configurableChildren();
  for (auto child_iter = children.begin(); child_iter != children.end(); ++child_iter)
  {
    const std::string& child_name = child_iter->first;
    cedar::aux::ConfigurablePtr child = child_iter->second;

    std::string path = pathSoFar;
    if (!path.empty())
    {
      path += ".";
    }
    path += child_name;

    QTreeWidgetItem* child_item = new QTreeWidgetItem();
    child_item->setText(0, QString::fromStdString(child_name));
    pItem->addChild(child_item);
    this->mpPropertyTree->setFirstItemColumnSpanned(child_item, true);
    this->append(child, child_item, path);
  }
}

void cedar::aux::gui::Configurable::append(cedar::aux::ParameterPtr parameter, QTreeWidgetItem* pNode, const std::string& pathSoFar)
{
  std::string path = pathSoFar;
  if (!pathSoFar.empty())
  {
    path += ".";
  }
  path += parameter->getName();

  QTreeWidgetItem* parameter_item = new QTreeWidgetItem();
  parameter_item->setText(0, QString::fromStdString(parameter->getName()));
  parameter_item->setData(1, Qt::UserRole, QString::fromStdString(path));
  pNode->addChild(parameter_item);
  this->mpPropertyTree->openPersistentEditor(parameter_item, 1);
}

void cedar::aux::gui::Configurable::clear()
{
  QAbstractItemDelegate * p_delegate = this->mpPropertyTree->itemDelegateForColumn(1);
  if (p_delegate != NULL)
  {
    delete p_delegate;
  }
}
