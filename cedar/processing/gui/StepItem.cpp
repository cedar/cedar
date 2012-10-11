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

    File:        StepItem.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 05 27

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/gui/StepItem.h"
#include "cedar/processing/gui/Scene.h"
#include "cedar/processing/gui/DataSlotItem.h"
#include "cedar/processing/gui/TriggerItem.h"
#include "cedar/processing/gui/Settings.h"
#include "cedar/processing/gui/exceptions.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/Manager.h"
#include "cedar/processing/Step.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/namespace.h"
#include "cedar/auxiliaries/gui/DataPlotter.h"
#include "cedar/auxiliaries/gui/PlotManager.h"
#include "cedar/auxiliaries/gui/PlotDeclaration.h"
#include "cedar/auxiliaries/TypeHierarchyMap.h"
#include "cedar/auxiliaries/Data.h"
#include "cedar/auxiliaries/Singleton.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/casts.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/units/TimeUnit.h"

// SYSTEM INCLUDES
#include <QPen>
#include <QPainter>
#include <QLabel>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QGraphicsDropShadowEffect>
#include <QLayout>
#include <QResource>
#include <iostream>


//----------------------------------------------------------------------------------------------------------------------
// static members
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::EnumType<cedar::proc::gui::StepItem::DisplayMode> cedar::proc::gui::StepItem::DisplayMode::mType;

const int cedar::proc::gui::StepItem::mIconSize = 40;
const qreal cedar::proc::gui::StepItem::mDefaultWidth = 160;
const qreal cedar::proc::gui::StepItem::mDefaultHeight = 50;
const qreal cedar::proc::gui::StepItem::mBaseDataSlotSize = 12.0;

QIcon cedar::proc::gui::StepItem::mLoopedIcon(":/decorations/looped.svg");

#ifndef CEDAR_COMPILER_MSVC
const cedar::proc::gui::StepItem::DisplayMode::Id cedar::proc::gui::StepItem::DisplayMode::ICON_AND_TEXT;
const cedar::proc::gui::StepItem::DisplayMode::Id cedar::proc::gui::StepItem::DisplayMode::ICON_ONLY;
#endif // CEDAR_COMPILER_MSVC

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::StepItem::StepItem(cedar::proc::StepPtr step, QMainWindow* pMainWindow)
:
cedar::proc::gui::GraphicsBase(cedar::proc::gui::StepItem::mDefaultWidth,
                               cedar::proc::gui::StepItem::mDefaultHeight,
                               cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_STEP,
                               cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_NONE),
mRunTimeMeasurementTimerId(0),
mpMainWindow(pMainWindow),
mStepIcon(":/steps/no_icon.svg"),
mDisplayMode(cedar::proc::gui::StepItem::DisplayMode::ICON_AND_TEXT)
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);

  this->setStep(step);
  
  this->construct();
}

cedar::proc::gui::StepItem::StepItem(QMainWindow* pMainWindow)
:
cedar::proc::gui::GraphicsBase(cedar::proc::gui::StepItem::mDefaultWidth,
                               cedar::proc::gui::StepItem::mDefaultHeight,
                               cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_STEP,
                               cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_NONE),
mRunTimeMeasurementTimerId(0),
mpMainWindow(pMainWindow),
mStepIcon(":/steps/no_icon.svg"),
mDisplayMode(cedar::proc::gui::StepItem::DisplayMode::ICON_AND_TEXT)
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);

  this->construct();
}

void cedar::proc::gui::StepItem::construct()
{
  this->setFlags(this->flags() | QGraphicsItem::ItemIsSelectable
                               | QGraphicsItem::ItemIsMovable
                               );

  if (cedar::proc::gui::Settings::instance().useGraphicsItemShadowEffects())
  {
    QGraphicsDropShadowEffect *p_effect = new QGraphicsDropShadowEffect();
    p_effect->setBlurRadius(5.0);
    p_effect->setOffset(3.0, 3.0);
    this->setGraphicsEffect(p_effect);
  }
}

cedar::proc::gui::StepItem::Decoration::Decoration
(
  cedar::proc::gui::StepItem* pStep,
  const QIcon& icon,
  const QString& description
)
:
mIconSource(icon)
{
  this->mpRectangle = new QGraphicsRectItem(-1, -1, 10, 10, pStep);
  this->mpIcon = new QGraphicsPixmapItem
                 (
                   mIconSource.pixmap(static_cast<int>(cedar::proc::gui::StepItem::mBaseDataSlotSize)),
                   this->mpRectangle
                 );
  this->mpIcon->setToolTip(description);

  QPen pen = this->mpRectangle->pen();
  pen.setWidth(1);
  pen.setColor(QColor(0, 0, 0));
  QBrush bg(QColor(255, 255, 255));
  this->mpRectangle->setPen(pen);
  this->mpRectangle->setBrush(bg);
}

cedar::proc::gui::StepItem::~StepItem()
{
  cedar::aux::LogSingleton::getInstance()->freeing(this);

  mStateChangedConnection.disconnect();
  mSlotAddedConnection.disconnect();
  mSlotRemovedConnection.disconnect();

  if (this->scene())
  {
    cedar::aux::asserted_cast<cedar::proc::gui::Scene*>(this->scene())->removeStepItem(this);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::StepItem::Decoration::setPosition(const QPointF& pos)
{
  this->mpRectangle->setPos(pos);
}

void cedar::proc::gui::StepItem::Decoration::setSize(double sizeFactor)
{
  const qreal padding = 1;
  const qreal size = static_cast<qreal>(0.7)
                     * static_cast<qreal>(sizeFactor)
                     * cedar::proc::gui::StepItem::mBaseDataSlotSize;
  QRectF new_dims = this->mpRectangle->rect();
  new_dims.setWidth(size + 2*padding);
  new_dims.setHeight(size + 2*padding);
  this->mpRectangle->setRect(new_dims);
  this->mpIcon->setPixmap(mIconSource.pixmap(static_cast<int>(size)));
}

void cedar::proc::gui::StepItem::slotAdded(cedar::proc::DataRole::Id role, const std::string& name)
{
  this->addDataItemFor(this->getStep()->getSlot(role, name));
  this->updateAttachedItems();
}

void cedar::proc::gui::StepItem::slotRemoved(cedar::proc::DataRole::Id role, const std::string& name)
{
  cedar::proc::gui::DataSlotItem* p_item = NULL;

  DataSlotMap::iterator iter = this->mSlotMap.find(role);
  CEDAR_ASSERT(iter != this->mSlotMap.end());

  DataSlotNameMap& name_map = iter->second;
  DataSlotNameMap::iterator name_iter = name_map.find(name);

  CEDAR_ASSERT(name_iter != name_map.end());
  p_item = name_iter->second;
  name_map.erase(name_iter);
  delete p_item;
}

void cedar::proc::gui::StepItem::timerEvent(QTimerEvent * /* pEvent */)
{
  cedar::unit::Milliseconds run_time(this->mStep->getRunTimeMeasurement());
  cedar::unit::Milliseconds run_time_avg(this->mStep->getRunTimeAverage());
  cedar::unit::Milliseconds lock_time(this->mStep->getLockTimeMeasurement());
  cedar::unit::Milliseconds lock_time_avg(this->mStep->getLockTimeAverage());
  QString tool_tip
    = QString("<table>"
              "  <tr>"
              "    <th>Measurement:</th>"
              "    <th>Last</th>"
              "    <th>Average</th>"
              "  </tr>"
              "  <tr>"
              "    <td>locking</td>"
              "    <td>%3</td>"
              "    <td>%4</td>"
              "  </tr>"
              "  <tr>"
              "    <td>compute call</td>"
              "    <td>%1</td>"
              "    <td>%2</td>"
              "  </tr>"
              "  <tr>"
              "    <td>sum</td>"
              "    <td>%5</td>"
              "    <td>%6</td>"
              "  </tr>"
              " </table>")
      .arg(QString::fromStdString(cedar::aux::toString(run_time)))
      .arg(QString::fromStdString(cedar::aux::toString(run_time_avg)))
      .arg(QString::fromStdString(cedar::aux::toString(lock_time)))
      .arg(QString::fromStdString(cedar::aux::toString(lock_time_avg)))
      .arg(QString::fromStdString(cedar::aux::toString(run_time + lock_time)))
      .arg(QString::fromStdString(cedar::aux::toString(run_time_avg + lock_time_avg)))
      ;
  this->setToolTip(tool_tip);
}

bool cedar::proc::gui::StepItem::hasGuiConnection
     (
       const std::string& fromSlot,
       cedar::proc::gui::StepItem* pToItem,
       const std::string& toSlot
     ) const
{
  CEDAR_DEBUG_ASSERT(pToItem != NULL);

  cedar::proc::gui::DataSlotItem const* p_source_slot = this->getSlotItem(cedar::proc::DataRole::OUTPUT, fromSlot);
  CEDAR_DEBUG_ASSERT(p_source_slot != NULL);

  cedar::proc::gui::DataSlotItem const* p_target_slot = pToItem->getSlotItem(cedar::proc::DataRole::INPUT, toSlot);
  CEDAR_DEBUG_ASSERT(p_target_slot != NULL);

  return p_source_slot->hasGuiConnectionTo(p_target_slot);
}

void cedar::proc::gui::StepItem::stepStateChanged()
{
  switch (this->mStep->getState())
  {
    case cedar::proc::Step::STATE_EXCEPTION:
    case cedar::proc::Step::STATE_NOT_RUNNING:
      this->setOutlineColor(Qt::darkGray);
      this->setFillColor(QColor(235, 235, 235));

      if (this->mRunTimeMeasurementTimerId != 0)
      {
        this->killTimer(this->mRunTimeMeasurementTimerId);
        this->mRunTimeMeasurementTimerId = 0;
      }
      break;

    case cedar::proc::Step::STATE_RUNNING:
      if (this->mRunTimeMeasurementTimerId == 0)
      {
        this->mRunTimeMeasurementTimerId = this->startTimer(1000); //!@todo Add option to change the frequency
      }
    default:
      this->setOutlineColor(cedar::proc::gui::GraphicsBase::mDefaultOutlineColor);
      this->setFillColor(cedar::proc::gui::GraphicsBase::mDefaultFillColor);
  }
  // append <font>s to make this a rich text, thus automatically word-wrapping the tool tip.
  const std::string& annotation = this->mStep->getStateAnnotation();
  QString tool_tip = "";
  if (!annotation.empty())
  {
    // Replace any non-html characters in the annotation string by their html equivalents.
    QString escaped_annotation = QString::fromStdString(annotation)
                                     .replace("&","&amp;")
                                     .replace(">","&gt;")
                                     .replace("<","&lt;");
    tool_tip = QString("<font color=\"black\">") + escaped_annotation + QString("</font>");
  }
  this->setToolTip(tool_tip);
  this->update();
}

void cedar::proc::gui::StepItem::redraw()
{
  this->update();
}

void cedar::proc::gui::StepItem::setStep(cedar::proc::StepPtr step)
{
  this->setElement(step);
  this->mStep = step;
  this->mClassId = cedar::proc::DeclarationRegistrySingleton::getInstance()->getDeclarationOf(this->mStep);

  //!@todo This code is redundant with code in ElementClassList; unify
  QResource ex_test(QString::fromStdString(this->mClassId->getIconPath()));
  if (ex_test.isValid())
  {
    this->mStepIcon = QIcon(QString::fromStdString(this->mClassId->getIconPath()));
    if (this->mStepIcon.isNull())
    {
      this->mStepIcon = QIcon(":/steps/no_icon.svg");
    }
  }
  else
  {
    this->mStepIcon = QIcon(":/steps/broken_icon.svg");
  }

  this->addDataItems();
  this->addTriggerItems();
  this->addDecorations();

  mStateChangedConnection = step->connectToStateChanged(boost::bind(&cedar::proc::gui::StepItem::stepStateChanged, this));
  QObject::connect(step.get(), SIGNAL(nameChanged()), this, SLOT(redraw()));

  mSlotAddedConnection.disconnect();
  mSlotRemovedConnection.disconnect();

  mSlotAddedConnection
    = step->connectToSlotAdded(boost::bind(&cedar::proc::gui::StepItem::slotAdded, this, _1, _2));
  mSlotRemovedConnection
    = step->connectToSlotRemoved(boost::bind(&cedar::proc::gui::StepItem::slotRemoved, this, _1, _2));
}

void cedar::proc::gui::StepItem::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  this->cedar::proc::gui::GraphicsBase::readConfiguration(node);

  cedar::aux::ConfigurationNode::const_assoc_iterator style_iter = node.find("display style");
  if (style_iter != node.not_found())
  {
    std::string style = style_iter->second.get_value<std::string>();
    this->setDisplayMode(cedar::proc::gui::StepItem::DisplayMode::type().get(style));
  }
}

void cedar::proc::gui::StepItem::writeConfiguration(cedar::aux::ConfigurationNode& root) const
{
  root.put("step", this->mStep->getName());
  root.put("display style", cedar::proc::gui::StepItem::DisplayMode::type().get(this->mDisplayMode).name());
  this->cedar::proc::gui::GraphicsBase::writeConfiguration(root);
}

void cedar::proc::gui::StepItem::addDecorations()
{
  this->mDecorations.clear();

  if (this->getStep() && this->getStep()->isLooped())
  {
    DecorationPtr decoration
    (
      new Decoration(this, mLoopedIcon, "This step is looped, i.e., it expects to be connected to a looped trigger.")
    );

    this->mDecorations.push_back(decoration);
  }

  this->updateDecorationPositions();
}

void cedar::proc::gui::StepItem::updateDecorationPositions()
{
  QPointF origin(this->width(), this->height());

  switch (this->mDisplayMode)
  {
    case DisplayMode::ICON_ONLY:
      origin.setX(origin.x() - 7.5);
      break;

    default:
      origin.setX(origin.x() - 15.0);
      origin.setY(origin.y() - 5.0);
  }

  QPointF offset_dir(-1, 0);
  qreal distance = 10.0;
  for (size_t i = 0; i < this->mDecorations.size(); ++i)
  {
    DecorationPtr decoration = this->mDecorations[i];
    decoration->setPosition(origin + static_cast<qreal>(i) * distance * offset_dir);

    switch (this->mDisplayMode)
    {
      case DisplayMode::ICON_ONLY:
        decoration->setSize(0.7);
        break;

      default:
        decoration->setSize(1.0);
    }
  }
}

void cedar::proc::gui::StepItem::addTriggerItems()
{
  CEDAR_DEBUG_ASSERT(this->mStep);

  qreal padding = static_cast<qreal>(3);
  QPointF origin(0, this->height() + padding);
  qreal trigger_size = 10.0;
  mTriggers.clear();

  for (size_t i = 0; i < this->mStep->getTriggerCount(); ++i)
  {
    cedar::proc::TriggerPtr trigger = this->mStep->getTrigger(i);
    cedar::proc::gui::TriggerItem* p_trigger_item = new cedar::proc::gui::TriggerItem(trigger);
    p_trigger_item->setParentItem(this);
    p_trigger_item->setPos(origin + QPointF(0, 1) * static_cast<qreal>(i) * (trigger_size + padding));
    p_trigger_item->isDocked(true);
    p_trigger_item->setWidth(trigger_size);
    p_trigger_item->setHeight(trigger_size);
    this->mTriggers.push_back(p_trigger_item);
  }
}

void cedar::proc::gui::StepItem::addDataItems()
{
  for (std::vector<cedar::aux::Enum>::const_iterator enum_it = cedar::proc::DataRole::type().list().begin();
      enum_it != cedar::proc::DataRole::type().list().end();
      ++enum_it)
  {
    if ( (*enum_it) == cedar::aux::Enum::UNDEFINED)
      continue;

    // populate step item list
    mSlotMap[*enum_it] = DataSlotNameMap();

    try
    {
      const cedar::proc::Step::SlotList& slotmap = this->mStep->getOrderedDataSlots(*enum_it);
      for (cedar::proc::Step::SlotList::const_iterator iter = slotmap.begin(); iter != slotmap.end(); ++iter)
      {
        // use a non-const version of this slot
        this->addDataItemFor(this->mStep->getSlot(*enum_it, (*iter)->getName()));
      }
    }
    catch(const cedar::proc::InvalidRoleException&)
    {
      // ok -- a step may not have any data for this role.
    }
  }

  this->updateAttachedItems();
}

void cedar::proc::gui::StepItem::addDataItemFor(cedar::proc::DataSlotPtr slot)
{
  cedar::proc::gui::DataSlotItem *p_item = new cedar::proc::gui::DataSlotItem(this, slot);
  mSlotMap[slot->getRole()][slot->getName()] = p_item;
}

void cedar::proc::gui::StepItem::updateAttachedItems()
{
  this->updateDataSlotPositions();
  this->updateDecorationPositions();
}


void cedar::proc::gui::StepItem::updateDataSlotPositions()
{
  qreal padding = static_cast<qreal>(3);
  qreal style_factor;

  switch (this->mDisplayMode)
  {
    case cedar::proc::gui::StepItem::DisplayMode::ICON_ONLY:
      style_factor = static_cast<qreal>(0.75);
      break;

    default:
      style_factor = static_cast<qreal>(1);
  }
  std::map<cedar::proc::DataRole::Id, QPointF> add_origins;
  std::map<cedar::proc::DataRole::Id, QPointF> add_directions;
  std::map<cedar::proc::DataRole::Id, qreal> data_slot_size;

  data_slot_size[cedar::proc::DataRole::BUFFER] = mBaseDataSlotSize * static_cast<qreal>(0.75) * style_factor;
  add_directions[cedar::proc::DataRole::BUFFER] = QPointF(1, 0);
  add_origins[cedar::proc::DataRole::BUFFER] = QPointF(0, -padding - data_slot_size[cedar::proc::DataRole::BUFFER]);

  data_slot_size[cedar::proc::DataRole::INPUT] = mBaseDataSlotSize * style_factor;
  add_origins[cedar::proc::DataRole::INPUT] = QPointF(-padding - data_slot_size[cedar::proc::DataRole::INPUT], 0);
  add_directions[cedar::proc::DataRole::INPUT] = QPointF(0, 1);

  data_slot_size[cedar::proc::DataRole::OUTPUT] = mBaseDataSlotSize * style_factor;
  add_origins[cedar::proc::DataRole::OUTPUT] = QPointF(this->width() + padding, 0);
  add_directions[cedar::proc::DataRole::OUTPUT] = QPointF(0, 1);

  for (DataSlotMap::iterator role_it = mSlotMap.begin(); role_it != mSlotMap.end(); ++role_it)
  {
    cedar::proc::DataRole::Id role = role_it->first;
    DataSlotNameMap& slot_item_map = role_it->second;

    if (role == cedar::aux::Enum::UNDEFINED)
      continue;


    CEDAR_DEBUG_ASSERT(add_origins.find(role) != add_origins.end());
    CEDAR_DEBUG_ASSERT(add_directions.find(role) != add_directions.end());
    CEDAR_DEBUG_ASSERT(data_slot_size.find(role) != data_slot_size.end());

    const QPointF& origin = add_origins[role];
    const QPointF& direction = add_directions[role];
    qreal slot_size = data_slot_size[role];

    try
    {
      qreal count = 0;
      const cedar::proc::Step::SlotList& slotmap = this->mStep->getOrderedDataSlots(role);
      for (cedar::proc::Step::SlotList::const_iterator iter = slotmap.begin(); iter != slotmap.end(); ++iter)
      {
        const std::string& slot_name = (*iter)->getName();
        CEDAR_DEBUG_ASSERT(slot_item_map.find(slot_name) != slot_item_map.end());
        cedar::proc::gui::DataSlotItem *p_item = slot_item_map[slot_name];
        p_item->setWidth(slot_size);
        p_item->setHeight(slot_size);
        p_item->setPos(origin + count * direction * (slot_size + padding));
        count += static_cast<qreal>(1.0);
      }
    }
    catch(const cedar::proc::InvalidRoleException&)
    {
      // ok -- a step may not have any data for this role.
    }
  }
}

cedar::proc::gui::DataSlotItem* cedar::proc::gui::StepItem::getSlotItem
                                (
                                  cedar::proc::DataRole::Id role, const std::string& name
                                )
{
  return const_cast<cedar::proc::gui::DataSlotItem*>
         (
           static_cast<cedar::proc::gui::StepItem const*>(this)->getSlotItem(role, name)
         );
}

cedar::proc::gui::DataSlotItem const* cedar::proc::gui::StepItem::getSlotItem
                                      (
                                        cedar::proc::DataRole::Id role,
                                        const std::string& name
                                      ) const
{
  DataSlotMap::const_iterator role_map = this->mSlotMap.find(role);

  if (role_map == this->mSlotMap.end())
  {
    CEDAR_THROW(cedar::proc::InvalidRoleException, "No slot items stored for role "
                                                   + cedar::proc::DataRole::type().get(role).prettyString()
                                                   );
  }

  DataSlotNameMap::const_iterator iter = role_map->second.find(name);
  if (iter == role_map->second.end())
  {
    CEDAR_THROW(cedar::proc::InvalidNameException, "No slot item named \"" + name +
                                                   "\" found for role "
                                                   + cedar::proc::DataRole::type().get(role).prettyString()
                                                   + " in StepItem for step \"" + this->mStep->getName() + "\"."
                                                   );
  }

  return iter->second;
}

cedar::proc::gui::StepItem::DataSlotNameMap& cedar::proc::gui::StepItem::getSlotItems(
                                                                             cedar::proc::DataRole::Id role
                                                                           )
{
  DataSlotMap::iterator role_map = this->mSlotMap.find(role);

  if (role_map == this->mSlotMap.end())
  {
    CEDAR_THROW(cedar::proc::InvalidRoleException, "Unknown role  "
                                                   + cedar::proc::DataRole::type().get(role).prettyString()
                                                   );
  }
  return role_map->second;
}

void cedar::proc::gui::StepItem::addRoleSeparator(const cedar::aux::Enum& e, QMenu* pMenu)
{
  std::string label = e.prettyString() + "s";
  pMenu->addSeparator();
  QAction *p_label_action = pMenu->addAction(QString::fromStdString(label));
  QFont font = p_label_action->font();
  font.setBold(true);
  p_label_action->setFont(font);
  pMenu->addSeparator();
  p_label_action->setEnabled(false);
}

void cedar::proc::gui::StepItem::fillPlots
     (
       QMenu* pMenu,
       std::map<QAction*, std::pair<cedar::aux::gui::PlotDeclarationPtr, cedar::aux::Enum> >& declMap
     )
{
  /*typedef cedar::aux::gui::PlotDeclarationManager::Node PlotNode;
  typedef cedar::aux::gui::PlotDeclarationManager::ConstNodePtr ConstPlotNodePtr;*/

  for (std::vector<cedar::aux::Enum>::const_iterator enum_it = cedar::proc::DataRole::type().list().begin();
      enum_it != cedar::proc::DataRole::type().list().end();
      ++enum_it)
  {
    try
    {
      const cedar::aux::Enum& e = *enum_it;
      this->addRoleSeparator(e, pMenu);

      const cedar::proc::Step::SlotList& slotmap = this->mStep->getOrderedDataSlots(e.id());
      for
      (
        cedar::proc::Step::SlotList::const_iterator slot_iter = slotmap.begin();
        slot_iter != slotmap.end();
        ++slot_iter
      )
      {
        cedar::proc::DataSlotPtr slot = *slot_iter;
        QMenu *p_menu = pMenu->addMenu(slot->getText().c_str());
        cedar::aux::DataPtr data = slot->getData();
        if (!data)
        {
          p_menu->setTitle("[unconnected] " + p_menu->title());
          p_menu->setEnabled(false);
        }
        else
        {
          std::set<cedar::aux::gui::PlotDeclarationPtr> plots;
          cedar::aux::gui::PlotManagerSingleton::getInstance()->getPlotClassesFor(data, plots);

          if (plots.empty())
          {
            QAction *p_action = p_menu->addAction("no plots available");
            p_action->setDisabled(true);
          }
          else
          {
            for
            (
              std::set<cedar::aux::gui::PlotDeclarationPtr>::iterator iter = plots.begin();
              iter != plots.end();
              ++iter
            )
            {
              cedar::aux::gui::PlotDeclarationPtr declaration = *iter;
              QAction *p_action = p_menu->addAction(QString::fromStdString(declaration->getPlotClass()));
              p_action->setData(QString::fromStdString(slot->getName()));
              declMap[p_action] = std::make_pair(declaration, e);

              if (declaration == cedar::aux::gui::PlotManagerSingleton::getInstance()->getDefaultDeclarationFor(data))
              {
                QFont font = p_action->font();
                font.setBold(true);
                p_action->setFont(font);
              }
            }
          }
        }
      }
    }
    catch (const cedar::proc::InvalidRoleException& e)
    {
      // that's ok, a step may not have any data in a certain role.
    }
  }
}
void cedar::proc::gui::StepItem::showPlot
(
  const QPoint& position,
  cedar::aux::gui::PlotInterface* pPlot,
  cedar::proc::DataSlotPtr slot,
  std::string title
)
{
  //!@todo It would be better if setting the title would be part of the actual widget
  if (title.empty())
  {
    title = slot->getText();
    title += " (" + this->mStep->getName();
    title += "." + slot->getName() + ")";
  }

  QDockWidget *p_dock = this->createPlotDockWidget(title);

  QRect geometry = p_dock->geometry();
  geometry.setTopLeft(position);
  geometry.setSize(QSize(200, 200));
  p_dock->setGeometry(geometry);

  p_dock->setWidget(pPlot);
  p_dock->show();
}

void cedar::proc::gui::StepItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
  cedar::proc::gui::Scene *p_scene = dynamic_cast<cedar::proc::gui::Scene*>(this->scene());
  CEDAR_DEBUG_ASSERT(p_scene);

  QMenu menu;

  if (this->scene()->selectedItems().size() > 1)
  {
    p_scene->networkGroupingContextMenuEvent(menu);
    menu.exec(event->screenPos());
    return;
  }

  QMenu *p_data = menu.addMenu("data");

  menu.addSeparator();

  QAction *p_plot_all = menu.addAction("plot all");
  QMenu *p_element_plots = menu.addMenu("defined plots");
  QMenu *p_advanced_plotting = menu.addMenu("advanced plotting");

  menu.addSeparator();

  this->fillDefinedPlots(p_element_plots, event->screenPos());

  std::map<QAction*, std::pair<cedar::aux::gui::PlotDeclarationPtr, cedar::aux::Enum> > advanced_plot_map;
  this->fillPlots(p_advanced_plotting, advanced_plot_map);

  QMenu *p_actions_menu = menu.addMenu("actions");
  menu.addSeparator();

  const cedar::proc::Step::ActionMap& map = this->mStep->getActions();
  if (map.empty())
  {
    p_actions_menu->setEnabled(false);
  }
  else
  {
    for (cedar::proc::Step::ActionMap::const_iterator iter = map.begin(); iter != map.end(); ++iter)
    {
      p_actions_menu->addAction(iter->first.c_str());
    }
  }

  menu.addSeparator();
  p_scene->networkGroupingContextMenuEvent(menu);

  // Actions for data plotting -----------------------------------------------------------------------------------------
  std::map<QAction*, cedar::aux::Enum> action_type_map;
  bool has_data = false;

  for (std::vector<cedar::aux::Enum>::const_iterator enum_it = cedar::proc::DataRole::type().list().begin();
      enum_it != cedar::proc::DataRole::type().list().end();
      ++enum_it)
  {
    try
    {
      const cedar::aux::Enum& e = *enum_it;
      this->addRoleSeparator(e, p_data);

      const cedar::proc::Step::SlotList& slotmap = this->mStep->getOrderedDataSlots(e.id());
      for (cedar::proc::Step::SlotList::const_iterator iter = slotmap.begin(); iter != slotmap.end(); ++iter)
      {
        cedar::proc::DataSlotPtr slot = *iter;
        QAction *p_action = p_data->addAction(slot->getText().c_str());
        p_action->setData(QString::fromStdString(slot->getName()));
        if (slot->getData().get() == NULL)
        {
          p_action->setText("[unconnected] " + p_action->text());
          p_action->setEnabled(false);
        }
        action_type_map[p_action] = e;
        has_data = true;
      }
    }
    catch (const cedar::proc::InvalidRoleException& e)
    {
      // that's ok, a step may not have any data in a certain role.
    }
  }

  if (!has_data)
  {
    QAction *p_action = p_data->addAction("no data");
    p_action->setEnabled(false);
  }

  menu.addSeparator();
  this->fillDisplayStyleMenu(&menu);

  QAction *a = menu.exec(event->screenPos());

  if (a == NULL)
    return;

  // plot data
  if (a->parentWidget() == p_data)
  {
    std::string data_name = a->data().toString().toStdString();
    const cedar::aux::Enum& e = action_type_map[a];

    cedar::aux::DataPtr p_data = this->mStep->getData(e, data_name);
    cedar::proc::DataSlotPtr slot = this->mStep->getSlot(e, data_name);
    cedar::aux::gui::DataPlotter *p_plotter = new cedar::aux::gui::DataPlotter();
    p_plotter->plot(p_data, slot->getText());
    this->showPlot(event->screenPos(), p_plotter, slot);
  }
  // plot all data slots
  else if (a == p_plot_all)
  {
    this->multiplot(event->screenPos());
  }
  // execute an action
  else if (a->parentWidget() == p_actions_menu)
  {
    std::string action = a->text().toStdString();
    this->mStep->callAction(action);
  }
  // advanced plots
  else if (advanced_plot_map.find(a) != advanced_plot_map.end())
  {
    cedar::aux::gui::PlotDeclarationPtr declaration = advanced_plot_map.find(a)->second.first;
    const cedar::aux::Enum& e = advanced_plot_map.find(a)->second.second;

    std::string data_name = a->data().toString().toStdString();

    cedar::aux::DataPtr p_data = this->mStep->getData(e, data_name);
    cedar::proc::DataSlotPtr slot = this->mStep->getSlot(e, data_name);
    cedar::aux::gui::PlotInterface *p_plotter = declaration->createPlot();
    p_plotter->plot(p_data, slot->getText());
    this->showPlot(event->screenPos(), p_plotter, slot);
  }
}

void cedar::proc::gui::StepItem::fillDefinedPlots(QMenu* pMenu, const QPoint& plotPosition)
{
  // get declaration of the element displayed by this item
  cedar::proc::ElementDeclarationPtr decl
    = cedar::proc::DeclarationRegistrySingleton::getInstance()->getDeclarationOf(this->mStep);

  if (!decl)
  {
    pMenu->setDisabled(true);
    pMenu->setToolTip("No declaration was found for this element.");
    return;
  }

  if (decl->definedPlots().empty())
  {
    pMenu->setDisabled(true);
    pMenu->setToolTip("No plots defined for this element.");
    return;
  }

  QObject::connect(pMenu, SIGNAL(triggered(QAction*)), this, SLOT(openDefinedPlotAction(QAction*)));

  // list all defined plots, if available
  for (size_t i = 0; i < decl->definedPlots().size(); ++i)
  {
    const std::string& plot_name = decl->definedPlots()[i].first;
    QAction* p_action = pMenu->addAction(QString::fromStdString(plot_name));
    p_action->setData(plotPosition);
  }
}

void cedar::proc::gui::StepItem::openDefinedPlotAction(QAction* pAction)
{
  std::string plot_name = pAction->text().toStdString();

  // get declaration of the element displayed by this item
  cedar::proc::ElementDeclarationPtr decl
    = cedar::proc::DeclarationRegistrySingleton::getInstance()->getDeclarationOf(this->mStep);

  // find the list of data to plot for this item
  size_t list_index = decl->definedPlots().size();
  for (size_t i = 0; i < decl->definedPlots().size(); ++i)
  {
    const std::string& name = decl->definedPlots()[i].first;
    if (plot_name == name)
    {
      list_index = i;
      break;
    }
  }

  if (list_index >= decl->definedPlots().size())
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Error: could not find the definition for plot \"" + plot_name + "\".",
      "cedar::proc::gui::StepItem::openDefinedPlotAction(QAction*)",
      "Plotting"
    );
    return;
  }

  this->multiplot(pAction->data().toPoint(), decl->definedPlots()[list_index].second);
}

void cedar::proc::gui::StepItem::fillDisplayStyleMenu(QMenu* pMenu)
{
  QMenu* p_sub_menu = pMenu->addMenu("display style");

  for (size_t i = 0; i < cedar::proc::gui::StepItem::DisplayMode::type().list().size(); ++i)
  {
    const cedar::aux::Enum& e = cedar::proc::gui::StepItem::DisplayMode::type().list().at(i);
    QAction* p_action = p_sub_menu->addAction(QString::fromStdString(e.prettyString()));
    p_action->setData(QString::fromStdString(e.name()));

    p_action->setCheckable(true);
    if (e == this->mDisplayMode)
    {
      p_action->setChecked(true);
    }
  }

  QObject::connect(p_sub_menu, SIGNAL(triggered(QAction*)), this, SLOT(displayStyleMenuTriggered(QAction*)));
}

void cedar::proc::gui::StepItem::displayStyleMenuTriggered(QAction* pAction)
{
  std::string enum_name = pAction->data().toString().toStdString();

  cedar::proc::gui::StepItem::DisplayMode::Id mode;
  mode = cedar::proc::gui::StepItem::DisplayMode::type().get(enum_name);
  this->setDisplayMode(mode);
}

void cedar::proc::gui::StepItem::setDisplayMode(cedar::proc::gui::StepItem::DisplayMode::Id mode)
{
  this->mDisplayMode = mode;

  switch (mode)
  {
    case cedar::proc::gui::StepItem::DisplayMode::ICON_ONLY:
      this->setWidth(cedar::proc::gui::StepItem::mIconSize);
      this->setHeight(cedar::proc::gui::StepItem::mIconSize);
      break;

    case cedar::proc::gui::StepItem::DisplayMode::ICON_AND_TEXT:
      this->setWidth(cedar::proc::gui::StepItem::mDefaultWidth);
      this->setHeight(cedar::proc::gui::StepItem::mDefaultHeight);
      break;
  }

  this->updateAttachedItems();
  this->updateConnections();
  this->update();
}

QDockWidget* cedar::proc::gui::StepItem::createPlotDockWidget(const std::string& title) const
{
  QDockWidget *p_dock = new QDockWidget(QString::fromStdString(title), this->mpMainWindow);
  p_dock->setFloating(true);
  p_dock->setContentsMargins(0, 0, 0, 0);
  p_dock->setAllowedAreas(Qt::NoDockWidgetArea);

  return p_dock;
}

void cedar::proc::gui::StepItem::multiplot
     (
       const QPoint& position,
       std::vector<std::pair<cedar::proc::DataRole::Id, std::string> > data
     )
{
  typedef std::vector<std::pair<cedar::proc::DataRole::Id, std::string> > PlotList;

  int grid_spacing = 2;
  int columns = 2;

  if (data.empty())
  {
    for (std::vector<cedar::aux::Enum>::const_iterator enum_it = cedar::proc::DataRole::type().list().begin();
         enum_it != cedar::proc::DataRole::type().list().end();
         ++enum_it)
    {
      const cedar::aux::Enum& e = *enum_it;

      try
      {
        const cedar::proc::Step::SlotMap& slotmap = this->mStep->getDataSlots(e.id());
        for (cedar::proc::Step::SlotMap::const_iterator iter = slotmap.begin(); iter != slotmap.end(); ++iter)
        {
          cedar::proc::DataSlotPtr slot = iter->second;
          data.push_back(std::make_pair(e.id(), slot->getName()));
        }
      }
      catch (const cedar::proc::InvalidRoleException& e)
      {
        // that's ok, a step may not have any data in a certain role.
      }
    }
  }

  // initialize dock
  QDockWidget *p_dock = createPlotDockWidget(this->getStep()->getName());

  // initialize widget & layout
  QWidget *p_widget = new QWidget();
  p_widget->setContentsMargins(0, 0, 0, 0);
  p_dock->setWidget(p_widget);
  QGridLayout *p_layout = new QGridLayout();
  p_layout->setContentsMargins(grid_spacing, grid_spacing, grid_spacing, grid_spacing);
  p_layout->setSpacing(grid_spacing);
  p_widget->setLayout(p_layout);

  // iterate over all data slots
  int count = 0;
  QLabel *p_last_label = NULL;
  bool is_multiplot = false;
  cedar::aux::gui::DataPlotter *p_plotter = NULL;

  for (PlotList::const_iterator iter = data.begin(); iter != data.end(); ++iter)
  {

    try
    {
      const std::string& slot_name = iter->second;
      cedar::proc::DataRole::Id role = iter->first;
      cedar::proc::DataSlotPtr slot = this->mStep->getSlot(role, slot_name);
      cedar::aux::DataPtr data = slot->getData();
      const std::string& title = slot->getText();

      // skip slots that aren't set
      if (data)
      {
        int column = count % columns;
        int row = 2 * (count / columns);

        if (p_plotter == NULL || !p_plotter->canAppend(data))
        {
          // label
          p_last_label = new QLabel(QString::fromStdString(slot->getText()));
          p_layout->addWidget(p_last_label, row, column);
          p_layout->setRowStretch(row, 0);

          // plotter
          p_plotter = new cedar::aux::gui::DataPlotter();
          try
          {
            p_plotter->plot(data, title);
            p_layout->addWidget(p_plotter, row + 1, column);
            p_layout->setRowStretch(row + 1, 1);

            count += 1;
          }
          catch (cedar::aux::UnknownTypeException& exc)
          {
            // clean up allocated data
            delete p_plotter;
            p_plotter = NULL;
            delete p_last_label;
            p_last_label = NULL;
          }
        }
        else
        {
          try
          {
            p_plotter->append(data, title);
            p_last_label->setText("");
            is_multiplot = true;
          }
          catch (cedar::aux::UnknownTypeException& exc)
          {
            // nothing to do here, just do not append the data
          }
        }
      }
    }
    catch (const cedar::proc::InvalidRoleException& e)
    {
      cedar::aux::LogSingleton::getInstance()->warning
      (
        "Could not plot data. Exception: " + e.exceptionInfo(),
        "cedar::proc::gui::StepItem::multiplot"
      );
    }
    catch (const cedar::proc::InvalidNameException& e)
    {
      cedar::aux::LogSingleton::getInstance()->warning
      (
        "Could not plot data. Exception: " + e.exceptionInfo(),
        "cedar::proc::gui::StepItem::multiplot"
      );
    }
  }

  // if there is only one plot and it is a multiplot, we need no label
  if (count == 1 && is_multiplot)
  {
    delete p_last_label;
  }

  // make all columns have the same stretch factor
  for (int column = 0; column < p_layout->columnCount(); ++column)
  {
    p_layout->setColumnStretch(column, 1);
  }

  // adapt size of plot widget/layout
  int base_size = 200;
  p_dock->setGeometry
  (
    QRect
    (
      position,
      QSize(base_size * p_layout->columnCount(), base_size * p_layout->rowCount() / 2)
    )
  );
  p_dock->show();
}

void cedar::proc::gui::StepItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  painter->save(); // save current painter settings

  //! @todo make drawing of steps pretty.

  qreal padding;

  switch (this->mDisplayMode)
  {
    case cedar::proc::gui::StepItem::DisplayMode::ICON_ONLY:
      padding = 0;
      break;

    default:
      padding = 5;
  }

  this->paintFrame(painter, style, widget);

  this->mStepIcon.paint(painter, padding, padding, mIconSize, mIconSize);

  if (this->mDisplayMode == cedar::proc::gui::StepItem::DisplayMode::ICON_AND_TEXT)
  {
    painter->drawText(QPointF(2 * padding + mIconSize, 15), this->mClassId->getClassName().c_str());
    painter->drawText(QPointF(2 * padding + mIconSize, 25), this->mStep->getName().c_str());
  }

  painter->restore(); // restore saved painter settings
}

cedar::proc::StepPtr cedar::proc::gui::StepItem::getStep()
{
  return this->mStep;
}

void cedar::proc::gui::StepItem::disconnect()
{
}
