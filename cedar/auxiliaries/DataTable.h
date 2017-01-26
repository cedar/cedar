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

    File:        DataTable.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 05 10

    Description: Header file for the class cedar::aux::DataTable.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_DATA_TABLE_H
#define CEDAR_AUX_DATA_TABLE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/DataTable.fwd.h"
#include "cedar/auxiliaries/Data.fwd.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/function.hpp>
  #include <boost/bind.hpp>
  #include <boost/enable_shared_from_this.hpp>
  #include <boost/bimap.hpp>
#endif // Q_MOC_RUN
#include <vector>
#include <string>
#include <set>
#include <map>


/*!@brief A class for storing data in an organized way. Mainly intended as storage for trial results.
 *
 * @todo Serialization and deserialization of the data table.
 * @todo For all methods that take string identifiers, also offer corresponding ones that take the integer handle for efficiency
 * @todo Widget for the data table, open plots for mat data stored in each row.
 */
class cedar::aux::DataTable : public boost::enable_shared_from_this<cedar::aux::DataTable>
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  struct IndexMap
  {
    size_t getOrCreateIndex(const std::string& indexName);

    size_t createIndex(const std::string& indexName);

    size_t getIndex(const std::string& indexName) const;

    std::set<std::string> listIndexNames() const;

    boost::bimap<std::string, size_t> map;
  };

public:
  //!@cond SKIPPED_DOCUMENTATION

  CEDAR_DECLARE_AUX_CLASS(RowCollection);
  CEDAR_DECLARE_AUX_CLASS(Row);

  //!@endcond

  /*! A row in the DataTable.
   */
  class Row
  {
    friend class RowCollection;

    private:
      //! Constructor. New rows must be created from the data table.
      Row(DataTablePtr table);

    public:
      //! Sets the value of the given counter.
      void setCounter(const std::string& counterName, int value);

      //! Retrieves the value of the given counter.
      int getCounterValue(const std::string& counterName) const;

      //! Checks if a value for the given counter is present.
      bool hasCounterValue(const std::string& counterName) const;

      //! Sets the value of the given id.
      void setId(const std::string& idName, const std::string& value);

      //! Sets the data of a given data column. Does not copy the data.
      void setData(const std::string& dataColumn, cedar::aux::DataPtr data);

      //! Returns the data for the given data column.
      cedar::aux::ConstDataPtr getData(const std::string& dataColumn) const;

      //! Checks if data for the given column is present.
      bool hasData(const std::string& dataColumn) const;

      //! Checks if a value for the given id is present.
      bool hasId(const std::string& idName) const;

      //! Retrieves the value of the given id.
      const std::string& getId(const std::string& idName) const;

      //! Adds a tag to the row.
      void addTag(const std::string& tag);

      //! Checks if the row has the given tag.
      bool hasTag(const std::string& tag) const;

    private:
      std::map<size_t, int> mCounterValues;

      std::map<size_t, std::string> mIdValues;

      std::vector<std::string> mTags;

      std::map<size_t, cedar::aux::DataPtr> mData;

      DataTablePtr mTable;
  };

  //! A collection of rows
  class RowCollection
  {
  public:
    //! Creates a new row collection for the given table.
    RowCollection(cedar::aux::DataTablePtr table);

    /*! Returns all rows for which the method passed as an argument returns true.
     */
    ConstRowCollectionPtr select(const boost::function<bool(ConstRowPtr)>& selector) const;

    /*! Returns all rows that have the given tag.
     */
    ConstRowCollectionPtr selectByTag(const std::string& tag) const;

    /*! Returns all rows that have the given value for the given counter
     */
    ConstRowCollectionPtr selectByCounter(const std::string& counter, int value) const;

    /*! Returns all rows that have the given value for the given counter
    */
    ConstRowCollectionPtr selectById(const std::string& id, const std::string& idValue) const;

    /*! Runs the given function for all rows in this collection.
     */
    void forAll(const boost::function<void(ConstRowPtr)>& valueFunction) const;

    /*! Runs the given function for all rows in this collection and returns the average of the results.
     */
    double average(const boost::function<double(ConstRowPtr)>& valueFunction) const;

    /*! Runs the given function for all rows in this collection and returns the sum of the return values.
    */
    double sum(const boost::function<double(ConstRowPtr)>& valueFunction) const;

    //! Appends a new row to the collection and returns the row.
    RowPtr appendNewRow();

    //! Appends a given row to the collection.
    void appendRow(RowPtr);

    /*! Appends the rows from another collection to this one.
     *
     * @remarks No data is copied.
     */
    void append(ConstRowCollectionPtr other);

    //! Returns the size of the collection.
    size_t size() const;

    //! Returns an iterator to the beginning of the rows in the collection.
    std::vector<RowPtr>::const_iterator begin() const
    {
      return this->mRows.begin();
    }

    //! Returns an iterator to the end of the rows in the collection.
    std::vector<RowPtr>::const_iterator end() const
    {
      return this->mRows.end();
    }

    //! Returns the data table to which this collection belongs.
    cedar::aux::ConstDataTablePtr getTable() const;

  private:
    std::vector<RowPtr> mRows;

    cedar::aux::DataTableWeakPtr mDataTable;
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  DataTable();
  
  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! get the table
  RowCollectionPtr getTable();

  /*! Creates a counter, i.e., a row containing signed integer numbers.
   */
  size_t createCounter(const std::string& counterName);

  /*! Creates an id, i.e., a row containing a unique string identifier.
  */
  size_t createId(const std::string& idName);

  /*! Creates a column for storing data.
   */
  size_t createDataColumn(const std::string& dataColumn);

  //! Returns the id for a given index. If none exists yet, a new one is created.
  size_t getOrCreateIdIndex(const std::string& idName);

  //! Returns the id for a given counter. If none exists yet, a new one is created.
  size_t getOrCreateCounterIndex(const std::string& counterName);

  //! Returns the id for a given data column. If none exists yet, a new one is created.
  size_t getOrCreateDataColumnIndex(const std::string& dataColumn);

  //! Returns the id for a given index.
  size_t getIdIndex(const std::string& idName) const;

  //! Returns the id for a given counter.
  size_t getCounterIndex(const std::string& counterName) const;

  //! Returns the id for a given data column.
  size_t getDataColumnIndex(const std::string& dataColumn) const;

  //! Returns the list of all available id names.
  std::set<std::string> listIds() const;

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
  RowCollectionPtr mTable;

  IndexMap mIdMap;

  IndexMap mCounterMap;

  IndexMap mDataMap;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::aux::DataTable

#endif // CEDAR_AUX_DATA_TABLE_H

