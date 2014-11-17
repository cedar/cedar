/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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
#include "cedar/processing/gui/Settings.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/GroupDeclaration.h"
#include "cedar/processing/GroupDeclarationManager.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/auxiliaries/PluginDeclarationTemplate.h"
#include "cedar/auxiliaries/casts.h"

// SYSTEM INCLUDES
#include <QContextMenuEvent>
#include <QApplication>
#include <QResource>
#include <QPainter>
#include <QPixmap>
#include <QIcon>
#include <QMenu>
#include <vector>


// the following methods make sure that a plugin declaration can be passed around via drag & drop
Q_DECLARE_METATYPE(cedar::aux::PluginDeclaration*)

QDataStream &operator<<(QDataStream &out, cedar::aux::PluginDeclaration* const &rhs)
{
  out.writeRawData(reinterpret_cast<const char*>(&rhs), sizeof(rhs));
  return out;
}

QDataStream & operator >> (QDataStream &in, cedar::aux::PluginDeclaration *&rhs)
{
  in.readRawData(reinterpret_cast<char*>(&rhs), sizeof(rhs));
  return in;
}

bool registerPluginDeclaratioMetaType()
{
  qRegisterMetaTypeStreamOperators<cedar::aux::PluginDeclaration*>("cedar::aux::PluginDeclaration*");
  return true;
}

bool metatype_registered = registerPluginDeclaratioMetaType();

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

  QObject::connect
  (
    cedar::proc::gui::SettingsSingleton::getInstance()->getElementListShowsDeprecatedParameter().get(),
    SIGNAL(valueChanged()),
    this,
    SLOT(rebuild())
  );
}

cedar::proc::gui::ElementClassList::~ElementClassList()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::ElementClassList::contextMenuEvent(QContextMenuEvent* pEvent)
{
  QMenu context_menu(this);
  auto action = context_menu.addAction("show deprecated steps");
  action->setCheckable(true);
  action->setChecked(cedar::proc::gui::SettingsSingleton::getInstance()->getElementListShowsDeprecated());
  QObject::connect(action, SIGNAL(toggled(bool)), this, SLOT(showDeprecatedSteps(bool)));

  context_menu.exec(pEvent->globalPos());
}

void cedar::proc::gui::ElementClassList::showDeprecatedSteps(bool show)
{
  cedar::proc::gui::SettingsSingleton::getInstance()->setElementListShowsDeprecated(show);
}

void cedar::proc::gui::ElementClassList::rebuild()
{
  this->showList(this->mCategoryName);
}

void cedar::proc::gui::ElementClassList::showList(const std::string& categoryName)
{
  using cedar::proc::ElementDeclarationPtr;
  using cedar::proc::GroupDeclarationPtr;

  this->mCategoryName = categoryName;
  auto entries = ElementManagerSingleton::getInstance()->getCategoryEntries(this->mCategoryName);

  auto group_entries = cedar::proc::GroupDeclarationManagerSingleton::getInstance()->getDefinitions();

  this->clear();

  for (auto iter = entries.begin(); iter != entries.end(); ++iter)
  {
    cedar::proc::ElementDeclarationManager::ConstBasePluginDeclarationPtr class_id = *iter;
    auto elem_decl = boost::dynamic_pointer_cast<cedar::proc::ConstElementDeclaration>(class_id);
    CEDAR_ASSERT(elem_decl);

    std::string class_name = class_id->getClassNameWithoutNamespace();
    std::string full_class_name = class_id->getClassName();

    // group sources and sinks should not be shown to the user
    if (full_class_name == "cedar.processing.sources.GroupSource" || full_class_name == "cedar.processing.sinks.GroupSink")
    {
      continue;
    }

    QIcon icon = elem_decl->getIcon();
    std::vector<QString> decorations;
    if (!class_id->getSource().empty())
    {
      decorations.push_back(":/decorations/from_plugin.svg");
    }

    if (class_id->isDeprecated())
    {
      if (!cedar::proc::gui::SettingsSingleton::getInstance()->getElementListShowsDeprecated())
      {
        continue;
      }
      decorations.push_back(":/cedar/auxiliaries/gui/warning.svg");
    }

    std::string description;
    if (!elem_decl->getDescription().empty())
    {
      description = elem_decl->getDescription();
    }

    std::string deprecation;
    if (class_id->isDeprecated() && !class_id->getDeprecationDescription().empty())
    {
      deprecation = class_id->getDeprecationDescription();
    }

    std::string source;
    if (!class_id->getSource().empty())
    {
      source = class_id->getSource();
    }

    this->addListEntry(class_name, full_class_name, icon, decorations, description, deprecation, source, elem_decl);
  }

  for (auto group_entry : group_entries)
  {
    auto definition = group_entry.second;
    std::vector<QString> decorations;
    decorations.push_back(":/decorations/template.svg");
    if (definition->getCategory() == categoryName)
    {
      this->addListEntry(definition->getClassName(), "group template", definition->getIcon(), decorations, "", "", "", definition);
    }
  }
}

Qt::DropActions cedar::proc::gui::ElementClassList::supportedDropActions() const
{
  return Qt::CopyAction | Qt::LinkAction;
}

void cedar::proc::gui::ElementClassList::addListEntry
(
  const std::string& className,
  const std::string& fullClassName,
  const QIcon& icon,
  const std::vector<QString>& decorations,
  const std::string& description,
  const std::string& deprecation,
  const std::string& source,
  cedar::aux::ConstPluginDeclarationPtr declaration
)
{
  QString label = QString::fromStdString(className);
  QListWidgetItem *p_item = new QListWidgetItem(label);
  p_item->setFlags(p_item->flags() | Qt::ItemIsDragEnabled);

  QSize icon_size = this->iconSize();
  int decoration_size = 12;

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
  class_description += "<nobr>class <big><b>" + QString::fromStdString(className) + "</b></big></nobr>";

  class_description += "<hr />";
  class_description += "<div align=\"right\"><nobr><small><i>" + QString::fromStdString(fullClassName) + "</i></small></nobr></div>";
  if (!description.empty())
  {
    QString description_qt = "<p>" + QString::fromStdString(description).replace("\n", "<br />") + "</p>";
    class_description += description_qt;
  }


  if (!deprecation.empty())
  {
    class_description += "<br /><b>This class is deprecated:</b> ";
    class_description += QString::fromStdString(deprecation);
  }

  if (!source.empty())
  {
    class_description += "<br /><b>From plugin:</b> "
                         + QString::fromStdString(source);
  }

  p_item->setToolTip(class_description);

//  p_item->setData(Qt::UserRole, QVariant(fullClassName.c_str()));
  p_item->setData(Qt::UserRole, QVariant::fromValue(const_cast<cedar::aux::PluginDeclaration*>(declaration.get())));
  this->addItem(p_item);
}
