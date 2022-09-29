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

    File:        ForceDirectedLayout.h

    Maintainer:  Lars Janssen
    Email:       lars.janssen@ini.rub.de
    Date:        2022 08 16

    Description: Header file for the class cedar::proc::gui::layout::ForceDirectedLayout.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_LAYOUT_FORCE_DIRECTED_LAYOUT_H
#define CEDAR_PROC_GUI_LAYOUT_FORCE_DIRECTED_LAYOUT_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/layout/Layout.h"

// FORWARD DECLARATIONS
#include "cedar/processing/gui/layout/ForceDirectedLayout.fwd.h"
#include "cedar/processing/gui/Element.fwd.h"

// SYSTEM INCLUDES
#include <string>
#include <vector>
#include <boost/graph/fruchterman_reingold.hpp>
#include <boost/graph/random_layout.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topology.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <boost/random/linear_congruential.hpp>
#include <boost/progress.hpp>
#include <QPointF>

/*! Force-Directed layout
 *
 * Arranges steps according to the Fruchterman-Reingold algorithm implemented in boost
 */
class cedar::proc::gui::layout::ForceDirectedLayout : public cedar::proc::gui::layout::Layout
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  typedef boost::rectangle_topology<> topology_type;
  typedef topology_type::point_type point_type;

  typedef boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS,
    boost::property<boost::vertex_name_t, std::string>> Graph;

  typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;

  typedef std::map<std::string, Vertex> NameToVertex;

  struct factored_square_distance_attractive_force
  {
    double attractionForceFactor;
    template < typename Graph, typename T >
    T operator()(typename boost::graph_traits< Graph >::edge_descriptor, T k, T d,
                 const Graph&) const
    {
      d = d - 100;
      return attractionForceFactor * d * d / k;
    }
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ForceDirectedLayout();

  //!@brief Destructor
  virtual ~ForceDirectedLayout();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  void arrange() override;

  Vertex getVertex(const std::string& name, Graph& g, NameToVertex& names);

  void setAttractionForceFactor(double factor);

  double getAttractionForceFactor();

  void reset();

  void setGroup(cedar::proc::gui::Group* pGroup) override;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:

  void arrangeElements(cedar::proc::gui::Group* group, Graph graph);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:

  double mAttractionForceFactor;
  std::map<std::string, QPointF> mInitialPositions;

}; // class cedar::proc::gui::layout::ForceDirectedLayout

#endif // CEDAR_PROC_GUI_LAYOUT_FORCE_DIRECTED_LAYOUT_H

