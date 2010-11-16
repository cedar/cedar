/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        Thread.h

 ----- Author:      Christian Faubel
 ----- Email:       christian.faubel@ini.rub.de
 ----- Date:        2010 10 12

 ----- Description: Header for the @em cedar::aux::Thread class.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_AUX_THREAD_H
#define CEDAR_AUX_THREAD_H

// LOCAL INCLUDES
#include "namespace.h"
#include "Base.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <string>
#include <iostream>
#include <QThread>


/*!@brief Cedar interface for threads
 *
 * Use this interface if your class should be executable as a Qt thread.
 * Just inherit from this class and implement a step function with the
 * given parameter.
 *
 * - to start your thread, call start()
 * - to stop your thread, call stop()
 * - before you delete your object, make sure that there was enough time
 *   to stop the current execution of step()
 *
 * Note that every child class must implement step(). If you design your own wrapper
 * thread with multiple objects that inherit the thread interface, you can call
 * all step functions consecutively and also pass measured time to each step function
 * to fulfill real-time constraints.
 */
class cedar::aux::Thread : public Base, public QThread
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief The standard constructor.
   *
   * The standard constructor sets the idle time between
   * two executions of step() to 100 microseconds.
   */
  Thread(void);

  /*!@brief Constructor with idle time parameter.
   *
   * This constructor sets the idle time between two executions of
   * step() to a given value.
   *
   * @param idleTime the time in microseconds used in usleep
   */
  Thread(int idleTime);

  //!@brief Destructor
  virtual ~Thread(void) = 0;

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Run executes step() and usleep in a while loop.
  virtual void run();

  /*!@brief All calculations for each time step are put into step().
   *
   * @param time determines externally measured time if step()
   */
  virtual void step(double time = 0.001) = 0;

  //!@brief Stops the thread.
  void stop();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
  bool mStop;
  bool mIsRunning;
  int mIdleTime; //!< idle time in microseconds
protected:
  // none yet
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet
protected:
  // none yet
private:
  // none yet

}; // class cedar::aux::Thread

#endif // CEDAR_AUX_THREAD_H

