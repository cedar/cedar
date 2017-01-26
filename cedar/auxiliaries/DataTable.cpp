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

    File:        DataTable.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 05 10

    Description: Source file for the class cedar::aux::DataTable.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/DataTable.h"
#include "cedar/auxiliaries/assert.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::DataTable::DataTable()
{
}

cedar::aux::DataTable::RowCollection::RowCollection(cedar::aux::DataTablePtr table)
:
mDataTable(table)
{
}

cedar::aux::DataTable::Row::Row(DataTablePtr table)
:
mTable(table)
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::DataTable::Row::addTag(const std::string& tag)
{
  this->mTags.push_back(tag);
}

bool cedar::aux::DataTable::Row::hasTag(const std::string& tag) const
{
  auto iter = std::find(this->mTags.begin(), this->mTags.end(), tag);
  return iter != this->mTags.end();
}

void cedar::aux::DataTable::Row::setCounter(const std::string& counterName, int value)
{
  size_t index = this->mTable->getOrCreateCounterIndex(counterName);
  this->mCounterValues[index] = value;
}

int cedar::aux::DataTable::Row::getCounterValue(const std::string& counterName) const
{
  auto index = this->mTable->getCounterIndex(counterName);
  auto iter = this->mCounterValues.find(index);
  CEDAR_ASSERT(iter != this->mCounterValues.end());
  return iter->second;
}

bool cedar::aux::DataTable::Row::hasCounterValue(const std::string& counterName) const
{
  auto index = this->mTable->getCounterIndex(counterName);
  return this->mCounterValues.find(index) != this->mCounterValues.end();
}

void cedar::aux::DataTable::Row::setId(const std::string& idName, const std::string& value)
{
  size_t index = this->mTable->getOrCreateIdIndex(idName);
  this->mIdValues[index] = value;
}

const std::string& cedar::aux::DataTable::Row::getId(const std::string& idName) const
{
  auto index = this->mTable->getIdIndex(idName);
  auto iter = this->mIdValues.find(index);
  CEDAR_ASSERT(iter != this->mIdValues.end());
  return iter->second;
}

bool cedar::aux::DataTable::Row::hasId(const std::string& idName) const
{
  auto index = this->mTable->getIdIndex(idName);
  auto iter = this->mIdValues.find(index);
  return iter != this->mIdValues.end();
}

void cedar::aux::DataTable::Row::setData(const std::string& dataColumn, cedar::aux::DataPtr data)
{
  size_t index = this->mTable->getOrCreateDataColumnIndex(dataColumn);
  this->mData[index] = data;
}

cedar::aux::ConstDataPtr cedar::aux::DataTable::Row::getData(const std::string& dataColumn) const
{
  size_t index = this->mTable->getDataColumnIndex(dataColumn);
  auto iter = this->mData.find(index);
  CEDAR_ASSERT(iter != this->mData.end());
  return iter->second;
}

bool cedar::aux::DataTable::Row::hasData(const std::string& dataColumn) const
{
  size_t index = this->mTable->getDataColumnIndex(dataColumn);
  auto iter = this->mData.find(index);
  return iter != this->mData.end();
}


cedar::aux::ConstDataTablePtr cedar::aux::DataTable::RowCollection::getTable() const
{
  auto table = this->mDataTable.lock();
  CEDAR_ASSERT(table);
  return table;
}

cedar::aux::DataTable::RowCollectionPtr cedar::aux::DataTable::getTable()
{
  if (!this->mTable)
  {
    this->mTable = cedar::aux::DataTable::RowCollectionPtr(new cedar::aux::DataTable::RowCollection(this->shared_from_this()));
  }
  return this->mTable;
}

size_t cedar::aux::DataTable::RowCollection::size() const
{
  return this->mRows.size();
}

cedar::aux::DataTable::RowPtr cedar::aux::DataTable::RowCollection::appendNewRow()
{
  auto table = this->mDataTable.lock();
  CEDAR_ASSERT(table);
  RowPtr row(new Row(table));
  this->appendRow(row);
  return row;
}

void cedar::aux::DataTable::RowCollection::appendRow(RowPtr row)
{
  this->mRows.push_back(row);
}

void cedar::aux::DataTable::RowCollection::append(ConstRowCollectionPtr other)
{
  // both collections must be from the same table
  CEDAR_ASSERT(other->mDataTable.lock() == this->mDataTable.lock());

  for (auto row : other->mRows)
  {
    this->appendRow(row);
  }
}

size_t cedar::aux::DataTable::IndexMap::createIndex(const std::string& indexName)
{
  CEDAR_ASSERT(!indexName.empty());
  size_t id = this->map.size();
  this->map.insert(boost::bimap<std::string, size_t>::value_type(indexName, id));
  return id;
}

size_t cedar::aux::DataTable::IndexMap::getOrCreateIndex(const std::string& indexName)
{
  auto iter = this->map.left.find(indexName);
  if (iter == this->map.left.end())
  {
    return this->createIndex(indexName);
  }
  else
  {
    return iter->second;
  }
}

size_t cedar::aux::DataTable::IndexMap::getIndex(const std::string& indexName) const
{
  auto iter = this->map.left.find(indexName);
  if (iter == this->map.left.end())
  {
    CEDAR_THROW(cedar::aux::UnknownNameException, "The index for identifier \"" + indexName + "\" was not found in this table.");
  }
  return iter->second;
}

std::set<std::string> cedar::aux::DataTable::IndexMap::listIndexNames() const
{
  std::set<std::string> ids;

  for (const auto& name_id_pair : this->map.left)
  {
    ids.insert(name_id_pair.first);
  }

  return ids;
}

size_t cedar::aux::DataTable::createCounter(const std::string& counterName)
{
  return this->mCounterMap.createIndex(counterName);
}

size_t cedar::aux::DataTable::createId(const std::string& idName)
{
  return this->mIdMap.createIndex(idName);
}

size_t cedar::aux::DataTable::createDataColumn(const std::string& dataColumn)
{
  return this->mDataMap.createIndex(dataColumn);
}

size_t cedar::aux::DataTable::getOrCreateIdIndex(const std::string& idName)
{
  return this->mIdMap.getOrCreateIndex(idName);
}

size_t cedar::aux::DataTable::getOrCreateCounterIndex(const std::string& counterName)
{
  return this->mCounterMap.getOrCreateIndex(counterName);
}

size_t cedar::aux::DataTable::getOrCreateDataColumnIndex(const std::string& dataColumn)
{
  return this->mDataMap.getOrCreateIndex(dataColumn);
}

size_t cedar::aux::DataTable::getIdIndex(const std::string& idName) const
{
  return this->mIdMap.getIndex(idName);
}

size_t cedar::aux::DataTable::getCounterIndex(const std::string& counterName) const
{
  return this->mCounterMap.getIndex(counterName);
}

size_t cedar::aux::DataTable::getDataColumnIndex(const std::string& dataColumn) const
{
  return this->mDataMap.getIndex(dataColumn);
}

cedar::aux::DataTable::ConstRowCollectionPtr cedar::aux::DataTable::RowCollection::select(const boost::function<bool(ConstRowPtr)>& selector) const
{
  auto table = this->mDataTable.lock();
  CEDAR_ASSERT(table);
  RowCollectionPtr result(new RowCollection(table));

  for (auto row : this->mRows)
  {
    if (selector(row))
    {
      result->appendRow(row);
    }
  }

  return result;
}

/*! Returns all rows that have the given tag.
*/
cedar::aux::DataTable::ConstRowCollectionPtr cedar::aux::DataTable::RowCollection::selectByTag(const std::string& tag) const
{
  auto selector = [](cedar::aux::DataTable::ConstRowPtr row, const std::string& tag)
  {
    return row->hasTag(tag);
  };
  return this->select(boost::bind<bool>(selector, _1, tag));
}

cedar::aux::DataTable::ConstRowCollectionPtr cedar::aux::DataTable::RowCollection::selectByCounter(const std::string& counter, int value) const
{
  auto selector = [](cedar::aux::DataTable::ConstRowPtr row, const std::string& counter, int match_value)
  {
    if (!row->hasCounterValue(counter))
    {
      return false;
    }
    else
    {
      return row->getCounterValue(counter) == match_value;
    }
  };
  return this->select(boost::bind<bool>(selector, _1, counter, value));
}

cedar::aux::DataTable::ConstRowCollectionPtr cedar::aux::DataTable::RowCollection::selectById(const std::string& id, const std::string& value) const
{
  auto selector = [](cedar::aux::DataTable::ConstRowPtr row, const std::string& id, const std::string& match_value)
  {
    if (!row->hasId(id))
    {
      return false;
    }
    else
    {
      return row->getId(id) == match_value;
    }
  };
  return this->select(boost::bind<bool>(selector, _1, id, value));
}

void cedar::aux::DataTable::RowCollection::forAll(const boost::function<void(ConstRowPtr)>& valueFunction) const
{
  for (auto row : this->mRows)
  {
    valueFunction(row);
  }
}

double cedar::aux::DataTable::RowCollection::sum(const boost::function<double(ConstRowPtr)>& valueFunction) const
{
  double sum = 0.0;

  for (auto row : this->mRows)
  {
    sum += valueFunction(row);
  }

  return sum;
}

double cedar::aux::DataTable::RowCollection::average(const boost::function<double(ConstRowPtr)>& valueFunction) const
{
  double sum = this->sum(valueFunction);
  return sum / this->mRows.size();
}

std::set<std::string> cedar::aux::DataTable::listIds() const
{
  return this->mIdMap.listIndexNames();
}
