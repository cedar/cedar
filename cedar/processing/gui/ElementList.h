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

    File:        ElementList.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2015 03 30

    Description: Header file for the class cedar::proc::gui::ElementList.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_ELEMENT_LIST_H
#define CEDAR_PROC_GUI_ELEMENT_LIST_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES

// FORWARD DECLARATIONS
#include "cedar/processing/gui/ElementList.fwd.h"
#include "cedar/processing/ElementDeclaration.fwd.h"
#include "cedar/processing/GroupDeclaration.fwd.h"
#include "cedar/auxiliaries/PluginDeclaration.fwd.h"

// SYSTEM INCLUDES
#include <QTabWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QStandardItem>
#include <QGraphicsSceneDragDropEvent>
#include <map>
#include <string>
#include <vector>

// pseudo-nested class (because the Qt moc doesn't do real nested classes)
namespace cedar
{
  namespace proc
  {
    namespace gui
    {
      namespace elementList
      {
        /*!@brief A line edit that displays like a search bar, i.e., with a search icon and a button to clear it.
         *
         */
        class SearchBar : public QLineEdit
        {
          Q_OBJECT

          public:
            //! Constructor.
            SearchBar(QWidget* pParent = nullptr);

          signals:
            //! Signal that is emitted whenver the search string changes and the results should be updated.
            void searchStringChanged(QString searchString);

          protected:
            //! Positions the search/clear icon
            void resizeEvent(QResizeEvent* pEvent);

          private slots:
            void textChanged(const QString & text);

            void clearClicked();

          private:
            QPushButton* mpSearchClearIcon;
        };
      }
    }
  }
}


/*!@brief A widget for displaying elements that can be added to an architecture.
 */
class cedar::proc::gui::ElementList : public QTabWidget
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  class TabBase : public QListView
  {
    public:
      TabBase(QWidget* pParent = nullptr);

      void clear();

      static cedar::aux::ConstPluginDeclaration* getDeclarationFromItem(QStandardItem* pItem);

    protected:
      //! What drop actions are supported by this widget.
      Qt::DropActions supportedDropActions() const;

      //! Opens up the context menu.
      void contextMenuEvent(QContextMenuEvent* pEvent);

      //! Adds multiple entries to the list. This is faster than calling addEntry for all items in the list!
      void addEntries(const std::vector<cedar::aux::ConstPluginDeclarationPtr>& entries);

      //! Add a single entry to the list.
      void addEntry(cedar::aux::ConstPluginDeclarationPtr declaration);

      cedar::aux::ConstPluginDeclaration* getDeclarationFromIndex(const QModelIndex& index) const;

    private:
      QStandardItem* makeItemFromElementDeclaration(cedar::proc::ConstElementDeclarationPtr declaration);

      QStandardItem* makeItemFromGroupDeclaration(cedar::proc::ConstGroupDeclarationPtr declaration);

      QStandardItem* makeItem
      (
        const std::string& className,
        const std::string& fullClassName,
        const QIcon& icon,
        std::vector<QString> decorations,
        const std::string& description,
        const std::string& deprecation,
        const std::string& source,
        cedar::aux::ConstPluginDeclarationPtr declaration
      );
  };

  class CategoryTab : public TabBase
  {
    public:
      CategoryTab(const std::string& categoryName, QWidget* pParent = nullptr);

      void update();

    private:
      std::string mCategoryName;
  };

  class FavoritesTab : public TabBase
  {
    public:
      FavoritesTab(QWidget* pParent = nullptr);

      void update();
  };

  class SearchResultsTab : public TabBase
  {
    public:
      SearchResultsTab(QWidget* pParent = nullptr);

      void update(const std::string& searchFilter);

      static bool searchFilterMatches(const std::string& searchFilter, cedar::aux::ConstPluginDeclarationPtr declaration);

      static bool searchFilterMatches(const std::string& searchFilter, cedar::aux::ConstPluginDeclaration* declaration);
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ElementList(QWidget* pParent = nullptr);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public slots:
  void reset();

public:
  static cedar::aux::ConstPluginDeclaration* declarationFromDrop(QGraphicsSceneDragDropEvent* pEvent);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! Places the search bar
  void resizeEvent(QResizeEvent* pEvent);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private slots:
  void updateSearchResults(QString searchText);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! Tab containing the user's favorite elements
  FavoritesTab* mpFavoritesTab;

  //! Widgets for each list of element classes.
  std::map<std::string, CategoryTab*> mCategoryWidgets;

  //! Search input
  cedar::proc::gui::elementList::SearchBar* mpSearchBox;

  //! Tab for displaying search results
  SearchResultsTab* mpSearchResultTab;

  //! Used to remember the index that was active before searching
  int mPreSearchIndex;
}; // class cedar::proc::gui::ElementList

#endif // CEDAR_PROC_GUI_ELEMENT_LIST_H

