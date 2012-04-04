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

    File:        main.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 03 21

    Description: 

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/TypeHierarchyMap.h"
#include "cedar/auxiliaries/stringFunctions.h"

// SYSTEM INCLUDES

// some sample classes to test the map.
class A
{
  public:
    virtual ~A()
    {
    }
};

// naming scheme for the classes: first letter is the first base, second letter the second one and so on.

class B : virtual public A
{
};

class C : virtual public A
{
};

class CA : public C
{
};

class CB : public C
{
};


class BA : public B
{
};

class BB : public B
{
};

class D : virtual public B, public C
{
};

class E : virtual public B, public CA
{
};

class F : public E, public D
{
};

typedef cedar::aux::TypeHierarchyMap<A, std::string> AHierarchy;


void printNodePath(AHierarchy::ConstNodePtr node, const std::string& indentation = "")
{
  std::cout << indentation << node->getData() << " <" << node.get() << ">" << std::endl;
  for
  (
    AHierarchy::Node::const_parents_iterator iter = node->parentsBegin();
    iter != node->parentsEnd();
    ++iter
  )
  {
    if ((*iter).lock() == node)
    {
      std::cout << indentation << " LOOP!" << std::endl;
    }
    else
    {
      printNodePath((*iter).lock(), indentation + " ");
    }
  }
}

void printNodeWithChildren
(
  AHierarchy::ConstNodePtr node,
  const std::string& indentation = "",
  std::set<AHierarchy::ConstNodePtr> visited = std::set<AHierarchy::ConstNodePtr>()
)
{
  if (visited.find(node) != visited.end())
  {
    std::cout << indentation << " LOOP!" << std::endl;
    return;
  }

  std::cout << indentation << node->getData() << " <" << node.get() << ">" << std::endl;
  visited.insert(node);

  for
  (
    AHierarchy::Node::const_children_iterator iter = node->childrenBegin();
    iter != node->childrenEnd();
    ++iter
  )
  {
    printNodeWithChildren(*iter, indentation + " ", visited);
  }
}

#define FIND_NODE_CHECKED(classId) \
    AHierarchy::ConstNodePtr node_ ## classId; \
    try \
    { \
      node_ ## classId = hierarchy.find<classId>(); \
    } \
    catch (cedar::aux::UnknownTypeException&) \
    {} \
    if (!node_ ## classId) \
    { \
      std::cout << "ERROR: Could not get node for " #classId "." << std::endl; \
      ++errors; \
      return errors; \
    }

#define CHECK_ANCESTOR(classIdA, classIdB) \
    if (!node_ ## classIdA ->isAncestor(node_ ## classIdB)) \
    { \
      std::cout << "ERROR: class " #classIdA " is not an ancestor of class " #classIdB << std::endl; \
      ++errors; \
    }

#define CHECK_NODE_CHILDREN_SIZE(classId, size) \
{ \
  if (node_ ## classId -> childrenSize() != size) \
  { \
    std::cout << "ERROR: class node " #classId " has the wrong number of children" \
       " has: " << node_ ## classId -> childrenSize() << ", should have " #size << std::endl;\
    ++errors; \
  } \
}

int checkHierarchy(const AHierarchy& hierarchy)
{
  int errors = 0;

  if (hierarchy.size() != 10)
  {
    std::cout << "Tree has the wrong size: should be 9, is " << hierarchy.size() << std::endl;
    ++errors;
  }

  std::cout << "Tree after inserting:" << std::endl;
  printNodeWithChildren(hierarchy.getRootNode());

  FIND_NODE_CHECKED(A);
  FIND_NODE_CHECKED(B);
  FIND_NODE_CHECKED(C);
  FIND_NODE_CHECKED(BA);
  FIND_NODE_CHECKED(BB);
  FIND_NODE_CHECKED(CA);
  FIND_NODE_CHECKED(CB);
  FIND_NODE_CHECKED(D);
  FIND_NODE_CHECKED(E);
  FIND_NODE_CHECKED(F);

  CHECK_NODE_CHILDREN_SIZE(A, 2);
  CHECK_NODE_CHILDREN_SIZE(B, 4);
  CHECK_NODE_CHILDREN_SIZE(C, 3);


  CHECK_ANCESTOR(B, A);
  CHECK_ANCESTOR(C, A);
  CHECK_ANCESTOR(BA, B);
  CHECK_ANCESTOR(BB, B);
  CHECK_ANCESTOR(BA, A);
  CHECK_ANCESTOR(BB, A);
  CHECK_ANCESTOR(CA, C);
  CHECK_ANCESTOR(CB, C);
  CHECK_ANCESTOR(CA, A);
  CHECK_ANCESTOR(CB, A);
  CHECK_ANCESTOR(D, A);
  CHECK_ANCESTOR(D, B);
  CHECK_ANCESTOR(D, C);
  CHECK_ANCESTOR(E, B);
  CHECK_ANCESTOR(E, CA);
  CHECK_ANCESTOR(F, E);
  CHECK_ANCESTOR(F, D);

  std::cout << "node path for D: " << std::endl;
  printNodePath(node_D);

  std::cout << "node path for E: " << std::endl;
  printNodePath(node_E);

  return errors;
}

int main()
{
  // the number of errors encountered in this test
  int errors = 0;

  {
    std::cout << ">>>>>>>>>>>>>> Creating hierarchy." << std::endl;
    AHierarchy hierarchy("data_A");

    std::cout << "Inserting data." << std::endl;
    hierarchy.insert<B>("data_B");
    hierarchy.insert<C>("data_C");
    hierarchy.insert<BB>("data_BB");
    hierarchy.insert<BA>("data_BA");
    hierarchy.insert<CA>("data_CA");
    hierarchy.insert<CB>("data_CB");
    hierarchy.insert<D>("data_D");
    hierarchy.insert<E>("data_E");
    std::cout << "Inserting F" << std::endl;
    hierarchy.insert<F>("data_F");

    errors += checkHierarchy(hierarchy);
  }

  {
    std::cout << ">>>>>>>>>>>>>> Creating hierarchy differently." << std::endl;
    AHierarchy hierarchy("data_A");

    std::cout << "Inserting data." << std::endl;
    hierarchy.insert<F>("data_F");
    hierarchy.insert<CA>("data_CA");
    hierarchy.insert<BB>("data_BB");
    hierarchy.insert<E>("data_E");
    hierarchy.insert<C>("data_C");
    hierarchy.insert<BA>("data_BA");
    hierarchy.insert<B>("data_B");
    hierarchy.insert<CB>("data_CB");
    std::cout << "Inserting D" << std::endl;
    hierarchy.insert<D>("data_D");

    errors += checkHierarchy(hierarchy);
  }

  {
    std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Creating hierarchy even more differently. <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
    AHierarchy hierarchy("data_A");

    std::cout << "########### Inserting data_BB." << std::endl;
    hierarchy.insert<BB>("data_BB");
    printNodeWithChildren(hierarchy.getRootNode());
    std::cout << "########### Inserting data_E." << std::endl;
    hierarchy.insert<E>("data_E");
    printNodeWithChildren(hierarchy.getRootNode());
    std::cout << "########### Inserting data_C." << std::endl;
    hierarchy.insert<C>("data_C");
    printNodeWithChildren(hierarchy.getRootNode());
    std::cout << "########### Inserting data_BA." << std::endl;
    hierarchy.insert<BA>("data_BA");
    printNodeWithChildren(hierarchy.getRootNode());
    std::cout << "########### Inserting data_F." << std::endl;
    hierarchy.insert<F>("data_F");
    printNodeWithChildren(hierarchy.getRootNode());
    std::cout << "########### Inserting data_B." << std::endl;
    hierarchy.insert<B>("data_B");
    printNodeWithChildren(hierarchy.getRootNode());
    std::cout << "########### Inserting data_CB." << std::endl;
    hierarchy.insert<CB>("data_CB");
    printNodeWithChildren(hierarchy.getRootNode());
    std::cout << "########### Inserting data_D." << std::endl;
    hierarchy.insert<D>("data_D");
    printNodeWithChildren(hierarchy.getRootNode());
    std::cout << "########### Inserting data_CA." << std::endl;
    hierarchy.insert<CA>("data_CA");
    printNodeWithChildren(hierarchy.getRootNode());

    errors += checkHierarchy(hierarchy);
  }

  std::cout << "Test completed, there were " << errors << " errors." << std::endl;

  return errors;
}
