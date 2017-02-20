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

    File:        BoostControl.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 06 28

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/BoostControl.h"
#include "cedar/processing/gui/View.h"
#include "cedar/processing/gui/GraphicsBase.h"
#include "cedar/processing/gui/Element.h"
#include "cedar/processing/sources/Boost.h"
#include "cedar/processing/Group.h"
#include "cedar/processing/GroupPath.h"
#include "cedar/processing/Element.h"
#include "cedar/auxiliaries/gui/Parameter.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/Singleton.h"
#include "cedar/auxiliaries/TypeBasedFactory.h"
#include "cedar/auxiliaries/casts.h"

// SYSTEM INCLUDES
#include <QHeaderView>
#include <string>


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::BoostControl::BoostControl(cedar::proc::gui::View* view)
:
mpView(view)
{
  this->setupUi(this);

  QObject::connect(this->mpBoostTree, SIGNAL(itemActivated(QTreeWidgetItem*, int)), this, SLOT(itemActivated(QTreeWidgetItem*, int)));

  auto filter_boosts = [] (cedar::proc::ConstElementPtr element)
  {
    return static_cast<bool>(boost::dynamic_pointer_cast<cedar::proc::sources::ConstBoost>(element));
  };
  this->mpBoostTree->setFilter(boost::bind<bool>(filter_boosts, _1));

#ifdef CEDAR_USE_QT5
  this->mpBoostTree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
  this->mpBoostTree->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
#else
  this->mpBoostTree->header()->setResizeMode(0, QHeaderView::Stretch);
  this->mpBoostTree->header()->setResizeMode(1, QHeaderView::ResizeToContents);
#endif

  this->mBoostAddedConnection = this->mpBoostTree->connectToElementAddedSignal
      (
        boost::bind
        (
          &cedar::proc::gui::BoostControl::boostAdded,
          this,
          _1,
          _2
        )
      );
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::BoostControl::itemActivated(QTreeWidgetItem* pItem, int column)
{
  // only activating the name centers the boost
  if (column == 0 && isBoostItem(pItem))
  {
    auto boost = this->findBoostFor(pItem);
    if (!boost)
    {
      return;
    }

    if (auto graphics_item = this->mpView->getScene()->getGraphicsItemFor(boost.get()))
    {
      this->mpView->centerOn(graphics_item);
    }
  }
}

bool cedar::proc::gui::BoostControl::isBoostItem(QTreeWidgetItem* pItem) const
{
  // there are only group- and boost items, and group items are expanded
  return pItem->isExpanded() == false;
}

cedar::proc::sources::BoostPtr cedar::proc::gui::BoostControl::findBoostFor(QTreeWidgetItem* pItem) const
{
  cedar::proc::GroupPath path;

  // traverse the parents and add them to the path
  QTreeWidgetItem* parent = pItem->parent();
  while (parent != nullptr && parent != this->mpBoostTree->invisibleRootItem() && !parent->text(0).isEmpty())
  {
    path += parent->text(0).toStdString();
    parent = parent->parent();
  }

  path += pItem->text(0).toStdString();

  return boost::dynamic_pointer_cast<cedar::proc::sources::Boost>(this->mGroup->getElement(path));
}

void cedar::proc::gui::BoostControl::setGroup(cedar::proc::GroupPtr group)
{
  this->mGroup = group;
  this->mpBoostTree->setGroup(group);
}

void cedar::proc::gui::BoostControl::boostAdded(QTreeWidgetItem* pItem, cedar::proc::ElementPtr element)
{
  auto boost = boost::dynamic_pointer_cast<cedar::proc::sources::Boost>(element);
  if (!boost)
  {
    return;
  }

  pItem->setToolTip(0, "Double-click to show the boost in the architecture.");

  // create checkbox for enabling/disabling boost (by creating a gui::BoolParameter and passing the boost's parameter)
  auto active_parameter = boost->getParameter("active");
  auto p_enabler = cedar::aux::gui::ParameterFactorySingleton::getInstance()->get(active_parameter)->allocateRaw();
  this->mpBoostTree->setItemWidget(pItem, 1, p_enabler);
  p_enabler->setParameter(active_parameter);

  // create spinbox for setting boost strength (by creating a gui::DoubleParameter and passing the boost's parameter)
  auto strength_parameter = boost->getParameter("strength");
  auto p_strength = cedar::aux::gui::ParameterFactorySingleton::getInstance()->get(strength_parameter)->allocateRaw();
  this->mpBoostTree->setItemWidget(pItem, 2, p_strength);
  p_strength->setParameter(strength_parameter);
}
