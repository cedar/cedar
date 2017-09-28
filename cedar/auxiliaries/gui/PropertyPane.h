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

    File:        PropertyPane.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 03 09

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_PROPERTY_PANE_H
#define CEDAR_AUX_GUI_PROPERTY_PANE_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/TypeBasedFactory.h"
#include "cedar/auxiliaries/Configurable.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/gui/Parameter.fwd.h"
#include "cedar/auxiliaries/gui/PropertyPane.fwd.h"

// SYSTEM INCLUDES
#include <QTableWidget>
#include <QCheckBox>
#include <QLabel>
#ifndef Q_MOC_RUN
  #include <boost/signals2/connection.hpp>
#endif
#include <vector>
#include <string>
#include <map>


/*!@brief A widget for displaying the parameters of configurables.
 *
 *        This widget offers a quick way of creating a user interface for classes that implement the
 *        cedar::aux::Configurable interface. Parameters of the configurable are displayed in a tabular widget by their
 *        name and have an edit widget associated with them.
 *
 */
class cedar::aux::gui::PropertyPane : public QTableWidget
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief The standard constructor.
   */
  PropertyPane(QWidget* pParent = NULL);

  //!@brief Destructor.
  ~PropertyPane();

  /*!@brief Displays the parameters for a given configurable.
   *
   * @param pConfigurable the given configurable.
   */
  void display(cedar::aux::ConfigurablePtr pConfigurable);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

public slots:
  /*!@brief Resets the contents of the widget.
   */
  void resetContents();

  /*!@brief Recreates the widget.
   */
  void redraw();

  /*!@brief Slot that reacts to a change in row size.
   */
  void rowSizeChanged();

  /*!@brief Slot that reacts when one of the parameters displayed by this widget changes its changed flag.
   */
  void parameterChangeFlagChanged();

  /*!@brief Set whether advanced parameters should be shown.
   */
  void showAdvanced(bool show);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  /*!@brief Determines what type identifier to display to the user.
   */
  virtual std::string getInstanceTypeId(cedar::aux::ConfigurablePtr pConfigurable) const;

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*!@brief Appends a configurable to the property pane.
   */
  void append(const std::string& title, cedar::aux::ConfigurablePtr configurable);

  /*!@brief Appends the representation for a parameter to the table.
   */
  void append(cedar::aux::Configurable::ParameterList& parameters);

  /*!@brief Adds a row containing a heading.
   */
  void addHeadingRow(const std::string& label);

  /*!@brief Adds a row that contains only a label.
   */
  void addLabelRow(const std::string& label);

  //! Adds a widget that takes up a whole row.
  void addWidgetRow(QWidget* pWidget);

  /*!@brief Adds a row that displays a given parameter.
   */
  void addPropertyRow(cedar::aux::ParameterPtr parameter);

  /*!@brief Sets the label style to indicate that the parameter was changed.
   */
  void indicateChange(QLabel *pLabel, bool changed);

  /*!@brief In events sent by cedar::aux::gui::Parameters, this function returns the row displaying the sender.
   */
  int getSenderParameterRowWidget() const;

  /*!@brief In events sent by cedar::aux::Parameters, this function returns the row displaying the sender.
   */
  int getSenderParameterRow() const;

  /*!@brief Disconnects all relevant signals of the given parameters.
   */
  void disconnect();

  //! Returns whether the pane should show advanced parameters.
  bool showAdvanced() const
  {
    return this->mpShowAdvanced->isChecked();
  }

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! Weak pointer to the configurable being displayed by this property pane.
  boost::weak_ptr<cedar::aux::Configurable> mDisplayedConfigurable;

  //! Association from cedar::aux::gui::Parameters to rows.
  std::map<cedar::aux::gui::Parameter*, int> mParameterWidgetRowIndex;

  //! Association from cedar::aux::Parameters to rows.
  std::map<cedar::aux::Parameter*, int> mParameterRowIndex;

  //! Connection to the configurable's tree changed signal.
  std::vector<boost::signals2::connection> mSlotConnections;

  //! Checkbox that lets the user show advanced parameters.
  QCheckBox* mpShowAdvanced;

}; // class cedar::aux::gui::PropertyPane

#endif // CEDAR_AUX_GUI_PROPERTY_PANE_H
