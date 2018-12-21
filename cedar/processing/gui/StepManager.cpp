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

    File:        StepManager.h

    Maintainer:  Jan Tekülve
    Email:       jan.tekuelve@ini.rub.de
    Date:        2018 12 18

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/StepManager.h"
#include "cedar/processing/gui/Settings.h"
#include "cedar/auxiliaries/gui/BoolParameter.h"
#include "cedar/auxiliaries/gui/UIntParameter.h"
#include "cedar/auxiliaries/gui/Settings.h"
#include "cedar/auxiliaries/Settings.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/ElementDeclaration.h"

// SYSTEM INCLUDES
#ifdef CEDAR_USE_QT5
    #include <QtWidgets/QHeaderView>
    #include <QtWidgets/QTableWidgetItem>
#else
    #include <QHeaderView>
    #include <QTableWidgetItem>
#endif //CEDAR_USE_QT5
//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::StepManager::StepManager(QWidget *pParent)
:
QWidget(pParent),
mCustomSetName("custom")
{
  this->setupUi(this);
  this->fillPresetLists();

  checkBoxMapper = new QSignalMapper();

  QObject::connect(checkBoxMapper, SIGNAL(mapped(QString)),this, SIGNAL(checkBoxToggled(QString)));
  QObject::connect(this, SIGNAL(checkBoxToggled(QString)),this, SLOT(handleCheckBoxToggle(QString)));

  QObject::connect(this->mpComboBox,SIGNAL(activated(const QString)),this,SLOT(handleComboBoxSelection(const QString)));


  this->fillTable();

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

//void cedar::proc::gui::StepManager::reject()
//{
//  // just (re-)load the settings that are currently stored on the disk
//  cedar::proc::gui::SettingsSingleton::getInstance()->load();
//}
//
//void cedar::proc::gui::StepManager::accept()
//{
//  // write the settings to the disk
//  cedar::proc::gui::SettingsSingleton::getInstance()->save();
//}


void cedar::proc::gui::StepManager::fillTable()
{

#ifdef CEDAR_USE_QT5
    this->mpTable->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
    this->mpTable->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
    this->mpTable->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Interactive);
#else
    this->mpTable->horizontalHeader()->setResizeMode(0,QHeaderView::ResizeToContents);
    this->mpTable->horizontalHeader()->setResizeMode(1,QHeaderView::Stretch);
    this->mpTable->horizontalHeader()->setResizeMode(2,QHeaderView::Interactive);
#endif //CEDAR_USE_QT5
  

  int index = this->mpComboBox->findText(QString::fromStdString(SettingsSingleton::getInstance()->getHiddenPreset()));
  if ( index != -1 ) { // -1 for not found
    this->mpComboBox->setCurrentIndex(index);
  }
  else
  {
    if(SettingsSingleton::getInstance()->getHiddenPreset() == mCustomSetName)
    {
    this->mpComboBox->addItem(QString::fromStdString(mCustomSetName));
    this->mpComboBox->setCurrentIndex(this->mpComboBox->count() -1);
    }
//    std::cout<<"Did not find customSet in Box!" <<std::endl;
//    std::cout<<"Did not find saved HiddenPreset: "<< SettingsSingleton::getInstance()->getHiddenPreset() <<" in Box!" <<std::endl;
  }


  auto categories = ElementManagerSingleton::getInstance()->listCategories();
  int rowCount = categories.size();
  for(auto category:categories)
  {
    auto entries = ElementManagerSingleton::getInstance()->getCategoryEntries(category);
    rowCount+=entries.size();
  }
  this->mpTable->setRowCount(rowCount);


  int i=0;
  for(auto category:categories)
  {
    auto entries = ElementManagerSingleton::getInstance()->getCategoryEntries(category);

    //Show the Category
    QTableWidgetItem *newItem = new QTableWidgetItem(QString::fromStdString("Category: "+category));
    QFont boldFont = newItem->font();
    boldFont.setWeight(QFont::Bold);
    newItem->setFont(boldFont);
    newItem->setBackgroundColor(QColor::fromRgb(159,159,159));
    newItem->setTextAlignment(Qt::AlignCenter);
    this->mpTable->setItem(i, 0, newItem);
    this->mpTable->setSpan(i, 0, 1, 3);

    i = i+1;
    for (auto entry: entries)
    {
      auto declaration = boost::dynamic_pointer_cast<cedar::proc::ConstElementDeclaration>(entry);
      CEDAR_ASSERT(declaration);
      QTableWidgetItem *newItem = new QTableWidgetItem(QString::fromStdString(declaration->getClassNameWithoutNamespace()));
      newItem->setIcon(declaration->getIcon());
      this->mpTable->setItem(i, 0, newItem);

      QTableWidgetItem *descItem = new QTableWidgetItem(QString::fromStdString(declaration->getShortDescription()));
      QFont italicFont = descItem->font();
      italicFont.setItalic(true);
      descItem->setFont(italicFont);
      this->mpTable->setItem(i, 1, descItem);

      auto myBox = new QCheckBox(this);
      myBox->setStyleSheet("margin-left:50%; margin-right:50%;");
      bool isHidden = SettingsSingleton::getInstance()->isHiddenElement(declaration->getClassName());
      myBox->setChecked(!isHidden);

      //Set up the Mapping between Button and Class
      QObject::connect(myBox, SIGNAL(clicked()), checkBoxMapper, SLOT(map()));
      QString fullClassName = QString::fromStdString(declaration->getClassName());
      checkBoxMapper->setMapping(myBox, fullClassName);
      mCheckBoxes[declaration->getClassName()] = myBox;

      this->mpTable->setCellWidget(i,2,myBox);
      i = i+1;
    }


  }


}


void cedar::proc::gui::StepManager::handleCheckBoxToggle(QString fullClassName)
{
  std::string className = fullClassName.toStdString();
  SettingsSingleton::getInstance()->toggleHidden(className);

  //Now it becomes a custom Set!
  SettingsSingleton::getInstance()->setHiddenPreset(mCustomSetName);
  int index = this->mpComboBox->findText(QString::fromStdString(mCustomSetName));
  if ( index != -1 ) { // -1 for not found
    this->mpComboBox->setCurrentIndex(index);
  }
  else
  {
    this->mpComboBox->addItem(QString::fromStdString(mCustomSetName));
    this->mpComboBox->setCurrentIndex(this->mpComboBox->count() -1);
//    std::cout<<"Did not find customSet in Box!" <<std::endl;
  }
}

void cedar::proc::gui::StepManager::handleComboBoxSelection(const QString presetName)
{
  std::string setName = presetName.toStdString();

  if(setName != mCustomSetName)
  {

    int index = this->mpComboBox->findText(QString::fromStdString(mCustomSetName));
    if ( index != -1 ) { // -1 for not found
      this->mpComboBox->removeItem(index);
    }

    auto hiddenList = mPreSets[setName];

    auto categories = ElementManagerSingleton::getInstance()->listCategories();
    //We have to iterate through all Declarations to adjust their visibility, Oof

    for(auto category:categories)
    {
      auto entries = ElementManagerSingleton::getInstance()->getCategoryEntries(category);
      for (auto entry: entries)
      {
        auto declaration = boost::dynamic_pointer_cast<cedar::proc::ConstElementDeclaration>(entry);
        bool isVisible = true;
        if(std::find(hiddenList.begin(), hiddenList.end(), declaration->getClassName()) != hiddenList.end()) {
          isVisible = false;
        }
        SettingsSingleton::getInstance()->setHidden(declaration->getClassName(),!isVisible,false);

        if(auto checkBox = mCheckBoxes[declaration->getClassName()])
        {
          checkBox->setChecked(isVisible);
        }
      }
    }

    SettingsSingleton::getInstance()->emitHiddenElementsChangedSignal();
    SettingsSingleton::getInstance()->setHiddenPreset(setName);
  } else
  {
//    std::cout<<"We changed to Custom! Again!" <<std::endl;
  }
}


void cedar::proc::gui::StepManager::fillPresetLists()
{

  //This could be read from a config, but for now we are fine ;-)
  std::vector<std::string> legacySteps{
          "cedar.processing.AbsoluteValue",
          "cedar.processing.steps.AddConstant",
          "cedar.processing.steps.CrossProduct",
          "cedar.processing.DivideElementwise",
          "cedar.processing.steps.InnerProduct",
          "cedar.processing.MatrixMultiply",
          "cedar.processing.steps.Maximum",
          "cedar.processing.steps.Minimum",
          "cedar.processing.Norm",
          "cedar.processing.steps.OuterProduct",
          "cedar.processing.PseudoInverse",
          "cedar.processing.RemoveMean",
          "cedar.processing.ScalarDivision",
          "cedar.processing.SubtractElementwise",
          "cedar.processing.Transpose",
          "cedar.dynamics.HarmonicOscillator",
          "cedar.processing.steps.Clamp",
          "cedar.processing.Mask",
          "cedar.processing.steps.ShiftedMultiplication",
          "cedar.processing.steps.Stack",
          "cedar.processing.steps.VariableBox",
          "cedar.processing.steps.VariableGauss",
          "cedar.processing.steps.LinearLateralShift",
          "cedar.processing.steps.Histogram",
          "cedar.proc.steps.ImageGradient",
          "cedar.processing.steps.ExponentialSmoothing",
          "cedar.processing.steps.BufferReader",
          "cedar.processing.steps.Count",
          "cedar.processing.steps.Delay",
          "cedar.processing.steps.Noop",
          "cedar.processing.steps.OverTime",
          "cedar.processing.steps.Switch",
          "cedar.proc.steps.UnitConverter",
          "cedar.processing.VectorToScalars",
          "cedar.dynamics.LogTimeInterval",
          "cedar.processing.steps.ConfigurationLimits",
          "cedar.processing.steps.DampedPseudoInverseKinematics",
          "cedar.processing.steps.LinearDynamics",
          "cedar.processing.Nullspace",
          "cedar.processing.steps.RotationOnPlane",
          "cedar.processing.steps.SinusDynamics",
          "cedar.processing.steps.VehicleRotation",
          "cedar.processing.sources.ConstDiagonalMatrix",
          "cedar.processing.sources.ConstMatrix",
          "cedar.processing.sources.LinearSpatialTemplate",
          "cedar.processing.sources.SinusSignal",
          "cedar.processing.sources.SpatialTemplate",
          "cedar.processing.sources.StepSignal",
          "cedar.processing.sources.VirtualCamera",
          "cedar.dynamics.steps.TwoStepInput",
          "cedar.processing.steps.NumericalIntegration",
          "cedar.processing.steps.NumericalDifferentiation"
  };

  std::vector<std::string> minimalSteps{
          "cedar.processing.AbsoluteValue",
          "cedar.processing.steps.AddConstant",
          "cedar.processing.steps.CrossProduct",
          "cedar.processing.DivideElementwise",
          "cedar.processing.steps.InnerProduct",
          "cedar.processing.MatrixMultiply",
          "cedar.processing.steps.Maximum",
          "cedar.processing.steps.Minimum",
          "cedar.processing.Norm",
          "cedar.processing.steps.OuterProduct",
          "cedar.processing.PseudoInverse",
          "cedar.processing.RemoveMean",
          "cedar.processing.ScalarDivision",
          "cedar.processing.SubtractElementwise",
          "cedar.processing.Transpose",
          "cedar.dynamics.HarmonicOscillator",
          "cedar.processing.steps.Clamp",
          "cedar.processing.Mask",
          "cedar.processing.steps.ShiftedMultiplication",
          "cedar.processing.steps.Stack",
          "cedar.processing.steps.VariableBox",
          "cedar.processing.steps.VariableGauss",
          "cedar.processing.steps.LinearLateralShift",
          "cedar.processing.steps.Histogram",
          "cedar.proc.steps.ImageGradient",
          "cedar.processing.steps.ExponentialSmoothing",
          "cedar.processing.steps.NumericalDifferentiation",
          "cedar.processing.steps.NumericalIntegration",
          "cedar.processing.steps.BufferReader",
          "cedar.processing.steps.Count",
          "cedar.processing.steps.Delay",
          "cedar.processing.steps.Noop",
          "cedar.processing.steps.OverTime",
          "cedar.processing.steps.Switch",
          "cedar.proc.steps.UnitConverter",
          "cedar.processing.VectorToScalars",
          "cedar.dynamics.LogTimeInterval",
          "cedar.processing.steps.ConfigurationLimits",
          "cedar.processing.steps.DampedPseudoInverseKinematics",
          "cedar.processing.steps.LinearDynamics",
          "cedar.processing.Nullspace",
          "cedar.processing.steps.RotationOnPlane",
          "cedar.processing.steps.SinusDynamics",
          "cedar.processing.steps.VehicleRotation",
          "cedar.processing.sources.ConstDiagonalMatrix",
          "cedar.processing.sources.ConstMatrix",
          "cedar.processing.sources.LinearSpatialTemplate",
          "cedar.processing.sources.SinusSignal",
          "cedar.processing.sources.SpatialTemplate",
          "cedar.processing.sources.StepSignal",
          "cedar.processing.sources.VirtualCamera",
          "cedar.dynamics.steps.TwoStepInput",
          "cedar.processing.steps.CoordinateTransformation",
          "cedar.processing.ComponentMultiply",
          "cedar.processing.Normalization",
          "cedar.processing.TransferFunction",
          "cedar.processing.WeightedSum",
          "cedar.processing.steps.ConcatVertically",
          "cedar.processing.Flip",
          "cedar.dynamics.HebbianConnection",
          "cedar.processing.steps.PositionOfMaximum",
          "cedar.processing.ScalarsToVector",
          "cedar.processing.steps.Jacobian",
          "cedar.processing.VideoSink",
          "cedar.processing.steps.ShapeVisualisation",
          "cedar.processing.sources.Noise"
  };


  std::vector<std::string> jokeitSpecial{
          "cedar.processing.WeightedSum",
          "cedar.dynamics.HebbianConnection",
          "cedar.processing.sources.SpatialTemplate",
          "cedar.processing.steps.Convolution",
          "cedar.processing.steps.CoordinateTransformation",
          "cedar.dynamics.Preshape",
          "cedar.dynamics.NeuralField",
          "cedar.dynamics.SpaceToRateCode",
          "cedar.dynamics.SpaceCodeToRateMatrix",
          "cedar.dynamics.RateMatrixToSpaceCode",
          "cedar.dynamics.SerialOrder",
          "cedar.processing.steps.ColorConversion",
          "cedar.processing.steps.Histogram",
          "cedar.processing.steps.Threshold",
          "cedar.proc.steps.ImageGradient",
          "cedar.processing.steps.ChannelMerge",
          "cedar.processing.steps.ChannelSplit",
          "cedar.processing.steps.ImageTransform",
          "cedar.processing.steps.pairsGame"
  };

  std::vector<std::string> allSteps;

  mPreSets.insert(std::make_pair("legacy",legacySteps));

  mPreSets.insert(std::make_pair("minimal",minimalSteps));

  mPreSets.insert(std::make_pair("all",allSteps));

  mPreSets.insert(std::make_pair("jokeit special",jokeitSpecial));

//  std::vector<std::string> customSteps; //This probably empty on initialization
//  mPreSets.insert(std::make_pair(mCustomSetName,customSteps));

  auto it = mPreSets.begin();
  while(it != mPreSets.end())
  {
    mpComboBox->addItem(QString::fromStdString(it->first));
    it++;
  }

}
