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
#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/utilities.h"

// SYSTEM INCLUDES
#include <boost/enable_shared_from_this.hpp>
#include <set>
#include <queue>

/*!@todo describe.
 *
 * @todo describe more.
 *
 * @todo Right now, this cannot deal with multiple inheritance. Should it ever be able to do so?
 * @remarks RootType must provide a default constructor.
 */
template <typename RootType, typename DataType>
class cedar::aux::TypeHierarchyMap
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  CEDAR_GENERATE_POINTER_TYPES(RootType);

  // forward declaration of child classes
public:
  class Node;
  CEDAR_GENERATE_POINTER_TYPES(Node);

private:
  template <typename T> class NodeTemplate;

public:
  class Node : public boost::enable_shared_from_this<Node>
  {
    private:
      typedef std::set<NodePtr> Children;
      typedef std::set<NodeWeakPtr> Parents;
    public:
      typedef typename Children::const_iterator const_children_iterator;
      typedef typename Parents::const_iterator const_parents_iterator;
      typedef typename Children::iterator children_iterator;
      typedef typename Parents::iterator parents_iterator;

    public:
      Node(RootTypePtr probeInstance, const DataType& data = DataType())
      :
      mProbeInstance(probeInstance),
      mData(data)
      {
      }

      const_children_iterator childrenBegin() const
      {
        return this->mChildren.begin();
      }

      const_children_iterator childrenEnd() const
      {
        return this->mChildren.end();
      }

      const_parents_iterator parentsBegin() const
      {
        return this->mParents.begin();
      }

      const_parents_iterator parentsEnd() const
      {
        return this->mParents.end();
      }


      void setData(const DataType& data)
      {
        this->mData = data;
      }

      const DataType& getData() const
      {
        return this->mData;
      }

      DataType& getData()
      {
        return this->mData;
      }

      template <class T>
      NodePtr insert(ConstRootTypePtr probeInstance, const DataType& data, NodePtr correspondingNode = NodePtr())
      {
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
            correspondingNode = child->insert<T>(probeInstance, data, correspondingNode);
            is_child = true;
          }
          else if (child->derivesFrom<T>())
          {
            // in this case, nothing else may be done.
            return this->insertSuperClass<T>(data, correspondingNode);
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


      void print
      (
        const std::string& indentation = "",
        std::set<ConstNodePtr> visited = std::set<ConstNodePtr>()
      ) const
      {
        if (visited.find(this->shared_from_this()) != visited.end())
        {
          std::cout << indentation << " LOOP!" << std::endl;
          return;
        }

        std::cout << indentation << cedar::aux::objectTypeToString(this->mProbeInstance)
                  << " <" << this << ">" << std::endl;
        visited.insert(this->shared_from_this());

        for
        (
          const_children_iterator iter = this->childrenBegin();
          iter != this->childrenEnd();
          ++iter
        )
        {
          (*iter)->print(indentation + " ", visited);
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

        for
        (
          const_children_iterator iter = this->childrenBegin();
          iter != this->childrenEnd();
          ++iter
        )
        {
          (*iter)->findBases(instance, bases);
        }
      }

      ConstNodePtr find(ConstRootTypePtr instance) const
      {
        if (this->matchesExact(instance))
        {
          return this->shared_from_this();
        }
        else
        {
          // check all children
          for (const_children_iterator iter = this->mChildren.begin(); iter != this->mChildren.end(); ++iter)
          {
            ConstNodePtr child = *iter;
            // isDerived can avoid checking all the children; this should save some time.
            if (child->matchesDerived(instance))
            {
              ConstNodePtr node = (*iter)->find(instance);
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

      bool isParent(ConstNodePtr node) const
      {
        NodeWeakPtr weaky(boost::const_pointer_cast<Node>(node));
        return this->mParents.find(weaky) != this->mParents.end();
      }


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

      virtual bool matchesExact(ConstRootTypePtr instance) const = 0;

      // true if instance can be dynamic_cast to the type of the node
      virtual bool matchesDerived(ConstRootTypePtr instance) const = 0;

      // true if the type of the node can be dynamic_cast to the given type
      template <class T>
      bool derivesFrom() const
      {
        return boost::dynamic_pointer_cast<const T>(this->mProbeInstance);
      }

      size_t countConnected() const
      {
        std::set<ConstNodePtr> nodes;
        this->listAllConnectedNodes(nodes);
        return nodes.size();
      }

      size_t childrenSize() const
      {
        return this->mChildren.size();
      }

    private:
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
          if (child->derivesFrom<T>())
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

      void removeParent(NodePtr node)
      {
        parents_iterator iter = this->mParents.find(node);
        if (iter != this->mParents.end())
        {
          this->mParents.erase(iter);
          node->removeChild(this->shared_from_this());
        }
      }

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
      RootTypePtr mProbeInstance;

      DataType mData;

      Children mChildren;

      Parents mParents;
  };

private:
  template <typename T>
  class NodeTemplate : public Node
  {
    public:
      NodeTemplate(const DataType& data = DataType())
      :
      Node(RootTypePtr(new T()), data)
      {
      }

      bool matchesExact(ConstRootTypePtr instance) const
      {
        return typeid(*instance) == typeid(T);
      }

      bool matchesDerived(ConstRootTypePtr instance) const
      {
        return boost::dynamic_pointer_cast<const T>(instance);
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

  //!@brief Destructor

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  template <typename T>
  void insert(const DataType& data)
  {
    RootTypePtr instance(new T());
    this->mRootNode->insert<T>(instance, data);
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

  size_t size() const
  {
    return this->mRootNode->countConnected();
  }

  template <class T>
  ConstNodePtr find() const
  {
    RootTypePtr instance(new T());
    return this->find(instance);
  }

  template <class T>
  NodePtr find()
  {
    RootTypePtr instance(new T());
    return this->find(instance);
  }

  void setRootData(const DataType& data)
  {
    this->mRootNode->setData(data);
  }

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
  NodePtr mRootNode;

}; // class cedar::aux::TypeHierarchyMap

#endif // CEDAR_AUX_TYPE_HIERARCHY_MAP_H

