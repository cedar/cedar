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
#include "cedar/processing/gui/PropertyPane.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/Step.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/auxiliaries/gui/DataPlotter.h"
#include "cedar/auxiliaries/gui/PlotManager.h"
#include "cedar/auxiliaries/gui/PlotDeclaration.h"
#include "cedar/auxiliaries/gui/exceptions.h"
#include "cedar/auxiliaries/TypeHierarchyMap.h"
#include "cedar/auxiliaries/Data.h"
#include "cedar/auxiliaries/Singleton.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/casts.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/units/Time.h"

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
#include <QMessageBox>
#include <QPushButton>
#include <string>
#include <set>


//----------------------------------------------------------------------------------------------------------------------
// static members
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::EnumType<cedar::proc::gui::StepItem::DisplayMode> cedar::proc::gui::StepItem::DisplayMode::mType;

const int cedar::proc::gui::StepItem::mIconSize = 40;
const qreal cedar::proc::gui::StepItem::mDefaultWidth = static_cast<qreal>(160);
const qreal cedar::proc::gui::StepItem::mDefaultHeight = static_cast<qreal>(50);
const qreal cedar::proc::gui::StepItem::M_BASE_DATA_SLOT_SIZE = static_cast<qreal>(12.0);
const qreal cedar::proc::gui::StepItem::M_DATA_SLOT_PADDING = static_cast<qreal>(3.0);

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
mpMainWindow(pMainWindow),
mDisplayMode(cedar::proc::gui::StepItem::DisplayMode::ICON_AND_TEXT),
mpIconDisplay(NULL)
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
mpMainWindow(pMainWindow),
mDisplayMode(cedar::proc::gui::StepItem::DisplayMode::ICON_AND_TEXT)
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);

  this->construct();
}

void cedar::proc::gui::StepItem::construct()
{
  QObject::connect(this, SIGNAL(stepStateChanged()), this, SLOT(updateStepState()));

  this->setFlags(this->flags() | QGraphicsItem::ItemIsSelectable
                               | QGraphicsItem::ItemIsMovable
                               );

  if (cedar::proc::gui::SettingsSingleton::getInstance()->useGraphicsItemShadowEffects())
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
  const QString& icon,
  const QString& description,
  const QColor& bgColor
)
{
  qreal padding = 1;
  this->mpRectangle = new QGraphicsRectItem
                      (
                        -padding,
                        -padding,
                        cedar::proc::gui::StepItem::M_BASE_DATA_SLOT_SIZE + 2 * padding,
                        cedar::proc::gui::StepItem::M_BASE_DATA_SLOT_SIZE + 2 * padding,
                        pStep
                      );
  this->mpIcon = new QGraphicsSvgItem
                 (
                   icon,
                   this->mpRectangle
                 );

  // setting this cache mode makes sure that when writing out an svg file, the icon will not be pixelized
  this->mpIcon->setCacheMode(QGraphicsItem::NoCache);

  this->mpIcon->setToolTip(description);

  qreal h = this->mpIcon->boundingRect().height();
  this->mpIcon->setScale(cedar::proc::gui::StepItem::M_BASE_DATA_SLOT_SIZE / h);

  QPen pen = this->mpRectangle->pen();
  pen.setWidth(1);
  pen.setColor(QColor(0, 0, 0));
  QBrush bg(bgColor);
  this->mpRectangle->setPen(pen);
  this->mpRectangle->setBrush(bg);
}

cedar::proc::gui::StepItem::~StepItem()
{
  for(auto child_widget : mChildWidgets)
  {
    child_widget->close();
  }
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

void cedar::proc::gui::StepItem::itemSelected(bool selected)
{
  for (auto role_iter = this->mSlotMap.begin(); role_iter != this->mSlotMap.end(); ++role_iter)
  {
    auto slot_map = role_iter->second;
    for (auto slot_iter = slot_map.begin(); slot_iter != slot_map.end(); ++slot_iter)
    {
      auto slot = slot_iter->second;
      slot->setHighlightedBySelection(selected);
    }
  }
}

void cedar::proc::gui::StepItem::demagnetizeSlots()
{
  auto slot_map_iter = this->mSlotMap.find(cedar::proc::DataRole::INPUT);

  if (slot_map_iter == this->mSlotMap.end())
  {
    return;
  }

  bool changes = false;
  auto slot_map = slot_map_iter->second;
  for (auto slot_iter = slot_map.begin(); slot_iter != slot_map.end(); ++slot_iter)
  {
    cedar::proc::gui::DataSlotItem* p_slot = slot_iter->second;
    if (p_slot->getMagneticScale() != 1.0)
    {
      changes = true;
    }

    p_slot->setMagneticScale(1.0);
  }

  if (changes)
  {
    this->updateDataSlotPositions();
  }
}

void cedar::proc::gui::StepItem::magnetizeSlots(const QPointF& mousePositionInScene)
{
  double max_distance = 100.0;
  double scale_factor = cedar::proc::gui::SettingsSingleton::getInstance()->getDataSlotScaling();
  double scale_sensitivity = cedar::proc::gui::SettingsSingleton::getInstance()->getDataSlotScalingSensitivity();

  if (!cedar::proc::gui::SettingsSingleton::getInstance()->getDataSlotScalingEnabled())
  {
    scale_factor = 1.0;
  }

  auto slot_map_iter = this->mSlotMap.find(cedar::proc::DataRole::INPUT);

  if (slot_map_iter == this->mSlotMap.end())
  {
    return;
  }

  bool changes = false;
  auto slot_map = slot_map_iter->second;
  for (auto slot_iter = slot_map.begin(); slot_iter != slot_map.end(); ++slot_iter)
  {
    cedar::proc::gui::DataSlotItem* p_slot = slot_iter->second;
    qreal min_distance = p_slot->width(); // since they are circular, both width and height correspond to the diameter
    QPointF diff = p_slot->scenePos() - mousePositionInScene;
    qreal distance = std::max(min_distance, sqrt(diff.x() * diff.x() + diff.y() * diff.y()));
    if (distance < max_distance)
    {
      changes = true;
      qreal closeness = static_cast<qreal>(1.0) - (distance - min_distance)/(max_distance - min_distance);
      qreal factor = std::pow(closeness, scale_sensitivity);
      CEDAR_DEBUG_ASSERT(factor <= static_cast<qreal>(1.0));
      CEDAR_DEBUG_ASSERT(factor >= static_cast<qreal>(0.0));
      qreal scale = (static_cast<qreal>(1.0) - factor) * static_cast<qreal>(1.0) + factor * scale_factor;
      p_slot->setMagneticScale(scale);
    }
  }

  if (changes)
  {
    this->updateDataSlotPositions();
  }
}

void cedar::proc::gui::StepItem::Decoration::setPosition(const QPointF& pos)
{
  this->mpRectangle->setPos(pos);
}

void cedar::proc::gui::StepItem::Decoration::setSize(double sizeFactor)
{
  const qreal padding = 1;
  const qreal size = static_cast<qreal>(0.8)
                     * static_cast<qreal>(sizeFactor)
                     * cedar::proc::gui::StepItem::M_BASE_DATA_SLOT_SIZE;
  QRectF new_dims = this->mpRectangle->rect();
  new_dims.setWidth(size + 2*padding);
  new_dims.setHeight(size + 2*padding);
  this->mpRectangle->setRect(new_dims);
  qreal h = this->mpIcon->boundingRect().height();
  this->mpIcon->setScale(size / h);
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

void cedar::proc::gui::StepItem::updateToolTip()
{
  QString tool_tip
    = QString("<table>"
                "<tr>"
                  "<th>Measurement:</th>"
                  "<th>Last</th>"
                  "<th>Average</th>"
                "</tr>"
                "<tr>"
                  "<td>locking</td>"
                  "<td align=\"right\">%3</td>"
                  "<td align=\"right\">%4</td>"
                "</tr>"
                "<tr>"
                  "<td>compute call</td>"
                  "<td align=\"right\">%1</td>"
                  "<td align=\"right\">%2</td>"
                "</tr>"
                "<tr>"
                  "<td>round time</td>"
                  "<td align=\"right\">%5</td>"
                  "<td align=\"right\">%6</td>"
                "</tr>"
              "</table>");

  std::vector<boost::function<cedar::unit::Time ()> > measurements;
  measurements.push_back(boost::bind(&cedar::proc::Step::getRunTimeMeasurement, this->mStep));
  measurements.push_back(boost::bind(&cedar::proc::Step::getRunTimeAverage, this->mStep));
  measurements.push_back(boost::bind(&cedar::proc::Step::getLockTimeMeasurement, this->mStep));
  measurements.push_back(boost::bind(&cedar::proc::Step::getLockTimeAverage, this->mStep));
  measurements.push_back(boost::bind(&cedar::proc::Step::getRoundTimeMeasurement, this->mStep));
  measurements.push_back(boost::bind(&cedar::proc::Step::getRoundTimeAverage, this->mStep));

  for (size_t i = 0; i < measurements.size(); ++i)
  {
    try
    {
      cedar::unit::Time ms = measurements.at(i)();
      double dval = ms / (0.001 * cedar::unit::seconds);
      tool_tip = tool_tip.arg(QString("%1 ms").arg(dval, 0, 'f', 1));
    }
    catch (const cedar::proc::NoMeasurementException&)
    {
      tool_tip = tool_tip.arg("n/a");
    }
  }

  const auto& annotation = this->mStep->getStateAnnotation();
  if (!annotation.empty())
  {
    // Replace any non-html characters in the annotation string by their html equivalents.
    QString escaped_annotation = QString::fromStdString(annotation)
                                     .replace("&","&amp;")
                                     .replace(">","&gt;")
                                     .replace("<","&lt;");

    if (!escaped_annotation.isEmpty())
    {
      tool_tip += escaped_annotation;
    }
  }

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

void cedar::proc::gui::StepItem::updateStepState()
{
  this->setFillStyle(Qt::SolidPattern, false);

  switch (this->mStep->getState())
  {
    case cedar::proc::Step::STATE_EXCEPTION_ON_START:
      this->setFillStyle(Qt::BDiagPattern);
    case cedar::proc::Step::STATE_EXCEPTION:
    case cedar::proc::Step::STATE_NOT_RUNNING:
      this->setOutlineColor(Qt::red);
      this->setFillColor(QColor(255, 175, 175));
      break;

    case cedar::proc::Step::STATE_RUNNING:
    default:
      this->setOutlineColor(cedar::proc::gui::GraphicsBase::mDefaultOutlineColor);
      this->setFillColor(cedar::proc::gui::GraphicsBase::mDefaultFillColor);
  }
  this->update();
}

void cedar::proc::gui::StepItem::handleStepNameChanged()
{
  this->redraw();
  // change title of child widgets
  QString step_name = QString::fromStdString(this->mStep->getName());
  for(auto childWidget : mChildWidgets)
  {
    childWidget->setWindowTitle(step_name);
  }
}

void cedar::proc::gui::StepItem::redraw()
{
  this->update();
}

void cedar::proc::gui::StepItem::setStep(cedar::proc::StepPtr step)
{
  switch (cedar::proc::gui::SettingsSingleton::getInstance()->getDefaultDisplayMode())
  {
    case cedar::proc::gui::Settings::StepDisplayMode::ICON_ONLY:
      this->setDisplayMode(cedar::proc::gui::StepItem::DisplayMode::ICON_ONLY);
      break;

    case cedar::proc::gui::Settings::StepDisplayMode::ICON_AND_TEXT:
      this->setDisplayMode(cedar::proc::gui::StepItem::DisplayMode::ICON_AND_TEXT);
      break;

    case cedar::proc::gui::Settings::StepDisplayMode::TEXT_FOR_LOOPED:
      if (step->isLooped())
      {
        this->setDisplayMode(cedar::proc::gui::StepItem::DisplayMode::ICON_AND_TEXT);
      }
      else
      {
        this->setDisplayMode(cedar::proc::gui::StepItem::DisplayMode::ICON_ONLY);
      }
      break;
  }

  this->setElement(step);
  this->mStep = step;
  this->mClassId = cedar::proc::ElementManagerSingleton::getInstance()->getDeclarationOf(this->mStep);
  CEDAR_DEBUG_ASSERT(boost::dynamic_pointer_cast<cedar::proc::ConstElementDeclaration>(this->mClassId))
  cedar::proc::ConstElementDeclarationPtr elem_decl
    = boost::static_pointer_cast<cedar::proc::ConstElementDeclaration>(this->mClassId);

  if (this->mpIconDisplay != NULL)
  {
    delete this->mpIconDisplay;
    this->mpIconDisplay = NULL;
  }
  this->mpIconDisplay = new QGraphicsSvgItem(elem_decl->determinedIconPath(), this);

  // setting this cache mode makes sure that when writing out an svg file, the icon will not be pixelized
  this->mpIconDisplay->setCacheMode(QGraphicsItem::NoCache);

  this->updateIconGeometry();

  this->addDataItems();
  this->addTriggerItems();
  this->addDecorations();

  mStateChangedConnection = step->connectToStateChanged(boost::bind(&cedar::proc::gui::StepItem::emitStepStateChanged, this));
  QObject::connect(step.get(), SIGNAL(nameChanged()), this, SLOT(handleStepNameChanged()));

  mSlotAddedConnection.disconnect();
  mSlotRemovedConnection.disconnect();

  mSlotAddedConnection
    = step->connectToSlotAddedSignal(boost::bind(&cedar::proc::gui::StepItem::slotAdded, this, _1, _2));
  mSlotRemovedConnection
    = step->connectToSlotRemovedSignal(boost::bind(&cedar::proc::gui::StepItem::slotRemoved, this, _1, _2));
}

void cedar::proc::gui::StepItem::updateIconGeometry()
{
  if (this->mpIconDisplay == NULL)
  {
    return;
  }

  qreal padding = this->getContentsPadding();
  this->mpIconDisplay->setPos(padding, padding);
  qreal dest = static_cast<qreal>(this->mIconSize);
  qreal w = this->mpIconDisplay->boundingRect().width();
  qreal h = this->mpIconDisplay->boundingRect().width();
  qreal major = std::max(w, h);
  this->mpIconDisplay->setScale(dest / major);
}

void cedar::proc::gui::StepItem::emitStepStateChanged()
{
  emit stepStateChanged();
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
  else
  {
    // apply settings for the currently selected (default) display mode
    this->setDisplayMode(this->mDisplayMode);
  }
}

void cedar::proc::gui::StepItem::writeConfiguration(cedar::aux::ConfigurationNode& root) const
{
  root.put("step", this->mStep->getName());
  root.put("display style", cedar::proc::gui::StepItem::DisplayMode::type().get(this->mDisplayMode).name());
  this->cedar::proc::gui::GraphicsBase::writeConfiguration(root);
}

void cedar::proc::gui::StepItem::setRecorded(bool status)
{
	if (status)
	{
	  if (!mpRecordedDecoration)
	  {
      mpRecordedDecoration = DecorationPtr(
        new Decoration
        (
          this,
          ":/decorations/record.svg",
          "This step has one or more slots registered in the recorder."
        )
      );
      this->mDecorations.push_back(mpRecordedDecoration);
	  }
	}
	else
	{
	    for (unsigned int i = 0; i < mDecorations.size();i++)
	    {
	      if (mDecorations[i]==mpRecordedDecoration)
	      {
	         mDecorations.erase(mDecorations.begin()+i);
	         mpRecordedDecoration.reset();
	         break;
	      }
	    }
	}

	this->updateDecorationPositions();

}

void cedar::proc::gui::StepItem::addDecorations()
{
  this->mDecorations.clear();

  if (this->getStep() && this->getStep()->isLooped())
  {
    DecorationPtr decoration
    (
      new Decoration
      (
        this,
        ":/decorations/looped.svg",
        "This step is looped, i.e., it expects to be connected to a looped trigger."
      )
    );

    this->mDecorations.push_back(decoration);
  }

  auto declaration = cedar::proc::ElementManagerSingleton::getInstance()->getDeclarationOf(this->getElement());

  if (declaration->isDeprecated())
  {
    std::string dep_msg = "This step is deprecated.";

    if (!declaration->getDeprecationDescription().empty())
    {
      dep_msg += " " + declaration->getDeprecationDescription();
    }

    DecorationPtr decoration
    (
      new Decoration
      (
        this,
        ":/cedar/auxiliaries/gui/warning.svg",
        QString::fromStdString(dep_msg),
        QColor(255, 240, 110)
      )
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

  qreal factor;
  switch (this->mDisplayMode)
  {
    case DisplayMode::ICON_ONLY:
      factor = 0.7;
      break;

    default:
      factor = 1.0;
  }

  QPointF offset_dir(-1, 0);
  qreal distance = 15.0;
  for (size_t i = 0; i < this->mDecorations.size(); ++i)
  {
    DecorationPtr decoration = this->mDecorations[i];
    decoration->setPosition(origin + static_cast<qreal>(i) * factor * distance * offset_dir);
    decoration->setSize(factor);
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
  for (auto role : cedar::proc::DataRole::type().list())
  {
    if (role == cedar::aux::Enum::UNDEFINED)
      continue;

    // populate step item list
    this->mSlotMap[role] = DataSlotNameMap();

    if (this->mStep->hasRole(role))
    {
      for (auto slot : this->mStep->getOrderedDataSlots(role))
      {
        // use a non-const version of this slot
        this->addDataItemFor(this->mStep->getSlot(role, slot->getName()));
      }
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

  data_slot_size[cedar::proc::DataRole::BUFFER] = M_BASE_DATA_SLOT_SIZE * static_cast<qreal>(0.75) * style_factor;
  add_directions[cedar::proc::DataRole::BUFFER] = QPointF(1, 0);
  add_origins[cedar::proc::DataRole::BUFFER] = QPointF(0, -M_DATA_SLOT_PADDING - data_slot_size[cedar::proc::DataRole::BUFFER]);

  data_slot_size[cedar::proc::DataRole::INPUT] = M_BASE_DATA_SLOT_SIZE * style_factor;
  add_origins[cedar::proc::DataRole::INPUT] = QPointF(-M_DATA_SLOT_PADDING - data_slot_size[cedar::proc::DataRole::INPUT], 0);
  add_directions[cedar::proc::DataRole::INPUT] = QPointF(0, 1);

  data_slot_size[cedar::proc::DataRole::OUTPUT] = M_BASE_DATA_SLOT_SIZE * style_factor;
  add_origins[cedar::proc::DataRole::OUTPUT] = QPointF(this->width() + M_DATA_SLOT_PADDING, 0);
  add_directions[cedar::proc::DataRole::OUTPUT] = QPointF(0, 1);

  for (auto role_map_pair : this->mSlotMap)
  {
    cedar::proc::DataRole::Id role = role_map_pair.first;
    DataSlotNameMap& slot_item_map = role_map_pair.second;

    if (role == cedar::aux::Enum::UNDEFINED)
      continue;


    CEDAR_DEBUG_ASSERT(add_origins.find(role) != add_origins.end());
    CEDAR_DEBUG_ASSERT(add_directions.find(role) != add_directions.end());
    CEDAR_DEBUG_ASSERT(data_slot_size.find(role) != data_slot_size.end());

    const QPointF& origin = add_origins[role];
    const QPointF& direction = add_directions[role];

    if (this->mStep->hasRole(role))
    {
      QPointF current_origin = QPointF(0, 0);
      const cedar::proc::Step::SlotList& slotmap = this->mStep->getOrderedDataSlots(role);
      for (cedar::proc::Step::SlotList::const_iterator iter = slotmap.begin(); iter != slotmap.end(); ++iter)
      {
        const std::string& slot_name = (*iter)->getName();
        CEDAR_DEBUG_ASSERT(slot_item_map.find(slot_name) != slot_item_map.end());

        cedar::proc::gui::DataSlotItem *p_item = slot_item_map[slot_name];

        qreal slot_size = data_slot_size[role] * p_item->getMagneticScale();
        qreal size_diff = slot_size - data_slot_size[role];

        p_item->setWidth(slot_size);
        p_item->setHeight(slot_size);
        qreal x = origin.x();
        qreal y = origin.y();
        p_item->setPos(QPointF(x - size_diff, y) + current_origin);
        current_origin += direction * (slot_size + M_DATA_SLOT_PADDING);
      }
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
    CEDAR_THROW(cedar::aux::InvalidNameException, "No slot item named \"" + name +
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
       std::map<QAction*, std::pair<cedar::aux::gui::ConstPlotDeclarationPtr, cedar::aux::Enum> >& declMap
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
          std::set<cedar::aux::gui::ConstPlotDeclarationPtr> plots;
          cedar::aux::gui::PlotManagerSingleton::getInstance()->getPlotClassesFor(data, plots);

          if (plots.empty())
          {
            QAction *p_action = p_menu->addAction("no plots available");
            p_action->setDisabled(true);
          }
          else
          {
            for (auto iter = plots.begin(); iter != plots.end(); ++iter)
            {
              cedar::aux::gui::ConstPlotDeclarationPtr declaration = *iter;
              QAction *p_action = p_menu->addAction(QString::fromStdString(declaration->getClassName()));
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
  std::string& dataName,
  const cedar::aux::Enum& role
)
{
  cedar::aux::gui::ConstPlotDeclarationPtr decl =
    cedar::aux::gui::PlotManagerSingleton::getInstance()->getDefaultDeclarationFor(this->mStep->getData(role, dataName));
  showPlot(position, dataName, role, decl);
}

void cedar::proc::gui::StepItem::showPlot
(
  const QPoint& position,
  std::string& dataName,
  const cedar::aux::Enum& role,
  cedar::aux::gui::ConstPlotDeclarationPtr declaration
)
{
  // create data-List
  cedar::proc::ElementDeclaration::DataList data_list;
  data_list.push_back(cedar::proc::PlotDataPtr(new cedar::proc::PlotData(role.id(), dataName, false, declaration->getClassName())));

  auto p_plot_widget = new cedar::proc::gui::PlotWidget(this->mStep, data_list);
  auto p_dock_widget = this->createDockWidgetForPlots(this->mStep->getName(), p_plot_widget, position);

  p_dock_widget->show();
}

void cedar::proc::gui::StepItem::openProperties()
{
  cedar::proc::gui::PropertyPane* props = new cedar::proc::gui::PropertyPane();
  auto p_dock_widget = this->createDockWidget("Properties", props);
  props->display(this->getStep());
  p_dock_widget->show();
}

void cedar::proc::gui::StepItem::openActionsDock()
{
  QWidget* p_actions = new QWidget();
  QVBoxLayout* p_layout = new QVBoxLayout();
  const cedar::proc::Step::ActionMap& map = this->mStep->getActions();
  for (cedar::proc::Step::ActionMap::const_iterator iter = map.begin(); iter != map.end(); ++iter)
  {
    QPushButton* p_button = new QPushButton(iter->first.c_str());
    p_layout->addWidget(p_button);
    QObject::connect(p_button, SIGNAL(clicked()), this, SLOT(handleExternalActionButtons()));
  }

  p_actions->setLayout(p_layout);
  std::string title = "Actions of step \"" + this->mStep->getName() + "\"";
  auto p_dock_widget = this->createDockWidget(title, p_actions);
  p_dock_widget->show();
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
  p_data->setIcon(QIcon(":/menus/data.svg"));

  menu.addSeparator(); // ----------------------------------------------------------------------------------------------

  this->fillDefinedPlots(menu, event->screenPos());

  QMenu *p_advanced_plotting = menu.addMenu("advanced plotting");
  p_advanced_plotting->setIcon(QIcon(":/menus/plot_advanced.svg"));

  QAction* p_close_all_plots = menu.addAction("close all plots");
  p_close_all_plots->setIcon(QIcon(":/menus/close_all_plots.svg"));
  QObject::connect(p_close_all_plots, SIGNAL(triggered()), this, SLOT(closeAllPlots()));

  QAction* p_toggle_visibility_of_plots = menu.addAction("toggle visibility");
  p_toggle_visibility_of_plots->setIcon(QIcon(":/menus/toggle_plot_visibility.svg"));
  QObject::connect(p_toggle_visibility_of_plots, SIGNAL(triggered()), this, SLOT(toggleVisibilityOfPlots()));

  menu.addSeparator(); // ----------------------------------------------------------------------------------------------

  QAction* p_properties = menu.addAction("open properties widget");
  QObject::connect(p_properties, SIGNAL(triggered()), this, SLOT(openProperties()));
  p_properties->setIcon(QIcon(":/menus/properties.svg"));

  menu.addSeparator(); // ----------------------------------------------------------------------------------------------

  std::map<QAction*, std::pair<cedar::aux::gui::ConstPlotDeclarationPtr, cedar::aux::Enum> > advanced_plot_map;
  this->fillPlots(p_advanced_plotting, advanced_plot_map);

  QMenu *p_actions_menu = menu.addMenu("actions");
  p_actions_menu->setIcon(QIcon(":/menus/actions.svg"));
  menu.addSeparator(); // ----------------------------------------------------------------------------------------------

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
    p_actions_menu->addSeparator();
    p_actions_menu->addAction("open actions dock");
  }

  menu.addSeparator(); // ----------------------------------------------------------------------------------------------
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

  menu.addSeparator(); // ----------------------------------------------------------------------------------------------
  this->fillDisplayStyleMenu(&menu);

  QAction *a = menu.exec(event->screenPos());

  if (a == NULL)
    return;

  // plot data
  if (a->parentWidget() == p_data)
  {
    std::string data_name = a->data().toString().toStdString();
    const cedar::aux::Enum& role = action_type_map[a];
    
    this->showPlot(event->screenPos(), data_name, role);
  }
  // execute an action
  else if (a->parentWidget() == p_actions_menu)
  {
    std::string action = a->text().toStdString();
    // decide whether this is a normal action or the docking action
    if (action != "open actions dock")
    {
      this->mStep->callAction(action);
    }
    else
    {
      this->openActionsDock();
    }
  }
  // advanced plots
  else if (advanced_plot_map.find(a) != advanced_plot_map.end())
  {
    cedar::aux::gui::ConstPlotDeclarationPtr declaration = advanced_plot_map.find(a)->second.first;
    const cedar::aux::Enum& role = advanced_plot_map.find(a)->second.second;
    std::string data_name = a->data().toString().toStdString();

    this->showPlot(event->screenPos(), data_name, role, declaration);
  }
}

void cedar::proc::gui::StepItem::fillDefinedPlots(QMenu& menu, const QPoint& plotPosition)
{
  // get declaration of the element displayed by this item
  auto decl = cedar::proc::ElementManagerSingleton::getInstance()->getDeclarationOf(this->mStep);
  CEDAR_DEBUG_ASSERT(boost::dynamic_pointer_cast<cedar::proc::ConstElementDeclaration>(decl));
  auto elem_decl = boost::static_pointer_cast<cedar::proc::ConstElementDeclaration>(decl);

  if (!elem_decl->getDefaultPlot().empty())
  {
    QAction* p_default_plot = menu.addAction(QString::fromStdString(elem_decl->getDefaultPlot()));

    // find declaration for the default plot
    size_t default_index = 0;
#ifdef DEBUG    
    bool found = false;
#endif    
    for (size_t i = 0; i < elem_decl->definedPlots().size(); ++i)
    {
      const std::string& plot_name = elem_decl->definedPlots()[i].mName;
      if (plot_name == elem_decl->getDefaultPlot())
      {
        default_index = i;
#ifdef DEBUG        
        found = true;
#endif        
        break;
      }
    }
    CEDAR_DEBUG_ASSERT(found);

    if (!elem_decl->definedPlots()[default_index].mIcon.empty())
    {
      p_default_plot->setIcon(QIcon(QString::fromStdString(elem_decl->definedPlots()[default_index].mIcon)));
    }
    p_default_plot->setData(plotPosition);
    QObject::connect(p_default_plot, SIGNAL(triggered()), this, SLOT(openDefinedPlotAction()));
  }
  else
  {
    this->addPlotAllAction(menu, plotPosition);
  }

  if (!decl || !elem_decl || elem_decl->definedPlots().empty() || elem_decl->getDefaultPlot().empty())
  {
    QAction *p_defined_plots = menu.addAction("defined plots");
    p_defined_plots->setIcon(QIcon(":/menus/plot.svg"));
    p_defined_plots->setDisabled(true);
  }
  else
  {
    QMenu *p_defined_plots = menu.addMenu("defined plots");
    p_defined_plots->setIcon(QIcon(":/menus/plot.svg"));

    this->addPlotAllAction(*p_defined_plots, plotPosition);
    p_defined_plots->addSeparator();

    // list all defined plots, if available
    for (size_t i = 0; i < elem_decl->definedPlots().size(); ++i)
    {
      const std::string& plot_name = elem_decl->definedPlots()[i].mName;
      QAction* p_action = p_defined_plots->addAction(QString::fromStdString(plot_name));
      p_action->setData(plotPosition);

      if (!elem_decl->definedPlots()[i].mIcon.empty())
      {
        p_action->setIcon(QIcon(QString::fromStdString(elem_decl->definedPlots()[i].mIcon)));
      }
      QObject::connect(p_action, SIGNAL(triggered()), this, SLOT(openDefinedPlotAction()));
    }
  }
}

void cedar::proc::gui::StepItem::addPlotAllAction(QMenu& menu, const QPoint& plotPosition)
{
  QAction* p_plot_all = menu.addAction("plot all");
  p_plot_all->setIcon(QIcon(":/menus/plot_all.svg"));
  p_plot_all->setData(plotPosition);
  QObject::connect(p_plot_all, SIGNAL(triggered()), this, SLOT(plotAll()));
}

void cedar::proc::gui::StepItem::plotAll()
{
  QAction* p_sender = dynamic_cast<QAction*>(QObject::sender());
  CEDAR_DEBUG_ASSERT(p_sender != NULL);

  // get datalist of step
  cedar::proc::ElementDeclaration::DataList data = cedar::proc::ElementDeclaration::DataList();
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
        data.push_back(cedar::proc::PlotDataPtr(new cedar::proc::PlotData(e.id(), slot->getName())));
      }
    }
    catch (const cedar::proc::InvalidRoleException& e)
    {
      // that's ok, a step may not have any data in a certain role.
      // Kai: I disagree, exceptions should be exceptional see http://pragmatictips.com/34
    }
  }
  auto p_plot_widget = new PlotWidget(this->mStep, data);
  auto p_dock_widget = this->createDockWidgetForPlots(this->mStep->getName(), p_plot_widget, p_sender->data().toPoint());
  
  p_dock_widget->show();
}

void cedar::proc::gui::StepItem::openDefinedPlotAction()
{
  QAction* p_action = dynamic_cast<QAction*>(QObject::sender());
  CEDAR_DEBUG_ASSERT(p_action != NULL);
  std::string plot_name = p_action->text().toStdString();

  // get declaration of the element displayed by this item
  auto decl = cedar::proc::ElementManagerSingleton::getInstance()->getDeclarationOf(this->mStep);
  CEDAR_DEBUG_ASSERT(boost::dynamic_pointer_cast<cedar::proc::ConstElementDeclaration>(decl));
  auto elem_decl = boost::static_pointer_cast<cedar::proc::ConstElementDeclaration>(decl);

  // find the list of data to plot for this item
  size_t list_index = elem_decl->definedPlots().size();
  for (size_t i = 0; i < elem_decl->definedPlots().size(); ++i)
  {
    const std::string& name = elem_decl->definedPlots()[i].mName;
    if (plot_name == name)
    {
      list_index = i;
      break;
    }
  }

  if (list_index >= elem_decl->definedPlots().size())
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Error: could not find the definition for plot \"" + plot_name + "\".",
      "cedar::proc::gui::StepItem::openDefinedPlotAction(QAction*)",
      "Plotting"
    );
    return;
  }

  auto p_plot_widget = new PlotWidget(this->mStep, elem_decl->definedPlots()[list_index].mData);
  auto p_dock_widget = this->createDockWidgetForPlots(this->mStep->getName(), p_plot_widget, p_action->data().toPoint());
  
  p_dock_widget->show();
}

void cedar::proc::gui::StepItem::fillDisplayStyleMenu(QMenu* pMenu)
{
  QMenu* p_sub_menu = pMenu->addMenu("display style");
  p_sub_menu->setIcon(QIcon(":/menus/display_style.svg"));

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

  this->updateIconGeometry();
  this->updateAttachedItems();
  this->updateConnections();
  this->update();
}

QWidget* cedar::proc::gui::StepItem::createDockWidgetForPlots(const std::string& title, cedar::proc::gui::PlotWidget* pPlotWidget, const QPoint& position)
{
  auto p_dock_widget = this->createDockWidget(title, pPlotWidget);

  int base_size = 200;
  p_dock_widget->setGeometry
  (
    QRect
    (
      position,
      QSize(base_size * pPlotWidget->getColumnCount(), base_size * pPlotWidget->getRowCount())
    )
  );

  return p_dock_widget;
}

QWidget* cedar::proc::gui::StepItem::createDockWidget(const std::string& title, QWidget* pWidget)
{
  //!@todo There's duplicated code here -- unify
  if (this->mpMainWindow)
  {
    QDockWidget *p_dock = new QDockWidget(QString::fromStdString(title), this->mpMainWindow);
    p_dock->setAttribute(Qt::WA_DeleteOnClose, true);
    p_dock->setFloating(true);
    p_dock->setContentsMargins(0, 0, 0, 0);
    p_dock->setAllowedAreas(Qt::NoDockWidgetArea);
    p_dock->setWidget(pWidget);

    mChildWidgets.push_back(p_dock);
    QObject::connect(p_dock, SIGNAL(destroyed()), this, SLOT(removeChildWidget()));

    return p_dock;
  }
  else
  {
    QWidget* p_widget = new QWidget();
    p_widget->setAttribute(Qt::WA_DeleteOnClose, true);
    p_widget->setWindowTitle(QString::fromStdString(title));
    auto p_layout = new QVBoxLayout();
    p_layout->setContentsMargins(2, 2, 2, 2);
    p_layout->addWidget(pWidget);
    p_widget->setLayout(p_layout);

    mChildWidgets.push_back(p_widget);
    QObject::connect(p_widget, SIGNAL(destroyed()), this, SLOT(removeChildWidget()));

    return p_widget;
  }
}

void cedar::proc::gui::StepItem::removeChildWidget()
{
  auto it = mChildWidgets.begin();
  while(*it != QObject::sender() && it != mChildWidgets.end())
  {
    it++;
  }
  if(*it == QObject::sender())
  {
    mChildWidgets.erase(it);
  }
  else
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Could not find a reference to the destroyed ChildWidget.",
      "cedar::proc::gui::StepItem::removeChildWidget()"
    );
  }
}

qreal cedar::proc::gui::StepItem::getContentsPadding() const
{
  switch (this->mDisplayMode)
  {
    case cedar::proc::gui::StepItem::DisplayMode::ICON_ONLY:
      return static_cast<qreal>(0);
      break;

    default:
      return static_cast<qreal>(5);
  }
}

void cedar::proc::gui::StepItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  painter->save(); // save current painter settings

  qreal padding = this->getContentsPadding();

  this->paintFrame(painter, style, widget);

  if (this->mDisplayMode == cedar::proc::gui::StepItem::DisplayMode::ICON_AND_TEXT)
  {
    painter->drawText(QPointF(2 * padding + mIconSize, 15), this->mClassId->getClassNameWithoutNamespace().c_str());
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

void cedar::proc::gui::StepItem::handleExternalActionButtons()
{
  std::string action = cedar::aux::asserted_cast<QPushButton*>(QObject::sender())->text().toStdString();
  this->mStep->callAction(action);
}

void cedar::proc::gui::StepItem::writeOpenChildWidgets(cedar::aux::ConfigurationNode& node) const
{
  for(auto childWidget : mChildWidgets)
  {
    // all widgets in the mChildWidgets Vector should be QDockWidgets that contain a QWidget
    QWidget* dock_widget_child = cedar::aux::asserted_cast<QDockWidget*>(childWidget)->widget();
    // The contained QWidget may be of different types, we're only interested in the cedar::proc::gui::PlotWidget ones
    if(cedar::aux::objectTypeToString(dock_widget_child) == "cedar::proc::gui::PlotWidget")
    {
      cedar::aux::ConfigurationNode value_node;
      static_cast<cedar::proc::gui::PlotWidget*>(dock_widget_child)->writeConfiguration(value_node);
      node.push_back(cedar::aux::ConfigurationNode::value_type("", value_node));
    }
  }
}

void cedar::proc::gui::StepItem::addPlotWidget(cedar::proc::gui::PlotWidget* pPlotWidget, int x, int y, int width, int height)
{
  QPoint position = QPoint(x, y);
  auto p_dock_widget = this->createDockWidgetForPlots(this->mStep->getName(), pPlotWidget, position);
  p_dock_widget->resize(width, height);
  p_dock_widget->show();
}

void cedar::proc::gui::StepItem::closeAllPlots()
{
  this->closeAllChildWidgets();
}

void cedar::proc::gui::StepItem::toggleVisibilityOfPlots(bool visible)
{
  for(auto childWidget : mChildWidgets)
  {
    childWidget->setVisible(visible);
  }
}

void cedar::proc::gui::StepItem::closeAllChildWidgets()
{
  for(auto childWidget : mChildWidgets)
  {
    childWidget->close();
  }
  // mChildWidgets is emptied through close event.
}
