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

    File:        ForceDirectedLayout.cpp

    Maintainer:  Lars Janssen
    Email:       lars.janssen@ini.rub.de
    Date:        2022 08 16

    Description: Source file for the class cedar::proc::gui::layout::ForceDirectedLayout.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/gui/layout/ForceDirectedLayout.h"
#include "cedar/processing/gui/Group.h"
#include "cedar/processing/Element.h"
#include "cedar/processing/DataConnection.h"

// CEDAR INCLUDES

// SYSTEM INCLUDES
#include <cmath>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::layout::ForceDirectedLayout::ForceDirectedLayout()
:
mAttractionForceFactor(6)
{
}

cedar::proc::gui::layout::ForceDirectedLayout::~ForceDirectedLayout()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::layout::ForceDirectedLayout::Vertex
cedar::proc::gui::layout::ForceDirectedLayout::getVertex(const std::string& name, Graph& g, NameToVertex& names)
{
  NameToVertex::iterator i = names.find(name);
  if (i == names.end())
    i = names.insert(std::make_pair(name, add_vertex(name, g))).first;
  return i->second;
}

void cedar::proc::gui::layout::ForceDirectedLayout::setAttractionForceFactor(double factor)
{
  this->mAttractionForceFactor = factor;
}

double cedar::proc::gui::layout::ForceDirectedLayout::getAttractionForceFactor()
{
  return this->mAttractionForceFactor;
}

void cedar::proc::gui::layout::ForceDirectedLayout::setGroup(cedar::proc::gui::Group* pGroup)
{
  this->mpGroup = pGroup;

  //Save position of all elements
  this->mInitialPositions.clear();
  std::map<std::string, cedar::proc::ElementPtr> elements = this->mpGroup->getGroup()->getElements();

  for(auto iter = elements.begin(); iter != elements.end(); iter++)
  {
    cedar::proc::gui::Element *guiElement = this->mpGroup->getScene()->getGraphicsItemFor(iter->second);
    this->mInitialPositions.emplace(iter->second->getName(), guiElement->pos());
  }
}

void cedar::proc::gui::layout::ForceDirectedLayout::reset()
{
  CEDAR_ASSERT(this->mpGroup != nullptr)
  // Reset position of all steps
  std::map<std::string, cedar::proc::ElementPtr> elements = this->mpGroup->getGroup()->getElements();

  for(auto iter = elements.begin(); iter != elements.end(); iter++)
  {
    cedar::proc::gui::Element *guiElement = this->mpGroup->getScene()->getGraphicsItemFor(iter->second);
    if(this->mInitialPositions.find(iter->second->getName()) != this->mInitialPositions.end())
    {
      QPointF pos = this->mInitialPositions.at(iter->second->getName());
      guiElement->setPos(pos);
    }
  }
}

void cedar::proc::gui::layout::ForceDirectedLayout::arrange()
{
  CEDAR_ASSERT(this->mpGroup != nullptr)

  std::map<std::string, cedar::proc::ElementPtr> elements = this->mpGroup->getGroup()->getElements();
  int numElements = elements.size();
  if(numElements <= 0)
  {
    return;
  }
  double width = ceil(sqrt(numElements)) * 250;
  double height = ceil(sqrt(numElements)) * 120;

  Graph g;
  NameToVertex names;


  // Read nodes
  int i = 0;
  for (auto iter = elements.begin(); iter != elements.end(); ++iter)
  {
    std::string source = iter->first;
    if(auto con = dynamic_cast<cedar::proc::Connectable*>(iter->second.get()))
    {
      std::vector<cedar::proc::DataSlotPtr> outputSlots = con->getOrderedDataSlots(cedar::proc::DataRole::OUTPUT);
      for(cedar::proc::DataSlotPtr outputSlot : outputSlots)
      {
        std::vector<cedar::proc::DataConnectionPtr> outputConns = outputSlot->getDataConnections();
        for(cedar::proc::DataConnectionPtr outputConn : outputConns)
        {
          std::string target = outputConn->getTarget()->getParentPtr()->getName();
          add_edge(this->getVertex(source, g, names), this->getVertex(target, g, names), g);
        }
      }
    }
  }

  typedef std::vector<point_type> PositionVec;
  PositionVec position_vec(num_vertices(g));
  typedef boost::iterator_property_map<PositionVec::iterator, boost::property_map<Graph, boost::vertex_index_t>::type>
    PositionMap;
  PositionMap position(position_vec.begin(), get(boost::vertex_index, g));

  boost::minstd_rand gen;
  topology_type topo(gen, -width / 2, -height / 2, width / 2, height / 2);
  random_graph_layout(g, position, topo);

  auto attraction = factored_square_distance_attractive_force();
  attraction.attractionForceFactor = this->mAttractionForceFactor;
  fruchterman_reingold_force_directed_layout(g, position, topo,
                                             boost::attractive_force(attraction));
  boost::graph_traits<Graph>::vertex_iterator vi, vi_end;
  for (boost::tie(vi, vi_end) = vertices(g); vi != vi_end; ++vi)
  {
    cedar::proc::ElementPtr element = elements.at(get(boost::vertex_name, g, *vi));
    if(element != nullptr)
    {
      cedar::proc::gui::Element* guiElement = this->mpGroup->getScene()->getGraphicsItemFor(element);
      guiElement->setPos(position[*vi][0], position[*vi][1]);
    }
  }
}
