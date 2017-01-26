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

    File:        main.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 03 21

    Description: This test can be run with multiple (command line) options:
                 --full    The full set of possible insert orders is tested. This takes a long time, therefore, this is
                           disabled by default.

                 --strict  The test doesn't only check for correct mapping of the inheritance hieararchy, but also
                           checks for redundant entries in the tree. This currently fails, however, this may be an
                           unfixable problem that exceeds the limitations of what can be done.

                 --stop    Halt test execution on the first error.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/TypeHierarchyMap.h"
#include "cedar/auxiliaries/stringFunctions.h"

// SYSTEM INCLUDES

// global variables
// using strict mode checks additional conditions; however, at this time it seems that the strict test can never
// complete successfully in all cases.
bool strict = false;

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

/*
 * What the hierarchy should look like:
 *
 *           / BA
 *      _ B < -------
 *     /   \ \ BB   |
 * A -<     \__D----+----- F
 *     \     /      -- E -/
 *      \_  /  CA _/
 *         C <
 *             CB
 *
 * Or, in this test's print syntax:
 * A
 *   B
 *     BA
 *     BB
 *     E
 *       F
 *     D
 *       F
 *   C
 *     CA
 *       E
 *         F
 *     CB
 *     D
 *       F
 */
int checkHierarchy(const AHierarchy& hierarchy)
{
  int errors = 0;

  if (hierarchy.size() != 10)
  {
    std::cout << "Tree has the wrong size: should be 9, is " << hierarchy.size() << std::endl;
    ++errors;
  }

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

  if (strict)
  {
    CHECK_NODE_CHILDREN_SIZE(A, 2);
    CHECK_NODE_CHILDREN_SIZE(B, 4);
    CHECK_NODE_CHILDREN_SIZE(C, 3);
    CHECK_NODE_CHILDREN_SIZE(BA, 0);
    CHECK_NODE_CHILDREN_SIZE(BB, 0);
    CHECK_NODE_CHILDREN_SIZE(CA, 1);
    CHECK_NODE_CHILDREN_SIZE(CB, 0);
    CHECK_NODE_CHILDREN_SIZE(D, 1);
    CHECK_NODE_CHILDREN_SIZE(E, 1);
    CHECK_NODE_CHILDREN_SIZE(F, 0);
  }

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

  return errors;
}


// helper classes for inserting data
class Helper
{
  public:
    virtual ~Helper(){};
    virtual void insertInto(AHierarchy& hierarchy) = 0;

    virtual std::string toString() const = 0;
};

CEDAR_GENERATE_POINTER_TYPES(Helper);

template <class T>
class HelperTemplate : public Helper
{
  public:
    virtual ~HelperTemplate(){};
    void insertInto(AHierarchy& hierarchy)
    {
      std::string data_name = "data_" + cedar::aux::typeToString<T>();
      hierarchy.insert<T>(data_name);
    }

    std::string toString() const
    {
      return cedar::aux::typeToString<T>();
    }
};


void permutate
(
  std::vector<std::vector<HelperPtr> >& orders,
  std::set<HelperPtr> toInsert,
  std::vector<HelperPtr> path = std::vector<HelperPtr>())
{
  if (toInsert.empty())
  {
    orders.push_back(path);
  }

  std::vector<HelperPtr> path_plus_one;
  path_plus_one.insert(path_plus_one.begin(), path.begin(), path.end());
  path_plus_one.push_back(HelperPtr());
  for (std::set<HelperPtr>::iterator iter = toInsert.begin(); iter != toInsert.end(); ++iter)
  {
    std::set<HelperPtr> subset;
    subset.insert(toInsert.begin(), toInsert.end());
    subset.erase(subset.find(*iter));
    path_plus_one.back() = *iter;
    permutate(orders, subset, path_plus_one);
  }
}


int checkHierarchy(const std::vector<HelperPtr>& insertOrder)
{
  AHierarchy hierarchy("data_A");

  for (size_t i = 0; i < insertOrder.size(); ++i)
  {
    insertOrder.at(i)->insertInto(hierarchy);
  }

  int errors = checkHierarchy(hierarchy);

  if (errors > 0)
  {
    std::cout << " ========================================================================== " << std::endl;
    std::cout << "  Error with insert order:";

    for (size_t i = 0; i < insertOrder.size(); ++i)
    {
      std::cout << " " << insertOrder.at(i)->toString();
    }
    std::cout << std::endl
        << " ========================================================================== " << std::endl;

    std::cout << std::endl << "Hierarchy:" << std::endl;
    printNodeWithChildren(hierarchy.getRootNode());
  }

  return errors;
}

int main(int argc, char** argv)
{
  // the number of errors encountered in this test
  int errors = 0;

  // number of tests to perform when not in full mode
  size_t num_tests_nonfull = 2500;

  std::cout << "Constructing insert orders ... ";
  std::cout.flush();
  // whether the test should stop on the first error.
  bool stop_on_first_error = false;
  bool full = false;
  for (int arg = 1; arg < argc; ++arg)
  {
    if (std::string(argv[arg]) == "--stop")
      stop_on_first_error = true;
    else if (std::string(argv[arg]) == "--full")
      full = true;
    else if (std::string(argv[arg]) == "--strict")
      strict = true;
  }

  // shuffle and create all possible orders for insertion of the hierarchy
  std::vector<std::vector<HelperPtr> > insert_orders;

  std::set<HelperPtr> classes;
  classes.insert(HelperPtr(new HelperTemplate<B>()));
  classes.insert(HelperPtr(new HelperTemplate<BA>()));
  classes.insert(HelperPtr(new HelperTemplate<BB>()));
  classes.insert(HelperPtr(new HelperTemplate<C>()));
  classes.insert(HelperPtr(new HelperTemplate<CA>()));
  classes.insert(HelperPtr(new HelperTemplate<CB>()));
  classes.insert(HelperPtr(new HelperTemplate<D>()));
  classes.insert(HelperPtr(new HelperTemplate<E>()));
  classes.insert(HelperPtr(new HelperTemplate<F>()));

  permutate(insert_orders, classes);

  std::cout << "done." << std::endl;

  if (full)
  {
    std::cout << "Testing " << insert_orders.size() << " randomly chosen orders." << std::endl;

    for (size_t i = 0; i < insert_orders.size(); ++i)
    {
      errors += checkHierarchy(insert_orders.at(i));

      if (i % num_tests_nonfull == 0)
      {
        std::cout << "\rDone with test no. " << i;
        std::cout.flush();
      }

      if (stop_on_first_error && errors > 0)
      {
        std::cout << "Stopping on test no. " << i << " with " << errors << " error(s)." << std::endl;
        return errors;
      }
    }

    std::cout << std::endl;
  }
  else
  {
    std::cout << "Testing " << num_tests_nonfull << " orders." << std::endl;

    std::vector<size_t> indices;

    for (size_t i = 0; i < insert_orders.size(); ++i)
    {
      indices.push_back(i);
    }

    for (size_t test_num = 0; test_num < num_tests_nonfull; ++test_num)
    {
      size_t rnd_index = rand() % indices.size();
      CEDAR_DEBUG_ASSERT(rnd_index < insert_orders.size());
      size_t test_index = indices.at(rnd_index);
      indices.erase(indices.begin() + rnd_index);

      errors += checkHierarchy(insert_orders.at(test_index));

      if (stop_on_first_error && errors > 0)
      {
        std::cout << "Stopping on test no. " << test_num << " with " << errors << " error(s)." << std::endl;
        return errors;
      }
    }
  }

  std::cout << "Test completed, there were " << errors << " error(s)." << std::endl;

  return errors;
}
