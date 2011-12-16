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

    File:        namespace.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2010 11 25

    Description: Namespace file for cedar::aux::gui.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_NAMESPACE_H
#define CEDAR_AUX_GUI_NAMESPACE_H

// CEDAR INCLUDES
#ifdef DEBUG
  #include "cedar/auxiliaries/debug/lib.h"
#else
  #include "cedar/auxiliaries/release/lib.h"
#endif
#include "cedar/auxiliaries/namespace.h"

// SYSTEM INCLUDES
#include <boost/smart_ptr.hpp>

namespace cedar
{
  namespace aux
  {
    //!@brief Namespace for widget classes.
    namespace gui
    {
      //!@cond SKIPPED_DOCUMENTATION
      CEDAR_DECLARE_AUX_CLASS(BaseWidget);
      CEDAR_DECLARE_AUX_CLASS(SceneWidget);
      CEDAR_DECLARE_AUX_CLASS(Viewer);

      /* Plot widgets */
      CEDAR_DECLARE_AUX_CLASS(DataPlotInterface);
      CEDAR_DECLARE_AUX_CLASS(DataPlotter);
      CEDAR_DECLARE_AUX_CLASS(MatrixPlot);
      CEDAR_DECLARE_AUX_CLASS(MatrixPlot1D);
      CEDAR_DECLARE_AUX_CLASS(MatrixPlot2D);
      CEDAR_DECLARE_AUX_CLASS(ImagePlot);

      CEDAR_DECLARE_AUX_CLASS(HistoryPlot);
      CEDAR_DECLARE_AUX_CLASS(HistoryPlot0D);

      /* Exceptions */
      class InvalidPlotData;
      //!@endcond
    }
  }
}

// to prevent redefinitions
#undef CEDAR_CLASS_PREFIX

#endif // CEDAR_AUX_GUI_NAMESPACE_H
