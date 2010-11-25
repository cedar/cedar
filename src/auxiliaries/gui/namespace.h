/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        Namespace.h

 ----- Author:      Stephan Zibner
 ----- Email:       stephan.zibner@ini.rub.de
 ----- Date:        2010 11 25

 ----- Description: Namespace file for cedar::aux::gui.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_AUX_GUI_NAMESPACE_H
#define CEDAR_AUX_GUI_NAMESPACE_H

// LOCAL INCLUDES

// PROJECT INCLUDES
#include "src/namespace.h"

// SYSTEM INCLUDES
#include <boost/smart_ptr.hpp>

namespace cedar
{
  namespace aux
  {
    //!@brief Namespace for widget classes.
    namespace gui
    {
      class BaseWidget;
      typedef boost::shared_ptr<BaseWidget> BaseWidgetPtr;
    }
  }
}

#endif // CEDAR_AUX_GUI_NAMESPACE_H
