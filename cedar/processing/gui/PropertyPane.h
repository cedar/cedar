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

    File:        PropertyPane.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 03 09

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_PROPERTY_PANE_H
#define CEDAR_PROC_PROPERTY_PANE_H

// CEDAR INCLUDES
#include "cedar/processing/gui/namespace.h"
#include "cedar/processing/namespace.h"
#include "cedar/auxiliaries/gui/namespace.h"
#include "cedar/auxiliaries/TypeBasedFactory.h"
#include "cedar/processing/Step.h"

// SYSTEM INCLUDES
#include <QTableWidget>
#include <QLabel>
#include <boost/signals2/connection.hpp>


/*!@brief A widget for displaying the parameters of configurables.
 *
 *        This widget offers a quick way of creating a user interface for classes that implement the
 *        cedar::aux::Configurable interface. Parameters of the configurable are displayed in a tabular widget by their
 *        name and have an edit widget associated with them.
 */
class cedar::proc::gui::PropertyPane : public QTableWidget
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

private:
  //! Type of a factory that creates control widgets based on parameter types.
  typedef
      cedar::aux::TypeBasedFactory
      <
        cedar::aux::Parameter,
        cedar::aux::gui::Parameter,
        cedar::aux::ParameterPtr
      >
      DataWidgetTypes;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief The standard constructor.
   */
  PropertyPane(QWidget *pParent = NULL);

  //!@brief Destructor.
  ~PropertyPane();

  /*!@brief Displays the parameters for a cedar::proc::Step.
   *
   * @todo  Remove this function, use name undecorating instead.
   */
  void display(cedar::proc::StepPtr pStep);


  /*!@brief Displays the parameters for a cedar::proc::Trigger.
   *
   * @todo  Remove this function, use name undecorating instead.
   */
  void display(cedar::proc::TriggerPtr pTrigger);

  /*!@brief Displays the parameters for a given configurable.
   *
   * @param reset Whether or not the resetContents() function should be called.
   */
  void display(cedar::aux::ConfigurablePtr pConfigurable, bool reset = true);

  /*!@brief Resets the contents of the widget.
   */
  void resetContents();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Returns the factory used for creating widgets based on the data types.
   */
  DataWidgetTypes& dataWidgetTypes();

public slots:
  /*!@brief Resets the pointer to the configurable displayed in this widget.
   */
  void resetPointer();

  /*!@brief Recreates the widget.
   */
  void redraw();

  /*!@brief Slot that reacts to a change in row size.
   */
  void rowSizeChanged();

  /*!@brief Slot that reacts when one of the parameters displayed by this widget changes its changed flag.
   */
  void parameterChangeFlagChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*!@brief Appends the representation for a parameter to the table.
   */
  void append(cedar::aux::Configurable::ParameterList& parameters);

  /*!@brief Adds a row containing a heading.
   */
  void addHeadingRow(const std::string& label);

  /*!@brief Adds a row that contains only a label.
   */
  void addLabelRow(const std::string& label);

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
  void disconnect(cedar::aux::ConfigurablePtr pConfigurable);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! Factory that associates parameter types with the widgets used to display them.
  static DataWidgetTypes mDataWidgetTypes;

  //! Weak pointer to the configurable being displayed by this property pane.
  boost::weak_ptr<cedar::aux::Configurable> mDisplayedConfigurable;

  //! Association from cedar::aux::gui::Parameters to rows.
  std::map<cedar::aux::gui::Parameter*, int> mParameterWidgetRowIndex;

  //! Association from cedar::aux::Parameters to rows.
  std::map<cedar::aux::Parameter*, int> mParameterRowIndex;

  //! Connection to the configurable's tree changed signal.
  boost::signals2::connection mSlotConnection;

}; // class cedar::proc::gui::PropertyPane

#endif // CEDAR_PROC_PROPERTY_PANE_H
