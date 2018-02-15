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

    File:        Multi0DPlot.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2015 02 02

    Description: Header file for the class cedar::aux::gui::Multi0DPlot.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_MULTI0DPLOT_H
#define CEDAR_AUX_GUI_MULTI0DPLOT_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"


#ifdef CEDAR_PORTABLE
// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/QCLinePlot.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/gui/Multi0DPlot.fwd.h"

// SYSTEM INCLUDES
#include <map>
#include <string>


/*!@brief A plot that can plot several 0d values next to each other.
 *
 *        This plot is intended for plotting activation of multiple nodes, such as those that are part of behavioral
 *        organization.
 */
class cedar::aux::gui::Multi0DPlot : public cedar::aux::gui::QCLinePlot
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  typedef cedar::aux::gui::QCLinePlot Super;

  class LabeledScaleDraw;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Multi0DPlot(QWidget* pParent = nullptr);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  bool canAppend(cedar::aux::ConstDataPtr data) const;

  bool canDetach(cedar::aux::ConstDataPtr data) const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  void timerEvent(QTimerEvent *pEvent);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void doAppend(cedar::aux::ConstDataPtr data, const std::string& title);

  void doDetach(cedar::aux::ConstDataPtr data);

  void splitTitleString(const std::string& title, std::string& titlePart, std::string& subtitlePart) const;

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! A map from subtitle to mat data for the title.
  std::map<std::string, cedar::aux::MatDataPtr> mSubtitleData;

  //! A map from title to an index.
  std::map<std::string, unsigned int> mTitleIndex;

  //! A map from title.subtitle to the data to plot
  std::map<std::string, cedar::aux::ConstMatDataPtr> mData;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::aux::gui::Multi0DPlot

#else
#ifdef CEDAR_USE_QWT

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/QwtLinePlot.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/gui/Multi0DPlot.fwd.h"

// SYSTEM INCLUDES
#include <map>
#include <string>


/*!@brief A plot that can plot several 0d values next to each other.
 *
 *        This plot is intended for plotting activation of multiple nodes, such as those that are part of behavioral
 *        organization.
 */
class cedar::aux::gui::Multi0DPlot : public cedar::aux::gui::QwtLinePlot
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  typedef cedar::aux::gui::QwtLinePlot Super;

  class LabeledScaleDraw;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Multi0DPlot(QWidget* pParent = nullptr);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  bool canAppend(cedar::aux::ConstDataPtr data) const;

  bool canDetach(cedar::aux::ConstDataPtr data) const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  void timerEvent(QTimerEvent *pEvent);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void doAppend(cedar::aux::ConstDataPtr data, const std::string& title);

  void doDetach(cedar::aux::ConstDataPtr data);

  void splitTitleString(const std::string& title, std::string& titlePart, std::string& subtitlePart) const;

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! A map from subtitle to mat data for the title.
  std::map<std::string, cedar::aux::MatDataPtr> mSubtitleData;

  //! A map from title to an index.
  std::map<std::string, unsigned int> mTitleIndex;

  //! A map from title.subtitle to the data to plot
  std::map<std::string, cedar::aux::ConstMatDataPtr> mData;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::aux::gui::Multi0DPlot

#endif // CEDAR_USE_QWT

#endif

#endif // CEDAR_AUX_GUI_MULTI0DPLOT_H
