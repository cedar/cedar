/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        Robot.h

 ----- Author:      Mathis Richter
 ----- Email:       mathis.richter@ini.rub.de
 ----- Date:        2010 11 08

 ----- Description: Header for the @em cedar::dev::robot::Robot class.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_DEV_ROBOT_ROBOT_H
#define CEDAR_DEV_ROBOT_ROBOT_H

// LOCAL INCLUDES
#include "Namespace.h"

// PROJECT INCLUDES
#include "src/auxiliaries/Base.h"

// SYSTEM INCLUDES
#include <vector>
#include <string>
#include <set>
#include <map>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::dev::robot::Robot : public cedar::aux::Base
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief constructor
  Robot();
  //!@brief destructor
  virtual ~Robot() = 0;

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*! @brief Returns a pointer to the component with the name @em componentName.
   *
   * @return Pointer to the requested component.
   * @param[in] componentName Name of the component that is to be returned.
   */
  ComponentPtr& getComponent(const std::string& rComponentName);

  /*! @brief Creates a specified component.
   *
   * Abstract factory class, which will be implemented by the concrete subclass.
   */
  virtual ComponentPtr createComponent(const std::string& rComponentName) = 0;

  /*! @brief Checks if a component with the supplied @em rComponentName exists within the robot.
   *
   * @param[in] componentName Name of the subcomponent we need to check.
   */
  bool isComponentAvailable(const std::string& rComponentName) const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  /*! @brief Checks if a subcomponent with the supplied @em rComponentName exists for a parent component
   * with the name @em parentComponentName.
   *
   * @param[in] rComponentName Name of the subcomponent we need to check.
   * @param[in] rParentComponentName Name of the component, that might be the parent of @em componentName.
   */
  bool isComponentAvailable(
                             const std::string& rComponentName,
                             const std::string& rParentComponentName
                           ) const;

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
protected:
  //! map of pointers to all sub components
  std::map<std::string, ComponentPtr> mComponents;
  //!< names of all components and their corresponding sub-components
  std::map<std::string, std::set<std::string> > _mSubComponentNames;

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

}; // class cedar::dev::robot::Robot

#endif // CEDAR_DEV_ROBOT_ROBOT_H
