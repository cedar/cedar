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

    File:        IgnoreLocaleDoubleSpinBox.cpp

    Maintainer:  Jan Tekülve
    Email:       jan.tekuelve@ini.rub.de
    Date:        2018 03 08

    Description: Source file for the class cedar::aux::gui::IgnoreLocaleDoubleSpinBox.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/auxiliaries/gui/IgnoreLocaleDoubleSpinBox.h"

// CEDAR INCLUDES

// SYSTEM INCLUDES
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <QLocale>
#include <QLatin1Char>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::IgnoreLocaleDoubleSpinBox::IgnoreLocaleDoubleSpinBox(QWidget *parent)
:QDoubleSpinBox(parent)
{
  mLastDefinedDigit = 1.0;
  QObject::connect(this, SIGNAL(inputPrecisionChanged(double)),this, SLOT(updatePrecision(double)));
}

cedar::aux::gui::IgnoreLocaleDoubleSpinBox::~IgnoreLocaleDoubleSpinBox()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
double cedar::aux::gui::IgnoreLocaleDoubleSpinBox::valueFromText(const QString& text) const
{
//  std::cout<<"Called Value From Text: " << text.toStdString() << std::endl;
  std::string inputText = text.toStdString();
  std::replace( inputText.begin(), inputText.end(), ',', '.');

  double returnValue;
  try
  {
    returnValue = boost::lexical_cast<double>(inputText);
  }
  catch(const boost::bad_lexical_cast &)
  {
    returnValue = this->value();
//    std::cout<<"Got this text:" << text.toStdString() <<", but could not lexical cast therefore kept the value at: " << returnValue<<std::endl;
  }

  //set a "intelligentStepSize" depending on the last typed input. Maximum Stepsize however should be 1.
  double stepSize = this->getLastDefinedDigit(inputText);
  emit inputPrecisionChanged(stepSize);

  return returnValue;


}

double cedar::aux::gui::IgnoreLocaleDoubleSpinBox::getLastDefinedDigit(std::string textInput) const
{
  //We work on the text input to avoid any floating point precision problems
  size_t dotPosition = textInput.find(".");
  double precisions[5] = {1.0,0.1,0.01,0.001,0.0001};

  if(dotPosition != std::string::npos)
  {
    int numberOfValuesAfterDot = textInput.length() - dotPosition -1;
    switch(numberOfValuesAfterDot)
    {
      case 0: // Some strange case = 1. for example
        return  precisions[0];
      case 1:
        return  precisions[1];
      case 2:
        return precisions[2];
      case 3:
        return precisions[3];
      case 4:
      {
        //Here we could have an already formatted number, so check which parts of the number are actually not 0.
        std::string fourDigits = textInput.substr (dotPosition+1);
        for(int i=3; i>=0;i--)
        {
          if(boost::lexical_cast<int>(fourDigits.at(i)) != 0)
          {
            return precisions[i + 1];
          }
          //None of the decimals is nonzero
          if(i== 0)
            return precisions[0];
        }
      }
      default: //Maybe there is a number with more than 4 digits
        return  precisions[4];
    }
  }
  else //No '.' -> No floating point number
    return  1.0;
}

void cedar::aux::gui::IgnoreLocaleDoubleSpinBox::updatePrecision(double newPrecision)
{
//  std::cout<<"Received a signal to update the precision to: "<< newPrecision << std::endl;
  this->mLastDefinedDigit = newPrecision;
}

void cedar::aux::gui::IgnoreLocaleDoubleSpinBox::keyPressEvent(QKeyEvent *event)
{
  QDoubleSpinBox::keyPressEvent(event);
  //Only enter changes the stepsize for real
  if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
  {
    this->setSingleStep(this->mLastDefinedDigit);
  }
}