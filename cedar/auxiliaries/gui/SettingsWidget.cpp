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

    File:        SettingsWidget.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 10 04

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/SettingsWidget.h"
#include "cedar/auxiliaries/gui/Settings.h"
#include "cedar/auxiliaries/gui/DirectoryParameter.h"
#include "cedar/auxiliaries/gui/PlotManager.h"
#include "cedar/auxiliaries/gui/PlotDeclaration.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/Settings.h"
#include "cedar/auxiliaries/DirectoryParameter.h"
#include "cedar/auxiliaries/Recorder.h"

// SYSTEM INCLUDES
#include <set>
#include <string>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::SettingsWidget::SettingsWidget(QWidget *pParent)
:
QWidget(pParent)
{
  this->setupUi(this);

  this->mpRecorderWorkspaceEdit->setParameter
                                 (
                                   cedar::aux::SettingsSingleton::getInstance()->getRecorderWorkspaceParameter()
                                 );

  this->fillPossible2dMatDataPlots();
  QObject::connect(this->mpDefault2dMatDataPlot, SIGNAL(currentIndexChanged(int)), this, SLOT(default2dMatDataPlotChanged()));

  this->mpRecordingFormatSelector->setParameter(cedar::aux::SettingsSingleton::getInstance()->getRecorderSerializationFormatParameter());

//  QObject::connect(this->mpYarpConfigString,SIGNAL(cedar::aux::gui::Parameter::valueChanged()),this,SLOT(yarpConfigInfoChanged()));
  this->mpYarpConfigString->setParameter(cedar::aux::SettingsSingleton::getInstance()->getYarpConfigInfoParameter());

  this->mpYarpConfigString->setPlaceHolderText("XXX.XXX.XXX.XX YYYY");


}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::SettingsWidget::fillPossible2dMatDataPlots()
{
  std::string current_default = cedar::aux::gui::SettingsSingleton::getInstance()->getDefault2dMatDataPlot();

  // list of available plots
  //!@todo There should be a factory manager for this; this hard-coding is a quick-fix.
  //!@todo This should also be done for 1d plots
  std::set<std::string> declarations;
  std::string base_namespace = "cedar::aux::gui::";

#ifdef CEDAR_USE_QWTPLOT3D
  declarations.insert(base_namespace + "QwtSurfacePlot");
#endif // CEDAR_USE_QWTPLOT3D

#ifdef CEDAR_USE_VTK
  declarations.insert(base_namespace + "VtkSurfacePlot");
#endif // CEDAR_USE_VTK

  declarations.insert(base_namespace + "ImagePlot");

  // fill the list
  int selected = -1;
  this->mpDefault2dMatDataPlot->clear();
  for (const auto& class_name : declarations)
  {
    if (class_name == current_default)
    {
      selected = this->mpDefault2dMatDataPlot->count();
    }
    this->mpDefault2dMatDataPlot->addItem(QString::fromStdString(class_name));
  }

  if (selected >= 0 && selected < this->mpDefault2dMatDataPlot->count())
  {
    bool blocked = this->mpDefault2dMatDataPlot->blockSignals(true);
    this->mpDefault2dMatDataPlot->setCurrentIndex(selected);
    this->mpDefault2dMatDataPlot->blockSignals(blocked);
  }
}

void cedar::aux::gui::SettingsWidget::default2dMatDataPlotChanged()
{
  if (this->mpDefault2dMatDataPlot->currentIndex() < 0)
  {
    return;
  }
  std::string selection = this->mpDefault2dMatDataPlot->currentText().toStdString();
  cedar::aux::gui::SettingsSingleton::getInstance()->setDefault2dMatDataPlot(selection);
}


void cedar::aux::gui::SettingsWidget::reject()
{
  // just (re-)load the settings that are currently stored on the disk
  cedar::aux::SettingsSingleton::getInstance()->load();
}

void cedar::aux::gui::SettingsWidget::accept()
{
  // write the settings to the disk
  cedar::aux::SettingsSingleton::getInstance()->save();
  cedar::aux::SettingsSingleton::getInstance()->updateYarpNameServerContact();
}

