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
#include "cedar/processing/gui/Settings.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/GroupDeclaration.h"
#include "cedar/processing/GroupDeclarationManager.h"
#include "cedar/auxiliaries/PluginDeclarationTemplate.h"
#include "cedar/auxiliaries/PluginProxy.h"
#include "cedar/auxiliaries/casts.h"

// SYSTEM INCLUDES
#include <QContextMenuEvent>
#include <QApplication>
#include <QStandardItemModel>
#include <QResource>
#include <QPainter>
#include <QPixmap>
#include <QIcon>
#include <QMenu>
#include <vector>
#include <QMimeData>

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
mpFavoritesTab(nullptr),
mpSearchBox(new cedar::proc::gui::elementList::SearchBar(this)),
mpSearchResultTab(nullptr),
mPreSearchIndex(-1)
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

  QObject::connect
  (
          cedar::proc::gui::SettingsSingleton::getInstance().get(),
          SIGNAL(hiddenElementsChanged()),
          this,
          SLOT(reset())
  );

  cedar::aux::PluginProxy::connectToPluginDeclaredSignal
  (
    boost::bind(&cedar::proc::gui::ElementList::reset, this)
  );

  QObject::connect
  (
    this->mpSearchBox,
    SIGNAL(searchStringChanged(QString)),
    this,
    SLOT(updateSearchResults(QString))
  );

  this->mpSearchBox->setFixedHeight(this->tabBar()->height());

  this->reset();

  // update search results once to create the search tab; this takes time when called for the first time, and is fast afterwards
  this->updateSearchResults(QString());
}

cedar::proc::gui::ElementList::TabBase::TabBase(QWidget* pParent)
:
QListView(pParent)
{
  this->setModel(new QStandardItemModel());
  this->setViewMode(QListView::IconMode);
  this->setMovement(QListView::Static);
  this->setResizeMode(QListView::Adjust);
  this->setDragEnabled(true);
  this->setIconSize(QSize(40, 40));
}

cedar::proc::gui::ElementList::SearchResultsTab::SearchResultsTab(QWidget* pParent)
:
cedar::proc::gui::ElementList::TabBase(pParent)
{
  // gather all declarations
  std::multimap<std::string, cedar::aux::ConstPluginDeclarationPtr> ordered_entries;

  // first, go trough all element declaration entries and put them in the map, thus ordering them
  auto entries = ElementManagerSingleton::getInstance()->getDeclarations();
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
    ordered_entries.insert(std::make_pair(cedar::aux::toLower(declaration->getClassName()), declaration));
  }

  // add declarations to the list
  this->clear();
  std::vector<cedar::aux::ConstPluginDeclarationPtr> to_add;
  for (const auto& name_params_pair : ordered_entries)
  {
    const auto& declaration = name_params_pair.second;
    to_add.push_back(declaration);
  }

  this->addEntries(to_add);

  // update search results (to display only those matching the current search string)
  this->update(std::string());
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

cedar::proc::gui::elementList::SearchBar::SearchBar(QWidget* pParent)
:
QLineEdit(pParent)
{
  this->setFixedWidth(300);

  this->mpSearchClearIcon = new QPushButton(QIcon(":/cedar/auxiliaries/gui/search.svg"), "", this);
  this->mpSearchClearIcon->setFixedSize(16, 16);
  this->mpSearchClearIcon->setFlat(true);
  this->mpSearchClearIcon->setFocusPolicy(Qt::NoFocus);
  this->mpSearchClearIcon->setStyleSheet("QPushButton{background:rgba(0,0,0,0);}");

  QObject::connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(textChanged(const QString&)));
  QObject::connect(this->mpSearchClearIcon, SIGNAL(clicked()), this, SLOT(clearClicked()));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::ConstPluginDeclaration* cedar::proc::gui::ElementList::declarationFromDrop(QGraphicsSceneDragDropEvent* pEvent)
{
  auto source = dynamic_cast<QListView*>(pEvent->source());

  if (source)
  {
    QByteArray itemData = pEvent->mimeData()->data("application/x-qstandarditemmodeldatalist");
    QDataStream stream(&itemData, QIODevice::ReadOnly);

    int r, c;
    QMap<int, QVariant> v;
    stream >> r >> c >> v;

    auto model = cedar::aux::asserted_cast<QStandardItemModel*>(source->model());
    auto p_item = model->item(r, c);

    if (p_item)
    {
      return cedar::proc::gui::ElementList::TabBase::getDeclarationFromItem(p_item);
    }
  }

  return nullptr;
}

void cedar::proc::gui::elementList::SearchBar::textChanged(const QString& text)
{
  if (text.isEmpty())
  {
    this->mpSearchClearIcon->setIcon(QIcon(":/cedar/auxiliaries/gui/search.svg"));
  }
  else
  {
    this->mpSearchClearIcon->setIcon(QIcon(":/cedar/auxiliaries/gui/clear.svg"));
  }

  emit searchStringChanged(text);
}

void cedar::proc::gui::elementList::SearchBar::clearClicked()
{
  this->setText("");
}

void cedar::proc::gui::elementList::SearchBar::resizeEvent(QResizeEvent* pEvent)
{
  QLineEdit::resizeEvent(pEvent);

  // update size/position of icon
  auto geometry = this->mpSearchClearIcon->geometry();
  int icon_w = geometry.width();
  int icon_h = geometry.height();
  int w = pEvent->size().width();
  int h = pEvent->size().height();
  int v_space_top = (h - icon_h) / 2;
  geometry.setLeft(w - icon_w - v_space_top);
  geometry.setRight(w - v_space_top);
  geometry.setTop(v_space_top);
  geometry.setBottom(v_space_top + icon_h);
  this->mpSearchClearIcon->setGeometry(geometry);

  // update text margins
  auto margins = this->textMargins();
  margins.setRight(icon_w + v_space_top);
  this->setTextMargins(margins);
}

void cedar::proc::gui::ElementList::resizeEvent(QResizeEvent* pEvent)
{
  QTabWidget::resizeEvent(pEvent);

  // put the search bar into the top-right corner
  auto geomertry = this->mpSearchBox->geometry();
  geomertry.setLeft(pEvent->size().width() - geomertry.width());
  geomertry.setRight(pEvent->size().width());
  this->mpSearchBox->setGeometry(geomertry);

  // make sure the search box has the same height as the tab bar
  this->mpSearchBox->setFixedHeight(this->tabBar()->height());

  // resize the tab bar
  this->tabBar()->setMaximumWidth(pEvent->size().width() - this->mpSearchBox->width());
}

cedar::aux::ConstPluginDeclaration* cedar::proc::gui::ElementList::TabBase::getDeclarationFromIndex(const QModelIndex& index) const
{
  auto data = this->model()->itemData(index);
  auto iter = data.find(Qt::UserRole);
  // there must be data for the user role, as that contains the pointer to the declaration
  CEDAR_DEBUG_ASSERT(iter != data.end());
  return iter->value<cedar::aux::PluginDeclaration*>();
}

cedar::aux::ConstPluginDeclaration* cedar::proc::gui::ElementList::TabBase::getDeclarationFromItem(QStandardItem* pItem)
{
  if (pItem->data(Qt::UserRole).isValid())
  {
    return pItem->data(Qt::UserRole).value<cedar::aux::PluginDeclaration*>();
  }
  else
  {
    return nullptr;
  }
}


void cedar::proc::gui::ElementList::TabBase::contextMenuEvent(QContextMenuEvent* pEvent)
{
  QMenu context_menu(this);

  auto index = this->indexAt(pEvent->pos());

  QAction* fav_action = context_menu.addAction("favorite");
  fav_action->setCheckable(true);
  cedar::aux::ConstPluginDeclaration* p_declaration = nullptr;
  std::string class_name;
  if (index.isValid())
  {
    p_declaration = this->getDeclarationFromIndex(index);
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

QStandardItem* cedar::proc::gui::ElementList::TabBase::makeItemFromElementDeclaration(cedar::proc::ConstElementDeclarationPtr declaration)
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
    return nullptr;
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
      return nullptr;
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

  return this->makeItem
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

QStandardItem* cedar::proc::gui::ElementList::TabBase::makeItemFromGroupDeclaration(cedar::proc::ConstGroupDeclarationPtr declaration)
{
  std::vector<QString> decorations;
  decorations.push_back(":/decorations/template.svg");

  return this->makeItem
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

void cedar::proc::gui::ElementList::TabBase::addEntries(const std::vector<cedar::aux::ConstPluginDeclarationPtr>& entries)
{
  bool blocked = this->model()->blockSignals(true);
  for (const auto& declaration : entries)
  {
    if (!SettingsSingleton::getInstance()->isHiddenElement(declaration->getClassName()))
    {
      QStandardItem* p_item = nullptr;
      if (auto element_declaration = boost::dynamic_pointer_cast<cedar::proc::ConstElementDeclaration>(declaration))
      {
        p_item = this->makeItemFromElementDeclaration(element_declaration);
      }
      else if (auto group_declaration = boost::dynamic_pointer_cast<cedar::proc::ConstGroupDeclaration>(declaration))
      {
        p_item = this->makeItemFromGroupDeclaration(group_declaration);
      }
      else
      {
        CEDAR_NON_CRITICAL_ASSERT(false || "Unhandled declaration type. This should not happen...");
      }

      // p_item can be a nullptr if the declaration is not supposed to be shown (e.g., if it is deprecated)
      if (p_item != nullptr)
      {
        cedar::aux::asserted_cast<QStandardItemModel*>(this->model())->appendRow(p_item);
      }
    }
  }
  this->model()->blockSignals(blocked);
  this->update();
}

void cedar::proc::gui::ElementList::TabBase::addEntry(cedar::aux::ConstPluginDeclarationPtr declaration)
{
  std::vector<cedar::aux::ConstPluginDeclarationPtr> entries;
  entries.push_back(declaration);
  this->addEntries(entries);
}

void cedar::proc::gui::ElementList::TabBase::clear()
{
  static_cast<QStandardItemModel*>(this->model())->clear();
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
  std::vector<cedar::aux::ConstPluginDeclarationPtr> to_add;
  for (const auto& name_params_pair : ordered_entries)
  {
    const auto& declaration = name_params_pair.second;
    to_add.push_back(declaration);
  }
  this->addEntries(to_add);
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

  std::vector<cedar::aux::ConstPluginDeclarationPtr> to_add;
  for (const auto& name_declaration_pair : ordered_list)
  {
    to_add.push_back(name_declaration_pair.second);
  }
  this->addEntries(to_add);
}

QStandardItem* cedar::proc::gui::ElementList::TabBase::makeItem
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
  auto p_item = new QStandardItem(label);
  //QListWidgetItem *p_item = new QListWidgetItem(label);
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
  class_description += "<div align=\"right\"><nobr><tt>" + QString::fromStdString(fullClassName) + "</tt></nobr></div>";
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

  p_item->setData(QVariant::fromValue(const_cast<cedar::aux::PluginDeclaration*>(declaration.get())), Qt::UserRole);
  return p_item;
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

    // sort the tabs alphabetically using bubble sort
    for (int n = this->tabBar()->count(); n > 1; --n)
    {
      // start at i = 1 because the first tab is the favorites tab and should always stay in the first place
      for (int i = 1; i < n - 1; ++i)
      {
        std::string text_1 = this->tabBar()->tabText(i).toStdString();
        std::string text_2 = this->tabBar()->tabText(i + 1).toStdString();

        if (cedar::aux::toLower(text_1) > cedar::aux::toLower(text_2))
        {
          this->tabBar()->moveTab(i, i + 1);
        }
      }
    }

    // if the category does not contain any displayed items, remove it
    if (p_tab->model()->rowCount() == 0)
    {
      this->removeTab(this->indexOf(p_tab));
      auto iter = mCategoryWidgets.find(category_name);
      CEDAR_DEBUG_ASSERT(iter != mCategoryWidgets.end());
      mCategoryWidgets.erase(iter);
    }
  }
}

void cedar::proc::gui::ElementList::updateSearchResults(QString searchText)
{
  if (this->mpSearchResultTab == nullptr)
  {
    this->mpSearchResultTab = new SearchResultsTab();
  }

  if (searchText.isEmpty())
  {
    // remove the tab to "hide" it (qt has no method for hiding individual tabs)
    int tab_index = -1;
    for (int i = 0; i < this->count(); ++i)
    {
      if (this->widget(i) == this->mpSearchResultTab)
      {
        tab_index = i;
        break;
      }
    }

    if (tab_index == this->currentIndex() && this->mPreSearchIndex >= 0)
    {
      this->setCurrentIndex(this->mPreSearchIndex);
    }

    this->mPreSearchIndex = -1;

    if (tab_index >= 0)
    {
      this->removeTab(tab_index);
    }
  }
  else
  {
    if (this->mPreSearchIndex < 0)
    {
      this->addTab(this->mpSearchResultTab, QIcon(":/cedar/auxiliaries/gui/search.svg"), "search results");
    }

    // find out the index of the search result tab
    int tab_index = -1;
    for (int i = 0; i < this->count(); ++i)
    {
      if (this->widget(i) == this->mpSearchResultTab)
      {
        tab_index = i;
        break;
      }
    }

    // if the user has changed the tab, remember where we were before
    if (tab_index >= 0 && tab_index != this->currentIndex())
    {
      this->mPreSearchIndex = this->currentIndex();
      this->setCurrentIndex(tab_index);
    }

    this->mpSearchResultTab->update(searchText.toStdString());
  }
}

void cedar::proc::gui::ElementList::SearchResultsTab::update(const std::string& searchFilter)
{
  for (int row = 0; row < this->model()->rowCount(); ++row)
  {
    QStandardItem* item = cedar::aux::asserted_cast<QStandardItem*>(cedar::aux::asserted_cast<QStandardItemModel*>(this->model())->item(row, 0));
    auto declaration = this->getDeclarationFromIndex(item->index());

    this->setRowHidden(row, !searchFilterMatches(searchFilter, declaration));
  }
}

bool cedar::proc::gui::ElementList::SearchResultsTab::searchFilterMatches(const std::string& searchFilter, cedar::aux::ConstPluginDeclarationPtr declaration)
{
  return searchFilterMatches(searchFilter, declaration.get());
}

bool cedar::proc::gui::ElementList::SearchResultsTab::searchFilterMatches(const std::string& searchFilter, cedar::aux::ConstPluginDeclaration* declaration)
{
  std::string normalized_classname = cedar::aux::toLower(declaration->getClassName());
  std::string normalized_search_filter = cedar::aux::toLower(searchFilter);

  // see if the search filter is part of the class name
  if (normalized_classname.find(normalized_search_filter) != std::string::npos)
  {
    return true;
  }

  // no match
  return false;
}
