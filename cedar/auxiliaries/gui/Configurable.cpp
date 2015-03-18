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
#include <QTreeWidgetItemIterator>
#include <QApplication>
#include <QList>
#include <QLabel>

#define PARAMETER_NAME_COLUMN 0
#define PARAMETER_EDITOR_COLUMN 1


//----------------------------------------------------------------------------------------------------------------------
// private class: DataDelegate
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::Configurable::DataDelegate::DataDelegate
(
  cedar::aux::gui::Configurable* configurableWidget,
  bool readOnly
)
:
mConfigurableWidget(configurableWidget),
mReadOnly(readOnly)
{
}

cedar::aux::gui::Configurable::DataDelegate::~DataDelegate()
{
  while (!this->mOpenedEditors.empty())
  {
    auto widget = *(this->mOpenedEditors.begin());
    // this will also remove the widget from the set of open editors via cedar::aux::gui::Configurable::DataDelegate::widgetDestroyed.
    delete widget;
  }
}

QWidget* cedar::aux::gui::Configurable::DataDelegate::createEditor(QWidget *pParent, const QStyleOptionViewItem& /*option*/, const QModelIndex &index) const
{
  cedar::aux::ParameterPtr parameter = this->mConfigurableWidget->parameterFromModelIndex(index);

  QWidget* p_ret = nullptr;
  try
  {
    cedar::aux::gui::Parameter *p_widget
      = cedar::aux::gui::ParameterFactorySingleton::getInstance()->get(parameter)->allocateRaw();
    p_widget->setParameter(parameter);
    QObject::connect(p_widget, SIGNAL(heightChanged()), this->mConfigurableWidget, SLOT(fitRowsToContents()), Qt::QueuedConnection);
    QObject::connect(parameter.get(), SIGNAL(valueChanged()), this->mConfigurableWidget, SIGNAL(settingsChanged()));
    p_ret = p_widget;
    p_widget->setReadOnly(this->mReadOnly);
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
  connect(p_ret, SIGNAL(destroyed(QObject*)), this, SLOT(widgetDestroyed(QObject*)));
  mOpenedEditors.insert(p_ret);
  return p_ret;
}

void cedar::aux::gui::Configurable::DataDelegate::widgetDestroyed(QObject* removed)
{
  if (auto widget = dynamic_cast<QObject*>(removed))
  {
    auto entry = mOpenedEditors.find(widget);
    if (entry != mOpenedEditors.end())
    {
      mOpenedEditors.erase(entry);
    }
  }
}

//!@cond SKIPPED_DOCUMENTATION
class cedar::aux::gui::Configurable::ParameterItem : public QTreeWidgetItem
{
public:
  ParameterItem(cedar::aux::ParameterPtr parameter, const std::string& path)
  :
  mParameter(parameter)
  {
    this->setText(PARAMETER_NAME_COLUMN, QString::fromStdString(parameter->getName()));
    this->setData(PARAMETER_NAME_COLUMN, Qt::UserRole, QVariant::fromValue(static_cast<void*>(parameter.get())));
    this->setData(PARAMETER_EDITOR_COLUMN, Qt::UserRole, QString::fromStdString(path));
  }

  QVariant data(int column, int role) const
  {
    switch (role)
    {
      case Qt::ToolTipRole:
      {
        switch (column)
        {
          case PARAMETER_NAME_COLUMN:
            return QString::fromStdString(this->mParameter->getName());

          default:
            return "";
        }
      }

      default:
        return this->QTreeWidgetItem::data(column, role);
    }
  }

private:
  cedar::aux::ParameterPtr mParameter;
};
//!@endcond

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::Configurable::Configurable(QWidget* pParent)
:
QWidget(pParent)
{
  // create layout
  QVBoxLayout* p_layout = new QVBoxLayout();
  this->setLayout(p_layout);

  // insert property tree
  this->mpPropertyTree = new CustomTree();
  p_layout->addWidget(mpPropertyTree, 1);
  p_layout->setContentsMargins(0, 0, 0, 0);
  this->mpPropertyTree->setAlternatingRowColors(true);

  // setup header
  this->mpPropertyTree->setColumnCount(2);
  QStringList header_labels;
  header_labels << "Property" << "Value";
  this->mpPropertyTree->setHeaderLabels(header_labels);

  // make first section stretch
  this->mpPropertyTree->header()->setResizeMode(PARAMETER_NAME_COLUMN, QHeaderView::Interactive);
  this->mpPropertyTree->header()->setResizeMode(PARAMETER_EDITOR_COLUMN, QHeaderView::Stretch);
  this->mpPropertyTree->header()->resizeSection(PARAMETER_NAME_COLUMN, 150);

  QObject::connect(this, SIGNAL(parameterAdded(int, QString)), this, SLOT(parameterAddedSlot(int, QString)));
  QObject::connect(this, SIGNAL(parameterRemoved(int, QVariant)), this, SLOT(parameterRemovedSlot(int, QVariant)));
  QObject::connect(this, SIGNAL(parameterRenamed(int, QString, QString)), this, SLOT(parameterRenamedSlot(int, QString, QString)));
}

cedar::aux::gui::Configurable::~Configurable()
{
  this->clear();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

int cedar::aux::gui::Configurable::getRootIndexForItem(QTreeWidgetItem* pItem) const
{
  QTreeWidgetItem* p_root = pItem;

  while (p_root->parent())
  {
    p_root = p_root->parent();
  }

  for (int i = 0; i < this->mpPropertyTree->topLevelItemCount(); ++i)
  {
    if (this->mpPropertyTree->topLevelItem(i) == p_root)
    {
      return i;
    }
  }

  CEDAR_THROW(cedar::aux::NotFoundException, "Could not find the item in the tree widget.");
}

cedar::aux::ParameterPtr cedar::aux::gui::Configurable::parameterFromModelIndex(const QModelIndex& index)
{
  QTreeWidgetItem* p_item = this->mpPropertyTree->getItemFromIndex(index);
  size_t i = static_cast<size_t>(this->getRootIndexForItem(p_item));

  std::string parameter_path = index.data(Qt::UserRole).toString().toStdString();
  return this->mDisplayedConfigurables[i]->getParameter(parameter_path);
}

void cedar::aux::gui::Configurable::translateParameterAddedSignal(unsigned int configurableIndex, cedar::aux::ParameterPtr parameter)
{
  QString path = QString::fromStdString(parameter->getName());
  emit parameterAdded(static_cast<int>(configurableIndex), path);
}

void cedar::aux::gui::Configurable::translateParameterRemovedSignal(unsigned int configurableIndex, cedar::aux::ParameterPtr parameter)
{
  QVariant parameter_variant = QVariant::fromValue((void*)(parameter.get()));
  emit parameterRemoved(static_cast<int>(configurableIndex), parameter_variant);
}

void cedar::aux::gui::Configurable::translateParameterNameChangedSignal(unsigned int configurableIndex, const std::string& oldName, const std::string& newName)
{
  QString old_name = QString::fromStdString(oldName);
  QString new_name = QString::fromStdString(newName);
  emit parameterRenamed(static_cast<int>(configurableIndex), old_name, new_name);
}

void cedar::aux::gui::Configurable::parameterAddedSlot(int configurableIndex, QString path)
{
  auto configurable = this->mDisplayedConfigurables[static_cast<size_t>(configurableIndex)];
  CEDAR_ASSERT(configurable != nullptr);
  auto parameter = configurable->getParameter(path.toStdString());
  auto item = this->mpPropertyTree->invisibleRootItem()->child(configurableIndex);
  CEDAR_ASSERT(item != nullptr);
  this->append(static_cast<size_t>(configurableIndex), parameter, item, std::string());

  this->fitRowsToContents();
}

void cedar::aux::gui::Configurable::parameterRemovedSlot(int /* configurableIndex */, QVariant parameterP)
{
  auto parameter = (cedar::aux::Parameter*) parameterP.value<void*>();
  auto item = this->getItemForParameter(parameter);
  CEDAR_ASSERT(item != nullptr);
  delete item;

  auto iter = this->mParameterRenamedConnections.find(parameter);
  if (iter != this->mParameterRenamedConnections.end())
  {
    this->mParameterRenamedConnections.erase(iter);
  }

  this->fitRowsToContents();
}

void cedar::aux::gui::Configurable::parameterRenamedSlot(int configurableIndex, QString /* oldName */, QString newName)
{
  auto parameter = this->mDisplayedConfigurables[static_cast<size_t>(configurableIndex)]->getParameter(newName.toStdString());
  auto item = this->getItemForParameter(parameter.get());
  item->setText(0, newName);
}

void cedar::aux::gui::Configurable::fitRowsToContents()
{
  this->mpPropertyTree->doItemsLayout();
}

QTreeWidgetItem* cedar::aux::gui::Configurable::appendHeading(QTreeWidgetItem* pParent, const QString& text, int hLevel)
{
  QTreeWidgetItem* p_item = new QTreeWidgetItem();
  pParent->addChild(p_item);
  this->makeHeading(p_item, text, hLevel);
  return p_item;
}

void cedar::aux::gui::Configurable::makeHeading(QTreeWidgetItem* pItem, const QString& text, int hLevel)
{
  pItem->setFirstColumnSpanned(true);
  QFont font = pItem->font(0);
  switch (hLevel)
  {
    case 1:
    case 2:
      font.setBold(true);
      font.setPointSize(font.pointSize() + 3 - hLevel);
      pItem->setBackground(0, this->palette().brush(QPalette::Dark));
      pItem->setForeground(0, this->palette().brush(QPalette::Light));
      break;
    default:
      font.setItalic(true);
  }
  pItem->setFont(0, font);

  pItem->setTextAlignment(0, Qt::AlignHCenter | Qt::AlignVCenter);
  pItem->setText(0, text);
}

void cedar::aux::gui::Configurable::display(std::vector<cedar::aux::ConfigurablePtr> configurables)
{
  this->clear();

  this->mpPropertyTree->setItemDelegateForColumn(PARAMETER_EDITOR_COLUMN, new cedar::aux::gui::Configurable::DataDelegate(this, false));

  for (auto configurable : configurables)
  {
    this->appendRootConfigurable(configurable);
  }
}

void cedar::aux::gui::Configurable::display(cedar::aux::ConfigurablePtr configurable, bool readOnly)
{
  this->clear();
  
  this->mpPropertyTree->setItemDelegateForColumn(PARAMETER_EDITOR_COLUMN, new cedar::aux::gui::Configurable::DataDelegate(this, readOnly));

  this->appendRootConfigurable(configurable);
}

void cedar::aux::gui::Configurable::appendRootConfigurable(cedar::aux::ConfigurablePtr configurable)
{
  size_t index = this->mDisplayedConfigurables.size();
  this->mDisplayedConfigurables.push_back(configurable);

  std::string type_name = cedar::aux::objectTypeToString(configurable);
  type_name = cedar::aux::replace(type_name, "::", ".");
  auto p_item = this->appendHeading(this->mpPropertyTree->invisibleRootItem(), QString::fromStdString(type_name), 1);
  p_item->setExpanded(true);
  this->mpPropertyTree->setRootIsDecorated(false);

  this->append(index, configurable, p_item, std::string());

  this->mParameterAddedConnection = configurable->connectToParameterAddedSignal
                                    (
                                      boost::bind
                                      (
                                        &cedar::aux::gui::Configurable::translateParameterAddedSignal,
                                        this,
                                        index,
                                        _1
                                      )
                                    );

  this->mParameterRemovedConnection = configurable->connectToParameterRemovedSignal
                                      (
                                        boost::bind
                                        (
                                          &cedar::aux::gui::Configurable::translateParameterRemovedSignal,
                                          this,
                                          index,
                                          _1
                                        )
                                      );
}

void cedar::aux::gui::Configurable::append(size_t configurableIndex, cedar::aux::ConfigurablePtr configurable, QTreeWidgetItem* pItem, const std::string& pathSoFar)
{
  QTreeWidgetItem* advanced_node = new QTreeWidgetItem();
  // the advanced node needs to be added here already as some properties (e.g., setFirstColumnSpanned) will otherwise not work
  pItem->addChild(advanced_node);
  this->makeHeading(advanced_node, "advanced", 3);
  advanced_node->setExpanded(false);

  for (auto parameter : configurable->getParameters())
  {
    auto parent = pItem;
    if (parameter->isAdvanced())
    {
      parent = advanced_node;
    }
    this->append(configurableIndex, parameter, parent, pathSoFar);
  }

  for (auto name_child_pair : configurable->configurableChildren())
  {
    const std::string& child_name = name_child_pair.first;
    cedar::aux::ConfigurablePtr child = name_child_pair.second;

    std::string path = pathSoFar;
    if (!path.empty())
    {
      path += ".";
    }
    path += child_name;

    QTreeWidgetItem* parent_node = pItem;
    if (child->isAdvanced())
    {
      parent_node = advanced_node;
    }

    auto child_item = this->appendHeading(parent_node, QString::fromStdString(child_name), 2);
    this->append(configurableIndex, child, child_item, path);
    child_item->setExpanded(true);
  }

  if (advanced_node->childCount() == 0)
  {
    pItem->removeChild(advanced_node);
  }
}

void cedar::aux::gui::Configurable::append(size_t configurableIndex, cedar::aux::ParameterPtr parameter, QTreeWidgetItem* pNode, const std::string& pathSoFar)
{
  if (parameter->isHidden())
  {
    return;
  }

  this->mParameterRenamedConnections[parameter.get()] =
                  parameter->connectToNameChangedSignal
                  (
                    boost::bind
                    (
                      &cedar::aux::gui::Configurable::translateParameterNameChangedSignal,
                      this,
                      configurableIndex,
                      _1,
                      _2
                    )
                  );

  std::string path = pathSoFar;
  if (!pathSoFar.empty())
  {
    path += ".";
  }
  path += parameter->getName();

  ParameterItem* parameter_item = new ParameterItem(parameter, path);
  pNode->addChild(parameter_item);
  this->mpPropertyTree->openPersistentEditor(parameter_item, PARAMETER_EDITOR_COLUMN);
  this->updateChangeState(parameter_item, parameter.get());
  this->updateLinkState(parameter_item, parameter.get());

  QObject::connect(parameter.get(), SIGNAL(changedFlagChanged()), this, SLOT(parameterChangeFlagChanged()));

  // check if parameter has a single configurable child
  if (parameter->hasSingleConfigurableChild())
  {
    this->append(configurableIndex, parameter->getSingleConfigurableChild(), parameter_item, path);
    QObject::connect(parameter.get(), SIGNAL(valueChanged()), this, SLOT(objectParameterValueChanged()));
    parameter_item->setExpanded(true);
  }
  // check if parameter is an object list parameter
  else if (parameter->canHaveConfigurableChildren())
  {
    QObject::connect(parameter.get(), SIGNAL(valueChanged()), this, SLOT(objectListParameterValueChanged()));

    this->appendObjectListParameter(configurableIndex, parameter, parameter_item, path);
    parameter_item->setExpanded(true);
  }
}

void cedar::aux::gui::Configurable::appendObjectListParameter
(
  size_t configurableIndex,
  cedar::aux::ParameterPtr objectListParameter,
  QTreeWidgetItem* pParent,
  const std::string& path
)
{
  for (size_t i = 0; i < objectListParameter->getNumberOfConfigurableChildren(); ++i)
  {
    cedar::aux::ConfigurablePtr configurable = objectListParameter->getConfigurableChild(i);
    std::string subpath = path + "[" + objectListParameter->childIndexToString(i) + "]";
    QString label = QString(QString::fromStdString(objectListParameter->getName()) + " [%1]").arg(i);
    auto sub_item = this->appendHeading(pParent, label, 2);
    this->append(configurableIndex, configurable, sub_item, subpath);
    sub_item->setExpanded(true);
  }
}

void cedar::aux::gui::Configurable::objectListParameterValueChanged()
{
  auto p_parameter = dynamic_cast<cedar::aux::Parameter*>(QObject::sender());
  CEDAR_DEBUG_ASSERT(p_parameter != nullptr);
  CEDAR_DEBUG_ASSERT(p_parameter->canHaveConfigurableChildren());

  auto item = this->getItemForParameter(p_parameter);

  if (item == nullptr)
  {
    cedar::aux::LogSingleton::getInstance()->debugMessage
    (
      "Could not update object parameter \"" + p_parameter->getName() + "\": corresponding item not found.",
      "void cedar::aux::gui::Configurable::objectListParameterValueChanged()"
    );
    return;
  }

  // iterate in reverse because we erase children, thus changing the index of all following children
  for (int child_index = item->childCount() - 1; child_index >= 0; --child_index)
  {
    auto child_item = item->child(child_index);
    this->disconnect(child_item);
    item->removeChild(child_item);
  }

  size_t i = static_cast<size_t>(this->getRootIndexForItem(item));
  this->appendObjectListParameter(i, p_parameter, item, this->getPathFromItem(item).toStdString());
}

void cedar::aux::gui::Configurable::objectParameterValueChanged()
{
  auto p_parameter = dynamic_cast<cedar::aux::Parameter*>(QObject::sender());
  CEDAR_DEBUG_ASSERT(p_parameter != nullptr);
  CEDAR_DEBUG_ASSERT(p_parameter->hasSingleConfigurableChild());

  auto item = this->getItemForParameter(p_parameter);

  if (item == nullptr)
  {
    cedar::aux::LogSingleton::getInstance()->debugMessage
    (
      "Could not update object parameter \"" + p_parameter->getName() + "\": corresponding item not found.",
      "void cedar::aux::gui::Configurable::parameterChangeFlagChanged()"
    );
    return;
  }

  // iterate in reverse because we erase children, thus changing the index of all following children
  for (int child_index = item->childCount() - 1; child_index >= 0; --child_index)
  {
    auto child_item = item->child(child_index);
    this->disconnect(child_item);
    item->removeChild(child_item);
  }
  
  size_t i = static_cast<size_t>(this->getRootIndexForItem(item));
  this->append(i, p_parameter->getSingleConfigurableChild(), item, this->getPathFromItem(item).toStdString());
}

QString cedar::aux::gui::Configurable::getPathFromItem(QTreeWidgetItem* item)
{
  return item->data(PARAMETER_EDITOR_COLUMN, Qt::UserRole).toString();
}

void cedar::aux::gui::Configurable::disconnect(QTreeWidgetItem* item)
{
  auto parameter = static_cast<cedar::aux::Parameter*>(item->data(PARAMETER_NAME_COLUMN, Qt::UserRole).value<void*>());
  if (parameter != nullptr)
  {
    QObject::disconnect(parameter, SIGNAL(changedFlagChanged()), this, SLOT(parameterChangeFlagChanged()));

    if (parameter->hasSingleConfigurableChild())
    {
      QObject::disconnect(parameter, SIGNAL(valueChanged()), this, SLOT(objectParameterValueChanged()));
    }
    else if (parameter->canHaveConfigurableChildren())
    {
      QObject::disconnect(parameter, SIGNAL(valueChanged()), this, SLOT(objectListParameterValueChanged()));
    }
  }
}

void cedar::aux::gui::Configurable::clear()
{
  for (QTreeWidgetItemIterator iter(this->mpPropertyTree); *iter != nullptr; ++iter)
  {
    this->mpPropertyTree->closePersistentEditor(*iter, PARAMETER_EDITOR_COLUMN);
    this->disconnect(*iter);
  }

  this->mpPropertyTree->clear();
  QAbstractItemDelegate * p_delegate = this->mpPropertyTree->itemDelegateForColumn(PARAMETER_EDITOR_COLUMN);
  if (p_delegate != nullptr)
  {
    delete p_delegate;
  }

  for (auto parameter_connetion_pair : this->mParameterRenamedConnections)
  {
    parameter_connetion_pair.second.disconnect();
  }
  this->mParameterRenamedConnections.clear();

  this->mParameterAddedConnection.disconnect();
  this->mParameterRemovedConnection.disconnect();
  this->mDisplayedConfigurables.clear();
}

void cedar::aux::gui::Configurable::parameterChangeFlagChanged()
{
  cedar::aux::Parameter *p_parameter = dynamic_cast<cedar::aux::Parameter*>(QObject::sender());
  CEDAR_DEBUG_ASSERT(p_parameter != nullptr);

  auto item = this->getItemForParameter(p_parameter);

  if (item != nullptr)
  {
    this->updateChangeState(item, p_parameter);
  }
  else
  {
    cedar::aux::LogSingleton::getInstance()->debugMessage
    (
      "Could not update changed state of parameter \"" + p_parameter->getName() + "\": corresponding item not found.",
      "void cedar::aux::gui::Configurable::parameterChangeFlagChanged()"
    );
  }
}

QTreeWidgetItem* cedar::aux::gui::Configurable::getItemForParameter(cedar::aux::Parameter* parameter)
{
  CEDAR_DEBUG_ASSERT(parameter != nullptr);

  for (QTreeWidgetItemIterator iter(this->mpPropertyTree); *iter != nullptr; ++iter)
  {
    auto item = *iter;
    if (item->data(PARAMETER_NAME_COLUMN, Qt::UserRole).value<void*>() == static_cast<void*>(parameter))
    {
      return item;
    }
  }

  return nullptr;
}

void cedar::aux::gui::Configurable::updateLinkState(QTreeWidgetItem* item, cedar::aux::Parameter* pParameter)
{
  QFont font = item->font(PARAMETER_NAME_COLUMN);
  font.setItalic(pParameter->isLinked());
  item->setFont(PARAMETER_NAME_COLUMN, font);

  if (pParameter->isLinked())
  {
    item->setTextColor(PARAMETER_NAME_COLUMN, QColor::fromRgb(0, 0, 128));
  }
}

void cedar::aux::gui::Configurable::updateChangeState(QTreeWidgetItem* item, cedar::aux::Parameter* pParameter)
{
  QFont font = item->font(PARAMETER_NAME_COLUMN);
  font.setBold(pParameter->isChanged());
  item->setFont(PARAMETER_NAME_COLUMN, font);
}
