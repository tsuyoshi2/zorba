/*
 * Copyright 2006-2008 The FLWOR Foundation.
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
#include <iostream>
#include <climits>
#include <memory>

#include <libxml/parser.h>

#include "zorbautils/hashfun.h"
#include "zorbautils/fatal.h"
#include "zorbatypes/rchandle.h"
#include "zorbaerrors/error_manager.h"
#include "zorbaerrors/Assert.h"

#include "store/util/hashmap_stringp.h"

#include "store/api/pul.h"

#include "store/naive/properties.h"
#include "store/naive/string_pool.h"
#include "store/naive/simple_store.h"
#include "store/naive/simple_temp_seq.h"
#include "store/naive/simple_lazy_temp_seq.h"
#include "store/naive/simple_collection.h"
#include "store/naive/simple_index.h"
#include "store/naive/simple_ic.h"
#include "store/naive/qname_pool.h"
#include "store/naive/loader.h"
#include "store/naive/store_defs.h"
#include "store/naive/node_items.h"
#include "store/naive/dataguide.h"
#include "store/naive/node_iterators.h"
#include "store/naive/simple_item_factory.h"
#include "store/naive/simple_iterator_factory.h"
#include "store/naive/query_context.h"
#include "store/naive/item_iterator.h"

#ifdef ZORBA_STORE_MSDOM
#include "store/naive/msdom_addon/import_msdom.h"
#include <time.h>
#endif

namespace zorba 
{ 

namespace simplestore 
{

typedef rchandle<store::TempSeq> TempSeq_t;

const ulong SimpleStore::NAMESPACE_POOL_SIZE = 128;
const ulong SimpleStore::DEFAULT_COLLECTION_MAP_SIZE = 32;

const char* SimpleStore::XS_URI = "http://www.w3.org/2001/XMLSchema";
const char* SimpleStore::XML_URI = "http://www.w3.org/2001/XML/1998/namespace";
const char* SimpleStore::ZXSE_URI = "http://www.zorba-xquery.com/zorba/schema-extensions";

const ulong SimpleStore::XML_URI_LEN = sizeof(SimpleStore::XML_URI);


/*******************************************************************************

********************************************************************************/
SimpleStore::SimpleStore()
  :
  theIsInitialized(false),
  theUriCounter(0),
  theCollectionCounter(1),
  theTreeCounter(1),
  theNamespacePool(NULL),
  theQNamePool(NULL),
  theItemFactory(NULL),
  theIteratorFactory(NULL),
  theDocuments(DEFAULT_COLLECTION_MAP_SIZE, true),
  theCollections(0, NULL, DEFAULT_COLLECTION_MAP_SIZE, true),
  theUriCollections(DEFAULT_COLLECTION_MAP_SIZE, true),
  theIndices(0, NULL, DEFAULT_COLLECTION_MAP_SIZE, true),
  theICs(0, NULL, DEFAULT_COLLECTION_MAP_SIZE, true),
  theTraceLevel(0)
{
}


/*******************************************************************************

********************************************************************************/
void SimpleStore::init()
{
  if (!theIsInitialized)
  {
    // This initializes the libxml2 library and checks potential ABI mismatches
    // between the version it was compiled for and the actual  shared library used.
    // Calling its init is done here because we also want to free it at the end,
    // i.e. when the store is shutdown
    LIBXML_TEST_VERSION

    store::Properties::load (0, NULL);

    theUriCounter = 0;
    theCollectionCounter = 1;
    theTreeCounter = 1;

    theNamespacePool = new StringPool(NAMESPACE_POOL_SIZE);

    theNamespacePool->insertc("", theEmptyNs);
    theNamespacePool->insertc(XS_URI, theXmlSchemaNs);

    theQNamePool = new QNamePool(QNamePool::MAX_CACHE_SIZE, theNamespacePool);

    initTypeNames();

    theItemFactory = new BasicItemFactory(theNamespacePool, theQNamePool);

    theIteratorFactory = new SimpleIteratorFactory();

    theTraceLevel = store::Properties::instance()->storeTraceLevel();

#ifdef ZORBA_STORE_MSDOM
    CoInitialize(NULL);
#endif
    theIsInitialized = true;
  }
}


/*******************************************************************************

********************************************************************************/
void SimpleStore::initTypeNames()
{
  theSchemaTypeNames.resize(XS_LAST);

  theSchemaTypeNames[0]  = theQNamePool->insert(XS_URI, "", "untyped");
  theSchemaTypeNames[1]  = theQNamePool->insert(XS_URI, "", "untypedAtomic");
  theSchemaTypeNames[2]  = theQNamePool->insert(XS_URI, "", "anyType");
  theSchemaTypeNames[3]  = theQNamePool->insert(XS_URI, "", "anySimpleType");
  theSchemaTypeNames[4]  = theQNamePool->insert(XS_URI, "", "anyAtomicType");

  theSchemaTypeNames[5]  = theQNamePool->insert(XS_URI, "", "string");
  theSchemaTypeNames[6]  = theQNamePool->insert(XS_URI, "", "normalizedString");
  theSchemaTypeNames[7]  = theQNamePool->insert(XS_URI, "", "language");
  theSchemaTypeNames[8]  = theQNamePool->insert(XS_URI, "", "token");
  theSchemaTypeNames[9]  = theQNamePool->insert(XS_URI, "", "NMTOKEN");

  theSchemaTypeNames[10] = theQNamePool->insert(XS_URI, "", "anyURI");
  theSchemaTypeNames[11] = theQNamePool->insert(XS_URI, "", "Name");
  theSchemaTypeNames[12] = theQNamePool->insert(XS_URI, "", "NCName");
  theSchemaTypeNames[13] = theQNamePool->insert(XS_URI, "", "QName");
  theSchemaTypeNames[14] = theQNamePool->insert(XS_URI, "", "notation");

  theSchemaTypeNames[15] = theQNamePool->insert(XS_URI, "", "ID");
  theSchemaTypeNames[16] = theQNamePool->insert(XS_URI, "", "IDREF");

  theSchemaTypeNames[17] = theQNamePool->insert(XS_URI, "", "ENTITY");

  theSchemaTypeNames[18] = theQNamePool->insert(XS_URI, "", "dateTime");
  theSchemaTypeNames[19] = theQNamePool->insert(XS_URI, "", "date");
  theSchemaTypeNames[20] = theQNamePool->insert(XS_URI, "", "time");
  theSchemaTypeNames[21] = theQNamePool->insert(XS_URI, "", "duration");
  theSchemaTypeNames[22] = theQNamePool->insert(XS_URI, "", "dayTimeDuration");
  theSchemaTypeNames[23] = theQNamePool->insert(XS_URI, "", "yearMonthDuration");

  theSchemaTypeNames[24] = theQNamePool->insert(XS_URI, "", "float");
  theSchemaTypeNames[25] = theQNamePool->insert(XS_URI, "", "double");
  theSchemaTypeNames[26] = theQNamePool->insert(XS_URI, "", "decimal");
  theSchemaTypeNames[27] = theQNamePool->insert(XS_URI, "", "integer");
  theSchemaTypeNames[28] = theQNamePool->insert(XS_URI, "", "nonPositiveInteger");
  theSchemaTypeNames[29] = theQNamePool->insert(XS_URI, "", "nonNegativeInteger");
  theSchemaTypeNames[30] = theQNamePool->insert(XS_URI, "", "negativeInteger");
  theSchemaTypeNames[31] = theQNamePool->insert(XS_URI, "", "positiveInteger");

  theSchemaTypeNames[32] = theQNamePool->insert(XS_URI, "", "long");
  theSchemaTypeNames[33] = theQNamePool->insert(XS_URI, "", "int");
  theSchemaTypeNames[34] = theQNamePool->insert(XS_URI, "", "short");
  theSchemaTypeNames[35] = theQNamePool->insert(XS_URI, "", "byte");
  theSchemaTypeNames[36] = theQNamePool->insert(XS_URI, "", "unsignedLong");
  theSchemaTypeNames[37] = theQNamePool->insert(XS_URI, "", "unsignedInt");
  theSchemaTypeNames[38] = theQNamePool->insert(XS_URI, "", "unsignedShort");
  theSchemaTypeNames[39] = theQNamePool->insert(XS_URI, "", "unsignedByte");

  theSchemaTypeNames[40] = theQNamePool->insert(XS_URI, "", "gYearMonth");
  theSchemaTypeNames[41] = theQNamePool->insert(XS_URI, "", "gYear");
  theSchemaTypeNames[42] = theQNamePool->insert(XS_URI, "", "gMonthDay");
  theSchemaTypeNames[43] = theQNamePool->insert(XS_URI, "", "gDay");
  theSchemaTypeNames[44] = theQNamePool->insert(XS_URI, "", "gMonth");

  theSchemaTypeNames[45] = theQNamePool->insert(XS_URI, "", "base64Binary");
  theSchemaTypeNames[46] = theQNamePool->insert(XS_URI, "", "hexBinary");
  theSchemaTypeNames[47] = theQNamePool->insert(XS_URI, "", "boolean");
  theSchemaTypeNames[48] = theQNamePool->insert(ZXSE_URI, "zxse", "tuple");
}


/*******************************************************************************

********************************************************************************/
SimpleStore::~SimpleStore()
{
  shutdown();
}


/*******************************************************************************

********************************************************************************/
void SimpleStore::shutdown()
{
  theIndices.clear();
  theICs.clear();

  theCollections.clear();
  theUriCollections.clear();

  theDocuments.clear();

  if (theItemFactory != NULL)
  {
    delete theItemFactory;
    theItemFactory = NULL;
  }

  if (theQNamePool != NULL)
  {
    ulong numTypes = theSchemaTypeNames.size();
    for (ulong i = 0; i < numTypes; i++)
      theSchemaTypeNames[i] = NULL;

    delete theQNamePool;
    theQNamePool = NULL;
  }

  if (theNamespacePool != NULL)
  {
    theEmptyNs = NULL;
    theXmlSchemaNs = NULL;

    delete theNamespacePool;
    theNamespacePool = NULL;
  }

  if (theIteratorFactory != NULL)
  {
    delete theIteratorFactory;
    theIteratorFactory = NULL;
  }

  // do cleanup of the libxml2 library
  // however, after that, a user will have to call 
  // LIBXML_TEST_VERSION if he wants to use libxml2
  // beyond the lifecycle of zorba
  xmlCleanupParser(); 

#ifdef ZORBA_STORE_MSDOM
    CoUninitialize();
#endif
  
  theIsInitialized = false;
}


/*******************************************************************************
  Create an id for a new collection
********************************************************************************/
ulong SimpleStore::createCollectionId()
{
  SYNC_CODE(AutoMutex lock(&theCollectionCounterMutex);)
  return theCollectionCounter++;
}


/*******************************************************************************
  create a tree id for a new tree that does not belong to any collection.
********************************************************************************/
ulong SimpleStore::createTreeId()
{
  SYNC_CODE(AutoMutex lock(&theTreeCounterMutex);)
  return theTreeCounter++;
}


/*******************************************************************************

********************************************************************************/
XmlLoader* SimpleStore::getXmlLoader(error::ErrorManager* aErrorManager)
{
#ifndef ZORBA_STORE_MSDOM
  return new FastXmlLoader(theItemFactory,
                           aErrorManager,
                           store::Properties::instance()->buildDataguide());
#else

  return new SimpleXmlLoader(theItemFactory, 
                            aErrorManager, 
                            (store::Properties::instance())->buildDataguide());
#endif
}


/*******************************************************************************
  Create an index with a given URI and return an rchandle to the index object. 
  If an index with the given URI exists already and the index we want to create
  is not a temporary one, raise an error.
********************************************************************************/
store::Index_t SimpleStore::createIndex(
    const store::Item_t& qname,
    const store::IndexSpecification& spec,
    store::Iterator* sourceIter)
{
  ulong numColumns = spec.theKeyTypes.size();

  store::Index_t index;
  store::Item_t domainItem;
  store::IndexKey* key = NULL;

  if (!spec.theIsTemp && theIndices.get(qname, index))
  {
    ZORBA_ERROR_PARAM(STR0001_INDEX_ALREADY_EXISTS,
                      qname->getStringValue()->c_str(), "");
  }

  if (spec.theIsSorted)
  {
    index = new STLMapIndex(qname, spec);
  }
  else
  {
    index = new HashIndex(qname, spec);
  }

  sourceIter->open();

  try
  {
    while (sourceIter->next(domainItem))
    {
      key = new store::IndexKey(numColumns);

      for (ulong i = 0; i < numColumns; ++i)
      {
        if (!sourceIter->next((*key)[i]))
        {
          ZORBA_ERROR_DESC(XQP0019_INTERNAL_ERROR, "Incomplete key during index build");
        }
      }
      
      static_cast<IndexImpl*>(index.getp())->insert(key, domainItem);
      key = NULL; // ownership of the key obj passes to the index.
    }
  }
  catch(...)
  {
    if (key != NULL)
      delete key;

    sourceIter->close();
    throw;
  }

  sourceIter->close();

  if (!spec.theIsTemp)
  {
    theIndices.insert(qname.getp(), index);
  }

  return index;
}


/*******************************************************************************

********************************************************************************/
void SimpleStore::addIndex(store::Index_t& index)
{
  if (index == NULL)
    return;

  theIndices.insert(static_cast<IndexImpl*>(index.getp())->getName(), index);
}


/*******************************************************************************

********************************************************************************/
store::Index* SimpleStore::getIndex(const store::Item* qname)
{
  if (qname == NULL)
    return NULL;

  store::Index_t index;
  if (theIndices.get(qname, index))
    return index.getp();

  return NULL;
}
    

/*******************************************************************************

********************************************************************************/
void SimpleStore::deleteIndex(const store::Item* qname)
{
  if (qname == NULL)
    return;

  theIndices.remove(qname);
}

/*******************************************************************************
  Iterator to return Index and Collection Names
********************************************************************************/
template < typename T >
class NameIterator : public store::Iterator
{
private:
  T*                   theItems;
  typename T::iterator theIterator;

public:
  NameIterator(T& aItems) {
    theItems = &aItems;
  }
  virtual ~NameIterator() { close(); }
  virtual void open() {
    theIterator = theItems->begin();
  }
  virtual bool next(store::Item_t& aResult) {
    if (theIterator == theItems->end()) {
       aResult = NULL;
      return false;
    }
    else {
      aResult = (*theIterator).first;
      ++theIterator;
      return true;
    }
  }
  virtual void reset() {
    theIterator = theItems->begin();
  }
  virtual void close() {}
};


store::Iterator_t SimpleStore::listIndexNames()
{
  return new NameIterator<IndexSet>(theIndices);
}


/*******************************************************************************

********************************************************************************/
store::IC_t SimpleStore::activateIC(
    const store::Item_t& icQName, 
    const store::Item_t& collectionQName)
{
  ZORBA_ASSERT(icQName != NULL);

  store::IC_t ic;

  if (theICs.get(icQName, ic))
  {
    ZORBA_ERROR_PARAM(STR0015_IC_ALREADY_EXISTS,
                      icQName->getStringValue()->c_str(), "");
  }

  ic = new ICCollectionImpl(icQName, collectionQName);

  theICs.insert(static_cast<ICCollectionImpl*>(ic.getp())->getICName(), ic);

  return ic;
}


store::IC_t SimpleStore::activateForeignKeyIC(
    const store::Item_t& icQName, 
    const store::Item_t& fromCollectionQName,
    const store::Item_t& toCollectionQName)
{
  ZORBA_ASSERT(icQName != NULL);

  store::IC_t ic;

  if (theICs.get(icQName, ic))
  {
    ZORBA_ERROR_PARAM(STR0015_IC_ALREADY_EXISTS,
                      icQName->getStringValue()->c_str(), "");
  }

  ic = new ICForeignKeyImpl(icQName, fromCollectionQName, toCollectionQName);

  theICs.insert(static_cast<ICForeignKeyImpl*>(ic.getp())->getICName(), ic);

  return ic;
}


void SimpleStore::deactivateIC(const store::Item* icQName)
{
  ZORBA_ASSERT(icQName != NULL);

  store::IC_t ic;
  
  if (!theICs.get(icQName, ic))
  {
    ZORBA_ERROR_PARAM(STR0016_IC_DOES_NOT_EXIST,
                      icQName->getStringValue()->c_str(), "");
  }

  theICs.remove(icQName);
}


store::Iterator_t SimpleStore::listActiveICNames()
{
  return new NameIterator<ICSet>(theICs);
}


store::IC* SimpleStore::getIC(const store::Item* icQName)
{
  store::IC_t ic;
  theICs.get(icQName, ic);

  return ic.getp();
}

/*******************************************************************************
  Create a collection with a given QName and return an rchandle to the new
  collection object. If a collection with the given QName exists already, raise
  an error.
********************************************************************************/
store::Collection_t SimpleStore::createCollection(store::Item_t& aName)
{
  if (aName == NULL)
    return NULL;

  store::Collection_t collection(new SimpleCollection(aName));

  const store::Item* lName = collection->getName();

  bool inserted = theCollections.insert(lName, collection);

  if (!inserted)
  {
    ZORBA_ERROR_PARAM(STR0008_COLLECTION_ALREADY_EXISTS, lName->getStringValue(), "");
    return NULL;
  }

  return collection;
}


store::Collection_t SimpleStore::createUriCollection(const xqpStringStore_t& uri)
{
  if (uri == NULL)
    return NULL;

  store::Item_t uriItem;
  xqpStringStore_t tmpuri(uri.getp());
  theItemFactory->createAnyURI(uriItem, tmpuri);

  store::Collection_t collection(new SimpleCollection(uriItem));

  const xqpStringStore* urip = collection->getName()->getStringValueP();
  bool inserted = theUriCollections.insert(urip, collection);

  if (!inserted)
  {
    ZORBA_ERROR_PARAM(STR0008_COLLECTION_ALREADY_EXISTS, uri->c_str(), "");
    return 0;
  }

  return collection;
}


/*******************************************************************************
  Return an rchandle to the Collection object corresponding to the given QName,
  or NULL if there is no collection with that QName.
********************************************************************************/
store::Collection_t SimpleStore::getCollection(const store::Item* aName)
{
  if (aName == NULL)
    return NULL;

  store::Collection_t collection;
  if (theCollections.get(aName, collection) )
    return collection.getp();
  else
    return NULL;
}


store::Collection_t SimpleStore::getUriCollection(const xqpStringStore_t& uri)
{
  if (uri == NULL)
    return NULL;

  store::Collection_t collection;
  if (theUriCollections.get(uri, collection) )
    return collection.getp();
  else
    return NULL;
}


/*******************************************************************************
  Delete the collection with the given QName. If there is no collection with
  that QName, this method is a NOOP.
********************************************************************************/
void SimpleStore::deleteCollection(const store::Item* aName)
{
  if (aName == NULL)
    return;

  if (!theCollections.remove(aName)) 
  {
    ZORBA_ERROR_PARAM(STR0009_COLLECTION_NOT_FOUND, aName->getStringValue(), "");
  }
}


void SimpleStore::deleteUriCollection(const xqpStringStore_t& uri)
{
  if (uri == NULL)
    return;

  bool deleted = theUriCollections.remove(uri);

  if (!deleted) 
  {
    ZORBA_ERROR_PARAM(STR0009_COLLECTION_NOT_FOUND, uri->c_str(), "");
  }
}

class CollectionNameIterator : public store::Iterator
{
private:
  CollectionSet*          theCollections;
  CollectionSet::iterator theIterator;

public:
  CollectionNameIterator(CollectionSet& aCollections) 
  {
    theCollections = &aCollections;
  }

  virtual ~CollectionNameIterator() { close(); }

  virtual void open() { theIterator = theCollections->begin(); }

  virtual bool next(store::Item_t& aResult) 
  {
    if (theIterator == theCollections->end()) 
    {
       aResult = NULL;
      return false;
    }
    else
    {
      aResult = (*theIterator).first;
      ++theIterator;
      return true;
    }
  }

  virtual void reset() { theIterator = theCollections->begin(); }

  virtual void close() {}
};

/*******************************************************************************
  Returns an iterator that lists the QName's of all the available collections.
********************************************************************************/
store::Iterator_t SimpleStore::listCollectionNames()
{
  return new NameIterator<CollectionSet>(theCollections);
}


/*******************************************************************************

********************************************************************************/
store::Item_t SimpleStore::loadDocument(
    const xqpStringStore_t& uri,
    std::istream& stream,
    bool storeDocument)
{
  if (uri == NULL)
    return NULL;

  const xqpStringStore* urip = uri.getp();

  XmlNode_t root;
  bool found = theDocuments.get(urip, root);

  if (found)
  {
    //+ daniel theDocuments.remove(uri);
    return root.getp();
  }

  error::ErrorManager lErrorManager;
  std::auto_ptr<XmlLoader> loader(getXmlLoader(&lErrorManager));

  root = loader->loadXml(uri, stream);
  if (lErrorManager.hasErrors()) 
  {
    ZORBA_ERROR_PARAM(lErrorManager.getErrors().front().theErrorCode,
                      lErrorManager.getErrors().front().theDescription, "");
  }

  if (root != NULL && storeDocument)
    theDocuments.insert(urip, root);

  return root.getp();
}

/*******************************************************************************
For lazy loading...
Param stream is a heap pointer to an input stream. This is to be deallocated by Zorba.
********************************************************************************/
store::Item_t SimpleStore::loadDocument(
    const xqpStringStore_t& uri, 
    std::istream* stream,
    bool storeDocument)
{
  store::Item_t docitem;
  try{
    //do full loading for now
    docitem = loadDocument(uri, *stream, storeDocument);
    delete stream;
  }
  catch(...)
  {
    delete stream;
  }
  return docitem;
}


/*******************************************************************************
  Add the given node with the given uri to the store. Essentially, this method
  establishes an association between a uri and a node. If the given uri is 
  already associated to another node, the method raises an error. If the given
  uri is already associated to the given node, this method is a noop.
********************************************************************************/
void SimpleStore::addNode(const xqpStringStore_t& uri, const store::Item_t& node)
{
  ZORBA_ASSERT(uri != NULL);

  if(node == NULL || !node->isNode())
  {
    ZORBA_ERROR_PARAM(API0021_ITEM_TO_LOAD_IS_NOT_NODE, uri, "");
  }

  XmlNode_t root = reinterpret_cast<XmlNode*>(node.getp());
  bool inserted = theDocuments.insert(uri, root);

  if (!inserted && node.getp() != root.getp())
  {
    ZORBA_ERROR_PARAM(API0020_DOCUMENT_ALREADY_EXISTS, uri, "");
  }
  else if (inserted)
  {
    root->setDocUri(uri);
  }

  ZORBA_FATAL(node.getp() == root.getp(), "");
}


/*******************************************************************************
  Return an rchandle to the root node of the document corresponding to the given
  URI, or NULL if there is no document with that URI.
********************************************************************************/
store::Item_t SimpleStore::getDocument(const xqpStringStore_t& uri)
{
  if (uri == NULL)
    return NULL;

  XmlNode_t root;
  bool found = theDocuments.get(uri, root);
  if (found)
    return root.getp();

  return NULL;
}


/*******************************************************************************
  Delete the document with the given URI. If there is no document with that
  URI, this method is a NOOP.
********************************************************************************/
void SimpleStore::deleteDocument(const xqpStringStore_t& uri)
{
  if (uri == NULL)
    return;

  theDocuments.remove(uri);
}


/*******************************************************************************
  Delete all the documents.
********************************************************************************/
void SimpleStore::deleteAllDocuments()
{
  theDocuments.clear();
}


/*******************************************************************************
  Compare two nodes, based on their node id. Return -1 if node1 < node2, 0, if
  node1 == node2, or 1 if node1 > node2.
********************************************************************************/
short SimpleStore::compareNodes(store::Item* node1, store::Item* node2) const
{
  ZORBA_FATAL(node1->isNode(), "");
  ZORBA_FATAL(node2->isNode(), "");

  if (node1 == node2)
    return 0;

  XmlNode* n1 = static_cast<XmlNode*>(node1);
  XmlNode* n2 = static_cast<XmlNode*>(node2);

  return n1->compare2(n2);
}


/*******************************************************************************
  Sorts the items of the passed iterator

  @param iterator to sort
  @param ascendent true for ascendent and false for descendant
  @param duplicate duplicate elemination should be applied
  @return iterator which produces the sorted items
********************************************************************************/
store::Iterator_t SimpleStore::sortNodes(
    store::Iterator* input,
    bool ascendent,
    bool duplicateElemination,
    bool aAllowAtomics)
{
  if (aAllowAtomics)
    return new StoreNodeSortOrAtomicIterator(input, ascendent, duplicateElemination);
  else
    return new StoreNodeSortIterator(input, ascendent, duplicateElemination);
}


/*******************************************************************************
  Create an iterator that eliminates the duplicate nodes in the set of items
  which is produced by the passed iterator
********************************************************************************/
store::Iterator_t SimpleStore::distinctNodes(store::Iterator* input, bool aAllowAtomics)
{
  if (aAllowAtomics)
    return new StoreNodeDistinctOrAtomicIterator(input);
  else
    return new StoreNodeDistinctIterator(input);
}


/*******************************************************************************
  Computes the URI for the given node.
********************************************************************************/
bool SimpleStore::getReference(store::Item_t& result, const store::Item* node)
{
  std::ostringstream stream;

  const XmlNode* n = reinterpret_cast<const XmlNode*>(node);

  if (n->getNodeKind() == store::StoreConsts::attributeNode)
  {
    stream << "zorba://node_reference/" 
           << n->getCollectionId() << "/" 
           << n->getTreeId() << "/"
           << n->getTree()->getPosition() + 1 << "/a/"
           << n->getOrdPath().serialize();
  }
  else
  {
    stream << "zorba://node_reference/" 
           << n->getCollectionId() << "/" 
           << n->getTreeId() << "/"
           << n->getTree()->getPosition() + 1 << "/c/"
           << n->getOrdPath().serialize();
  }

  xqpStringStore_t str(new xqpStringStore(stream.str()));

  result = new AnyUriItemImpl(str);
  return true;
}


/*******************************************************************************
  Returns Item which is identified by a reference

  @param uri Has to be an xs:URI item
  @returns referenced item if it exists, otherwise NULL
********************************************************************************/
bool SimpleStore::getNodeByReference(store::Item_t& result, const store::Item* uri)
{
  xqpStringStore* str = uri->getStringValueP();

  ulong prefixlen = strlen("zorba://node_reference/");

  if (strncmp(str->c_str(), "zorba://node_reference/", prefixlen))
    ZORBA_ERROR_PARAM_OSS(API0028_INVALID_NODE_URI, str->c_str(), "");

  const char* start;
  long tmp = 0;

  //
  // Decode collection id
  //
  start = str->c_str() + prefixlen;
  char* next = const_cast<char*>(start);

  tmp = strtol(start, &next, 10);

  if (tmp < 0 || tmp == LONG_MAX)
    ZORBA_ERROR_PARAM_OSS(API0028_INVALID_NODE_URI, str->c_str(), "");

  start = next;

  if (*start != '/')
    ZORBA_ERROR_PARAM_OSS(API0028_INVALID_NODE_URI, str->c_str(), "");

  ++start;

  ulong collectionId = (ulong)tmp;

  //
  // Decode tree id
  //
  tmp = strtol(start, &next, 10);

  if (tmp <= 0 || tmp == LONG_MAX)
    ZORBA_ERROR_PARAM_OSS(API0028_INVALID_NODE_URI, str->c_str(), "");

  start = next;

  if (*start != '/')
    ZORBA_ERROR_PARAM_OSS(API0028_INVALID_NODE_URI, str->c_str(), "");

  ++start;

  ulong treeId = (ulong)tmp;

  //
  // Decode tree position within collection
  //
  tmp = strtol(start, &next, 10);

  if (tmp <= 0 || tmp == LONG_MAX)
    ZORBA_ERROR_PARAM_OSS(API0028_INVALID_NODE_URI, str->c_str(), "");

  start = next;

  if (*start != '/')
    ZORBA_ERROR_PARAM_OSS(API0028_INVALID_NODE_URI, str->c_str(), "");

  ++start;

  ulong treePos = (ulong)tmp;

  std::cout << "tree pos = " << treePos << std::endl;

  //
  // Check if the uri specifies attribute node or not
  //
  bool attributeNode;

  if (*start == 'a')
    attributeNode = true;
  else if (*start == 'c')
    attributeNode = false;
  else
    ZORBA_ERROR_PARAM_OSS(API0028_INVALID_NODE_URI, str->c_str(), "");

  ++start;
  if (*start != '/')
    ZORBA_ERROR_PARAM_OSS(API0028_INVALID_NODE_URI, str->c_str(), "");

  ++start;

  //
  // Search for the tree
  //
  XmlNode* rootNode = NULL;

  if (collectionId == 0)
  {
    DocumentSet::iterator it = theDocuments.begin();
    DocumentSet::iterator end = theDocuments.end();

    for (; it != end; ++it)
    {
      rootNode = (*it).second.getp();

      if (rootNode->getTreeId() == treeId)
        break;
    }
  }
  else
  {
    // Look for the collection
    SimpleCollection* collection;

    CollectionSet::iterator it = theCollections.begin();
    CollectionSet::iterator end = theCollections.end();

    for (; it != end; ++it)
    {
      collection = static_cast<SimpleCollection*>((*it).second.getp());

      if (collection->getId() == collectionId)
        break;
    }

    // If collection found, look for the tree
    if (it != end)
    {
      store::Item_t rootItem = collection->nodeAt(treePos);

      rootNode = BASE_NODE(rootItem);

      if (rootNode == NULL || rootNode->getTreeId() != treeId)
      {
        store::Iterator_t treeIter = collection->getIterator();

        treeIter->open();

        while (treeIter->next(rootItem))
        {
          rootNode = BASE_NODE(rootItem);
          if (rootNode->getTreeId() == treeId)
            break;
        }

        treeIter->close();

        rootNode = BASE_NODE(rootItem);
      }
    }
  }

  if (rootNode == NULL) 
  {
    result = NULL;
    return false;
  }

  //
  // Search for node in the tree
  //
  
  OrdPath op((unsigned char*)start, strlen(start));

  if (rootNode->getOrdPath() == op) 
  {
    result = rootNode;
    return true;
  }

  XmlNode* parent = rootNode;

  while (1)
  {
    ulong i;

    if (parent->getNodeKind() != store::StoreConsts::documentNode &&
        parent->getNodeKind() != store::StoreConsts::elementNode)
    {
      result = NULL;
      return false;
    }

    if (attributeNode && parent->getNodeKind() == store::StoreConsts::elementNode)
    {
      ElementNode* elemParent = reinterpret_cast<ElementNode*>(parent);

      ulong numAttrs = elemParent->numAttributes();
      for (i = 0; i < numAttrs; i++)
      {
        XmlNode* child = elemParent->getAttr(i);

        OrdPath::RelativePosition pos =  child->getOrdPath().getRelativePosition(op);

        if (pos == OrdPath::SELF)
        {
          result = child;
          return true;
        }
        else if (pos == OrdPath::DESCENDANT)
        {
          parent = child;
          break;
        }
        else if (pos != OrdPath::FOLLOWING)
        {
          result = NULL;
          return false;
        }
      }
    }

    InternalNode* parent2 = reinterpret_cast<InternalNode*>(parent);

    ulong numChildren = parent2->numChildren();
    for (i = 0; i < numChildren; i++)
    {
      XmlNode* child = parent2->getChild(i);
      
      OrdPath::RelativePosition pos =  child->getOrdPath().getRelativePosition(op);

      if (pos == OrdPath::SELF)
      {
        result = child;
        return true;
      }
      else if (pos == OrdPath::DESCENDANT)
      {
        parent = child;
        break;
      }
      else if (pos != OrdPath::FOLLOWING)
      {
        result = NULL;
        return false;
      }
    }
    
    if (i == numChildren)
    {
      result = NULL;
      return false;
    }
  }
}


/*******************************************************************************

********************************************************************************/
bool SimpleStore::getPathInfo(
    const store::Item*               docUri,
    std::vector<const store::Item*>& contextPath,
    std::vector<const store::Item*>& relativePath,
    bool                             isAttrPath,
    bool&                            found,
    bool&                            unique)
{
  rchandle<XmlNode> docRoot = BASE_NODE(getDocument(docUri->getStringValue()));

  if (docRoot == NULL)
    return false;

  GuideNode* guideRoot = docRoot->getDataGuide();

  if (!guideRoot)
    return false;

  guideRoot->getPathInfo(contextPath, relativePath, isAttrPath, found, unique);
  return true;
}


/*******************************************************************************
  Creates a new TempSeq. The instance can be used, e.g. for variable bindings

  @param iterator   The source for the XMDInstance
  @param copyNodes  If true, all nodes are copied before they are saved in the
                    temp sequence.
  @param lazy		    Hint for the store. If possible a XMDInstance should be
                    evaluated lazily. For XQueryP it might be necassary to set
                    this to false.
********************************************************************************/
TempSeq_t SimpleStore::createTempSeq(
    store::Iterator* iterator,
    bool copyNodes,
    bool lazy)
{
  TempSeq_t tempSeq;
  if(lazy){
    tempSeq = new SimpleTempSeq(iterator, copyNodes);
    //tempSeq = new SimpleLazyTempSeq(iterator, copyNodes);
  }else{
    tempSeq = new SimpleTempSeq(iterator, copyNodes);
  }
  
  return tempSeq;
}


/*******************************************************************************
  Creates an empty TempSeq.
********************************************************************************/
TempSeq_t SimpleStore::createTempSeq()
{
  TempSeq_t tempSeq = new SimpleTempSeq();
  return tempSeq;
}


/*******************************************************************************
  Creates a temp seq initialized by the given vector.
  @param item_v - The vector to use to initialize the seq.
********************************************************************************/
TempSeq_t SimpleStore::createTempSeq(const std::vector<store::Item_t>& item_v)
{
  TempSeq_t tempSeq = new SimpleTempSeq(item_v);
  return tempSeq;
}

#ifdef ZORBA_STORE_MSDOM
/*******************************************************************************
  Export the Microsoft DOM tree representation of a Node Item.
  Exporting MS DOM tree is valid only for the MS DOM store.
  Compile the simple store with ZORBA_STORE_MSDOM in order to use this feature on Windows machines.
  For atomic items it returns NULL.
********************************************************************************/
IXMLDOMNode*   SimpleStore::exportItemAsMSDOM(store::Item_t it)
{
  if(it == NULL)
    return NULL;
  XmlNode *xml_node = dynamic_cast<XmlNode*>(it.getp());
  if(xml_node)
    return xml_node->GetDOMNode();
  else
    return NULL;
}

/*******************************************************************************
  Import a Microsoft DOM tree into the store as a xml document.
  This function is equivalent with loadDocument,  only that it loads the xml from
  an existing MS DOM tree. The existing MS DOM is kept unchanged at import time.
  Further updates on the document are reflected into the original MS DOM tree.
  Compile the simple store with ZORBA_STORE_MSDOM in order to use this feature
  on Windows machines.
*******************************************************************************/
store::Item_t  SimpleStore::importMSDOM(IXMLDOMNode* domNode,
                                        xqpStringStore_t docUri,
                                        xqpStringStore_t baseUri)
{
  ImportMSDOM   importer(theItemFactory);

  XmlNode_t root_node = importer.importMSDOM(domNode, docUri, baseUri);
  const xqpStringStore* urip = docUri.getp();
  theDocuments.insert(urip, root_node);

  return root_node.getp();
}

#endif

} // namespace store
} // namespace zorba
