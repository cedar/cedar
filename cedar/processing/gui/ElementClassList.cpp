/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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
#include "cedar/auxiliaries/PluginDeclarationTemplate.h"
#include "cedar/auxiliaries/casts.h"

// SYSTEM INCLUDES
#include <QApplication>
#include <QResource>
#include <QPainter>
#include <QPixmap>
#include <QIcon>


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

void cedar::proc::gui::ElementClassList::showList(const cedar::proc::ElementManager::BasePluginList& entries)
{
  using cedar::proc::Manager;
  using cedar::proc::ElementDeclarationPtr;

  this->clear();

  for (auto iter = entries.begin(); iter != entries.end(); ++iter)
  {
    cedar::proc::ElementManager::ConstBasePluginDeclarationPtr class_id = *iter;
    auto elem_decl = boost::dynamic_pointer_cast<cedar::proc::ConstElementDeclaration>(class_id);
    CEDAR_ASSERT(elem_decl);

    std::string only_class_name = class_id->getClassNameWithoutNamespace();
//    std::string namespace_name = class_id->getNamespaceName();

    QString label = QString::fromStdString(only_class_name);
    QListWidgetItem *p_item = new QListWidgetItem(label);
    p_item->setFlags(p_item->flags() | Qt::ItemIsDragEnabled);

    QSize icon_size = this->iconSize();
    int decoration_size = 12;

    QIcon icon = elem_decl->getIcon();
    std::vector<QString> decorations;
    if (!class_id->getSource().empty())
    {
      decorations.push_back(":/decorations/from_plugin.svg");
    }

    if (class_id->isDeprecated())
    {
      decorations.push_back(":/cedar/auxiliaries/gui/warning.svg");
    }

    QPixmap result(icon_size);
    result.fill(Qt::transparent);
    QPixmap icon_pm = icon.pixmap(icon_size);

    {
      QPainter overlayer(&result);
      overlayer.drawPixmap(0, 0, icon_pm);

      for (size_t i = 0; i < decorations.size(); ++i)
      {
        QIcon decoration(decorations.at(i));
        overlayer.drawPixmap
                  (
                    icon_size.width() - decoration_size, icon_size.height() - (i + 1) * decoration_size,
                    decoration.pixmap(decoration_size, decoration_size)
                  );
      }
    }

    p_item->setIcon(QIcon(result));

    QString class_description;
    class_description += "<nobr>class <big><b>" + QString::fromStdString(only_class_name) + "</b></big></nobr>";

    class_description += "<hr />";
    class_description += "<div align=\"right\"><nobr><small><i>" + QString::fromStdString(class_id->getClassName()) + "</i></small></nobr></div>";
    if (!elem_decl->getDescription().empty())
    {
      QString description = "<p>" + QString::fromStdString(elem_decl->getDescription()).replace("\n", "<br />") + "</p>";
      class_description += description;
    }


    if (class_id->isDeprecated() && !class_id->getDeprecationDescription().empty())
    {
      class_description += "<br /><b>This class is deprecated:</b> ";
      class_description += QString::fromStdString(class_id->getDeprecationDescription());
    }

    if (!class_id->getSource().empty())
    {
      class_description += "<br /><b>From plugin:</b> "
                           + QString::fromStdString(class_id->getSource());
    }

    p_item->setToolTip(class_description);

    p_item->setData(Qt::UserRole, QVariant(class_id->getClassName().c_str()));
    this->addItem(p_item);
  }
}
