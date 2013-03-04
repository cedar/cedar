/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        RobotCard.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 03 04

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_GUI_ROBOT_CARD_H
#define CEDAR_DEV_GUI_ROBOT_CARD_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/gui/namespace.h"

// SYSTEM INCLUDES
#include <QFrame>
#include <QLabel>
#include <QListWidgetItem>
#include <QComboBox>
#include <QLineEdit>


/*!@brief A class that displays an icon in the RobotCard widget.
 */
class cedar::dev::gui::RobotCardIconHolder : public QLabel
{
  Q_OBJECT

  public:
    RobotCardIconHolder();

  signals:
    void robotDropped(const QString& robotName);

  protected:
    void dragEnterEvent(QDragEnterEvent* pEvent);
    void dropEvent(QDropEvent* pEvent);

  private:
    QListWidgetItem* itemFromMime(QDropEvent* pEvent);

    void setRobotFromTemplate(const std::string& name);
};



/*!@todo describe.
 *
 * @todo describe more.
 */
class cedar::dev::gui::RobotCard : public QFrame
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  RobotCard(const QString& robotName);

  //!@brief Destructor
  virtual ~RobotCard();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  std::string getRobotName() const;

  std::string getRobotTemplateName() const
  {
    return this->mRobotTemplateName;
  }

public slots:
  void robotDropped(const QString& robotName);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private slots:
  void selectedConfigurationChanged(int index);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  QComboBox* mpConfigurationSelector;

  QLineEdit* mpRobotNameEdit;

  std::string mRobotTemplateName;
}; // class cedar::dev::gui::RobotCard

#endif // CEDAR_DEV_GUI_ROBOT_CARD_H

