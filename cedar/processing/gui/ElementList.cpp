/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        ElementList.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2015 03 30

    Description: Source file for the class cedar::proc::gui::ElementList.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/gui/ElementList.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/ElementClassList.h"
#include "cedar/processing/gui/Settings.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/GroupDeclaration.h"
#include "cedar/processing/GroupDeclarationManager.h"
#include "cedar/auxiliaries/PluginDeclarationTemplate.h"
#include "cedar/auxiliaries/PluginProxy.h"

// SYSTEM INCLUDES
#include <QContextMenuEvent>
#include <QApplication>
#include <QResource>
#include <QPainter>
#include <QPixmap>
#include <QIcon>
#include <QMenu>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------
//  the following code makes sure that a plugin declaration can be passed around within qt via drag & drop
// ---------------------------------------------------------------------------------------------------------------------

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

cedar::proc::gui::ElementList::ElementList(QWidget* pParent)
:
QTabWidget(pParent),
mpFavoritesTab(nullptr)
{
  QObject::connect
  (
    cedar::proc::gui::SettingsSingleton::getInstance()->getElementListShowsDeprecatedParameter().get(),
    SIGNAL(valueChanged()),
    this,
    SLOT(reset())
  );

  QObject::connect
  (
    cedar::proc::gui::SettingsSingleton::getInstance().get(),
    SIGNAL(elementFavoritesChanged()),
    this,
    SLOT(reset())
  );

  cedar::aux::PluginProxy::connectToPluginDeclaredSignal
  (
    boost::bind(&cedar::proc::gui::ElementList::reset, this)
  );

  this->reset();
}

cedar::proc::gui::ElementList::TabBase::TabBase(QWidget* pParent)
:
QListWidget(pParent)
{
  this->setViewMode(QListView::IconMode);
  this->setMovement(QListView::Static);
  this->setResizeMode(QListView::Adjust);
  this->setDragEnabled(true);
  this->setIconSize(QSize(40, 40));
}

cedar::proc::gui::ElementList::CategoryTab::CategoryTab(const std::string& categoryName, QWidget* pParent)
:
cedar::proc::gui::ElementList::TabBase(pParent),
mCategoryName(categoryName)
{
  this->update();
}

cedar::proc::gui::ElementList::FavoritesTab::FavoritesTab(QWidget* pParent)
:
cedar::proc::gui::ElementList::TabBase(pParent)
{
  this->update();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::ConstPluginDeclaration* cedar::proc::gui::ElementList::TabBase::getDeclarationFromItem(QListWidgetItem* pItem) const
{
  return pItem->data(Qt::UserRole).value<cedar::aux::PluginDeclaration*>();
}

void cedar::proc::gui::ElementList::TabBase::contextMenuEvent(QContextMenuEvent* pEvent)
{
  QMenu context_menu(this);

  auto item = this->itemAt(pEvent->pos());

  QAction* fav_action = context_menu.addAction("favorite");
  fav_action->setCheckable(true);
  cedar::aux::ConstPluginDeclaration* p_declaration = nullptr;
  std::string class_name;
  if (item)
  {
    p_declaration = this->getDeclarationFromItem(item);
    class_name = p_declaration->getClassName();
    fav_action->setChecked(cedar::proc::gui::SettingsSingleton::getInstance()->isFavoriteElement(class_name));
  }
  else
  {
    fav_action->setEnabled(false);
  }

  //!@todo Setting showing/hiding deprecated should be in the context menu, but rather in the settings
  context_menu.addSeparator();
  auto show_deprecated_action = context_menu.addAction("show deprecated steps");
  show_deprecated_action->setCheckable(true);
  show_deprecated_action->setChecked(cedar::proc::gui::SettingsSingleton::getInstance()->getElementListShowsDeprecated());

  auto selected_action = context_menu.exec(pEvent->globalPos());

  if (selected_action == show_deprecated_action)
  {
    cedar::proc::gui::SettingsSingleton::getInstance()->setElementListShowsDeprecated(show_deprecated_action->isChecked());
  }
  else if (selected_action == fav_action)
  {
    cedar::proc::gui::SettingsSingleton::getInstance()->setFavorite(class_name, fav_action->isChecked());
  }
}

void cedar::proc::gui::ElementList::TabBase::addElementDeclaration(cedar::proc::ConstElementDeclarationPtr declaration)
{
  std::string full_class_name = declaration->getClassName();

  // group sources and sinks should not be shown to the user
  if
  (
    full_class_name == "cedar.processing.sources.GroupSource"
    || full_class_name == "cedar.processing.sinks.GroupSink"
    || full_class_name == "cedar.processing.LoopedTrigger"
  )
  {
    return;
  }

  std::vector<QString> decorations;
  std::string source;
  std::string description;
  std::string deprecation;
  QIcon icon = declaration->getIcon();

  if (!declaration->getSource().empty())
  {
    decorations.push_back(":/decorations/from_plugin.svg");
  }

  if (declaration->isDeprecated())
  {
    if (!cedar::proc::gui::SettingsSingleton::getInstance()->getElementListShowsDeprecated())
    {
      return;
    }
    decorations.push_back(":/cedar/auxiliaries/gui/warning.svg");
  }

  if (!declaration->getDescription().empty())
  {
    description = declaration->getDescription();
  }

  if (declaration->isDeprecated() && !declaration->getDeprecationDescription().empty())
  {
    deprecation = declaration->getDeprecationDescription();
  }

  if (!declaration->getSource().empty())
  {
    source = declaration->getSource();
  }

  this->addListEntry
  (
    declaration->getClassNameWithoutNamespace(),
    full_class_name,
    icon,
    decorations,
    description,
    deprecation,
    source,
    declaration
  );
}

void cedar::proc::gui::ElementList::TabBase::addGroupDeclaration(cedar::proc::ConstGroupDeclarationPtr declaration)
{
  std::vector<QString> decorations;
  decorations.push_back(":/decorations/template.svg");

  this->addListEntry
  (
    declaration->getClassNameWithoutNamespace(),
    "group template",
    declaration->getIcon(),
    decorations,
    std::string(),
    std::string(),
    std::string(),
    declaration
  );
}

void cedar::proc::gui::ElementList::TabBase::addEntry(cedar::aux::ConstPluginDeclarationPtr declaration)
{
  if (auto element_declaration = boost::dynamic_pointer_cast<cedar::proc::ConstElementDeclaration>(declaration))
  {
    this->addElementDeclaration(element_declaration);
  }
  else if (auto group_declaration = boost::dynamic_pointer_cast<cedar::proc::ConstGroupDeclaration>(declaration))
  {
    this->addGroupDeclaration(group_declaration);
  }
  else
  {
    CEDAR_NON_CRITICAL_ASSERT(false || "Unhandled declaration type. This should not happen...");
  }
}

void cedar::proc::gui::ElementList::CategoryTab::update()
{
  this->clear();

  std::multimap<std::string, cedar::aux::ConstPluginDeclarationPtr> ordered_entries;

  // first, go trough all element declaration entries and put them in the map, thus ordering them
  auto entries = ElementManagerSingleton::getInstance()->getCategoryEntries(this->mCategoryName);
  for (const auto& base_declaration : entries)
  {
    auto declaration = boost::dynamic_pointer_cast<cedar::proc::ConstElementDeclaration>(base_declaration);
    CEDAR_ASSERT(declaration);
    ordered_entries.insert(std::make_pair(cedar::aux::toLower(declaration->getClassNameWithoutNamespace()), declaration));
  }

  // also go through all group declarations
  auto group_entries = cedar::proc::GroupDeclarationManagerSingleton::getInstance()->getDefinitions();
  for (auto group_entry : group_entries)
  {
    auto declaration = group_entry.second;
    if (declaration->getCategory() == this->mCategoryName)
    {
      ordered_entries.insert(std::make_pair(cedar::aux::toLower(declaration->getClassName()), declaration));
    }
  }

  // then, actually add the entries
  for (const auto& name_params_pair : ordered_entries)
  {
    const auto& declaration = name_params_pair.second;
    this->addEntry(declaration);
  }
}

void cedar::proc::gui::ElementList::FavoritesTab::update()
{
  this->clear();
  std::vector<std::string> favorites = cedar::proc::gui::SettingsSingleton::getInstance()->getFavedElements();

  std::multimap<std::string, cedar::aux::ConstPluginDeclarationPtr> ordered_list;
  for (const auto& class_name : favorites)
  {
    // see if the given declaration is an element declaration
    auto element_declaration = ElementManagerSingleton::getInstance()->getDeclarationNoThrow(class_name);
    if (element_declaration)
    {
      ordered_list.insert(std::make_pair(cedar::aux::toLower(element_declaration->getClassNameWithoutNamespace()), element_declaration));
      continue;
    }

    // see if the declaration is a group declaration
    auto group_declaration = cedar::proc::GroupDeclarationManagerSingleton::getInstance()->getDeclarationNoThrow(class_name);
    if (group_declaration)
    {
      ordered_list.insert(std::make_pair(cedar::aux::toLower(group_declaration->getClassName()), group_declaration));
      continue;
    }
  }

  for (const auto& name_declaration_pair : ordered_list)
  {
    this->addEntry(name_declaration_pair.second);
  }
}

void cedar::proc::gui::ElementList::TabBase::addListEntry
(
  const std::string& className,
  const std::string& fullClassName,
  const QIcon& icon,
  std::vector<QString> decorations,
  const std::string& description,
  const std::string& deprecation,
  const std::string& source,
  cedar::aux::ConstPluginDeclarationPtr declaration
)
{
  QString label = QString::fromStdString(className);
  QListWidgetItem *p_item = new QListWidgetItem(label);
  p_item->setFlags(p_item->flags() | Qt::ItemIsDragEnabled);

  if (cedar::proc::gui::SettingsSingleton::getInstance()->isFavoriteElement(declaration->getClassName()))
  {
    decorations.push_back(":/cedar/auxiliaries/gui/favorite.svg");
  }

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

  p_item->setData(Qt::UserRole, QVariant::fromValue(const_cast<cedar::aux::PluginDeclaration*>(declaration.get())));
  this->addItem(p_item);
}

Qt::DropActions cedar::proc::gui::ElementList::TabBase::supportedDropActions() const
{
  return Qt::CopyAction | Qt::LinkAction;
}

void cedar::proc::gui::ElementList::reset()
{
  if (this->mpFavoritesTab == nullptr)
  {
    this->mpFavoritesTab = new FavoritesTab();
    this->addTab(this->mpFavoritesTab, QIcon(":/cedar/auxiliaries/gui/favorite.svg"), "Favorites");
  }
  this->mpFavoritesTab->update();

  //!@todo This is inefficient; instead of resetting the entire list every time, just update each tab and create new ones if necessary
  for (const auto& category_name : cedar::proc::ElementManagerSingleton::getInstance()->listCategories())
  {
    CategoryTab *p_tab;
    if (this->mCategoryWidgets.find(category_name) == this->mCategoryWidgets.end())
    {
      p_tab = new CategoryTab(category_name);
      this->addTab(p_tab, QString::fromStdString(category_name));
      this->mCategoryWidgets[category_name] = p_tab;
    }
    else
    {
      p_tab = this->mCategoryWidgets[category_name];
      p_tab->update();
    }

    //!@todo: Re-sort the tabs

    // if the category does not contain any displayed items, remove it
    if (p_tab->count() == 0)
    {
      this->removeTab(this->indexOf(p_tab));
      auto iter = mCategoryWidgets.find(category_name);
      CEDAR_DEBUG_ASSERT(iter != mCategoryWidgets.end());
      mCategoryWidgets.erase(iter);
    }
  }
}
