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

    File:        ElementClassList.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 05

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/gui/ElementClassList.h"
#include "cedar/processing/ElementDeclaration.h"

// SYSTEM INCLUDES
#include <QApplication>
#include <QResource>


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::ElementClassList::ElementClassList(QWidget *pParent)
:
QListWidget(pParent)
{
  this->setViewMode(QListView::IconMode);
  this->setMovement(QListView::Static);
  this->setResizeMode(QListView::Adjust);
  this->setDragEnabled(true);
  this->setIconSize(QSize(40, 40));
}

cedar::proc::gui::ElementClassList::~ElementClassList()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::ElementClassList::showList(const cedar::proc::DeclarationRegistry::CategoryEntries& entries)
{
  using cedar::proc::Manager;
  using cedar::proc::ElementDeclarationPtr;

  this->clear();

  for (cedar::proc::DeclarationRegistry::CategoryEntries::const_iterator iter = entries.begin();
       iter != entries.end();
       ++iter
      )
  {
    const ElementDeclarationPtr& class_id = *iter;
    QString label = class_id->getClassName().c_str();
    QListWidgetItem *p_item = new QListWidgetItem(label);
    p_item->setFlags(p_item->flags() | Qt::ItemIsDragEnabled);

    QIcon icon;
    if (!class_id->getIconPath().empty())
    {
      QResource ex_test(QString::fromStdString(class_id->getIconPath()));
      if (ex_test.isValid())
      {
        icon = QIcon(class_id->getIconPath().c_str());
      }
      else
      {
        icon = QIcon(":/steps/broken_icon.svg");
      }
    }
    else
    {
      icon = QIcon(":/steps/no_icon.svg");
    }
    p_item->setIcon(icon);

    QString class_description;
    class_description += "<nobr>class <big><b>" + QString::fromStdString(class_id->getClassName()) + "</b></big></nobr>";

    if (class_id->isDeprecated())
    {
      class_description += "<br /><div align=\"right\"><b>DEPRECATED</b></div>";
      p_item->setBackground(QApplication::palette().brush(QPalette::Dark));
    }

    class_description += "<hr />";
    class_description += "<div align=\"right\"><nobr><small><i>" + QString::fromStdString(class_id->getClassId()) + "</i></small></nobr></div>";
    if (!class_id->getDescription().empty())
    {
      QString description = "<p>" + QString::fromStdString(class_id->getDescription()).replace("\n", "<br />") + "</p>";
      class_description += description;
    }


    if (class_id->isDeprecated() && !class_id->getDeprecationDescription().empty())
    {
      class_description += "<br /><b>This class is deprecated:</b> ";
      class_description += QString::fromStdString(class_id->getDeprecationDescription());
    }

    p_item->setToolTip(class_description);

    p_item->setData(Qt::UserRole, QVariant(class_id->getClassId().c_str()));
    this->addItem(p_item);
  }
}
