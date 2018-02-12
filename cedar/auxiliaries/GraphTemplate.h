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

    File:        GraphTemplate.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 09 17

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GRAPH_TEMPLATE_H
#define CEDAR_AUX_GRAPH_TEMPLATE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/GraphTemplate.fwd.h"

// SYSTEM INCLUDES
#include <map>
#include <vector>
#include <set>


/*!@brief A generic graph implementation.
 *
 * @todo Use policies to determine the way edges and nodes are stored.
 * @todo Policy for directedness of the graph.
 * @todo Unit test this class.
 */
template <typename NodePayloadT, typename EdgePayloadT>
class cedar::aux::GraphTemplate
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Type of the nodes' payloads.
  typedef NodePayloadT NodePayload;

  //! Type of the edges' payloads.
  typedef EdgePayloadT EdgePayload;

  //! Type used for indexing nodes.
  typedef unsigned int IndexType;

  /*! @brief A generic class that represents a node in a graph.
   *
   *         Nodes can be associated with custom data, i.e., a payload. The type of this data can be chosen by the user.
   */
  class Node
  {
    public:
      //! Constructor.
      Node(const NodePayload& payload)
      :
      mPayload(payload)
      {
      }

      //! Returns the payload of the node.
      const NodePayload& getPayload() const
      {
        return this->mPayload;
      }

    private:
      NodePayload mPayload;
  };
  //!@cond SKIPPED_DOCUMENTATION
  CEDAR_GENERATE_POINTER_TYPES(Node);
  //!@endcond

  //! A generic class for representing edges in a graph.
  class Edge
  {
    public:
      //! Standard constructor.
      Edge(const EdgePayload& payload = EdgePayload())
      :
      mPayload(payload)
      {
      }

    private:
      EdgePayload mPayload;
  };
  //!@cond SKIPPED_DOCUMENTATION
  CEDAR_GENERATE_POINTER_TYPES(Edge);
  //!@endcond

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  GraphTemplate()
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*! Adds a node to the graph.
   */
  void addNode(NodePtr node)
  {
    // get a new index for the node
    IndexType index = this->getNewIndex();

    // add the node to the indices
    this->mIdsByNode[node] = index;
    this->mNodesById[index] = node;
  }

  /*! Creates a node with the given payload, adds it to the graph and returns it.
   */
  NodePtr addNodeForPayload(const NodePayload& payload)
  {
    auto node = NodePtr(new Node(payload));
    this->addNode(node);
    return node;
  }

  /*! Adds an edge in the graph.
   */
  void addEdge(NodePtr source, NodePtr target, EdgePtr edge = EdgePtr(new Edge()))
  {
    IndexType source_id = this->getIdByNode(source);
    IndexType target_id = this->getIdByNode(target);

    if (this->mDirectedEdges.size() <= source_id)
    {
      this->mDirectedEdges.resize(source_id + 1);
    }

    //!@todo Throw an EdgeAlreadyExistsException
    CEDAR_ASSERT(!this->edgeExists(source_id, target_id));

    this->mDirectedEdges[source_id][target_id] = edge;
  }

  /*! Tests whether an edge exists between the given nodes.
   */
  bool edgeExists(NodePtr source, NodePtr target)
  {
    IndexType source_id = this->getIdByNode(source);
    IndexType target_id = this->getIdByNode(target);

    return edgeExists(source_id, target_id);
  }

  /*! Checks whether a node with the given payload exists in this graph.
   */
  bool hasNodeForPayload(const NodePayload& payload) const
  {
    if (this->getNodeByPayload(payload))
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  /*! Returns the node for the given payload.
   */
  NodePtr getNodeByPayload(const NodePayload& payload) const
  {
    NodePtr node;
    for (auto iter = this->mNodesById.begin(); iter != this->mNodesById.end(); ++iter)
    {
      if (iter->second->getPayload() == payload)
      {
        if (node)
        {
          //!@todo Proper exception: multiple nodes with the same payload.
          CEDAR_ASSERT(false);
        }
        else
        {
          node = iter->second;
        }
      }
    }

    return node;
  }

  /*!@brief Tests for cycles in the graph.
   */
  bool testForCycle() const
  {
    return !(this->getCycleComponents().empty());
  }

  /*! Returns a list of all the strongly connected components with more than one node (i.e., all cycles)
   */
  std::vector<std::set<NodePtr> > getCycleComponents()
  {
    // determine strongly connected components
    std::vector<std::set<NodePtr> > strongly_connected_components = tarjan();
    std::vector<std::set<NodePtr> > cycles;

    // per definition, if all strongly connected components consist of only one node, the graph is acyclic
    for (auto iter = strongly_connected_components.begin(); iter != strongly_connected_components.end(); ++iter)
    {
      if (iter->size() > 1)
      {
        cycles.push_back(*iter);
      }
    }
    return cycles;
  }

  /*! Returns a list of all direct successors of the given node.
   */
  std::vector<NodePtr> getDirectSuccessors(NodePtr source) const
  {
    IndexType source_id = getIdByNode(source);

    std::vector<NodePtr> successors;

    if (source_id >= mDirectedEdges.size())
    {
      // there are no edges coming from the node
      return successors;
    }

    auto edges = mDirectedEdges.at(source_id);

    for (auto edge_it = edges.begin(); edge_it != edges.end(); ++edge_it)
    {
      IndexType target_id = edge_it->first;
      NodePtr target = this->getNodeById(target_id);
      successors.push_back(target);
    }
    return successors;
  }

  //! Returns the maximum distance for all nodes in the graph, starting from the given node.
  std::map<NodePtr, unsigned int> getMaximumDistance(NodePtr from) const
  {
    std::map<NodePtr, unsigned int> distances;

    std::vector<NodePtr> to_explore;

    // initialize
    to_explore.push_back(from);
    distances[from] = 0;

    while (!to_explore.empty())
    {
      // take out first element still to be explored
      NodePtr current = to_explore.back();
      to_explore.pop_back();

      // determine the distance of the current element
      auto current_dist_iter = distances.find(current);
      CEDAR_DEBUG_ASSERT(current_dist_iter != distances.end());
      auto current_distance = current_dist_iter->second;

      // iterate over all its direct successors
      auto successors = this->getDirectSuccessors(current);
      for (auto iter = successors.begin(); iter != successors.end(); ++iter)
      {
        auto successor = *iter;
        auto dist_iter = distances.find(successor);

        if (dist_iter == distances.end())
        {
          distances[successor] = current_distance + 1;
        }
        else
        {
          dist_iter->second = std::max(current_distance + 1, dist_iter->second);
        }

        to_explore.push_back(successor);
      }
    }

    return distances;
  }

  /*! @brief  Implements Tarjan's algorithm for separating the graph's strongly connected components.
   *
   * @return  A list of sets of nodes, where each set is a strongly connected component.
   *
   * @remarks If each strongly connected component contains one node, the graph is acyclic.
   *
   * @see     http://en.wikipedia.org/wiki/Tarjan%E2%80%99s_strongly_connected_components_algorithm#The_algorithm_in_pseudocode
   */
  std::vector<std::set<NodePtr> > tarjan() const
  {
    std::vector<std::set<NodePtr> > strongly_connected_components;

    unsigned int index = 0;
    std::map<NodePtr, unsigned int> node_index;
    std::map<NodePtr, unsigned int> low_link;
    std::vector<NodePtr> node_stack;

    for (auto node_iter = this->mNodesById.begin(); node_iter != this->mNodesById.end(); ++node_iter)
    {
      NodePtr node = node_iter->second;

      if (node_index.find(node) == node_index.end())
      {
        this->tarjanStrongconnect(node, index, node_index, low_link, node_stack, strongly_connected_components);
      }
    }

    return strongly_connected_components;
  }

  /*! A helper function for Tarjan's algorithm.
   *
   * @see     http://en.wikipedia.org/wiki/Tarjan%E2%80%99s_strongly_connected_components_algorithm#The_algorithm_in_pseudocode
   */
  void tarjanStrongconnect
      (
        NodePtr node,
        unsigned int& index,
        std::map<NodePtr, unsigned int>& nodeIndex,
        std::map<NodePtr, unsigned int>& lowLink,
        std::vector<NodePtr>& nodeStack,
        std::vector<std::set<NodePtr> >& stronglyConnectedComponents
      )
      const
  {
    nodeIndex[node] = index;
    lowLink[node] = index;
    index += 1;
    nodeStack.push_back(node);

    auto successors = this->getDirectSuccessors(node);

    for (auto suc_iter = successors.begin(); suc_iter != successors.end(); ++suc_iter)
    {
      NodePtr successor = *suc_iter;

      auto node_index_iter = nodeIndex.find(successor);
      if (node_index_iter == nodeIndex.end())
      {
        // calculate strongly connected components of successor
        tarjanStrongconnect(successor, index, nodeIndex, lowLink, nodeStack, stronglyConnectedComponents);

        CEDAR_DEBUG_ASSERT(lowLink.find(successor) != lowLink.end());
        lowLink[node] = std::min(lowLink[node], lowLink[successor]);
      }
      else
      {
        // see if the successor is already in the node stack
        auto iter = std::find(nodeStack.begin(), nodeStack.end(), successor);
        if (iter != nodeStack.end())
        {
          CEDAR_DEBUG_ASSERT(nodeIndex.find(successor) != nodeIndex.end());
          lowLink[node] = std::min(lowLink[node], nodeIndex[successor]);
        }
      }
    }

    if (lowLink[node] == nodeIndex[node])
    {
      // start new strongly connected component
      stronglyConnectedComponents.push_back(std::set<NodePtr>());

      NodePtr top;
      while (top != node)
      {
        top = nodeStack.back();
        nodeStack.pop_back();
        stronglyConnectedComponents.back().insert(top);
      }
    }
  }

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  bool edgeExists(IndexType sourceId, IndexType targetId)
  {
    if (this->mDirectedEdges.size() <= sourceId || this->mDirectedEdges[sourceId].size() <= targetId)
    {
      return false;
    }

    if (this->mDirectedEdges[sourceId][targetId])
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  //! Returns a currently unused index.
  IndexType getNewIndex() const
  {
    if (this->mNodesById.empty())
    {
      return 0;
    }

    // get the end iterator
    auto last = this->mNodesById.end();

    // decrease it by one (get last element)
    --last;

    // use the index of the last element, +1
    IndexType new_index = last->first + 1;

    // make sure the index is unique
    CEDAR_ASSERT(this->mNodesById.find(new_index) == this->mNodesById.end());

    return new_index;
  }

  NodePtr getNodeById(IndexType index) const
  {
    auto iter = this->mNodesById.find(index);
    //!@todo Throw node not in graph exception
    CEDAR_ASSERT(iter != this->mNodesById.end());
    return iter->second;
  }

  IndexType getIdByNode(NodePtr node) const
  {
    auto iter = mIdsByNode.find(node);
    //!@todo Throw node not in graph exception
    CEDAR_ASSERT(iter != this->mIdsByNode.end());
    return iter->second;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  std::map<NodePtr, IndexType> mIdsByNode;

  std::map<IndexType, NodePtr> mNodesById;

  //! Encoding of edges; format: (source, target) <- edge
  std::vector<std::map<IndexType, EdgePtr> > mDirectedEdges;

}; // class cedar::aux::GraphTemplate

#endif // CEDAR_AUX_GRAPH_TEMPLATE_H

