/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        Robot.h

 ----- Author:      Mathis Richter
 ----- Email:       mathis.richter@ini.rub.de
 ----- Date:        2010 11 08

 ----- Description: Header for the \em cedar::dev::robot::Robot class.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_DEV_ROBOT_ROBOT_H
#define CEDAR_DEV_ROBOT_ROBOT_H

// LOCAL INCLUDES
#include "Namespace.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <vector>
#include <string>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::dev::robot::Robot
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
  /*! \brief Checks if a subcomponent with the supplied \em componentName exists for a parent component
   * with the name \em parentComponentName.
   *
   * @param[in] componentName Name of the subcomponent we need to check.
   * @param[in] parentComponentName Name of the component, that might be the parent of \em componentName.
   *
   */
  bool isComponentAvailable(
                             const std::string& componentName,
                             const std::string& parentComponentName
                           ) const
  {};

  /*! \brief Checks if a vector of subcomponents exist for a parent component with the
   * name \em parentComponentName.
   *
   * @param[in] components Vector of component names to be checked.
   * @param[in] parentComponentName Name of the component, that might be the parent of \em componentName.
   *
   */
  bool areComponentsAvailable(
                               const std::vector<std::string>& components,
                               const std::string& parentComponentName
                             ) const
  {};


  /*! \brief Returns the number of subcomponents for a given parent component. */
  unsigned int getNumberOfComponents(const std::string& parentComponentName) const;

  /*! \brief Returns a set containing the names of all available subcomponents for a given parent component. */
  const std::set<std::string>& getComponentNames(const std::string& parentComponentName) const;

  /*! \brief Checks if a component with the supplied \em componentName exists. */
  virtual bool isComponentAvailable(const std::string& componentName) const {};

  /*! \brief Checks if all components with the names supplied in the vector \em components exist. */
  virtual bool areComponentsAvailable(const std::vector<std::string>& components) const {};

  /*! \brief Returns a pointer to the parent component.
   *
   * @return Pointer to the parent component.
   */
  virtual ComponentPtr& getParent() {};

  /*! \brief Returns a pointer to the component with the name \em componentName.
   *
   * @return Pointer to the requested component.
   * @param[in] componentName Name of the component that is to be returned.
   */
  virtual ComponentPtr& getComponent(const std::string& componentName) {};

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

}; // class cedar::dev::robot::Robot

#endif // CEDAR_DEV_ROBOT_ROBOT_H
