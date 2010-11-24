/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        TestObject.h

 ----- Author:      Hendrik Reimann
 ----- Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 11 19

 ----- Description: Header for the @em cedar::tests::unit::aux::gl::TestObject class.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_TESTS_UNIT_AUX_GL_OBJECT_TEST_OBJECT_H
#define CEDAR_TESTS_UNIT_AUX_GL_OBJECT_TEST_OBJECT_H

// LOCAL INCLUDES
#include "namespace.h"

// PROJECT INCLUDES
#include "src/auxiliaries/gl/Object.h"

// SYSTEM INCLUDES

/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::tests::unit::aux::gl::Object::TestObject : public cedar::aux::gl::Object
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Constructor that gets a configuration file name.
  TestObject();

  //!@brief Destructor
  virtual ~TestObject(void);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void draw();


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
  // none yet (hopefully never!)
protected:
  //! test member bool

private:
  // none yet

}; // class cedar::tests::unit::aux::gl::Object::TestObject

#endif // CEDAR_TESTS_UNIT_AUX_CONFIGURATION_INTERFACE_TEST_CLASS_H

