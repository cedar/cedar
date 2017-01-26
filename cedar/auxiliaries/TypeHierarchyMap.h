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

    File:        TypeHierarchyMap.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 03 21

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_TYPE_HIERARCHY_MAP_H
#define CEDAR_AUX_TYPE_HIERARCHY_MAP_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/utilities.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/TypeHierarchyMap.fwd.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/enable_shared_from_this.hpp>
#endif
#include <set>
#include <queue>
#include <iostream>
#include <map>
#include <string>

/*!@brief   A structure capable of representing parts of the inheritance sturcture between types.
 *
 *          Given a root type, different class types that inherit from it are ordered into a graph structure. This graph
 *          is built in a way that represents the inheritance structure between two types: if class B inherits from
 *          class A, then B will be below A in the graph, either as its direct child, or if there are multiple classes
 *          in between that are known to the structure, as one of the grandchildren.
 *
 *          In addition to the representation of the hierarchy, each node in the graph is also capable of holding data
 *          of the type DataType.
 *
 *          As an example, the primary reason for developing this structure was to enable flexible plotting of data.
 *          Consider MatData and ImageData: ImageData inherits MatData. Thus, any plot capable of plotting MatData is
 *          automatically also capable of plotting ImageData. Therefore, we can create a type hierarchy with
 *          cedar::aux::Data as RootType and, e.g., use a string to identify the plot type. We can then insert the pair
 *          &gt;ImageData, "ImagePlot"&lt;, which thus becomes a direct child of the root node. Next, we can insert the
 *          pair &gt;MatData, "MatPlot"&lt;. The corresponding node is then inserted between the image node and the root
 *          node. Thus, if we want to know which plots are available for ImageData, we can find the corresponding node
 *          using tree.find&gt;ImageData&lt; which will return the resulting node, whose parents, in turn, can be
 *          traversed to find the list ["ImagePlot", "MatPlot"].
 *
 * @remark  RootType must provide a default constructor.
 *
 * @remark  This structure has certain limitations: it uses dynamic casts to determine inheritance. This means that
 *          there can, in principle, be an unlimited number of classes between two nodes that are connected by an edge
 *          in the graph. This also means that the following two situations cannot be distinguished: let B inherit A,
 *          C inherit B. Using dynamic casts, it is impossible to tell whether C inherits A directly as well as via B.
 *          Thus, information in this structure is never complete, but should always be correct (telling you at least,
 *          that B is a base of C, as well as A -- via B).
 */
template <typename RootType, typename DataType>
class cedar::aux::TypeHierarchyMap
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@cond SKIPPED_DOCUMENTATION
  CEDAR_GENERATE_POINTER_TYPES(RootType);
  //!@endcond

public:
  // forward declaration of node class.
  class Node;

  //!@cond SKIPPED_DOCUMENTATION
  CEDAR_GENERATE_POINTER_TYPES(Node);
  //!@endcond

private:
  // Forward-declaration of the node template.
  template <typename T> class NodeTemplate;

public:
  /*!@brief   A node in the type hierarchy.
   *
   *          This node holds a piece of data and represents a specific type inside the hierarchy.
   *
   * @remarks Each node holds a probe instance which is used to perform dynamic casts for testing inheritance.
   */
  class Node : public boost::enable_shared_from_this<Node>
  {
    private:
      //! Set of children of the node, i.e., the classes that (presumably) directly inherit it.
      typedef std::set<NodePtr> Children;

      //! Set of the parents of the node, i.e., the (presumed) direct bases classes of this class.
      typedef std::set<NodeWeakPtr> Parents;

    public:
      //! Constant iterator over the children.
      typedef typename Children::const_iterator const_children_iterator;

      //! Constant iterator over the parents.
      typedef typename Parents::const_iterator const_parents_iterator;

      //! Iterator over the children.
      typedef typename Children::iterator children_iterator;

      //! Iterator over the parents.
      typedef typename Parents::iterator parents_iterator;

    public:
      /*!@brief Constructor that receives a probe instantce and the data to be held by this node.
       */
      Node(RootTypePtr probeInstance, const DataType& data = DataType())
      :
      mProbeInstance(probeInstance),
      mData(data)
      {
      }

      virtual ~Node()
      {
      }

      //!@brief Returns the string version of the type held by this node.
      virtual std::string getTypeString() const = 0;

      //!@brief Returns a constant iterator to the beginning of the list of children.
      const_children_iterator childrenBegin() const
      {
        return this->mChildren.begin();
      }

      //!@brief Returns a constant iterator to the end of the list of children.
      const_children_iterator childrenEnd() const
      {
        return this->mChildren.end();
      }

      //!@brief Returns a constant iterator to the beginning of the list of parents.
      const_parents_iterator parentsBegin() const
      {
        return this->mParents.begin();
      }

      //!@brief Returns a constant iterator to the end of the list of parents.
      const_parents_iterator parentsEnd() const
      {
        return this->mParents.end();
      }

      //!@brief Sets the data held by the node.
      void setData(const DataType& data)
      {
        this->mData = data;
      }

      //!@brief Returns the data held by the node.
      const DataType& getData() const
      {
        return this->mData;
      }

      //!@brief Returns the data held by the node.
      DataType& getData()
      {
        return this->mData;
      }

      /*!@brief   Inserts a new type into the sub-graph below this node, setting the given data.
       *
       * @tparam  T The type to add.
       *
       * @param   probeInstance The probe instance to use.
       *
       * @param   data The data to be inserted into the newly created node.
       *
       * @param   correspondingNode If a node was already added along the way, it is passed via this parameter. All
       *                            subsequent nodes matching this type in any way attach to/detach from this node.
       *
       * @remarks This should usually be called via the TypeHierarchyMap containing this node.
       */
      template <class T>
      NodePtr insert(ConstRootTypePtr probeInstance, const DataType& data, NodePtr correspondingNode = NodePtr())
      {
        // if the
        if (this->matchesExact(probeInstance))
        {
          this->mData = data;
          return this->shared_from_this();
        }
        bool is_child = false;
        for
        (
          const_children_iterator child_iter = this->mChildren.begin();
          child_iter != this->mChildren.end();
          ++child_iter
        )
        {
          NodePtr child = *child_iter;
          if (child->matchesDerived(probeInstance))
          {
            correspondingNode = child->template insert<T>(probeInstance, data, correspondingNode);
            is_child = true;
          }
          else if (child->template derivesFrom<T>())
          {
            // in this case, nothing else may be done.
            return this->template insertSuperClass<T>(data, correspondingNode);
          }
        }

        if (!is_child)
        {
          // insert new node for the instance.
          if (!correspondingNode)
          {
            correspondingNode = NodePtr(new NodeTemplate<T>(data));
            this->addChild(correspondingNode);
            this->connectAllDerived(correspondingNode);
          }
          else
          {
            this->addChild(correspondingNode);
          }
        }

        return correspondingNode;
      }


      /*!@brief Prints the node and all its children to the stream.
       * @param indentation The indentation for everything that is printed.
       * @param visited     Set of all nodes already printed.
       * @param stream      The stream to which to write.
       */
      void print
      (
        const std::string& indentation = "",
        std::set<ConstNodePtr> visited = std::set<ConstNodePtr>(),
        std::ostream& stream = std::cout
      ) const
      {
        if (visited.find(this->shared_from_this()) != visited.end())
        {
          stream << indentation << " LOOP!" << std::endl;
          return;
        }

        stream << indentation << cedar::aux::objectTypeToString(this->mProbeInstance)
                  << " <" << this << ">" << std::endl;
        visited.insert(this->shared_from_this());

        for (auto child : this->mChildren)
        {
          child->print(indentation + " ", visited);
        }
      }

      /*!@brief Returns all known base classes of instance.
       */
      void findBases(ConstRootTypePtr instance, std::set<ConstNodePtr>& bases) const
      {
        if (this->matchesDerived(instance))
        {
          bases.insert(this->shared_from_this());
        }

        for (auto child : this->mChildren)
        {
          child->findBases(instance, bases);
        }
      }

      /*!@brief Finds the node corresponding to instance's type.
       */
      ConstNodePtr find(ConstRootTypePtr instance) const
      {
        if (this->matchesExact(instance))
        {
          return this->shared_from_this();
        }
        else
        {
          // check all children
          for (ConstNodePtr child : this->mChildren)
          {
            // isDerived can avoid checking all the children; this should save some time.
            if (child->matchesDerived(instance))
            {
              ConstNodePtr node = child->find(instance);
              if (node)
              {
                return node;
              }
            }
          }
        }

        // finally, return nothing.
        return ConstNodePtr();
      }

      /*!@brief Tests whether the given node is a parent of this node.
       */
      bool isParent(ConstNodePtr node) const
      {
        NodeWeakPtr weaky(boost::const_pointer_cast<Node>(node));
        return this->mParents.find(weaky) != this->mParents.end();
      }

      /*!@brief Returns the node that matchest type of instance closest.
       *
       *        When the tree contains an exact match, this match is returned. Otherwise, the deepest node in the
       *        hierarchy still matching the instance's type is returned.
       */
      ConstNodePtr getClosest(ConstRootTypePtr instance) const
      {
        std::map<ConstNodePtr, unsigned int> depth_map;
        unsigned int deepest = 0;
        ConstNodePtr deepest_node;

        std::queue<ConstNodePtr> queue;
        queue.push(this->shared_from_this());
        depth_map[this->shared_from_this()] = 0;

        while (!queue.empty())
        {
          ConstNodePtr node = queue.front();
          queue.pop();

          if (node->matchesExact(instance))
          {
            return node;
          }

          CEDAR_DEBUG_ASSERT(depth_map.find(node) != depth_map.end());
          unsigned int depth = depth_map[node];

          if (node->matchesDerived(instance) && depth > deepest)
          {
            deepest_node = node;
            deepest = depth;
          }

          for
          (
            const_children_iterator iter = node->childrenBegin();
            iter != node->childrenEnd();
            ++iter
          )
          {
            ConstNodePtr child_node = *iter;
            queue.push(child_node);
            unsigned int new_depth = depth + 1;

            typename std::map<ConstNodePtr, unsigned int>::const_iterator depth_iter = depth_map.find(child_node);
            if (depth_iter != depth_map.end())
            {
              new_depth = std::max(depth_iter->second, new_depth);
            }

            depth_map[child_node] = new_depth;
          }
        }

        if (!deepest_node)
        {
          CEDAR_THROW(cedar::aux::UnknownTypeException, "Could not find a matching node for the instance.");
        }

        return deepest_node;
      }

      /*!@brief Tests whether the given node is an ancestor of this node.
       */
      bool isAncestor(ConstNodePtr node) const
      {
        // either the node is a direct parent of this node ...
        if (this->isParent(node))
        {
          return true;
        }
        else
        {
          // ... or it is a parent of the parents of this node, or their parents, and so on
          for (const_parents_iterator iter = this->mParents.begin(); iter != this->mParents.end(); ++iter)
          {
            CEDAR_ASSERT(iter->lock());
            if (iter->lock()->isAncestor(node))
            {
              return true;
            }
          }

          // otherwise, it's not an ancestor
          return false;
        }
      }

      /*!@brief Tests, whether the type of instance mathes the type of this node exactly.
       */
      virtual bool matchesExact(ConstRootTypePtr instance) const = 0;

      /*!@brief   Tests, whether the type of instance derives from the type of this node.
       *
       * @remarks True if instance can be dynamic_cast to the type of this node.
       */
      virtual bool matchesDerived(ConstRootTypePtr instance) const = 0;

      /*!@brief   Tests, whether the type of this node derives from the given type.
       *
       * @tparam  The class of which the test is done for.
       *
       * @returns true if the type of the node can be dynamic_cast to the given type
       */
      template <class T>
      bool derivesFrom() const
      {
        return static_cast<bool>(boost::dynamic_pointer_cast<const T>(this->mProbeInstance));
      }

      /*!@brief Counts all nodes connected to this node (directly and transitively).
       */
      size_t countConnected() const
      {
        std::set<ConstNodePtr> nodes;
        this->listAllConnectedNodes(nodes);
        return nodes.size();
      }

      /*!@brief Returns the number of direct children of this node.
       */
      size_t childrenSize() const
      {
        return this->mChildren.size();
      }

    private:
      /*!@brief Connects all related classes to this node that are connected to the given node.
       */
      void connectAllDerived(NodePtr node)
      {
        std::set<NodePtr> nodes;
        node->listAllConnectedNodes(nodes);
        for (typename std::set<NodePtr>::iterator iter = nodes.begin(); iter != nodes.end(); ++iter)
        {
          NodePtr current_node = *iter;

          if (current_node == node)
          {
            continue;
          }

          if(current_node->matchesExact(node->mProbeInstance))
          {
            continue;
          }

          if (current_node->matchesDerived(node->mProbeInstance) && !node->isAncestor(current_node))
          {
            node->addParent(current_node);
          }

          if (node->matchesDerived(current_node->mProbeInstance) && !current_node->isAncestor(node))
          {
            current_node->addParent(node);
          }
        }
      }

      /*!@brief Lists all nodes connected to this node.
       */
      void listAllConnectedNodes(std::set<NodePtr>& nodes)
      {
        std::queue<NodePtr> to_explore;
        to_explore.push(this->shared_from_this());

        while (!to_explore.empty())
        {
          NodePtr current = to_explore.front();
          to_explore.pop();

          // insert all unexplored children
          for (children_iterator iter = current->mChildren.begin(); iter != current->mChildren.end(); ++iter)
          {
            if (nodes.find(*iter) == nodes.end())
            {
              nodes.insert(*iter);
              to_explore.push(*iter);
            }
          }
          // insert all unexplored parents
          for (parents_iterator iter = current->mParents.begin(); iter != current->mParents.end(); ++iter)
          {
            CEDAR_ASSERT(iter->lock());
            if (nodes.find(iter->lock()) == nodes.end())
            {
              nodes.insert(iter->lock());
              to_explore.push(iter->lock());
            }
          }
        }
      }

      /*!@brief Lists all nodes connected to this node.
       */
      void listAllConnectedNodes(std::set<ConstNodePtr>& nodes) const
      {
        std::queue<ConstNodePtr> to_explore;
        to_explore.push(this->shared_from_this());

        while (!to_explore.empty())
        {
          ConstNodePtr current = to_explore.front();
          to_explore.pop();

          // insert all unexplored children
          for (const_children_iterator iter = current->mChildren.begin(); iter != current->mChildren.end(); ++iter)
          {
            if (nodes.find(*iter) == nodes.end())
            {
              nodes.insert(*iter);
              to_explore.push(*iter);
            }
          }
          // insert all unexplored parents
          for (const_parents_iterator iter = current->mParents.begin(); iter != current->mParents.end(); ++iter)
          {
            CEDAR_ASSERT(iter->lock());
            if (nodes.find(iter->lock()) == nodes.end())
            {
              nodes.insert(iter->lock());
              to_explore.push(iter->lock());
            }
          }
        }
      }

      /*!@brief Inserts a class that is a superclass of this node.
       */
      template <class T>
      NodePtr insertSuperClass(const DataType& data, NodePtr insertedNode)
      {
        // add a new node, make the current child a child of the new node.
        if (!insertedNode)
          insertedNode = NodePtr(new NodeTemplate<T>(data));

        std::set<NodePtr> children_to_remove;
        for
        (
          const_children_iterator child_iter = this->mChildren.begin();
          child_iter != this->mChildren.end();
          ++child_iter
        )
        {
          NodePtr child = *child_iter;
          if (child->template derivesFrom<T>())
          {
            insertedNode->addChild(child);
            children_to_remove.insert(child);
          }
        }

        for
        (
          typename std::set<NodePtr>::iterator remove_it = children_to_remove.begin();
          remove_it != children_to_remove.end();
          ++remove_it
        )
        {
          this->removeChild(*remove_it);
        }

        this->addChild(insertedNode);
        this->connectAllDerived(insertedNode);
        return insertedNode;
      }

      /*!@brief Adds a parent to this node.
       */
      void addParent(NodePtr parent)
      {
        if (parent.get() == this)
        {
          return;
        }

        if (this->mParents.find(parent) == this->mParents.end())
        {
          this->mParents.insert(parent);
          parent->addChild(this->shared_from_this());
        }
      }

      /*!@brief Adds a child to this node.
       */
      void addChild(NodePtr child)
      {
        if (child.get() == this)
        {
          return;
        }

        if (this->mChildren.find(child) == this->mChildren.end())
        {
          this->mChildren.insert(child);
          child->addParent(this->shared_from_this());
        }
      }

      /*!@brief Removes one of this node's parents.
       */
      void removeParent(NodePtr node)
      {
        parents_iterator iter = this->mParents.find(node);
        if (iter != this->mParents.end())
        {
          this->mParents.erase(iter);
          node->removeChild(this->shared_from_this());
        }
      }

      /*!@brief Removes one of this node's children.
       */
      void removeChild(NodePtr node)
      {
        children_iterator iter = this->mChildren.find(node);
        if (iter != this->mChildren.end())
        {
          this->mChildren.erase(iter);
          node->removeParent(this->shared_from_this());
        }
      }

    private:
      //! The probe instance used for making dynamic casts.
      RootTypePtr mProbeInstance;

      //! The data held by this node.
      DataType mData;

      //! The children of this node.
      Children mChildren;

      //! The parents of this node.
      Parents mParents;
  };

private:
  /*!@brief  Node that represents a class of the given type.
   *
   * @tparam T Type held by this node.
   */
  template <typename T>
  class NodeTemplate : public Node
  {
    public:
      /*!@brief The default constructor.
       */
      NodeTemplate(const DataType& data = DataType())
      :
      Node(RootTypePtr(new T()), data)
      {
      }

      /*!@brief Returns a string representation of the type that this node represents.
       */
      std::string getTypeString() const
      {
        return cedar::aux::typeToString<T>();
      }

      /*!@brief Checks whether the type of the given instance is exactly the one represented by this node.
       */
      bool matchesExact(ConstRootTypePtr instance) const
      {
        return typeid(*instance) == typeid(T);
      }

      /*!@brief Checks whether the type of instance inherits the type represented by this node.
       */
      bool matchesDerived(ConstRootTypePtr instance) const
      {
        return static_cast<bool>(boost::dynamic_pointer_cast<const T>(instance));
      }
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  TypeHierarchyMap(const DataType& rootData = DataType())
  :
  mRootNode(new NodeTemplate<RootType>(rootData))
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief  Inserts a type into the hierarchy with the given data.
   *
   * @tparam T The type to insert.
   * @param  data Data to be associated with the type.
   */
  template <typename T>
  void insert(const DataType& data)
  {
    RootTypePtr instance(new T());
    this->mRootNode->template insert<T>(instance, data);
  }

  /*!@brief Checks if a node with a type corresponding to the given instance can be found.
   */
  bool canFind(ConstRootTypePtr instance) const
  {
    ConstNodePtr node = this->getRootNode()->find(instance);

    if (!node)
    {
      return false;
    }
    else
    {
      return true;
    }
  }

  /*!@brief Returns all the data along the path of the instance.
   */
  ConstNodePtr find(ConstRootTypePtr instance) const
  {
    ConstNodePtr node = this->getRootNode()->find(instance);

    if (!node)
    {
      CEDAR_THROW(cedar::aux::UnknownTypeException, "The type cannot be found in this hierarchy.");
    }

    return node;
  }

  /*!@brief Returns all the data along the path of the instance.
   */
  NodePtr find(ConstRootTypePtr instance)
  {
    ConstNodePtr node = this->getRootNode()->find(instance);

    if (!node)
    {
      CEDAR_THROW(cedar::aux::UnknownTypeException, "The type cannot be found in this hierarchy.");
    }

    return boost::const_pointer_cast<Node>(node);
  }

  /*!@brief Returns the base classes of the instance.
   */
  void findBases(ConstRootTypePtr instance, std::set<ConstNodePtr>& bases) const
  {
    bases.clear();
    this->mRootNode->findBases(instance, bases);
  }

  /*!@brief Returns the type in the hierarchy that represents the type of instance most closely.
   */
  const DataType& getClosest(ConstRootTypePtr instance) const
  {
    return this->getClosestNode(instance)->getData();
  }

  /*!@brief Returns the node corresponding to the type in the hierarchy that represents the type of instance most
   *        closely.
   */
  ConstNodePtr getClosestNode(ConstRootTypePtr instance) const
  {
    return this->mRootNode->getClosest(instance);
  }

  /*!@brief Returns the number of nodes currently stored in the hierarchy.
   */
  size_t size() const
  {
    return this->mRootNode->countConnected();
  }

  /*! Checks if the class can be found.
   */
  template <class T>
  bool canFind() const
  {
    RootTypePtr instance(new T());
    return this->canFind(instance);
  }

  /*!@brief Retrieves the node corresponding to the given type.
   */
  template <class T>
  ConstNodePtr find() const
  {
    RootTypePtr instance(new T());
    return this->find(instance);
  }

  /*!@brief Retrieves the node corresponding to the given type.
   */
  template <class T>
  NodePtr find()
  {
    RootTypePtr instance(new T());
    return this->find(instance);
  }

  /*!@brief Sets the data of the root node.
   */
  void setRootData(const DataType& data)
  {
    this->mRootNode->setData(data);
  }

  /*!@brief Returns the root node.
   */
  ConstNodePtr getRootNode() const
  {
    return this->mRootNode;
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
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! The root node of the hierarchy. This node represents RootType.
  NodePtr mRootNode;

}; // class cedar::aux::TypeHierarchyMap

#endif // CEDAR_AUX_TYPE_HIERARCHY_MAP_H

