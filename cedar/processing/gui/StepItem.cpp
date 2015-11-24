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
#include "cedar/processing/gui/PlotWidget.h"
#include "cedar/processing/gui/PropertyPane.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/gui/DataPlotter.h"
#include "cedar/auxiliaries/gui/PlotManager.h"
#include "cedar/auxiliaries/gui/PlotDeclaration.h"
#include "cedar/auxiliaries/gui/exceptions.h"
#include "cedar/auxiliaries/TypeHierarchyMap.h"
#include "cedar/auxiliaries/Data.h"
#include "cedar/auxiliaries/Path.h"
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
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::StepItem::StepItem(cedar::proc::StepPtr step, QMainWindow* pMainWindow)
:
cedar::proc::gui::Connectable
(
  cedar::proc::gui::Connectable::M_DEFAULT_WIDTH,
  cedar::proc::gui::Connectable::M_DEFAULT_HEIGHT,
  cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_STEP,
  pMainWindow
)
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);

  this->setStep(step);
  
  this->construct();
}

cedar::proc::gui::StepItem::StepItem(QMainWindow* pMainWindow)
:
cedar::proc::gui::Connectable
(
  cedar::proc::gui::Connectable::M_DEFAULT_WIDTH,
  cedar::proc::gui::Connectable::M_DEFAULT_HEIGHT,
  cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_STEP,
  pMainWindow
)
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

cedar::proc::gui::StepItem::~StepItem()
{
  cedar::aux::LogSingleton::getInstance()->freeing(this);

  mStateChangedConnection.disconnect();

  if (this->scene())
  {
    cedar::aux::asserted_cast<cedar::proc::gui::Scene*>(this->scene())->removeStepItem(this);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::StepItem::displayModeChanged()
{
  this->updateIconGeometry();
}

void cedar::proc::gui::StepItem::updateToolTip()
{
  QString tool_tip;

  tool_tip += "<table>";
  tool_tip += "<tr>"
                "<th>Measurement:</th>"
                "<th>Last</th>"
                "<th>Average</th>"
              "</tr>";

  for (unsigned int m = 0; m < this->getStep()->getNumberOfTimeMeasurements(); ++m)
  {
    const std::string& measurement = this->getStep()->getTimeMeasurementName(m);
    tool_tip += "<tr>";
    QString measurement_str = "<td>%1</td><td align=\"right\">%2</td><td align=\"right\">%3</td>";
    measurement_str = measurement_str.arg(QString::fromStdString(measurement));

    if (this->getStep()->hasTimeMeasurement(m))
    {
      cedar::unit::Time last_ms = this->getStep()->getLastTimeMeasurement(m);
      cedar::unit::Time average_ms = this->getStep()->getTimeMeasurementAverage(m);
      double last_d = last_ms / (0.001 * cedar::unit::seconds);
      double avg_d = average_ms / (0.001 * cedar::unit::seconds);
      measurement_str = measurement_str.arg(QString("%1 ms").arg(last_d, 0, 'f', 1));
      measurement_str = measurement_str.arg(QString("%1 ms").arg(avg_d, 0, 'f', 1));
    }
    else
    {
      measurement_str = measurement_str.arg("n/a");
      measurement_str = measurement_str.arg("n/a");
    }
    tool_tip += measurement_str;
    tool_tip += "</tr>";
  }

  tool_tip += "</table>";

  const auto& annotation = this->getStep()->getStateAnnotation();
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
  this->unsetOverrideFillColor(false);
  this->unsetOverrideFillStyle(false);

  switch (this->getStep()->getState())
  {
    case cedar::proc::Triggerable::STATE_EXCEPTION_ON_START:
      this->setOverrideFillStyle(Qt::BDiagPattern, false);
    case cedar::proc::Triggerable::STATE_EXCEPTION:
    case cedar::proc::Triggerable::STATE_NOT_RUNNING:
      this->setOutlineColor(Qt::red);
      this->setOverrideFillColor(QColor(255, 175, 175), false);
      break;

    case cedar::proc::Triggerable::STATE_INITIALIZING:
      this->setOutlineColor(QColor(200, 200, 30));
      this->setOverrideFillStyle(Qt::BDiagPattern, false);
      this->setOverrideFillColor(QColor(233, 233, 51), false);
      break;

    case cedar::proc::Triggerable::STATE_RUNNING:
    default:
      this->setOutlineColor(cedar::proc::gui::GraphicsBase::mDefaultOutlineColor);
  }

  this->update();
}

void cedar::proc::gui::StepItem::handleStepNameChanged()
{
  this->redraw();
  // change title of child widgets
  QString step_name = QString::fromStdString(this->getConnectable()->getName());
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
  this->setElement(step);
  this->setConnectable(step);
  this->resetDisplayMode();

//  this->addDecorations();

  mStateChangedConnection = step->connectToStateChanged(boost::bind(&cedar::proc::gui::StepItem::emitStepStateChanged, this));
  QObject::connect(step.get(), SIGNAL(nameChanged()), this, SLOT(handleStepNameChanged()));
}

void cedar::proc::gui::StepItem::updateIconGeometry()
{
  qreal padding = this->getContentsPadding();
  this->setIconBounds(padding, padding, static_cast<qreal>(cedar::proc::gui::Connectable::M_ICON_SIZE));
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
    this->setDisplayMode(this->getDisplayMode());
  }
}

void cedar::proc::gui::StepItem::writeConfiguration(cedar::aux::ConfigurationNode& root) const
{
  root.put("step", this->getStep()->getName());
  root.put("display style", cedar::proc::gui::StepItem::DisplayMode::type().get(this->getDisplayMode()).name());
  this->cedar::proc::gui::GraphicsBase::writeConfiguration(root);
}

//void cedar::proc::gui::StepItem::setRecorded(bool status)
//{
//	if (status)
//	{
//	  if (!mpRecordedDecoration)
//	  {
//      mpRecordedDecoration = DecorationPtr(
//        new Decoration
//        (
//          this,
//          ":/decorations/record.svg",
//          "This step has one or more slots registered in the recorder."
//        )
//      );
//      this->addDecoration(this->mpRecordedDecoration);
//	  }
//	}
//	else
//	{
//	  if (this->mpRecordedDecoration)
//	  {
//	    this->removeDecoration(this->mpRecordedDecoration);
//	    this->mpRecordedDecoration.reset();
//	  }
//	}
//}

void cedar::proc::gui::StepItem::openActionsDock()
{
  QWidget* p_actions = new QWidget();
  QVBoxLayout* p_layout = new QVBoxLayout();
  const cedar::proc::Step::ActionMap& map = this->getStep()->getActions();
  for (cedar::proc::Step::ActionMap::const_iterator iter = map.begin(); iter != map.end(); ++iter)
  {
    QPushButton* p_button = new QPushButton(iter->first.c_str());
    p_layout->addWidget(p_button);
    QObject::connect(p_button, SIGNAL(clicked()), this, SLOT(handleExternalActionButtons()));
  }

  p_actions->setLayout(p_layout);
  std::string title = "Actions of step \"" + this->getStep()->getName() + "\"";
  auto p_dock_widget = this->createDockWidget(title, p_actions);
  p_dock_widget->show();
}

void cedar::proc::gui::StepItem::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
  CEDAR_DEBUG_ONLY(cedar::proc::gui::Scene *p_scene = dynamic_cast<cedar::proc::gui::Scene*>(this->scene());)
  CEDAR_DEBUG_ASSERT(p_scene);

  QMenu menu;

  if (this->scene()->selectedItems().size() > 1)
  {
    menu.exec(event->screenPos());
    return;
  }

  this->fillConnectableMenu(menu, event);

  menu.addSeparator(); // ----------------------------------------------------------------------------------------------

  QMenu *p_actions_menu = menu.addMenu("actions");
  p_actions_menu->setIcon(QIcon(":/menus/actions.svg"));
  menu.addSeparator(); // ----------------------------------------------------------------------------------------------

  const cedar::proc::Step::ActionMap& map = this->getStep()->getActions();
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

  QAction *a = menu.exec(event->screenPos());

  if (a == nullptr)
    return;

  // execute an action
  if (a->parentWidget() == p_actions_menu)
  {
    std::string action = a->text().toStdString();
    // decide whether this is a normal action or the docking action
    if (action != "open actions dock")
    {
      this->getStep()->callAction(action);
    }
    else
    {
      this->openActionsDock();
    }
  }
  else
  {
    this->handleContextMenuAction(a, event);
  }
}

//!@todo Why isn't this function in gui::Connectable?
void cedar::proc::gui::StepItem::openDefinedPlotAction()
{
  QAction* p_action = dynamic_cast<QAction*>(QObject::sender());
  CEDAR_DEBUG_ASSERT(p_action != nullptr);
  std::string plot_name = p_action->text().toStdString();

  // get declaration of the element displayed by this item
  auto decl = cedar::proc::ElementManagerSingleton::getInstance()->getDeclarationOf(this->getStep());
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

  auto p_plot_widget = new cedar::proc::gui::PlotWidget(this->getStep(), elem_decl->definedPlots()[list_index].mData);
  auto p_dock_widget = this->createDockWidgetForPlots(this->getNameForTitle(), p_plot_widget, p_action->data().toPoint());
  
  p_dock_widget->show();
}

qreal cedar::proc::gui::StepItem::getContentsPadding() const
{
  switch (this->getDisplayMode())
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

  if (this->getDisplayMode() == cedar::proc::gui::StepItem::DisplayMode::ICON_AND_TEXT)
  {
    painter->drawText(QPointF(2 * padding + M_ICON_SIZE, 15), this->mClassId->getClassNameWithoutNamespace().c_str());
    painter->drawText(QPointF(2 * padding + M_ICON_SIZE, 25), this->getStep()->getName().c_str());
  }

  painter->restore(); // restore saved painter settings
}

cedar::proc::StepPtr cedar::proc::gui::StepItem::getStep()
{
  return boost::const_pointer_cast<cedar::proc::Step>
         (
           static_cast<const cedar::proc::gui::StepItem*>(this)->getStep()
         );
}

cedar::proc::ConstStepPtr cedar::proc::gui::StepItem::getStep() const
{
  return boost::dynamic_pointer_cast<cedar::proc::ConstStep>(this->getConnectable());
}

void cedar::proc::gui::StepItem::disconnect()
{
}

void cedar::proc::gui::StepItem::handleExternalActionButtons()
{
  std::string action = cedar::aux::asserted_cast<QPushButton*>(QObject::sender())->text().toStdString();
  this->getStep()->callAction(action);
}
