/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        Component.h

 ----- Author:      Mathis Richter
 ----- Email:       mathis.richter@ini.rub.de
 ----- Date:        2010 08 30

 ----- Description: Header for the \em cedar::dev::robot::Component class.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_DEV_ROBOT_COMPONENT_H
#define CEDAR_DEV_ROBOT_COMPONENT_H

// LOCAL INCLUDES
#include "Namespace.h"

// PROJECT INCLUDES
#include "src/auxiliaries/Base.h"

// SYSTEM INCLUDES
#include <vector>
#include <string>
#include <set>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::dev::robot::Component : public cedar::aux::Base
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief constructor
  Component();
  //!@brief destructor
  virtual ~Component() = 0;

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*! \brief Returns the name of the this component's parent. */
  const std::string& getParentName() const;

  /*! \brief Returns the number of subcomponents. */
  unsigned int getNumberOfComponents() const;

  /*! \brief Returns a set containing the names of all available subcomponents. */
  const std::set<std::string>& getComponentNames() const;


  /*! \brief Checks if a component with the supplied \em componentName exists. */
  virtual bool isComponentAvailable(const std::string& componentName) const;

  /*! \brief Checks if all components with the names supplied in the vector \em components exist. */
  virtual bool areComponentsAvailable(const std::vector<std::string>& components) const;

  /*! \brief Returns a pointer to the parent component.
   *
   * @return Pointer to the parent component.
   */
  virtual ComponentPtr& getParent();

  /*! \brief Returns a pointer to the component with the name \em componentName.
   *
   * @return Pointer to the requested component.
   * @param[in] componentName Name of the component that is to be returned.
   */
  virtual ComponentPtr& getComponent(const std::string& componentName);

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
  //! name of the category the component is in (in the config file)
  std::string mCategoryName;
  //! pointer to the robot the component belongs to
  RobotPtr mpRobot;

private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet
protected:
  //! name of the parent component (if it exists)
  std::string _mParentName;
private:
  // none yet

}; // class cedar::dev::robot::Component

#endif // CEDAR_DEV_ROBOT_COMPONENT_H
