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

    File:        ResourceDialog.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 02 27

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/ResourceDialog.h"
#include "cedar/auxiliaries/systemFunctions.h"
#include "cedar/auxiliaries/assert.h"

// SYSTEM INCLUDES
#include <QTreeWidgetItem>
#include <QDir>
#include <string>

#include <boost/version.hpp>
#if (BOOST_VERSION / 100 % 1000 < 46) // there was an interface change in boost
  #define BOOST_FILESYSTEM_VERSION 2
#else
  #define BOOST_FILESYSTEM_VERSION 3
#endif
#include <boost/filesystem.hpp>

//----------------------------------------------------------------------------------------------------------------------
// static constants
//----------------------------------------------------------------------------------------------------------------------

const int cedar::aux::gui::ResourceDialog::ITEM_TYPE_FOLDER = 1;
const int cedar::aux::gui::ResourceDialog::ITEM_TYPE_FILE = 2;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::ResourceDialog::ResourceDialog
                                 (
                                   QWidget* pParent,
                                   const std::vector<std::string>& extensions
                                 )
:
QDialog(pParent),
mExtensions(extensions),
mIconProvider(new QFileIconProvider())
{
  this->setupUi(this);

  QObject::connect(this->mpResourceList, SIGNAL(itemSelectionChanged()), this, SLOT(itemSelected()));
  QObject::connect(this->mpResourceList, SIGNAL(itemActivated(QTreeWidgetItem*, int)), this, SLOT(itemActivated(QTreeWidgetItem*, int)));

  this->fill();
}

cedar::aux::gui::ResourceDialog::~ResourceDialog()
{
  delete this->mIconProvider;
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::ResourceDialog::itemActivated(QTreeWidgetItem* pItem, int)
{
  // happens when the user double-clicks an item

  if (pItem == NULL || pItem->type() != cedar::aux::gui::ResourceDialog::ITEM_TYPE_FILE)
  {
    return;
  }

  this->setTextFromItem(pItem);
  this->accept();
}

void cedar::aux::gui::ResourceDialog::itemSelected()
{
  QList<QTreeWidgetItem*> selected_items = this->mpResourceList->selectedItems();

  if (selected_items.count() != 1)
  {
    return;
  }

  QTreeWidgetItem* p_item = selected_items.at(0);

  if (p_item == NULL)
  {
    return;
  }

  if (p_item->type() == cedar::aux::gui::ResourceDialog::ITEM_TYPE_FOLDER)
  {
    return;
  }

  this->setTextFromItem(p_item);
}

void cedar::aux::gui::ResourceDialog::setTextFromItem(QTreeWidgetItem* pItem)
{
  std::string path;
  while (pItem != this->mpResourceList->invisibleRootItem() && pItem != NULL)
  {
    path = pItem->text(0).toStdString() + (path.empty()? "" : "/" + path);
    pItem = pItem->parent();
  }

  this->mpSelectedText->setText(QString::fromStdString(path));
}

void cedar::aux::gui::ResourceDialog::fill()
{
  this->mpResourceList->clear();

  std::vector<std::string> paths = cedar::aux::listResourcePaths();

  CEDAR_ASSERT(this->mpResourceList->invisibleRootItem() != NULL);

  for (auto path_iter = paths.begin(); path_iter != paths.end(); ++path_iter)
  {
    this->appendDirectories(*path_iter, this->mpResourceList->invisibleRootItem());
  }

  this->removeEmpty(this->mpResourceList->invisibleRootItem());
}

void cedar::aux::gui::ResourceDialog::removeEmpty(QTreeWidgetItem* pParent)
{
  QList<QTreeWidgetItem*> to_remove;
  for (int i = 0; i < pParent->childCount(); ++i)
  {
    this->removeEmpty(pParent->child(i));

    if
    (
      pParent->child(i)->childCount() <= 0
      && pParent->child(i)->type() == cedar::aux::gui::ResourceDialog::ITEM_TYPE_FOLDER
    )
    {
      to_remove.append(pParent->child(i));
    }
  }

  for (int i = 0; i < to_remove.count(); ++i)
  {
    delete to_remove.at(i);
  }
}

void cedar::aux::gui::ResourceDialog::appendDirectories
     (
       const std::string& path,
       QTreeWidgetItem* pParent
     )
{
  using boost::filesystem::directory_iterator;

  CEDAR_ASSERT(pParent != NULL);

  if (!boost::filesystem::exists(path) || !boost::filesystem::is_directory(path))
  {
    // if the path is not a directory, nothing has to be appended
    return;
  }

  // list directories
  for (directory_iterator directory_iter(path); directory_iter != directory_iterator(); ++directory_iter)
  {
    boost::filesystem::path file = *directory_iter;
    if (boost::filesystem::is_directory(file))
    {
      QStringList node;

      // use QDir because boost doesn't offer the relativeTo funcitonality
      QDir base_dir(QString::fromStdString(path));
      std::string relative_path = base_dir.relativeFilePath(QString::fromStdString(file.string())).toStdString();

      QTreeWidgetItem* p_item = this->findPathNode(relative_path, pParent);
      if (p_item == NULL)
      {
        node << QString::fromStdString(relative_path);
        p_item = new QTreeWidgetItem(node, cedar::aux::gui::ResourceDialog::ITEM_TYPE_FOLDER);
        p_item->setIcon(0, mIconProvider->icon(QFileIconProvider::Folder));
        p_item->setFlags(p_item->flags() & (~Qt::ItemIsSelectable));

        pParent->addChild(p_item);
      }

      this->appendDirectories(file.string(), p_item);
    }
  }
  // TODO remove empty nodes

  // append files in this directory
  this->appendFiles(path, pParent);
}

void cedar::aux::gui::ResourceDialog::appendFiles
     (
       const std::string& path,
       QTreeWidgetItem* pParent
     )
{
  using boost::filesystem::directory_iterator;

  CEDAR_ASSERT(pParent != NULL);

  if (!boost::filesystem::exists(path) || !boost::filesystem::is_directory(path))
  {
    // if the path is not a directory, nothing has to be appended
    return;
  }

  for (directory_iterator directory_iter(path); directory_iter != directory_iterator(); ++directory_iter)
  {
    boost::filesystem::path file = *directory_iter;

    QDir base_dir(QString::fromStdString(path));
    std::string relative_path = base_dir.relativeFilePath(QString::fromStdString(file.string())).toStdString();

    if (boost::filesystem::is_regular_file(directory_iter->status()))
    {
      if (this->findPathNode(relative_path, pParent) != NULL)
      {
        // there is already an entry for a file with this path
        continue;
      }

      if (!this->mExtensions.empty())
      {
#if (BOOST_VERSION / 100 % 1000 < 46) // there was an interface change in boost
        std::string extension = file.extension();
#else
        std::string extension = file.extension().string();
#endif
        if (extension.empty())
        {
          // file has no extension, but there are extensions to be looked for
          continue;
        }

        // see if the file's extension matches any of the set ones
        bool matched = false;
        for (size_t i = 0; i < this->mExtensions.size(); ++i)
        {
          if (extension == this->mExtensions.at(i))
          {
            matched = true;
            break;
          }
        }

        if (!matched)
        {
          continue;
        }
      }

      QStringList node;
      node << QString::fromStdString(relative_path);
      QTreeWidgetItem* p_item = new QTreeWidgetItem(node, cedar::aux::gui::ResourceDialog::ITEM_TYPE_FILE);
      p_item->setIcon(0, mIconProvider->icon(QFileIconProvider::File));

      pParent->addChild(p_item);
    }
  }
}


QTreeWidgetItem* cedar::aux::gui::ResourceDialog::findPathNode
                 (
                   const std::string& relativePath,
                   QTreeWidgetItem* pParent
                 )
                 const
{
  boost::filesystem::path path(relativePath);

  if (pParent == NULL)
  {
    pParent = this->mpResourceList->invisibleRootItem();
  }

  CEDAR_ASSERT(pParent != NULL);

#if (BOOST_VERSION / 100 % 1000 < 46) // there was an interface change in boost
  std::string part = (*path.begin());
#else
  std::string part = (*path.begin()).string();
#endif
  boost::filesystem::path sub_path;

  auto path_iter = path.begin();
  ++path_iter;

  for (;path_iter != path.end(); ++path_iter)
  {
    sub_path /= *path_iter;
  }


  for (int i = 0; i < pParent->childCount(); ++i)
  {
    QTreeWidgetItem* p_current_item = pParent->child(i);
    if (p_current_item->text(0) == QString::fromStdString(part))
    {
      if (sub_path.empty())
      {
        return p_current_item;
      }
      else
      {
        return this->findPathNode(sub_path.string(), p_current_item);
      }
    }
  }

  return NULL;
}


cedar::aux::Path cedar::aux::gui::ResourceDialog::openResource
                 (
                   QWidget* pParent,
                   const std::vector<std::string>& extensions
                 )
{
  cedar::aux::gui::ResourceDialog* p_dialog = new cedar::aux::gui::ResourceDialog(pParent, extensions);

  int res = p_dialog->exec();

  if (res == QDialog::Accepted)
  {
    return cedar::aux::Path("resource://" + p_dialog->mpSelectedText->text().toStdString());
  }
  else
  {
    return cedar::aux::Path();
  }
}
