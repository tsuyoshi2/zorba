/*
 * Copyright 2006-2016 zorba.io
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "json_items.h"
#include "simple_item_factory.h"
#include "simple_store.h"
#include "item_iterator.h"

#include "store/api/copymode.h"

#include "diagnostics/assert.h"
#include "diagnostics/util_macros.h"
#include "util/mem_sizeof.h"

namespace zorba
{

namespace simplestore
{

namespace json
{

/******************************************************************************

*******************************************************************************/
store::Item* JSONNull::getType() const
{
  return GET_STORE().JS_NULL_QNAME;
}


/******************************************************************************

*******************************************************************************/
bool JSONNull::equals(
    const store::Item* other,
    long /* timezone */,
    const XQPCollator* /* collation */) const
{
  return other->getTypeCode() == store::JS_NULL;
}


/******************************************************************************

*******************************************************************************/
uint32_t JSONNull::hash(long /* tmz */, const XQPCollator* /* collation */) const
{
  const void* tmp = this; // there is only one instance in the store
  return hashfun::h32(&tmp, sizeof(void*), FNV_32_INIT);
}


/******************************************************************************

*******************************************************************************/
void JSONNull::getTypedValue(store::Item_t& val, store::Iterator_t& iter) const
{
  iter = NULL;
  val = this;
}


/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
//  JSON Item                                                                  //
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////

/******************************************************************************

*******************************************************************************/
JSONItem::~JSONItem()
{
  delete theCollectionInfo;
}


/******************************************************************************

*******************************************************************************/
void JSONItem::free()
{
  destroy();
}


/******************************************************************************

*******************************************************************************/
void JSONItem::destroy()
{
  GET_STORE().unregisterReferenceToUnusedNode(this);
  delete this;
}


/*******************************************************************************

********************************************************************************/
long JSONItem::getCollectionTreeRefCount() const
{
  return getRefCount();
}

/*******************************************************************************

********************************************************************************/
void JSONItem::attachToCollection(
    Collection* aCollection,
    const TreeId& aTreeId,
    const xs_integer& aPosition)
{
  ASSERT_INVARIANT();

  assert(aCollection);

  // Attach
  assert(theCollectionInfo == NULL);
  CollectionTreeInfoWithTreeId* collectionInfo = new CollectionTreeInfoWithTreeId();
  collectionInfo->setCollection(aCollection);
  collectionInfo->setTreeId(aTreeId);
  collectionInfo->setPosition(aPosition);
  collectionInfo->setRoot(this);
  setCollectionTreeInfo(collectionInfo);
  
  ASSERT_INVARIANT();
}

/*******************************************************************************

********************************************************************************/
void JSONItem::detachFromCollection()
{
  ASSERT_INVARIANT();
  
  
  CollectionTreeInfo* collectionInfo = theCollectionInfo;
  // Detach
  assert(collectionInfo);
  setCollectionTreeInfo(NULL);
  delete collectionInfo;

  ASSERT_INVARIANT();
}

#ifndef NDEBUG
/******************************************************************************

*******************************************************************************/
void JSONItem::assertInvariant() const
{
  if (theCollectionInfo != NULL)
  {
    assert(theCollectionInfo->getCollection() != NULL);
    assert(theCollectionInfo->getRoot() != NULL);
    assert(isThisTreeOfAllDescendants(theCollectionInfo));
    assert(theCollectionInfo->getRoot()->isInSubtree(this));
  }
}
#endif


/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
//  Object                                                                     //
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////


/******************************************************************************

*******************************************************************************/
store::Item* JSONObject::getType() const
{
  return GET_STORE().JS_OBJECT_QNAME;
}


/******************************************************************************

*******************************************************************************/
SimpleJSONObject::~SimpleJSONObject()
{
  ASSERT_INVARIANT();
  for (Pairs::iterator lIter = thePairs.begin();
       lIter != thePairs.end();
       ++lIter)
  {
    store::Item* lName = lIter->first;
    store::Item* lChild = lIter->second;
    if (getCollection() != NULL && lChild->isStructuredItem())
    {
      assert(dynamic_cast<StructuredItem*>(lChild));
      StructuredItem* lStructuredItem = static_cast<StructuredItem*>(lChild);
      lStructuredItem->setCollectionTreeInfo(NULL);
    }
    lName->removeReference();
    lChild->removeReference();
  }
  theKeys.clear();
  thePairs.clear();
  ASSERT_INVARIANT();
}


/******************************************************************************

*******************************************************************************/

size_t SimpleJSONObject::alloc_size() const
{
  return  ztd::alloc_sizeof( theKeys )
        + ztd::alloc_sizeof( thePairs );
}

size_t SimpleJSONObject::dynamic_size() const
{
  return sizeof( *this );
}

/******************************************************************************

*******************************************************************************/
store::Item* SimpleJSONObject::copy(
    store::Item* parent,
    const store::CopyMode& copymode) const
{
  ASSERT_INVARIANT();
  SimpleJSONObject* lNewObject = const_cast<SimpleJSONObject*>(this);
 
 if (copymode.theDoCopy)
  {
    lNewObject = new SimpleJSONObject();

    for (Pairs::const_iterator lIter = thePairs.begin();
         lIter != thePairs.end();
         ++lIter)
    {
      store::Item_t lKey = lIter->first;
      store::Item_t lValue = lIter->second;
      
      if (lValue->isStructuredItem())
      {
        store::Item_t lCopiedValue = lValue->copy(NULL, copymode);
        lNewObject->add(lKey, lCopiedValue, false);
      }
      else
      {
        lNewObject->add(lKey, lValue, false);
      }
    }
  }

  if (parent)
  {
    assert(parent->isArray());
    assert(dynamic_cast<JSONArray*>(parent));
    JSONArray* a = static_cast<JSONArray*>(parent);

    a->push_back(lNewObject);
  }
  
  return lNewObject;
}


/******************************************************************************

*******************************************************************************/
bool SimpleJSONObject::add(
    const store::Item_t& aName,
    const store::Item_t& aValue,
    bool accumulate)
{
  ASSERT_INVARIANT();
  zstring zname;
  aName->getStringValue2( zname );
  const char *const lName = zname.c_str();

  Keys::iterator ite = theKeys.find(lName);

  if (ite == theKeys.end())
  {
    store::Item* lValue = aValue.getp();

    if (getCollection() != NULL && (aValue->isStructuredItem()))
    {
      assert( dynamic_cast<StructuredItem*>(lValue) );
      StructuredItem* lStructuredItem = static_cast<StructuredItem*>(aValue.getp());
      lStructuredItem->setCollectionTreeInfo(theCollectionInfo);
    }
    
    theKeys.insert( std::make_pair( lName, thePairs.size() ) );
    thePairs.push_back(std::make_pair(aName.getp(), lValue));
    aName->addReference();
    lValue->addReference();

    ASSERT_INVARIANT();
    return true;
  }
  else if (accumulate)
  {
    size_type lPosition = ite->second;

    assert(thePairs[lPosition].first->getStringValue() == lName);

    store::Item* lValue = thePairs[lPosition].second;

    if (lValue->isArray())
    {
      static_cast<SimpleJSONArray*>(lValue)->push_back(aValue);
    }
    else
    {
      SimpleJSONArray_t array = new SimpleJSONArray();
      array->push_back(lValue);
      array->push_back(aValue);

      if (getCollection() != NULL)
      {
        array->setCollectionTreeInfo(theCollectionInfo);
      }

      lValue->removeReference();
      array->addReference();
      thePairs[lPosition].second = array;
    }
    ASSERT_INVARIANT();
    return true;
  }

  ASSERT_INVARIANT();
  return false;
}


/******************************************************************************

*******************************************************************************/
store::Item_t SimpleJSONObject::remove(const store::Item_t& aName)
{
  ASSERT_INVARIANT();

  zstring zname;
  aName->getStringValue2( zname );
  const char *const lName = zname.c_str();

  Keys::iterator lIter = theKeys.find(lName);
  if (lIter == theKeys.end())
  {
    ASSERT_INVARIANT();
    return NULL;
  }
  size_type lPosition = lIter->second;
  
  store::Item *const lKey = thePairs[lPosition].first;
  store::Item_t lValue( thePairs[lPosition].second );

  if (getCollection() != NULL && (lValue->isStructuredItem()))
  {
    assert(dynamic_cast<StructuredItem*>(lValue.getp()));
    StructuredItem* lStructuredItem = static_cast<StructuredItem*>(lValue.getp());
    lStructuredItem->setCollectionTreeInfo(NULL);
  }

  lKey->removeReference();
  lValue->removeReference();

  thePairs.erase(thePairs.begin() + lPosition);
  theKeys.erase(lIter);

  if (lPosition < thePairs.size())
  {
    Keys::iterator lKeysIte = theKeys.begin();
    Keys::iterator lKeysEnd = theKeys.end();
    for (; lKeysIte != lKeysEnd; ++lKeysIte)
    {
      size_type lPos = lKeysIte->second;
      if (lPos > lPosition)
      {
        lKeysIte->second = lPos - 1;
      }
    }
  }

  ASSERT_INVARIANT();
  return lValue;
}


/******************************************************************************

*******************************************************************************/
store::Item_t SimpleJSONObject::setValue(
    const store::Item_t& aName,
    const store::Item_t& aValue)
{
  ASSERT_INVARIANT();
  zstring zname;
  aName->getStringValue2( zname );
  const char *const lName = zname.c_str();

  Keys::const_iterator lIter = theKeys.find(lName);
  if (lIter == theKeys.end())
  {
    ASSERT_INVARIANT();
    return 0;
  }
  size_type lPosition = lIter->second;

  assert(thePairs[lPosition].first->getStringValue() == lName);

  store::Item_t lOldValue = thePairs[lPosition].second;

  if (getCollection() != NULL)
  {
    if (lOldValue->isStructuredItem())
    {
      assert(dynamic_cast<StructuredItem*>(lOldValue.getp()));
      StructuredItem* lStructuredItem =
          static_cast<StructuredItem*>(lOldValue.getp());
      lStructuredItem->setCollectionTreeInfo(NULL);
    }

    if (aValue->isStructuredItem())
    {
      assert(dynamic_cast<StructuredItem*>(aValue.getp()));
      StructuredItem* lStructuredItem =
          static_cast<StructuredItem*>(aValue.getp());
      lStructuredItem->setCollectionTreeInfo(theCollectionInfo);
    }
  }

  lOldValue->removeReference();
  aValue->addReference();
  thePairs[lPosition].second = aValue.getp();

  ASSERT_INVARIANT();
  return lOldValue;
}


/******************************************************************************

*******************************************************************************/
bool SimpleJSONObject::rename(
    const store::Item_t& aName,
    const store::Item_t& aNewName)
{
  ASSERT_INVARIANT();
  zstring zname, znewname;
  aName->getStringValue2( zname );
  aNewName->getStringValue2( znewname );
  const char *const lName = zname.c_str();
  const char *const lNewName = znewname.c_str();

  Keys::const_iterator lIter = theKeys.find(lNewName);
  if (lIter != theKeys.end())
  {
    ASSERT_INVARIANT();
    return false;
  }

  Keys::iterator ite = theKeys.find(lName);

  if (ite == theKeys.end())
  {
    ASSERT_INVARIANT();
    return false;
  }

  size_type lPosition = ite->second;
  assert(thePairs[lPosition].first->getStringValue() == lName);
  
  thePairs[lPosition].first->removeReference();
  aNewName->addReference();
  thePairs[lPosition].first = aNewName.getp();
  theKeys.erase(ite);
  theKeys.insert(std::make_pair(lNewName, lPosition));

  ASSERT_INVARIANT();
  return true;
}


/******************************************************************************

*******************************************************************************/
void SimpleJSONObject::swap(store::Item* anotherItem)
{
  SimpleJSONObject* lOther = dynamic_cast<SimpleJSONObject*>(anotherItem);
  assert(lOther);
  std::swap(theKeys, lOther->theKeys);
  std::swap(thePairs, lOther->thePairs);
  setCollectionTreeInfo(theCollectionInfo);
  lOther->setCollectionTreeInfo(lOther->theCollectionInfo);
}


/******************************************************************************

*******************************************************************************/
void SimpleJSONObject::setCollectionTreeInfo(CollectionTreeInfo* collectionInfo)
{
  theCollectionInfo = static_cast<CollectionTreeInfoWithTreeId*>(collectionInfo);

  for (Pairs::iterator ite = thePairs.begin();
       ite != thePairs.end();
       ++ite)
  {
    store::Item* value = ite->second;

    if (value->isStructuredItem())
    {
      StructuredItem* structuredItem = static_cast<StructuredItem*>(value);
      structuredItem->setCollectionTreeInfo(collectionInfo);
    }
  }
}


/******************************************************************************

*******************************************************************************/
zstring SimpleJSONObject::getStringValue() const
{
  ASSERT_INVARIANT();
  throw ZORBA_EXCEPTION(jerr::JNTY0024, ERROR_PARAMS("object"));
}


/******************************************************************************

*******************************************************************************/
void SimpleJSONObject::getStringValue2(zstring& val) const
{
  val = getStringValue();
}


/******************************************************************************

*******************************************************************************/
void SimpleJSONObject::appendStringValue(zstring& buf) const
{
  buf = getStringValue();
}


/******************************************************************************

*******************************************************************************/
void SimpleJSONObject::getTypedValue(store::Item_t& val, store::Iterator_t& iter) const
{
  ASSERT_INVARIANT();
  throw ZORBA_EXCEPTION(jerr::JNTY0004, ERROR_PARAMS("object"));
}


/******************************************************************************

*******************************************************************************/
store::Item_t SimpleJSONObject::getObjectValue(const store::Item_t& aKey) const
{
  ASSERT_INVARIANT();
  zstring zname;
  aKey->getStringValue2( zname );
  const char *const lName = zname.c_str();

  Keys::const_iterator lIter = theKeys.find(lName);

  if (lIter == theKeys.end())
  {
    return NULL;
  }
  size_type lPosition = lIter->second;

  assert(thePairs[lPosition].first->getStringValue() == lName);

  return thePairs[lPosition].second;
}


/******************************************************************************

*******************************************************************************/
store::Iterator_t SimpleJSONObject::getObjectKeys() const
{
  ASSERT_INVARIANT();
  return new KeyIterator(const_cast<SimpleJSONObject*>(this));
}


/******************************************************************************

*******************************************************************************/
xs_integer SimpleJSONObject::getNumObjectPairs() const
{
  return xs_integer(thePairs.size());
}


#ifndef NDEBUG

/******************************************************************************

*******************************************************************************/
void SimpleJSONObject::assertInvariant() const
{
  JSONItem::assertInvariant();
  assert(theKeys.size() == thePairs.size());

  for(Keys::const_iterator lIter = theKeys.begin();
      lIter != theKeys.end();
      ++lIter)
  {
    size_type lPosition = lIter->second;
    assert(lPosition < thePairs.size());
    assert(thePairs[lPosition].first != NULL);
    assert(thePairs[lPosition].first->isAtomic());
    assert(thePairs[lPosition].first->getStringValue() == lIter->first);
    assert(thePairs[lPosition].second != NULL);
  }
}


/******************************************************************************

*******************************************************************************/
bool SimpleJSONObject::isThisTreeOfAllDescendants(
    const CollectionTreeInfo* collectionInfo) const
{
  if (theCollectionInfo != collectionInfo)
  {
    return false;
  }

  for (Pairs::const_iterator lIter = thePairs.begin();
       lIter != thePairs.end();
       ++lIter)
  {
    store::Item* lValue = lIter->second;
    const JSONItem* lJSONItem = dynamic_cast<const JSONItem*>(lValue);
    if (lJSONItem != NULL && 
        !lJSONItem->isThisTreeOfAllDescendants(collectionInfo))
    {
      return false;
    }
  }
  return true;
}
#endif // NDEBUG


/******************************************************************************

*******************************************************************************/
bool SimpleJSONObject::isInSubtree(const StructuredItem* anItem) const
{
  if (this == anItem)
  {
    return true;
  }

  for (Pairs::const_iterator lIter = thePairs.begin();
       lIter != thePairs.end();
       ++lIter)
  {
    store::Item* lValue = lIter->second;
    if (lValue->isStructuredItem())
    {
      const StructuredItem* lStructuredItem =
        static_cast<const StructuredItem*>(lValue);
      if (lStructuredItem->isInSubtree(anItem))
      {
        return true;
      }
    }
  }
  return false;
}


/******************************************************************************

*******************************************************************************/
zstring SimpleJSONObject::show() const
{
  std::stringstream str;
  str << "{ }";

  return str.str();
}


/******************************************************************************

*******************************************************************************/
SimpleJSONObject::KeyIterator::~KeyIterator() 
{
}


/******************************************************************************

*******************************************************************************/
void SimpleJSONObject::KeyIterator::open()
{
  theIter = theObject->thePairs.begin();
}


/******************************************************************************

*******************************************************************************/
bool SimpleJSONObject::KeyIterator::next(store::Item_t& res)
{
  if (theIter != theObject->thePairs.end())
  {
    res = theIter->first;
    ++theIter;
    return true;
  }
  else
  {
    return false;
  }
}


/******************************************************************************

*******************************************************************************/
void SimpleJSONObject::KeyIterator::reset()
{
  open();
}


/******************************************************************************

*******************************************************************************/
void SimpleJSONObject::KeyIterator::close()
{
  theIter = theObject->thePairs.end();
}



/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
//  Array                                                                      //
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////


/******************************************************************************

*******************************************************************************/
store::Item* JSONArray::getType() const
{
  return GET_STORE().JS_ARRAY_QNAME;
}


/******************************************************************************

*******************************************************************************/
SimpleJSONArray::~SimpleJSONArray()
{
  ASSERT_INVARIANT();
  for (Members::const_iterator lIter = theContent.begin();
       lIter != theContent.end();
       ++lIter)
  {
    if (getCollection() != NULL &&
        ((*lIter)->isStructuredItem()))
    {
      assert(dynamic_cast<StructuredItem*>(*lIter));
      StructuredItem* lStructuredItem =
          static_cast<StructuredItem*>(*lIter);
      lStructuredItem->setCollectionTreeInfo(NULL);
    }
    (*lIter)->removeReference();
  }
}


/******************************************************************************

*******************************************************************************/

size_t SimpleJSONArray::alloc_size() const
{
  return ztd::alloc_sizeof( theContent );
}

size_t SimpleJSONArray::dynamic_size() const
{
  return sizeof( *this );
}

/******************************************************************************

*******************************************************************************/
void SimpleJSONArray::push_back(const store::Item_t& aValue)
{
  ASSERT_INVARIANT();

  if (getCollection() != NULL && (aValue->isStructuredItem()))
  {
    assert(dynamic_cast<StructuredItem*>(aValue.getp()));
    StructuredItem* lStructuredItem =
        static_cast<StructuredItem*>(aValue.getp());
    lStructuredItem->setCollectionTreeInfo(theCollectionInfo);
  }

  aValue->addReference();
  theContent.push_back(aValue.getp());

  ASSERT_INVARIANT();
}


/******************************************************************************

*******************************************************************************/
void SimpleJSONArray::push_back(const std::vector<store::Item_t>& members)
{
  ASSERT_INVARIANT();
  theContent.reserve(theContent.size() + members.size());
  add(theContent.size(), members);
  ASSERT_INVARIANT();
}


/******************************************************************************

*******************************************************************************/
void SimpleJSONArray::push_front(const std::vector<store::Item_t>& members)
{
  ASSERT_INVARIANT();
  theContent.reserve(theContent.size() + members.size());
  add(0, members);
  ASSERT_INVARIANT();
}


/******************************************************************************

*******************************************************************************/
void SimpleJSONArray::insert_before(
    const xs_integer& pos,
    const store::Item_t& member)
{
  ASSERT_INVARIANT();

  if (getCollection() != NULL &&
      (member->isStructuredItem()))
  {
    assert(dynamic_cast<StructuredItem*>(member.getp()));
    StructuredItem* lStructuredItem =
        static_cast<StructuredItem*>(member.getp());
    lStructuredItem->setCollectionTreeInfo(theCollectionInfo);
  }

  member->addReference();
  Members::size_type sPos = static_cast<Members::size_type>(cast(pos) - 1);
  theContent.insert(theContent.begin() + sPos, member.getp());

  ASSERT_INVARIANT();
}


/******************************************************************************

*******************************************************************************/
void SimpleJSONArray::insert_before(
    const xs_integer& aPos,
    const std::vector<store::Item_t>& members)
{
  ASSERT_INVARIANT();
  // need to reserve at the beginning because reserve invalidates
  // existing iterators
  theContent.reserve(theContent.size() + members.size());

  add(cast(aPos) - 1, members);

  ASSERT_INVARIANT();
}


/******************************************************************************

*******************************************************************************/
void SimpleJSONArray::insert_after(
    const xs_integer& aPos,
    const std::vector<store::Item_t>& members)
{
  ASSERT_INVARIANT();
  // need to reserve at the beginning because reserve invalidates
  // existing iterators
  theContent.reserve(theContent.size() + members.size());

  add(cast(aPos), members);
  ASSERT_INVARIANT();
}


/******************************************************************************

*******************************************************************************/
void SimpleJSONArray::add(
    uint64_t aTargetPos,
    const std::vector<store::Item_t>& aNewMembers)
{
  ASSERT_INVARIANT();
  for (size_t i = 0; i < aNewMembers.size(); ++i)
  {
    store::Item* lItem = aNewMembers[i].getp();

    if (getCollection() != NULL &&
        (lItem->isStructuredItem()))
    {
      assert(dynamic_cast<StructuredItem*>(lItem));
      StructuredItem* lStructuredItem =
          static_cast<StructuredItem*>(lItem);
      lStructuredItem->setCollectionTreeInfo(theCollectionInfo);
    }

    lItem->addReference();
    Members::size_type sPos = static_cast<Members::size_type>(aTargetPos + i);
    theContent.insert(theContent.begin() + sPos, lItem);
  }

  ASSERT_INVARIANT();
}


/******************************************************************************

*******************************************************************************/
store::Item_t SimpleJSONArray::remove(const xs_integer& aPos)
{
  ASSERT_INVARIANT();
  store::Item_t lItem = getArrayValue(aPos);

  if (getCollection() != NULL &&
      (lItem->isStructuredItem()))
  {
    assert(dynamic_cast<StructuredItem*>(lItem.getp()));
    StructuredItem* lStructuredItem =
        static_cast<StructuredItem*>(lItem.getp());
    lStructuredItem->setCollectionTreeInfo(NULL);
  }

  lItem->removeReference();
  uint64_t lPosStartingZero = cast(aPos) - 1;

  theContent.erase(theContent.begin() + static_cast<Members::size_type>(lPosStartingZero) );

  ASSERT_INVARIANT();
  return lItem;
}


/******************************************************************************

*******************************************************************************/
void SimpleJSONArray::swap(store::Item* anotherItem)
{
  SimpleJSONArray* lOther = dynamic_cast<SimpleJSONArray*>(anotherItem);
  assert(lOther);
  std::swap(theContent, lOther->theContent);
  setCollectionTreeInfo(theCollectionInfo);
  lOther->setCollectionTreeInfo(lOther->theCollectionInfo);
}

/******************************************************************************

*******************************************************************************/
store::Item_t SimpleJSONArray::replace(
    const xs_integer& aPos,
    const store::Item_t& value)
{
  ASSERT_INVARIANT();
  store::Item_t lItem = getArrayValue(aPos);

  if (getCollection() != NULL &&
      (lItem->isStructuredItem()))
  {
      assert(dynamic_cast<StructuredItem*>(lItem.getp()));
      StructuredItem* lStructuredItem =
          static_cast<StructuredItem*>(lItem.getp());
      lStructuredItem->setCollectionTreeInfo(NULL);
  }

  uint64_t pos = cast(aPos) - 1;

  if (getCollection() != NULL &&
      (value->isStructuredItem()))
  {
    assert(dynamic_cast<StructuredItem*>(value.getp()));
    StructuredItem* lStructuredItem =
        static_cast<StructuredItem*>(value.getp());
    lStructuredItem->setCollectionTreeInfo(theCollectionInfo);
  }
  Members::size_type sPos = static_cast<Members::size_type>(pos);
  theContent[sPos]->removeReference();
  value->addReference();
  theContent[sPos] = value.getp();

  ASSERT_INVARIANT();
  return lItem;
}


/******************************************************************************

*******************************************************************************/
void SimpleJSONArray::setCollectionTreeInfo(CollectionTreeInfo* collectionInfo)
{
  theCollectionInfo = static_cast<CollectionTreeInfoWithTreeId*>(collectionInfo);

  for (Members::const_iterator ite = theContent.begin();
       ite != theContent.end();
       ++ite)
  {
    store::Item* value = *ite;

    if (value->isStructuredItem())
    {
      StructuredItem* structuredItem = static_cast<StructuredItem*>(value);
      structuredItem->setCollectionTreeInfo(collectionInfo);
    }
  }
}


/******************************************************************************

*******************************************************************************/
uint64_t SimpleJSONArray::cast(const xs_integer& i)
{
  try 
  {
    return to_xs_unsignedLong(i);
  }
  catch (std::range_error const&)
  {
    throw ZORBA_EXCEPTION(
      zerr::ZSTR0060_RANGE_EXCEPTION,
      ERROR_PARAMS( i )
    );
  }
}


/******************************************************************************

*******************************************************************************/
store::Item_t SimpleJSONArray::getArrayValue(const xs_integer& aPosition) const
{
  ASSERT_INVARIANT();
  uint64_t lPos = cast(aPosition);

  if (lPos == 0 || lPos > theContent.size())
  {
    return 0;
  }
  else
  {
    return theContent[ static_cast<Members::size_type>(lPos-1) ];
  }
}


/******************************************************************************

*******************************************************************************/
store::Iterator_t SimpleJSONArray::getArrayValues() const
{
  ASSERT_INVARIANT();
  return new ValuesIterator(const_cast<SimpleJSONArray*>(this));
}


/******************************************************************************

*******************************************************************************/
xs_integer SimpleJSONArray::getArraySize() const
{
  ASSERT_INVARIANT();
  store::Item_t lRes;
  xs_integer lSize(theContent.size());
  return lSize;
}


/******************************************************************************

*******************************************************************************/
store::Item* SimpleJSONArray::copy(
    store::Item* parent,
    const store::CopyMode& copymode) const
{
  ASSERT_INVARIANT();
  SimpleJSONArray* lNewArray = const_cast<SimpleJSONArray*>(this);

  if (copymode.theDoCopy)
  {
    lNewArray = new SimpleJSONArray();
    lNewArray->theContent.reserve(theContent.size());

    for (Members::const_iterator lIter = theContent.begin();
         lIter != theContent.end();
         ++lIter)
    {
      store::Item_t lValue = *lIter;

      if (lValue->isStructuredItem())
      {
        lValue = lValue->copy(NULL, copymode);
      }

      lNewArray->push_back(lValue);
    }
  }

  if (parent)
  {
    assert(parent->isArray());
    JSONArray* a = static_cast<JSONArray*>(parent);
    a->push_back(lNewArray);
  }

  return lNewArray;
}


/******************************************************************************

*******************************************************************************/
zstring SimpleJSONArray::getStringValue() const
{
  ASSERT_INVARIANT();
  throw ZORBA_EXCEPTION(jerr::JNTY0024, ERROR_PARAMS("array"));
}


/******************************************************************************

*******************************************************************************/
void SimpleJSONArray::getStringValue2(zstring& val) const
{
  ASSERT_INVARIANT();
  val = getStringValue();
}


/******************************************************************************

*******************************************************************************/
void SimpleJSONArray::appendStringValue(zstring& buf) const
{
  ASSERT_INVARIANT();
  buf = getStringValue();
}


/******************************************************************************

*******************************************************************************/
void SimpleJSONArray::getTypedValue(store::Item_t& val, store::Iterator_t& iter) const
{
  ASSERT_INVARIANT();
  throw ZORBA_EXCEPTION(jerr::JNTY0004, ERROR_PARAMS("array"));
}


#ifndef NDEBUG

/******************************************************************************

*******************************************************************************/
bool SimpleJSONArray::isThisTreeOfAllDescendants(
    const CollectionTreeInfo* collectionInfo) const
{
  if (theCollectionInfo != collectionInfo)
  {
    return false;
  }

  for (Members::const_iterator lIter = theContent.begin();
       lIter != theContent.end();
       ++lIter)
  {
    store::Item* lValue = (*lIter);
    const JSONItem* lJSONItem = dynamic_cast<const JSONItem*>(lValue);
    if (lJSONItem != NULL &&
        !lJSONItem->isThisTreeOfAllDescendants(collectionInfo))
    {
      return false;
    }
  }
  return true;
}


#endif // NDEBUG

/******************************************************************************

*******************************************************************************/
bool SimpleJSONArray::isInSubtree(const StructuredItem* anItem) const
{
  if (this == anItem)
  {
    return true;
  }

  for (Members::const_iterator lIter = theContent.begin();
       lIter != theContent.end();
       ++lIter)
  {
    store::Item* lValue = *lIter;

    if (lValue->isStructuredItem())
    {
      const StructuredItem* structuredItem = 
      static_cast<const StructuredItem*>(lValue);

      if (structuredItem->isInSubtree(anItem))
      {
        return true;
      }
    }
  }

  return false;
}


/******************************************************************************

*******************************************************************************/
zstring SimpleJSONArray::show() const
{
  std::stringstream str;
  str << "Array size: " << theContent.size() << " [";
  for ( ulong i = 0; i < theContent.size(); i++)
   str << theContent[i]->show() << ", ";
  str << "]";
  return str.str();
}


/******************************************************************************

*******************************************************************************/
SimpleJSONArray::ValuesIterator::~ValuesIterator() 
{
}


/******************************************************************************

*******************************************************************************/
void SimpleJSONArray::ValuesIterator::open()
{
  theIter = theArray->theContent.begin();
}


/******************************************************************************

*******************************************************************************/
bool SimpleJSONArray::ValuesIterator::next(store::Item_t& res)
{
  if (theIter != theArray->theContent.end())
  {
    res = *theIter;
    ++theIter;
    return true;
  }
  else
  {
    return false;
  }
}


/******************************************************************************

*******************************************************************************/
void SimpleJSONArray::ValuesIterator::reset()
{
  open();
}


/******************************************************************************

*******************************************************************************/
void SimpleJSONArray::ValuesIterator::close()
{
  theIter = theArray->theContent.end();
}




} // namespace json
} // namespace simplestore
} // namespace zorba

/*
 * Local variables:
 * mode: c++
 * End:
 */
/* vim:set et sw=2 ts=2: */
