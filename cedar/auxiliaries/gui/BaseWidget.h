/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        BaseWidget.h

 ----- Author:      Christine Goehrke, Tim Dierkes, Oliver Lomp, Marc Hoener, Tobias Kirchhof

 ----- Date:        2010 01 19

 ----- Description:

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_AUX_GUI_BASE_WIDGET_H
#define CEDAR_AUX_GUI_BASE_WIDGET_H

// LOCAL INCLUDES
#include "namespace.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <string>
#include <QSettings>
#include <QKeyEvent>
#include <QWidget>

/*!
 * This class implements various base functions for widgets. This includes reading and writing of the window settings.
 *
 * @remarks To read/write the window settings, call the corresponding functions. To read/write custom settings,
 *          overwrite the readCustomSettings and writeCustomSettings methods.
 */
class cedar::aux::gui::BaseWidget : public QWidget
{
private:
  Q_OBJECT

public:
  // constructors
  /*!
   * @brief   Constructor that takes the widget name as argument.
   *
   *          To read the window settings, call readWindowSettings().
   */
  BaseWidget(const std::string& widgetName = "", QWidget *pParent = NULL);

  // destructor
  virtual ~BaseWidget(void);

protected:

  /*!
   * @brief the widget sends out a signal when it is shown
   */
  virtual void showEvent(QShowEvent*);
  /*!
   * @brief the widget sends out a signal when it is hidden
   */
  virtual void hideEvent(QHideEvent*);

  /*!
   * Overwrite this to read custom window settings.
   */
  virtual void readCustomSettings(QSettings&);

  /*!
   * Overwrite this to write custom window settings.
   */
  virtual void writeCustomSettings(QSettings&);

  /*!
   * @brief Reads the window settings using the QSettings class.
   */
  virtual void readWindowSettings();

  /*!
   * @brief Writes the window settings using the QSettings class.
   */
  virtual void writeWindowSettings();

  /*!
   * @brief Processes key events.
   *
   *        This function handles ctrl+G, which saves the window settings.
   */
  virtual void keyPressEvent(QKeyEvent* pEvent);

signals:
  void visibleChanged(bool);

private:
  //! Unique identifier for each instance. This is used to associate the window settings with the instance.
  std::string mWidgetName;
};

#endif  // OL_BASE_WIDGET_H
