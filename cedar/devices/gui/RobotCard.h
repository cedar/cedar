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
#include "cedar/devices/namespace.h"

// SYSTEM INCLUDES
#include <QFrame>
#include <QLabel>
#include <QListWidgetItem>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#ifndef Q_MOC_RUN
  #include <boost/signals2.hpp>
#endif // Q_MOC_RUN
#include <string>

// SYSTEM INCLUDES
#ifdef CEDAR_USE_QT5
    #include <QtWidgets/QCheckBox>
#else
    #include <QCheckBox>
#endif //CEDAR_USE_QT5
    


/*!@brief A class that displays an icon in the RobotCard widget.
 */
class cedar::dev::gui::RobotCardIconHolder : public QLabel
{
  Q_OBJECT

  public:
    RobotCardIconHolder(cedar::dev::gui::RobotCard* pCard);

    std::string getRobotName() const;

    void setIconFromTemplate(const std::string& templateName);

  signals:
    void robotDropped(const QString& robotName);

  protected:
    void dragEnterEvent(QDragEnterEvent* pEvent);
    void dropEvent(QDropEvent* pEvent);

  private:
    QListWidgetItem* itemFromMime(QDropEvent* pEvent);

    void setRobotFromTemplate(const std::string& name);

  private:
    cedar::dev::gui::RobotCard* mpCard;
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

  void setRobotTemplate(const std::string& templateName);

public slots:
  void robotDropped(const QString& robotName);

signals:
  void addBlankCard();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private slots:
  void robotNameEditValueChanged();

private:
  void robotRemoved(const std::string& robotName);

  void updateConnectionIcon();

private slots:
  void selectedConfigurationChanged(int index);

  void deleteClicked();

  void connectClicked();

  void automaticConnectClicked(int state);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  QComboBox* mpConfigurationSelector;

  QCheckBox* mpAutomaticConnectBox;

  QLineEdit* mpRobotNameEdit;

  cedar::dev::gui::RobotCardIconHolder* mpIcon;

  QPushButton* mpConnectButton;
  QPushButton* mpDeleteButton;

  std::string mRobotTemplateName;

  cedar::dev::RobotPtr mrRobot;

  boost::signals2::connection mRobotRemovedConnection;

  //This somehow does only work for the DisconnectHook
  std::vector<boost::signals2::connection> mRobotDisconnectConnections;

  std::string mCurrentName;
}; // class cedar::dev::gui::RobotCard

#endif // CEDAR_DEV_GUI_ROBOT_CARD_H

