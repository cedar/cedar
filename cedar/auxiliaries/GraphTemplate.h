/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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
#include "cedar/auxiliaries/namespace.h"

// SYSTEM INCLUDES


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
  typedef NodePayloadT NodePayload;
  typedef EdgePayloadT EdgePayload;
  typedef unsigned int IndexType;

  class Node
  {
    public:
      Node(const NodePayload& payload)
      :
      mPayload(payload)
      {
      }

      const NodePayload& getPayload() const
      {
        return this->mPayload;
      }

    private:
      NodePayload mPayload;
  };
  CEDAR_GENERATE_POINTER_TYPES(Node);

  class Edge
  {
    public:
      Edge(const EdgePayload& payload = EdgePayload())
      :
      mPayload(payload)
      {
      }

    private:
      EdgePayload mPayload;
  };
  CEDAR_GENERATE_POINTER_TYPES(Edge);

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
  void addNode(NodePtr node)
  {
    // get a new index for the node
    IndexType index = this->getNewIndex();

    // add the node to the indices
    this->mIdsByNode[node] = index;
    this->mNodesById[index] = node;
  }

  NodePtr addNodeForPayload(const NodePayload& payload)
  {
    auto node = NodePtr(new Node(payload));
    this->addNode(node);
    return node;
  }

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

  bool edgeExists(NodePtr source, NodePtr target)
  {
    IndexType source_id = this->getIdByNode(source);
    IndexType target_id = this->getIdByNode(target);

    return edgeExists(source_id, target_id);
  }

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

  bool testForCycle() const
  {
    for (auto iter = this->mNodesById.begin(); iter != this->mNodesById.end(); ++iter)
    {
      NodePtr node = iter->second;

      if (this->testNodeForCylce(node))
      {
        return true;
      }
    }

    return false;
  }

  bool testNodeForCylce(NodePtr fromNode) const
  {
    std::set<NodePtr> visited;
    return this->testNodeForCylce(fromNode, visited);
  }

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

    return distances;
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

  bool testNodeForCylce(NodePtr fromNode, std::set<NodePtr>& visited) const
  {
    std::vector<NodePtr> successors = this->getDirectSuccessors(fromNode);

    for (size_t i = 0; i < successors.size(); ++i)
    {
      NodePtr successor = successors.at(i);

      // if the node was already visited, we found a cycle
      if (visited.find(successor) != visited.end())
      {
        return true;
      }

      visited.insert(successor);

      if (this->testNodeForCylce(successor, visited))
      {
        return true;
      }
    }

    return false;
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

