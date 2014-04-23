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

    File:        Group.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 19

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/gui/Group.h"
#include "cedar/processing/gui/Connection.h"
#include "cedar/processing/gui/StepItem.h"
#include "cedar/processing/gui/TriggerItem.h"
#include "cedar/processing/gui/DataSlotItem.h"
#include "cedar/processing/gui/ConnectorItem.h"
#include "cedar/processing/gui/Settings.h"
#include "cedar/processing/gui/exceptions.h"
#include "cedar/processing/sources/GroupSource.h"
#include "cedar/processing/sinks/GroupSink.h"
#include "cedar/processing/Step.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/DataConnection.h"
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/Data.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/processing/exceptions.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/casts.h"
#include "cedar/auxiliaries/Recorder.h"
#include "cedar/processing/gui/StickyNote.h"
#include "cedar/processing/gui/GroupParameterDesigner.h"

// SYSTEM INCLUDES
#include <QEvent>
#include <QMenu>
#include <QAction>
#include <QInputDialog>
#include <QMessageBox>
#include <QGraphicsSceneContextMenuEvent>
#include <QSet>
#include <QList>
#include <QDialog>
#ifndef Q_MOC_RUN
  #include <boost/property_tree/json_parser.hpp>
#endif
#include <iostream>
#include <set>

//----------------------------------------------------------------------------------------------------------------------
// static members
//----------------------------------------------------------------------------------------------------------------------

const qreal cedar::proc::gui::Group::M_EXPANDED_SLOT_OFFSET = static_cast<qreal>(25);

const qreal cedar::proc::gui::Group::M_EXPANDED_ICON_SIZE = static_cast<qreal>(20);
const qreal cedar::proc::gui::Group::M_COLLAPSED_ICON_SIZE = cedar::proc::gui::StepItem::mIconSize;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::Group::Group
(
  QMainWindow *pMainWindow,
  cedar::proc::gui::Scene* scene,
  qreal width,
  qreal height,
  cedar::proc::GroupPtr group
)
:
cedar::proc::gui::Connectable(width, height, cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_GROUP),
mGroup(group),
mpScene(scene),
mpMainWindow(pMainWindow),
mHoldFitToContents(false),
_mSmartMode(new cedar::aux::BoolParameter(this, "smart mode", false)),
mPlotGroupsNode(cedar::aux::ConfigurationNode()),
_mIsCollapsed(new cedar::aux::BoolParameter(this, "collapsed", false)),
_mUncollapsedWidth(new cedar::aux::DoubleParameter(this, "uncollapsed width", width)),
_mUncollapsedHeight(new cedar::aux::DoubleParameter(this, "uncollapsed height", height))
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);

  if (!mGroup)
  {
    mGroup = cedar::proc::GroupPtr(new cedar::proc::Group());
  }

  this->setElement(mGroup);
  this->setConnectable(mGroup);

  this->setFlags(this->flags() | QGraphicsItem::ItemIsSelectable
                               | QGraphicsItem::ItemIsMovable
                               );

  mpNameDisplay = new QGraphicsTextItem(this);
  this->groupNameChanged();


  this->setCollapsed(false);
  this->updateCollapsedness();

  cedar::aux::ParameterPtr name_param = this->getGroup()->getParameter("name");
  QObject::connect(name_param.get(), SIGNAL(valueChanged()), this, SLOT(groupNameChanged()));
  QObject::connect(_mSmartMode.get(), SIGNAL(valueChanged()), this, SLOT(toggleSmartConnectionMode()));
  QObject::connect
  (
    this,
    SIGNAL(signalDataConnectionChange(QString, QString, QString, QString, cedar::proc::Group::ConnectionChange)),
    this,
    SLOT(dataConnectionChanged(QString, QString, QString, QString, cedar::proc::Group::ConnectionChange))
  );
  cedar::aux::ParameterPtr looped_param = this->getGroup()->getParameter("is looped");
  QObject::connect(looped_param.get(), SIGNAL(valueChanged()), this, SLOT(updateDecorations()));

  mDataConnectionChangedConnection = mGroup->connectToDataConnectionChangedSignal
                                     (
                                       boost::bind(&cedar::proc::gui::Group::checkDataConnection, this, _1, _2, _3)
                                     );
  mTriggerConnectionChangedConnection = mGroup->connectToTriggerConnectionChangedSignal
                                        (
                                          boost::bind
                                          (
                                            &cedar::proc::gui::Group::checkTriggerConnection,
                                            this,
                                            _1,
                                            _2,
                                            _3
                                          )
                                        );

  mNewElementAddedConnection
    = mGroup->connectToNewElementAddedSignal
      (
        boost::bind(&cedar::proc::gui::Group::processElementAddedSignal, this, _1)
      );
  mElementRemovedConnection
    = mGroup->connectToElementRemovedSignal
      (
        boost::bind(&cedar::proc::gui::Group::processElementRemovedSignal, this, _1)
      );

  this->connect(this->_mIsCollapsed.get(), SIGNAL(valueChanged()), SLOT(updateCollapsedness()));

  QObject::connect
  (
    this->mGroup.get(),
    SIGNAL(stepNameChanged(const std::string&, const std::string&)),
    this,
    SLOT(handleStepNameChanged(const std::string&, const std::string&))
  );
  this->addDecorations();
  this->update();
}

cedar::proc::gui::Group::~Group()
{
  cedar::aux::LogSingleton::getInstance()->freeing(this);

  if (mNewElementAddedConnection.connected())
  {
    mNewElementAddedConnection.disconnect();
  }
  if (mElementRemovedConnection.connected())
  {
    mElementRemovedConnection.disconnect();
  }
  if (mElementRemovedConnection.connected())
  {
    mElementRemovedConnection.disconnect();
  }
  if (mDataConnectionChangedConnection.connected())
  {
    mDataConnectionChangedConnection.disconnect();
  }
  if (mTriggerConnectionChangedConnection.connected())
  {
    mTriggerConnectionChangedConnection.disconnect();
  }
  if (this->scene())
  {
    cedar::aux::asserted_cast<cedar::proc::gui::Scene*>(this->scene())->removeGroupItem(this);
  }
  else
  {
//    CEDAR_DEBUG_ASSERT(this->isRootGroup());
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::Group::itemSceneHasChanged()
{
  this->addGuiItemsForGroup();
}

void cedar::proc::gui::Group::addGuiItemsForGroup()
{
  CEDAR_DEBUG_ASSERT(this->getGroup());

  auto elements = this->getGroup()->getElements();
  for (auto iter = elements.begin(); iter != elements.end(); ++iter)
  {
    auto element = iter->second;
    this->processElementAddedSignal(element);
  }
}

cedar::proc::gui::GraphicsBase* cedar::proc::gui::Group::getUiElementFor(cedar::proc::ElementPtr element) const
{
  return this->getScene()->getGraphicsItemFor(element.get());
}


void cedar::proc::gui::Group::duplicate(const QPointF& scenePos, const std::string& elementName, const std::string& newName)
{
  auto to_duplicate = this->getGroup()->getElement(elementName);
  auto to_duplicate_ui = this->getUiElementFor(to_duplicate);

  cedar::aux::ConfigurationNode conf;
  to_duplicate_ui->writeConfiguration(conf);

  std::string new_name = this->getGroup()->duplicate(elementName, newName);

  auto duplicate = this->getGroup()->getElement(new_name);
  auto duplicate_ui = this->getUiElementFor(duplicate);
  duplicate_ui->readConfiguration(conf);
  duplicate_ui->setPos(scenePos);

  // restore connections if this is another group
  if (auto group_item = dynamic_cast<cedar::proc::gui::Group*>(duplicate_ui))
  {
    group_item->restoreConnections();
  }
}

void cedar::proc::gui::Group::groupNameChanged()
{
  this->mpNameDisplay->setTextWidth(this->width());
  QString name = QString::fromStdString(this->getGroup()->getName());
  if (this->isCollapsed())
  {
    this->mpNameDisplay->setHtml(name);
  }
  else
  {
    this->mpNameDisplay->setHtml("<center>" + name + "</center>");
  }

  this->updateTextBounds();
}

QVariant cedar::proc::gui::Group::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant & value)
{
  cedar::proc::gui::GraphicsBase::GraphicsGroup filtered_groups = 0;
  filtered_groups |= cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_GROUP;
  filtered_groups |= cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_STEP;
  filtered_groups |= cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_TRIGGER;

  switch (change)
  {
    case QGraphicsItem::ItemChildAddedChange:
    {
      cedar::proc::gui::GraphicsBase *p_item
        = dynamic_cast<cedar::proc::gui::GraphicsBase*>(value.value<QGraphicsItem*>());
      if(p_item && (p_item->getGroup() & filtered_groups) != 0)
      {
        p_item->installSceneEventFilter(this);
      }
      return value;
    }

    case QGraphicsItem::ItemChildRemovedChange:
    {
      cedar::proc::gui::GraphicsBase *p_item
        = dynamic_cast<cedar::proc::gui::GraphicsBase*>(value.value<QGraphicsItem*>());
      if(p_item && (p_item->getGroup() & filtered_groups) != 0)
      {
        p_item->removeSceneEventFilter(this);
      }
      return value;
    }

    default:
      return this->cedar::proc::gui::GraphicsBase::itemChange(change, value);
  }
}

void cedar::proc::gui::Group::sizeChanged()
{
  this->cedar::proc::gui::Connectable::sizeChanged();

  this->updateTextBounds();
  this->updateIconBounds();
  this->updateConnectorPositions();

  // remember the current size for restoring it later
  if (!this->isCollapsed())
  {
    this->_mUncollapsedWidth->setValue(this->width());
    this->_mUncollapsedHeight->setValue(this->height());
  }
  this->updateDecorationPositions();
}

void cedar::proc::gui::Group::updateTextBounds()
{
  qreal bounds_factor = static_cast<qreal>(2);
  if (this->isCollapsed())
  {
    bounds_factor = static_cast<qreal>(1);
  }
  this->mpNameDisplay->setTextWidth
  (
    std::max
    (
      static_cast<qreal>(50),
      this->width() - bounds_factor * this->getIconSizeForCurrentMode()
    )
  );

  this->mpNameDisplay->setX(this->getIconSizeForCurrentMode());
}

qreal cedar::proc::gui::Group::getIconSizeForCurrentMode() const
{
  if (this->isCollapsed())
  {
    return cedar::proc::gui::Group::M_COLLAPSED_ICON_SIZE;
  }
  else
  {
    return cedar::proc::gui::Group::M_EXPANDED_ICON_SIZE;
  }
}

void cedar::proc::gui::Group::updateIconBounds()
{
  qreal padding = static_cast<qreal>(2);
  if (this->isCollapsed())
  {
    qreal y = (this->height() - cedar::proc::gui::Group::M_COLLAPSED_ICON_SIZE) / static_cast<qreal>(2);
    this->setIconBounds(padding, y, cedar::proc::gui::Group::M_COLLAPSED_ICON_SIZE - 2 * padding);
  }
  else
  {
    this->setIconBounds(padding, padding, cedar::proc::gui::Group::M_EXPANDED_ICON_SIZE - 2 * padding);
  }
}


//!@todo Can this method be removed?
bool cedar::proc::gui::Group::sceneEventFilter(QGraphicsItem * pWatched, QEvent *pEvent)
{
  if (!dynamic_cast<cedar::proc::gui::GraphicsBase*>(pWatched))
  {
    return cedar::proc::gui::GraphicsBase::sceneEventFilter(pWatched, pEvent);
  }
  return cedar::proc::gui::GraphicsBase::sceneEventFilter(pWatched, pEvent);
}

void cedar::proc::gui::Group::fitToContents(bool grow)
{
  if (this->isCollapsed() || this->scene() == NULL || mHoldFitToContents)
  {
    return;
  }

  //!@todo This function should take the grid size into account!

  qreal padding_top = static_cast<qreal>(5.0);
  qreal padding_bottom = static_cast<qreal>(5.0);
  qreal padding_left = static_cast<qreal>(5.0);
  qreal padding_right = static_cast<qreal>(5.0);

  // when no children are present, we cannot fit them
  if (this->childItems().empty())
  {
    return;
  }

  // get the set of child items
  QSet<QGraphicsItem*> children = this->childItems().toSet();

  // find the bounding box of all children
  QRectF bounds;
  for (QSet<QGraphicsItem*>::iterator i = children.begin(); i != children.end(); ++i)
  {
    QGraphicsItem* p_item = *i;
    if (!dynamic_cast<cedar::proc::gui::Connectable*>(p_item))
    {
      continue;
    }

    QRectF item_bounds = p_item->mapRectToParent(p_item->boundingRect());

    if (bounds.isEmpty())
    {
      bounds = item_bounds;
    }
    else
    {
      bounds |= item_bounds;
    }
  }


  // adjust the bow by the paddings specified above
  bounds.adjust(-padding_left, -padding_top, padding_right, padding_bottom);

  if (grow)
  {
    bounds |= this->boundingRect();
  }

  // apply the new bounding box
  QPointF old_pos_scene = this->scenePos();
  this->setPos(this->pos() + bounds.topLeft());
  this->setSize(bounds.width(), bounds.height());

  // setting a new position moves the children, thus, transform them back to keep their original positions
  QPointF old_pos_local = this->mapFromScene(old_pos_scene);
  // using a set avoids moving the same child more than once
  for (QSet<QGraphicsItem*>::iterator i = children.begin(); i != children.end(); ++i)
  {
    QGraphicsItem* p_item = *i;
    if (!dynamic_cast<cedar::proc::gui::Connectable*>(p_item))
    {
      continue;
    }
    p_item->setPos(old_pos_local + p_item->pos());
  }

  // finally, also resize parent item if it is a group
  if (cedar::proc::gui::Group *p_parent_group = dynamic_cast<cedar::proc::gui::Group *>(this->parentItem()))
  {
    p_parent_group->fitToContents(grow);
  }
}

bool cedar::proc::gui::Group::isRootGroup()
{
  return this->mpScene && this == this->mpScene->getRootGroup().get();
}

void cedar::proc::gui::Group::transformChildCoordinates(cedar::proc::gui::GraphicsBase* pItem)
{
  pItem->setPos(this->mapFromItem(pItem, QPointF(0, 0)));
}

bool cedar::proc::gui::Group::canAddAny(const QList<QGraphicsItem*>& items) const
{
  for (int i = 0; i < items.size(); ++i)
  {
    //!@todo This should cast to a cedar::proc::gui::Element class.
    if (auto connectable = dynamic_cast<cedar::proc::gui::Connectable*>(items.at(i)))
    {
      if (connectable->getConnectable()->getGroup() != this->getGroup())
      {
        return true;
      }
    }
  }

  return false;
}

void cedar::proc::gui::Group::addElements(const std::list<QGraphicsItem*>& elements)
{
  std::list<cedar::proc::ElementPtr> elements_to_move;
  std::list<cedar::proc::ElementPtr> all_elements;
  for (auto it = elements.begin(); it != elements.end(); ++it)
  {
    cedar::proc::ElementPtr element;
    //!@todo This if/else if stuff could probably be replaced by just casting to a common cedar::proc::gui::Element class.
    if (auto graphics_base = dynamic_cast<cedar::proc::gui::GraphicsBase*>(*it))
    {
      element = graphics_base->getElement();

      std::vector<QGraphicsItem*> items;
      items.push_back(graphics_base);
      while (!items.empty())
      {
        auto item = *items.begin();
        items.erase(items.begin());

        if (auto graphics_child = dynamic_cast<cedar::proc::gui::GraphicsBase*>(item))
        {
          auto child_element = graphics_child->getElement();
          // some objects such as data slots may not have an element
          //!@todo Cast to a common superclass, proc::gui::Element here.
          if (child_element)
          {
            all_elements.push_back(child_element);

            for (int i = 0; i < graphics_child->childItems().size(); ++i)
            {
              items.push_back(graphics_child->childItems().at(i));
            }
          }
        }
      }
    }
    else if (dynamic_cast<cedar::proc::gui::Connection*>(*it))
    {
      // connections are ignored, the underlying framework takes care of them
      continue;
    }
    else
    {
      CEDAR_THROW
      (
        cedar::aux::UnhandledTypeException,
        "cedar::proc::gui::Group::addElements cannot handle this type of QGraphicsItem."
      )
    }
    CEDAR_DEBUG_ASSERT(element);
    elements_to_move.push_back(element);
    all_elements.push_back(element);
  }
  //!@todo This member can probably be removed
  this->mHoldFitToContents = true;

  std::map<cedar::proc::ElementPtr, QPointF> item_scene_pos;
  std::map<cedar::proc::ElementPtr, cedar::aux::ConfigurationNode> item_configs;
  for (auto it = all_elements.begin(); it != all_elements.end(); ++it)
  {
    auto element = *it;
    CEDAR_DEBUG_ASSERT(element != NULL);

    auto graphics_item = this->mpScene->getGraphicsItemFor(element.get());
    // remember the item's scene positions
    item_scene_pos[element] = graphics_item->scenePos();

    // remember the item's configuration
    cedar::aux::ConfigurationNode config;
    graphics_item->writeConfiguration(config);
    item_configs[element] = config;
  }

  this->getGroup()->add(elements_to_move);

  for (auto element : all_elements)
  {
    auto graphics_item = this->mpScene->getGraphicsItemFor(element.get());

    // restore the item's configuration
    // this has to be done before position restoration as it also reads the item's old position (but in the wrong
    // reference frame)
    auto config_it = item_configs.find(element);
    CEDAR_DEBUG_ASSERT(config_it != item_configs.end());
    graphics_item->readConfiguration(config_it->second);

    // restore the item's position
    CEDAR_DEBUG_ASSERT(item_scene_pos.find(element) != item_scene_pos.end());
    const QPointF& scene_pos = item_scene_pos[element];
    auto parent = graphics_item->parentItem();
    if(parent == NULL)
    {
      parent = this;
    }

    graphics_item->setPos(parent->mapFromScene(scene_pos));
  }

  for (auto element : all_elements)
  {
    auto graphics_item = this->mpScene->getGraphicsItemFor(element.get());
    // if this is a group, restore all connections
    if (auto group_item = dynamic_cast<cedar::proc::gui::Group*>(graphics_item))
    {
      group_item->restoreConnections();
    }
  }

  this->mHoldFitToContents = false;
}

void cedar::proc::gui::Group::addElement(cedar::proc::gui::GraphicsBase *pElement)
{
  //!@todo Does this method do what its name suggests? If so, it should call addElements with a list containing only the given element.
  // reset parent item
  pElement->setParentItem(0);
}

const std::string& cedar::proc::gui::Group::getFileName() const
{
  return this->mFileName;
}

//!@todo Can this method be removed?
void cedar::proc::gui::Group::addElementsToScene()
{
}

void cedar::proc::gui::Group::setScene(cedar::proc::gui::Scene* pScene)
{
  //!@todo Why doesn't this use QGraphicsItem->scene() instead?
  // currently, switching the scene is not supported.
  CEDAR_ASSERT(this->mpScene == pScene || this->mpScene == NULL);

  this->mpScene = pScene;
}

cedar::proc::GroupPtr cedar::proc::gui::Group::getGroup()
{
  return this->mGroup;
}

cedar::proc::ConstGroupPtr cedar::proc::gui::Group::getGroup() const
{
  return this->mGroup;
}

void cedar::proc::gui::Group::write()
{
  this->write(this->mFileName);
}

void cedar::proc::gui::Group::write(const std::string& destination)
{
  this->mFileName = destination;
  cedar::aux::RecorderSingleton::getInstance()->setRecordedProjectName(mFileName);

  cedar::aux::ConfigurationNode root;

  this->mGroup->writeConfiguration(root);
  this->writeConfiguration(root);

  write_json(destination, root);
}

void cedar::proc::gui::Group::read(const std::string& source)
{
  this->mFileName = source;
  cedar::aux::RecorderSingleton::getInstance()->setRecordedProjectName(mFileName);

  cedar::aux::ConfigurationNode root;
  read_json(source, root);

  this->mGroup->readConfiguration(root);
  this->readConfiguration(root);
}

void cedar::proc::gui::Group::readConfiguration(const cedar::aux::ConfigurationNode& root)
{
  if (root.find("ui generic") != root.not_found())
  {
    auto node = root.get_child("ui generic");
    this->cedar::proc::gui::Connectable::readConfiguration(node);
    // restore plots that were open when architecture was last saved
    auto plot_list = node.find("open plots");
    if (plot_list != node.not_found())
    {
      this->readPlotList("open plots", plot_list->second);
    }
    // read defined plot groups
    auto plot_groups = node.find("plot groups");
    if (plot_groups != node.not_found())
    {
      this->mPlotGroupsNode = plot_groups->second;
    }

    auto color_node = node.find("background color");
    if (color_node != node.not_found())
    {
      std::string color_str = color_node->second.get_value<std::string>();
      std::vector<std::string> color_strs;
      cedar::aux::split(color_str, ",", color_strs);
      if (color_strs.size() == 3)
      {
        int r = cedar::aux::fromString<int>(color_strs.at(0));
        int g = cedar::aux::fromString<int>(color_strs.at(1));
        int b = cedar::aux::fromString<int>(color_strs.at(2));

        this->setBackgroundColor(QColor(r, g, b));
      }
    }
  }
  else
  {
    this->toggleSmartConnectionMode(false);
  }

  //read sticky notes
  if (root.find("ui") != root.not_found())
  {
    this->readStickyNotes(root.get_child("ui"));
  }
  //update recorder icons
  this->stepRecordStateChanged();
}

void cedar::proc::gui::Group::readPlotList(const std::string& plotGroupName, const cedar::aux::ConfigurationNode& node)
{
  std::set<std::string> removed_elements;
  for(auto it : node)
  {
    std::string step_name = cedar::proc::gui::PlotWidget::getStepNameFromConfiguration(it.second);
    try
    {
      auto step = this->getGroup()->getElement<cedar::proc::Step>(step_name);
      auto step_item = this->mpScene->getStepItemFor(step.get());
      cedar::proc::gui::PlotWidget::createAndShowFromConfiguration(it.second, step_item);
    }
    catch (cedar::aux::InvalidNameException& exc)
    {
      removed_elements.insert(step_name);
    }
  }
  if (removed_elements.size() > 0)
  {
    std::string message;
    message += "Some elements of the plot group " + plotGroupName + " do not exist anymore. These are:\n\n";
    for (auto element : removed_elements)
    {
      message += "  " + element + "\n";
    }
    message += "\nDo you want to remove them?";

    QMessageBox msgBox(this->mpMainWindow);
    msgBox.addButton(QMessageBox::Yes);
    msgBox.addButton(QMessageBox::No);
    msgBox.setWindowTitle("Missing elements");
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText(QString::fromStdString(message));

    int selection = msgBox.exec();
    if (selection == QMessageBox::Yes)
    {
      for (auto remove : removed_elements)
      {
        this->removeElementFromPlotGroup(plotGroupName, remove);
      }
    }
  }
}

void cedar::proc::gui::Group::writeConfiguration(cedar::aux::ConfigurationNode& root) const
{
  this->writeScene(root);

  cedar::aux::ConfigurationNode generic;

  //!@todo Does the name/group thing actually do anything, or can it be removed?
  generic.put("group", this->mGroup->getName());
  //!@todo There is a more elegant way to do this - investigate!
  if (this->getGroup()->isRoot())
  {
    // add open plots to architecture
    cedar::aux::ConfigurationNode node;
    this->writeOpenPlotsTo(node);
    generic.put_child("open plots", node);
    // add plot groups to architecture
    generic.put_child("plot groups", this->mPlotGroupsNode);
  }

  if (this->mBackgroundColor.isValid())
  {
    std::stringstream color_str;
    color_str << this->mBackgroundColor.red() << ","
        << this->mBackgroundColor.green() << ","
        << this->mBackgroundColor.blue();
    generic.put("background color", color_str.str());
  }

  this->cedar::proc::gui::Connectable::writeConfiguration(generic);

  root.add_child("ui generic", generic);
}

void cedar::proc::gui::Group::writeOpenPlotsTo(cedar::aux::ConfigurationNode& node) const
{
  for (auto step_map_item : this->mpScene->getStepMap())
  {
    step_map_item.second->writeOpenChildWidgets(node);
  }
}

void cedar::proc::gui::Group::writeScene(cedar::aux::ConfigurationNode& root) const
{
  cedar::aux::ConfigurationNode scene;
  
  std::vector<cedar::proc::gui::StickyNote*> stickyNotes = this->mpScene->getStickyNotes();

  for(cedar::proc::gui::StickyNote* note : stickyNotes)
  {
    cedar::aux::ConfigurationNode node;
    node.put("type","stickyNote");
    QRectF rect = note->boundingRect();
    node.put("width",rect.width());
    node.put("height",rect.height());
    node.put("x",note->scenePos().x());
    node.put("y",note->scenePos().y());
    node.put("text",note->getText());
    scene.push_back(cedar::aux::ConfigurationNode::value_type("", node));
  }

  auto elements = this->getGroup()->getElements();

  for
  (
    cedar::proc::Group::ElementMap::const_iterator element_iter = elements.begin();
    element_iter != elements.end();
    ++element_iter
  )
  {
    cedar::proc::ElementPtr element = element_iter->second;

    if
    (
      boost::dynamic_pointer_cast<cedar::proc::sources::GroupSource>(element)
      || boost::dynamic_pointer_cast<cedar::proc::sinks::GroupSink>(element)
    )
    {
      continue;
    }

    cedar::proc::gui::GraphicsBase *p_item = this->mpScene->getGraphicsItemFor(element.get());
    CEDAR_ASSERT(p_item != NULL);

    cedar::aux::ConfigurationNode node;
    switch (p_item->getGroup())
    {
      case cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_STEP:
        node.put("type", "step");
        break;

      case cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_TRIGGER:
        node.put("type", "trigger");
        break;

      case cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_GROUP:
      {
        // move along, nothing to do here
        node.put("type", "group");
        break;
      }

      default:
        continue;
    }

    if (p_item->getGroup() != cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_GROUP)
    {
      p_item->writeConfiguration(node);
      scene.push_back(cedar::aux::ConfigurationNode::value_type("", node));
    }


    // write UI information of the group item
    cedar::proc::GroupPtr group = boost::dynamic_pointer_cast<cedar::proc::Group>(element);
    cedar::proc::gui::Group *p_group_item = dynamic_cast<cedar::proc::gui::Group*>(p_item);
    if (group && p_group_item)
    {
      cedar::aux::ConfigurationNode::assoc_iterator groups_node = root.find("groups");
      if (groups_node == root.not_found())
      {
        groups_node = root.find("networks");
        if (groups_node == root.not_found())
        {
          cedar::aux::ConfigurationNode groups;
          root.put_child("groups", groups);
          groups_node = root.find("groups");
          CEDAR_DEBUG_ASSERT(groups_node != root.not_found());
        }
      }

      // check if there is already a node for the group; if not, add it
      cedar::aux::ConfigurationNode::assoc_iterator group_node = groups_node->second.find(group->getName());
      if (group_node == groups_node->second.not_found())
      {
        groups_node->second.add_child(group->getName(), cedar::aux::ConfigurationNode());
        group_node = groups_node->second.find(group->getName());
      }

      p_group_item->writeConfiguration(group_node->second);
    }
  }

  if (!scene.empty())
    root.add_child("ui", scene);
}

void cedar::proc::gui::Group::disconnect()
{
}

void cedar::proc::gui::Group::checkDataConnection
     (
       cedar::proc::ConstDataSlotPtr source,
       cedar::proc::ConstDataSlotPtr target,
       cedar::proc::Group::ConnectionChange change
     )
{
  emit signalDataConnectionChange
       (
         QString::fromStdString(source->getParent()),
         QString::fromStdString(source->getName()),
         QString::fromStdString(target->getParent()),
         QString::fromStdString(target->getName()),
         change
       );
}

void cedar::proc::gui::Group::dataConnectionChanged
     (
       QString sourceName,
       QString sourceSlot,
       QString targetName,
       QString targetSlot,
       cedar::proc::Group::ConnectionChange change
     )
{
  cedar::proc::gui::DataSlotItem* source_slot = NULL;
  //!@todo Write a getGraphicsItemFor(QString/std::string) method in scene; also, maybe these items should be managed in gui::Group
  auto element = this->getGroup()->getElement(sourceName.toStdString());

  // if the source is a group source, get its slot from the group
  if (auto group_source = boost::dynamic_pointer_cast<cedar::proc::sources::GroupSource>(element))
  {
    for (size_t i = 0; i < this->mConnectorSources.size(); ++i)
    {
      auto connector_source = this->mConnectorSources.at(i);
      if (connector_source->getSlot()->getParent() == sourceName.toStdString())
      {
        // should always be "output" on both sides as that is a defined name that never changes.
        CEDAR_DEBUG_ASSERT(sourceSlot.toStdString() == connector_source->getName());
        source_slot = connector_source;
        break;
      }
    }
  }
  else
  {
    cedar::proc::gui::GraphicsBase* p_base_source = this->mpScene->getGraphicsItemFor(element.get());
    if (auto connectable = dynamic_cast<cedar::proc::gui::Connectable*>(p_base_source))
    {
      source_slot = connectable->getSlotItem(cedar::proc::DataRole::OUTPUT, sourceSlot.toStdString());
    }
  }

  CEDAR_ASSERT(source_slot);

  cedar::proc::gui::DataSlotItem* target_slot = NULL;
  auto target = this->getGroup()->getElement(targetName.toStdString());

  // if the target is a group sink, get its slot from the group
  if (auto group_source = boost::dynamic_pointer_cast<cedar::proc::sinks::GroupSink>(target))
  {
    for (size_t i = 0; i < this->mConnectorSinks.size(); ++i)
    {
      auto connector_target = this->mConnectorSinks.at(i);
      if (connector_target->getSlot()->getParent() == targetName.toStdString())
      {
        // should always be "output" on both sides as that is a defined name that never changes.
        CEDAR_DEBUG_ASSERT(targetSlot.toStdString() == connector_target->getName());
        target_slot = connector_target;
        break;
      }
    }
  }
  else
  {
    cedar::proc::gui::GraphicsBase* p_base
      = this->mpScene->getGraphicsItemFor(this->getGroup()->getElement(targetName.toStdString()).get());
    if (auto connectable = dynamic_cast<cedar::proc::gui::Connectable*>(p_base))
    {
      target_slot = connectable->getSlotItem(cedar::proc::DataRole::INPUT, targetSlot.toStdString());
    }
  }
  CEDAR_ASSERT(target_slot);

  switch (change)
  {
    case cedar::proc::Group::CONNECTION_ADDED:
    {
      cedar::proc::gui::Connection* p_con = source_slot->connectTo(target_slot);
      p_con->setSmartMode(this->getSmartConnection());
      break;
    }
    case cedar::proc::Group::CONNECTION_REMOVED:
    {
      QList<QGraphicsItem*> items = this->mpScene->items();
      for (int i = 0; i < items.size(); ++i)
      {
        if (cedar::proc::gui::Connection* con = dynamic_cast<cedar::proc::gui::Connection*>(items[i]))
        {
          if (con->getSource() == source_slot && con->getTarget() == target_slot)
          {
            con->disconnect();
            this->mpScene->removeItem(con);
            delete con;
          }
        }
      }
      break;
    }
    default:
    {
      CEDAR_ASSERT(false);
    }
  }
}

void cedar::proc::gui::Group::checkTriggerConnection
     (
       cedar::proc::TriggerPtr source,
       cedar::proc::TriggerablePtr target,
       bool added
     )
{
  /* A signal is emitted regardless of the missing representation. This fails in finding "processingDone" in the current
   * group and results in an InvalidNameException. This exception is caught here. A debug assert assures that no other
   * element caused this exception.
   */
  cedar::proc::gui::TriggerItem* source_element;
  try
  {
    source_element
      = dynamic_cast<cedar::proc::gui::TriggerItem*>
        (
          this->mpScene->getGraphicsItemFor
          (
            this->getGroup()->getElement(source->getName()).get()
          )
        );
  }
  catch(cedar::aux::InvalidNameException& exc)
  {
    CEDAR_ASSERT(source->getName() == "processingDone");
    return;
  }
  cedar::proc::gui::GraphicsBase* target_element
    = this->mpScene->getGraphicsItemFor
      (
        this->getGroup()->getElement(boost::dynamic_pointer_cast<cedar::proc::Element>(target)->getName()).get()
      );
  if (added)
  {
    cedar::proc::gui::Connection* p_con = source_element->connectTo(target_element);
    p_con->setSmartMode(this->getSmartConnection());
  }
  else
  {
    QList<QGraphicsItem*> items = this->mpScene->items();
    for (int i = 0; i < items.size(); ++i)
    {
      if (cedar::proc::gui::Connection* con = dynamic_cast<cedar::proc::gui::Connection*>(items[i]))
      {
        if (con->getSource() == source_element && con->getTarget() == target_element)
        {
          con->disconnect();
          this->mpScene->removeItem(con);
          delete con;
          return;
        }
      }
    }
  }
}

void cedar::proc::gui::Group::updateConnectorPositions()
{
//  qreal distance = 20;
  qreal pad_side = 5;
//  QPointF start_dist(pad_side, this->getInputOutputSlotOffset());
//  QPointF direction(0, 1);

  for (size_t i = 0; i < this->mConnectorSources.size(); ++i)
  {
    auto source = this->mConnectorSources.at(i);
    auto slot_item = this->getSlotItem(cedar::proc::DataRole::INPUT, source->getSlot()->getParentPtr()->getName());
    source->setPos(pad_side, slot_item->pos().y());
    source->setWidth(slot_item->width());
    source->setHeight(slot_item->height());
  }

  qreal connector_max_width = 0;
  for (size_t i = 0; i < this->mConnectorSinks.size(); ++i)
  {
    auto sink = this->mConnectorSinks.at(i);
    std::string slot_name = sink->getSlot()->getParentPtr()->getName();
    try
    {
      auto slot_item = this->getSlotItem(cedar::proc::DataRole::OUTPUT, slot_name);
      connector_max_width = std::max(connector_max_width, slot_item->width());
    }
    catch (const cedar::proc::InvalidRoleException&)
    {
      cedar::aux::LogSingleton::getInstance()->debugMessage
      (
        "Warning: could not find group output slot for connector \""
          + slot_name + "\". Slot item may not be positioned incorrectly in the scene.",
        "void cedar::proc::gui::Group::updateConnectorPositions()"
      );
    }
  }

  for (size_t i = 0; i < this->mConnectorSinks.size(); ++i)
  {
    auto sink = this->mConnectorSinks.at(i);
    try
    {
      auto slot_item = this->getSlotItem(cedar::proc::DataRole::OUTPUT, sink->getSlot()->getParentPtr()->getName());
      sink->setPos(this->width() - pad_side - connector_max_width, slot_item->pos().y());
      sink->setWidth(slot_item->width());
      sink->setHeight(slot_item->height());
    }
    catch (const cedar::proc::InvalidRoleException&)
    {
      //!@todo Quickfix; Why can this even happen?
    }
  }
}

void cedar::proc::gui::Group::processElementAddedSignal(cedar::proc::ElementPtr element)
{
  // store the type, which can be compared to entries in a configuration node
  std::string current_type;
  cedar::proc::gui::GraphicsBase* p_scene_element = NULL;

  // if connector, add the corresponding item
  if (auto connector = boost::dynamic_pointer_cast<cedar::proc::sources::GroupSource>(element))
  {
    auto connector_item = new cedar::proc::gui::DataSlotItem(NULL, connector->getOutputSlot("output"));
    this->mpScene->addItem(connector_item);
    mConnectorSources.push_back(connector_item);
    p_scene_element = connector_item;
    current_type = "connector";
  }
  else if (auto connector = boost::dynamic_pointer_cast<cedar::proc::sinks::GroupSink>(element))
  {
    auto connector_item = new cedar::proc::gui::DataSlotItem(NULL, connector->getInputSlot("input"));
    this->mpScene->addItem(connector_item);
    mConnectorSinks.push_back(connector_item);
    p_scene_element = connector_item;
    current_type = "connector";
  }
  // if step, add a step item
  else if (cedar::proc::StepPtr step = boost::dynamic_pointer_cast<cedar::proc::Step>(element))
  {
    this->mpScene->addProcessingStep(step, QPointF(0, 0));
    current_type = "step";

    p_scene_element = this->mpScene->getStepItemFor(step.get());
  }
  // if group, add a new group item
  else if (cedar::proc::GroupPtr group = boost::dynamic_pointer_cast<cedar::proc::Group>(element))
  {
    this->mpScene->addGroup(QPointF(0, 0), group);
    current_type = "group";
    p_scene_element = this->mpScene->getGroupFor(group.get());
  }
  // if the new element is a trigger, add a trigger item
  else if (cedar::proc::TriggerPtr trigger = boost::dynamic_pointer_cast<cedar::proc::Trigger>(element))
  {
    this->mpScene->addTrigger(trigger, QPointF(0, 0));
    current_type = "trigger";
    p_scene_element = this->mpScene->getTriggerItemFor(trigger.get());
  }
  CEDAR_ASSERT(p_scene_element != NULL);

  // if not a root group, properly add the item as a child
  if (this->mpScene && !this->isRootGroup())
  {
    CEDAR_ASSERT(p_scene_element != NULL);
    if (p_scene_element->parentItem() != this)
    {
//      this->transformChildCoordinates(p_scene_element);
      p_scene_element->setParentItem(this);
    }
  }

  // if there is a configuration stored for the UI of the element, load it
  std::map<cedar::proc::Element*, cedar::aux::ConfigurationNode>::iterator iter
    = this->mNextElementUiConfigurations.find(p_scene_element->getElement().get());
  if (iter != this->mNextElementUiConfigurations.end())
  {
    p_scene_element->readConfiguration(iter->second);
    this->mNextElementUiConfigurations.erase(iter);
  }

  // see if there is a configuration for the UI item stored in the group's ui node
  if (current_type == "group")
  {
    auto config = this->getGroup()->getLastReadConfiguration();
    auto groups_iter = config.find("groups");
    if (groups_iter == config.not_found())
    {
      groups_iter = config.find("networks"); // compatibility
    }

    if (groups_iter != config.not_found())
    {
      auto group = cedar::aux::asserted_cast<cedar::proc::gui::Group*>(p_scene_element);
      auto groups = groups_iter->second;
      auto group_iter = groups.find(group->getGroup()->getName());
      if (group_iter != groups.not_found())
      {
        group->readConfiguration(group_iter->second);
      }
    }
  }
  else
  {
    auto config = this->getGroup()->getLastReadConfiguration();
    auto ui_iter = config.find("ui");
    if (ui_iter != config.not_found())
    {
      cedar::aux::ConfigurationNode& ui = ui_iter->second;
      for (cedar::aux::ConfigurationNode::iterator iter = ui.begin(); iter != ui.end(); ++iter)
      {
        const std::string& type = iter->second.get<std::string>("type");
        if (type == current_type)
        {
          if (iter->second.get<std::string>(current_type) == element->getName())
          {
            p_scene_element->readConfiguration(iter->second);
            ui.erase(iter);
            break;
          }
        }
      }
    }
  }

  if (current_type == "connector")
  {
    this->updateConnectorPositions();
  }

  if (this->isCollapsed())
  {
    p_scene_element->hide();
  }
}

void cedar::proc::gui::Group::slotRemoved(cedar::proc::DataRole::Id role, const std::string& name)
{
  this->cedar::proc::gui::Connectable::slotRemoved(role, name);

  this->updateConnectorPositions();
}

void cedar::proc::gui::Group::removeConnectorItem(bool isSource, const std::string& name)
{
  auto p_list = &mConnectorSources;
  if (!isSource)
  {
    p_list = &mConnectorSinks;
  }

  for (size_t i = 0; i < p_list->size(); ++i)
  {
    auto p_data_slot = p_list->at(i);
    if (p_data_slot->getSlot()->getParentPtr()->getName() == name)
    {
      p_list->erase(p_list->begin() + i);
      delete p_data_slot;
      return;
    }
  }

  CEDAR_THROW(cedar::aux::UnknownNameException, "Could not find connector for data slot \"" + name + "\".");
}

void cedar::proc::gui::Group::processElementRemovedSignal(cedar::proc::ConstElementPtr element)
{

  if (auto connector = boost::dynamic_pointer_cast<cedar::proc::sources::ConstGroupSource>(element))
  {
    this->removeConnectorItem(true, element->getName());
  }
  else if (auto connector = boost::dynamic_pointer_cast<cedar::proc::sinks::ConstGroupSink>(element))
  {
    this->removeConnectorItem(false, element->getName());
  }
  else
  {
    auto gui_element = this->mpScene->getGraphicsItemFor(element.get());
    CEDAR_DEBUG_NON_CRITICAL_ASSERT(gui_element != NULL);
    delete gui_element;
  }
}

void cedar::proc::gui::Group::toggleSmartConnectionMode()
{
  bool smart = this->_mSmartMode->getValue();
  QList<QGraphicsItem*> items = this->mpScene->items();
  for (int i = 0; i < items.size(); ++i)
  {
    if (cedar::proc::gui::Connection* con = dynamic_cast<cedar::proc::gui::Connection*>(items[i]))
    {
      con->setSmartMode(smart);
    }
  }
}

void cedar::proc::gui::Group::stepRecordStateChanged()
{
	std::map<const cedar::proc::Step*, cedar::proc::gui::StepItem*> steps = this->mpScene->getStepMap();

	for (auto iter = steps.begin(); iter != steps.end(); ++iter)
	{
		iter->second->setRecorded(iter->first->isRecorded());
	}
}

void cedar::proc::gui::Group::handleStepNameChanged(const std::string& from, const std::string& to)
{
  this->changeStepName(from, to);
}

void cedar::proc::gui::Group::addPlotGroup(std::string plotGroupName)
{
  cedar::aux::ConfigurationNode node;
  this->writeOpenPlotsTo(node);
  this->mPlotGroupsNode.put_child(plotGroupName, node);
}

void cedar::proc::gui::Group::removePlotGroup(std::string plotGroupName)
{
  auto plot_group = this->mPlotGroupsNode.find(plotGroupName);
  if(plot_group == this->mPlotGroupsNode.not_found())
  {
    CEDAR_THROW
    (
      cedar::aux::NotFoundException,
      "cedar::proc::gui::Group::removePlotGroup could not remove plot group. Does not exist."
    );
  }

  this->mPlotGroupsNode.erase(mPlotGroupsNode.to_iterator(plot_group));
}

void cedar::proc::gui::Group::renamePlotGroup(std::string from, std::string to)
{
  auto plot_group = this->mPlotGroupsNode.find(from);
  if(plot_group == this->mPlotGroupsNode.not_found())
  {
    CEDAR_THROW
    (
      cedar::aux::NotFoundException,
      "cedar::proc::gui::Group::renamePlotGroup could not rename plot group. Does not exist."
    );
  }
  // rename
  cedar::aux::ConfigurationNode node = plot_group->second;
  this->mPlotGroupsNode.erase(mPlotGroupsNode.to_iterator(plot_group));
  this->mPlotGroupsNode.put_child(to, node);
  //maybe just do: plot_group->first = to;
}

std::list<std::string> cedar::proc::gui::Group::getPlotGroupNames()
{
  std::list<std::string> plot_group_names;
  for (auto node : mPlotGroupsNode)
  {
    plot_group_names.push_back(node.first);
  }

  return plot_group_names;
}

void cedar::proc::gui::Group::displayPlotGroup(std::string plotGroupName)
{
  auto plot_group = this->mPlotGroupsNode.find(plotGroupName);
  if(plot_group == this->mPlotGroupsNode.not_found())
  {
    CEDAR_THROW
    (
      cedar::aux::NotFoundException,
      "cedar::proc::gui::Group::displayPlotGroup could not display plot group. Does not exist."
    );
  }

  this->readPlotList(plotGroupName, plot_group->second);
}

void cedar::proc::gui::Group::backgroundColorActionTriggered()
{
  auto p_action = dynamic_cast<QAction*>(QObject::sender());
  CEDAR_ASSERT(p_action != nullptr);

  QColor new_color = p_action->data().value<QColor>();
  this->setBackgroundColor(new_color);
}

void cedar::proc::gui::Group::setBackgroundColor(const QColor& color)
{
  this->setFillColor(color);
  this->mBackgroundColor = color;
}

void cedar::proc::gui::Group::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
  CEDAR_DEBUG_ONLY(cedar::proc::gui::Scene* p_scene = dynamic_cast<cedar::proc::gui::Scene*>(this->scene());)
  CEDAR_DEBUG_ASSERT(p_scene);

  QMenu menu;

  if (this->scene()->selectedItems().size() > 1)
  {
    menu.exec(event->screenPos());
    return;
  }

  QAction* p_collapse = menu.addAction("collapse");
  p_collapse->setCheckable(true);
  p_collapse->setChecked(this->isCollapsed());
  this->connect(p_collapse, SIGNAL(toggled(bool)), SLOT(setCollapsed(bool)));

  auto color_menu = menu.addMenu("color");
  auto colors = cedar::proc::gui::SettingsSingleton::getInstance()->getUserDefinedColors();
  colors.push_back
  (
    cedar::proc::gui::Settings::UserDefinedColorPtr
    (
      new cedar::proc::gui::Settings::UserDefinedColor("white=rgb(255,255,255)")
    )
  );

  for (auto color : colors)
  {
    if (color->hasName())
    {
      auto action = color_menu->addAction(QString::fromStdString(color->getName()));
      QPixmap pm(16, 16);
      QColor qcolor = color->toQColor();
      pm.fill(qcolor);
      action->setIcon(QIcon(pm));
      action->setData(QVariant::fromValue(qcolor));

      QObject::connect(action, SIGNAL(triggered()), this, SLOT(backgroundColorActionTriggered()));
    }
  }

  menu.addSeparator(); // ----------------------------------------------------------------------------------------------
  QAction* p_add_input = menu.addAction("add input");
  QAction* p_add_output = menu.addAction("add output");
  if (this->getGroup()->getState() == cedar::proc::Triggerable::STATE_RUNNING)
  {
    p_add_input->setEnabled(false);
    p_add_output->setEnabled(false);
  }

  menu.addSeparator(); // ----------------------------------------------------------------------------------------------

  QMenu* p_rename_input_menu = menu.addMenu("rename input");
  QMenu* p_rename_output_menu = menu.addMenu("rename output");
  if (this->getGroup()->getState() == cedar::proc::Triggerable::STATE_RUNNING)
  {
    p_rename_input_menu->setEnabled(false);
    p_rename_output_menu->setEnabled(false);
  }
  const cedar::proc::Group::ConnectorMap& connectors = this->getGroup()->getConnectorMap();
  for (auto it = connectors.begin(); it != connectors.end(); ++it)
  {
    if (it->second)
    {
      p_rename_input_menu->addAction(QString::fromStdString(it->first));
    }
    else
    {
      p_rename_output_menu->addAction(QString::fromStdString(it->first));
    }
  }
  if (p_rename_input_menu->isEmpty())
  {
    QAction* none = p_rename_input_menu->addAction("none");
    none->setEnabled(false);
  }
  if (p_rename_output_menu->isEmpty())
  {
    QAction* none = p_rename_output_menu->addAction("none");
    none->setEnabled(false);
  }

  menu.addSeparator(); // ----------------------------------------------------------------------------------------------

  QMenu* p_remove_input_menu = menu.addMenu("remove input");
  QMenu* p_remove_output_menu = menu.addMenu("remove output");
  if (this->getGroup()->getState() == cedar::proc::Triggerable::STATE_RUNNING)
  {
    p_remove_input_menu->setEnabled(false);
    p_remove_output_menu->setEnabled(false);
  }

  for (auto it = connectors.begin(); it != connectors.end(); ++it)
  {
    if (it->second)
    {
      p_remove_input_menu->addAction(QString::fromStdString(it->first));
    }
    else
    {
      p_remove_output_menu->addAction(QString::fromStdString(it->first));
    }
  }
  if (p_remove_input_menu->isEmpty())
  {
    QAction* none = p_remove_input_menu->addAction("none");
    none->setEnabled(false);
  }
  if (p_remove_output_menu->isEmpty())
  {
    QAction* none = p_remove_output_menu->addAction("none");
    none->setEnabled(false);
  }

  menu.addSeparator(); // ----------------------------------------------------------------------------------------------

  QAction* p_prune = menu.addAction("prune unused connectors");

  menu.addSeparator(); // ----------------------------------------------------------------------------------------------

  {
    QAction* edit_parameters_action = menu.addAction("edit parameters ...");
    QObject::connect(edit_parameters_action, SIGNAL(triggered()), this, SLOT(openParameterEditor()));
  }

  QAction* a = menu.exec(event->screenPos());

  if (a == NULL)
    return;

  // execute an action
  if (a == p_add_input || a == p_add_output)
  {
    QString default_name = QString::fromStdString(this->getGroup()->getNewConnectorName(a == p_add_input));
    QString name = QInputDialog::getText(0, "Enter a name", "Name", QLineEdit::Normal, default_name);
    if (!name.isEmpty())
    {
      if (this->getGroup()->hasConnector(name.toStdString()))
      {
        QMessageBox::critical(NULL, "Name exists", "A connector of this name already exists.");
      }
      else
      {
        this->getGroup()->addConnector(name.toStdString(), (a == p_add_input));
      }
    }
  }

  else if (a->parentWidget() == p_remove_input_menu)
  {
    std::string name = a->text().toStdString();
    this->getGroup()->removeConnector(name, true);
  }

  else if (a->parentWidget() == p_remove_output_menu)
  {
    std::string name = a->text().toStdString();
    this->getGroup()->removeConnector(name, false);
  }

  else if (a->parentWidget() == p_rename_input_menu)
  {
    std::string oldName = a->text().toStdString();
    QString newName = QInputDialog::getText(0, "Enter a new name", "Name", QLineEdit::Normal);
    if (!newName.isEmpty())
    {
      if (this->getGroup()->hasConnector(newName.toStdString()))
      {
        QMessageBox::critical(NULL, "New name exists", "A connector of this name already exists.");
      }
      else
      {
        this->getGroup()->renameConnector(oldName, newName.toStdString(), true);
      }
    }
  }

  else if (a->parentWidget() == p_rename_output_menu)
  {
    std::string oldName = a->text().toStdString();
    QString newName = QInputDialog::getText(0, "Enter a new name", "Name", QLineEdit::Normal);
    if (!newName.isEmpty())
    {
      if (this->getGroup()->hasConnector(newName.toStdString()))
      {
        QMessageBox::critical(NULL, "New name exists", "A connector of this name already exists.");
      }
      else
      {
        this->getGroup()->renameConnector(oldName, newName.toStdString(), false);
      }
    }
  }

  else if (a == p_prune)
  {
    this->getGroup()->pruneUnusedConnectors();
  }
}

void cedar::proc::gui::Group::openParameterEditor()
{
  auto p_dialog = new QDialog(this->mpMainWindow);
  auto p_layout = new QHBoxLayout();
  p_layout->setContentsMargins(0, 0, 0, 0);
  p_dialog->setLayout(p_layout);
  auto designer = new cedar::proc::gui::GroupParameterDesigner(this->getGroup());
  p_layout->addWidget(designer);
  p_dialog->exec();

  delete p_dialog;
}

void cedar::proc::gui::Group::setCollapsed(bool collapsed)
{
  this->_mIsCollapsed->setValue(collapsed);
}

void cedar::proc::gui::Group::updateCollapsedness()
{
  bool collapse = this->isCollapsed();
  auto children = this->childItems();
  for (int i = 0; i < children.size(); ++i)
  {
    auto child = children.at(i);
    //!@todo This should cast to cedar::proc::gui::Element, but that class doesn't exist (yet)
    if (auto element = dynamic_cast<cedar::proc::gui::Connectable*>(child))
    {
      element->setVisible(!collapse);
    }
    else if (auto element = dynamic_cast<cedar::proc::gui::TriggerItem*>(child))
    {
      element->setVisible(!collapse);
    }
  }

  for (size_t i = 0; i < this->mConnectorSinks.size(); ++i)
  {
    this->mConnectorSinks.at(i)->setVisible(!collapse);
  }

  for (size_t i = 0; i < this->mConnectorSources.size(); ++i)
  {
    this->mConnectorSources.at(i)->setVisible(!collapse);
  }

  // update the text of the group
  this->groupNameChanged();

  if (collapse)
  {
    this->setInputOutputSlotOffset(static_cast<qreal>(0));
    //!@todo Same size as processing steps/adapt to the number of inputs, outputs?
    this->setSize(cedar::proc::gui::StepItem::mDefaultWidth, cedar::proc::gui::StepItem::mDefaultHeight);

  }
  else
  {
    this->setInputOutputSlotOffset(cedar::proc::gui::Group::M_EXPANDED_SLOT_OFFSET);
    this->setSize(this->_mUncollapsedWidth->getValue(), this->_mUncollapsedHeight->getValue());
  }

  this->setResizeable(!collapse);
  this->updateConnections();
}

void cedar::proc::gui::Group::changeStepName(const std::string& from, const std::string& to)
{
  /* plot groups are structured like this:
    {
      group#1_name :
      [
        { step: name,
          position, plot info, etc. 
        },
        {
          step: name, 
          ...
        },
        ...
      ],
      group#2_name :
      ...
    }

    we have to search and replace the old step name in every step for every group
  */
  for(auto& plot_group : this->mPlotGroupsNode)
  {
    for(auto& plot : plot_group.second)
    {
      auto name = plot.second.get<std::string>("step");
      if(name == from)
      {
        auto node = plot.second.put("step", to);
      }
    }
  }
}

void cedar::proc::gui::Group::readStickyNotes(const cedar::aux::ConfigurationNode& node)
{

  for (auto iter = node.begin(); iter != node.end(); ++iter)
  {
    const std::string& type = iter->second.get<std::string>("type");
    if (type == "stickyNote")
    {
      int x = iter->second.get<int>("x");
      int y = iter->second.get<int>("y");
      int witdh = iter->second.get<int>("width");
      int height = iter->second.get<int>("height");
      const std::string& text = iter->second.get<std::string>("text");
      this->mpScene->addStickyNote(x, y, witdh, height, text);
    }
  }
}

void cedar::proc::gui::Group::restoreConnections()
{
  auto connections = this->getGroup()->getDataConnections();
  for (auto connection : connections)
  {
    this->checkDataConnection(connection->getSource(), connection->getTarget(), cedar::proc::Group::CONNECTION_ADDED);
  }
}

void cedar::proc::gui::Group::updateDecorations()
{
  this->addDecorations();
}

void cedar::proc::gui::Group::removeElementFromPlotGroup(const std::string& plotGroupName, const std::string& elementName)
{
  auto plot_group = this->mPlotGroupsNode.find(plotGroupName);
  if(plot_group == this->mPlotGroupsNode.not_found())
  {
    CEDAR_THROW
    (
      cedar::aux::NotFoundException,
      "Plot group " + plotGroupName + " does not exist."
    );
  }

  for (auto plot_iter = plot_group->second.begin(); plot_iter != plot_group->second.end(); )
  {
    auto name = plot_iter->second.get<std::string>("step");
    if (name == elementName)
    {
      plot_iter = plot_group->second.erase(plot_iter);
    }
    else
    {
      ++plot_iter;
    }
  }
}
