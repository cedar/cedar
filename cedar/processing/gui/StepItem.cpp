/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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
#include "cedar/auxiliaries/gui/DataPlotter.h"
#include "cedar/processing/gui/StepItem.h"
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
#include "cedar/auxiliaries/gui/PlotDeclaration.h"
#include "cedar/auxiliaries/TypeHierarchyMap.h"
#include "cedar/auxiliaries/Data.h"
#include "cedar/auxiliaries/Singleton.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/assert.h"

// SYSTEM INCLUDES
#include <QPainter>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QGraphicsDropShadowEffect>
#include <QLayout>
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::StepItem::StepItem(cedar::proc::StepPtr step, QMainWindow* pMainWindow)
:
cedar::proc::gui::GraphicsBase(160, 50,
                               cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_STEP,
                               cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_NONE),
mpMainWindow(pMainWindow),
mStepIcon(":/steps/no_icon.svg")
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);

  this->setStep(step);
  
  this->construct();
}

cedar::proc::gui::StepItem::StepItem(QMainWindow* pMainWindow)
:
cedar::proc::gui::GraphicsBase(160, 50,
                               cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_STEP,
                               cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_NONE),
mpMainWindow(pMainWindow),
mStepIcon(":/steps/no_icon.svg")
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

cedar::proc::gui::StepItem::~StepItem()
{
  cedar::aux::LogSingleton::getInstance()->freeing(this);

  mStateChangedConnection.disconnect();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::StepItem::stepStateChanged()
{
  switch (this->mStep->getState())
  {
    case cedar::proc::Step::STATE_EXCEPTION:
    case cedar::proc::Step::STATE_NOT_RUNNING:
      this->setOutlineColor(Qt::darkGray);
      this->setFillColor(QColor(235, 235, 235));
      break;

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
  this->mStep = step;
  this->mClassId = cedar::proc::DeclarationRegistrySingleton::getInstance()->getDeclarationOf(this->mStep);

  this->mStepIcon = QIcon(this->mClassId->getIconPath().c_str());
  if (this->mStepIcon.isNull())
  {
    this->mStepIcon = QIcon(":/steps/no_icon.svg");
  }

  this->addDataItems();
  this->addTriggerItems();

//  QObject::connect(step.get(), SIGNAL(stateChanged()), this, SLOT(stepStateChanged()));
  mStateChangedConnection = step->connectToStateChanged(boost::bind(&cedar::proc::gui::StepItem::stepStateChanged, this));
  QObject::connect(step.get(), SIGNAL(nameChanged()), this, SLOT(redraw()));
}

void cedar::proc::gui::StepItem::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  this->cedar::proc::gui::GraphicsBase::readConfiguration(node);
}

void cedar::proc::gui::StepItem::writeConfiguration(cedar::aux::ConfigurationNode& root) const
{
  root.put("step", this->mStep->getName());
  this->cedar::proc::gui::GraphicsBase::writeConfiguration(root);
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
  qreal data_size = 10.0; //!@todo don't hard-code the size of the data items
  qreal padding = static_cast<qreal>(3);
  std::map<cedar::proc::DataRole::Id, QPointF> add_origins;
  std::map<cedar::proc::DataRole::Id, QPointF> add_directions;

  add_origins[cedar::proc::DataRole::BUFFER] = QPointF(0, -padding - data_size);
  add_directions[cedar::proc::DataRole::BUFFER] = QPointF(1, 0);

  add_origins[cedar::proc::DataRole::INPUT] = QPointF(-padding - data_size, 0);
  add_directions[cedar::proc::DataRole::INPUT] = QPointF(0, 1);

  add_origins[cedar::proc::DataRole::OUTPUT] = QPointF(this->width() + padding, 0);
  add_directions[cedar::proc::DataRole::OUTPUT] = QPointF(0, 1);

  for (std::vector<cedar::aux::Enum>::const_iterator enum_it = cedar::proc::DataRole::type().list().begin();
      enum_it != cedar::proc::DataRole::type().list().end();
      ++enum_it)
  {
    if ( (*enum_it) == cedar::aux::Enum::UNDEFINED)
      continue;

    // populate step item list
    mSlotMap[*enum_it] = DataSlotNameMap();

#ifdef DEBUG
    if (add_origins.find(*enum_it) == add_origins.end() || add_directions.find(*enum_it) == add_directions.end())
    {
      std::cout << "Warning: the data role " << enum_it->prettyString() << " is not implemented properly in "
                   "StepItem::addDataItems. Skipping!" << std::endl;
      continue;
    }
#endif

    const QPointF& origin = add_origins[*enum_it];
    const QPointF& direction = add_directions[*enum_it];

    try
    {
      qreal count = 0;
      cedar::proc::Step::SlotMap& slotmap = this->mStep->getDataSlots(*enum_it);
      for (cedar::proc::Step::SlotMap::iterator iter = slotmap.begin(); iter != slotmap.end(); ++iter)
      {
        cedar::proc::DataSlotPtr slot = iter->second;
        cedar::proc::gui::DataSlotItem *p_item = new cedar::proc::gui::DataSlotItem(this, slot);
        p_item->setPos(origin + count * direction * (data_size + padding) );
        mSlotMap[slot->getRole()][slot->getName()] = p_item;
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
  DataSlotMap::iterator role_map = this->mSlotMap.find(role);

  if (role_map == this->mSlotMap.end())
  {
    CEDAR_THROW(cedar::proc::InvalidRoleException, "No slot items stored for role "
                                                   + cedar::proc::DataRole::type().get(role).prettyString()
                                                   );
  }

  DataSlotNameMap::iterator iter = role_map->second.find(name);
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
  typedef cedar::aux::gui::PlotDeclarationManager::Node PlotNode;
  typedef cedar::aux::gui::PlotDeclarationManager::ConstNodePtr ConstPlotNodePtr;

  for (std::vector<cedar::aux::Enum>::const_iterator enum_it = cedar::proc::DataRole::type().list().begin();
      enum_it != cedar::proc::DataRole::type().list().end();
      ++enum_it)
  {
    try
    {
      const cedar::aux::Enum& e = *enum_it;
      this->addRoleSeparator(e, pMenu);

      const cedar::proc::Step::SlotMap& slotmap = this->mStep->getDataSlots(e.id());
      for (cedar::proc::Step::SlotMap::const_iterator slot_iter = slotmap.begin(); slot_iter != slotmap.end(); ++slot_iter)
      {
        QMenu *p_menu = pMenu->addMenu(slot_iter->second->getText().c_str());
        cedar::aux::DataPtr data = slot_iter->second->getData();
        if (!data)
        {
          p_menu->setTitle("[unconnected] " + p_menu->title());
          p_menu->setEnabled(false);
        }
        else
        {
          // fill all possible plots into the menu
          try
          {
            std::set<ConstPlotNodePtr> bases;
            cedar::aux::gui::PlotDeclarationManagerSingleton::getInstance()->findDeepest(data, bases);
            for(std::set<ConstPlotNodePtr>::iterator iter = bases.begin(); iter != bases.end(); ++iter)
            {
              ConstPlotNodePtr node = *iter;
              cedar::aux::gui::PlotDeclarationPtr declaration = node->getData();
              QAction *p_action = p_menu->addAction(QString::fromStdString(declaration->getPlotClass()));
              p_action->setData(QString::fromStdString(slot_iter->first));
              declMap[p_action] = std::make_pair(declaration, e);
            }
          }
          catch (cedar::aux::UnknownTypeException&)
          {
            p_menu->addAction(QString::fromStdString("Type not found:" + cedar::aux::objectTypeToString(data)));
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
  QGraphicsSceneContextMenuEvent *event,
  cedar::aux::gui::PlotInterface* pPlot,
  cedar::proc::DataSlotPtr slot
)
{
  //!@todo It would be better if setting the title would be part of the actual widget
  std::string title = slot->getText();
  title += " (" + this->mStep->getName();
  title += "." + slot->getName() + ")";

  QDockWidget *p_dock = new QDockWidget(QString::fromStdString(title), mpMainWindow);
  p_dock->setFloating(true);
  p_dock->layout()->setContentsMargins(0, 0, 0, 0);

  QRect geometry = p_dock->geometry();
  geometry.setTopLeft(event->screenPos());
  geometry.setSize(QSize(200, 200));
  p_dock->setGeometry(geometry);

  p_dock->setWidget(pPlot);
  p_dock->show();
}

void cedar::proc::gui::StepItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
  QMenu menu;
  QMenu *p_data = menu.addMenu("data");

  QMenu *p_advanced_plotting = menu.addMenu("advanced plotting");
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

  QAction *p_delete_action = menu.addAction("delete");

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

      const cedar::proc::Step::SlotMap& slotmap = this->mStep->getDataSlots(e.id());
      for (cedar::proc::Step::SlotMap::const_iterator iter = slotmap.begin(); iter != slotmap.end(); ++iter)
      {
        QAction *p_action = p_data->addAction(iter->second->getText().c_str());
        p_action->setData(QString(iter->first.c_str()));
        if (iter->second->getData().get() == NULL)
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
    this->showPlot(event, p_plotter, slot);
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
    this->showPlot(event, p_plotter, slot);
  }
  // delete the step
  else if (a == p_delete_action)
  {
    //!@todo
  }
}

void cedar::proc::gui::StepItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  painter->save(); // save current painter settings

  //! @todo make drawing of steps pretty.

  qreal padding = 5;
  qreal icon_dim = 40;

  this->paintFrame(painter, style, widget);
  this->mStepIcon.paint(painter, padding, padding, icon_dim, icon_dim);
  painter->drawText(QPointF(2 * padding + icon_dim, 15), this->mClassId->getClassName().c_str());
  painter->drawText(QPointF(2 * padding + icon_dim, 25), this->mStep->getName().c_str());

  painter->restore(); // restore saved painter settings
}

cedar::proc::StepPtr cedar::proc::gui::StepItem::getStep()
{
  return this->mStep;
}

void cedar::proc::gui::StepItem::disconnect()
{
  // go through all DataSlots and remove connections
  for (size_t i = 0; i < cedar::proc::DataRole::type().list().size(); ++i)
  {
    cedar::proc::DataRole::Id id = cedar::proc::DataRole::type().list().at(i);
    if (id == cedar::aux::Enum::UNDEFINED)
    {
      continue;
    }
    cedar::proc::gui::StepItem::DataSlotNameMap& map = dynamic_cast<cedar::proc::gui::StepItem*>(this)->getSlotItems(id);
    for (cedar::proc::gui::StepItem::DataSlotNameMap::iterator it = map.begin(); it != map.end(); ++it)
    {
      it->second->removeAllConnections();
    }
  }

  // go through all triggers and remove their connections as well
  for (size_t i = 0; i < this->mTriggers.size(); ++i)
  {
    cedar::proc::gui::TriggerItem* p_trigger = this->mTriggers.at(i);
    p_trigger->removeAllConnections();
  }
}
