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
#include "stdafx.h"

#include <memory>

#include "diagnostics/assert.h"
#include "diagnostics/xquery_diagnostics.h"
#include "diagnostics/util_macros.h"

#include "zorbatypes/collation_manager.h"
#include "zorbatypes/numconversions.h"
#include "zorbatypes/URI.h"

#include "zorbautils/hashfun.h"
#include "zorbautils/icu_tokenizer.h"

#include "store/api/item.h"
#include "store/api/store.h"
#include "store/api/item_factory.h"
#include "store/api/collection.h"
#include "store/naive/qname_pool.h"
#include "store/naive/simple_store.h"
#include "store/naive/simple_item_factory.h"
#include "store/naive/store_defs.h"
#include "store/naive/item_iterator.h"
#include "store/naive/node_items.h"
#include "store/naive/atomic_items.h"

#include "util/ascii_util.h"
#include "util/string_util.h"
#include "util/utf8_util.h"

#define CREATE_XS_TYPE(aType) \
  GET_STORE().getItemFactory()->createQName(SimpleStore::XS_URI, "xs", aType);

#define CREATE_BOOLITEM(item, aValue) \
  GET_STORE().getItemFactory()->createBoolean(item, aValue)

using namespace std;

namespace zorba { namespace simplestore {


/*******************************************************************************

********************************************************************************/
store::Item_t AtomicItem::getAtomizationValue() const
{
  store::Item* lItem = const_cast<AtomicItem *>(this);
  return lItem;
}


/*******************************************************************************

********************************************************************************/
void AtomicItem::getTypedValue(store::Item_t& val, store::Iterator_t& iter) const
{
  store::Item* lItem = const_cast<AtomicItem *>(this);
  val = lItem;
}


/*******************************************************************************
  For numeric items or the untyped item: convert this item to a long item,
  if possible, i.e., if the conversion is going to be lossless.
********************************************************************************/
bool AtomicItem::castToLong(store::Item_t& result) const
{
  xs_long longValue;

  result = NULL;

  switch (getTypeCode())
  {
  case XS_UNTYPED_ATOMIC:
  {
    const UntypedAtomicItem* item = static_cast<const UntypedAtomicItem*>(this);
    if (NumConversions::strToLong(item->theValue.c_str(), longValue))
      GET_FACTORY().createLong(result, longValue);

    break;
  }

  case XS_DOUBLE:
  case XS_FLOAT:
  {
    double doubleValue = getDoubleValue().getNumber();
    longValue = static_cast<xs_long>(doubleValue);

    if (doubleValue == static_cast<double>(longValue))
      GET_FACTORY().createLong(result, longValue);

    break;
  }

  case XS_DECIMAL:
  {
    const DecimalItem* item = static_cast<const DecimalItem*>(this);
    if (NumConversions::decimalToLong(item->theValue, longValue))
      GET_FACTORY().createLong(result, longValue);

    break;
  }

  case XS_INTEGER:
  case XS_NON_POSITIVE_INTEGER:
  case XS_NEGATIVE_INTEGER:
  case XS_NON_NEGATIVE_INTEGER:
  case XS_POSITIVE_INTEGER:
  {
    const IntegerItem* item = static_cast<const IntegerItem*>(this);
    if (NumConversions::integerToLong(item->theValue, longValue))
      GET_FACTORY().createLong(result, longValue);

    break;
  }

  case XS_UNSIGNED_LONG:
  {
    const UnsignedLongItem* item = static_cast<const UnsignedLongItem*>(this);
    if ((item->theValue >> 63) == 0)
    {
      longValue = static_cast<xs_long>(item->theValue);
      GET_FACTORY().createLong(result, longValue);
    }
    break;
  }

  default:
  {
    RAISE_ERROR_NO_LOC(zerr::ZSTR0050_FUNCTION_NOT_IMPLEMENTED_FOR_ITEMTYPE,
    ERROR_PARAMS(__FUNCTION__, typeid(*this).name()));
  }
  }

  return (result != NULL);
}


/*******************************************************************************
  For numeric items: convert this item to a double item. If the conversion is
  a lossy one, set "lossy" to true, otherwise set "lossy" to false. If "force"
  is true, the conversion is done always; otherwise, the conversion is done only
  if it is lossy.
********************************************************************************/
void AtomicItem::coerceToDouble(store::Item_t& result, bool force, bool& lossy) const
{
  xs_double doubleValue;

  result = NULL;

  switch (getTypeCode())
  {
  case XS_DECIMAL:
  {
    const DecimalItem* item = static_cast<const DecimalItem*>(this);

    doubleValue = xs_double::parseDecimal(item->theValue);

    xs_decimal const decValue(doubleValue);

    lossy = (decValue != item->theValue);
    break;
  }

  case XS_INTEGER:
  case XS_NON_POSITIVE_INTEGER:
  case XS_NEGATIVE_INTEGER:
  case XS_NON_NEGATIVE_INTEGER:
  case XS_POSITIVE_INTEGER:
  {
    const IntegerItem* item = static_cast<const IntegerItem*>(this);

    doubleValue = xs_double::parseInteger(item->theValue);

    xs_integer const intValue( doubleValue );

    lossy = (intValue != item->theValue);
    break;
  }

  case XS_UNSIGNED_LONG:
  {
    const UnsignedLongItem* item = static_cast<const UnsignedLongItem*>(this);

    doubleValue = xs_double::parseULong(item->theValue);

    xs_unsignedLong ulongValue = static_cast<xs_unsignedLong>(doubleValue.getNumber());

    lossy = (ulongValue != item->theValue);
    break;
  }
  case XS_UNSIGNED_INT:
  {
    const UnsignedIntItem* item = static_cast<const UnsignedIntItem*>(this);

    doubleValue = xs_double::parseUInt(item->theValue);
    lossy = false;
    break;
  }
  case XS_UNSIGNED_SHORT:
  {
    const UnsignedShortItem* item = static_cast<const UnsignedShortItem*>(this);

    doubleValue = xs_double::parseUInt(item->theValue);

    lossy = false;
    break;
  }
  case XS_UNSIGNED_BYTE:
  {
    const UnsignedByteItem* item = static_cast<const UnsignedByteItem*>(this);

    doubleValue = xs_double::parseUInt(item->theValue);

    lossy = false;
    break;
  }

  case XS_LONG:
  {
    const LongItem* item = static_cast<const LongItem*>(this);

    doubleValue = xs_double::parseLong(item->theValue);

    xs_long longValue = static_cast<xs_long>(doubleValue.getNumber());

    lossy = (longValue != item->theValue);
    break;
  }
  case XS_INT:
  {
    const IntItem* item = static_cast<const IntItem*>(this);

    doubleValue = xs_double::parseLong(item->theValue);
    lossy = false;
    break;
  }
  case XS_SHORT:
  {
    const ShortItem* item = static_cast<const ShortItem*>(this);

    doubleValue = xs_double::parseInt(item->theValue);
    lossy = false;
    break;
  }
  case XS_BYTE:
  {
    const ByteItem* item = static_cast<const ByteItem*>(this);

    doubleValue = xs_double::parseInt(item->theValue);
    lossy = false;
    break;
  }

  default:
  {
    throw ZORBA_EXCEPTION(
      zerr::ZSTR0050_FUNCTION_NOT_IMPLEMENTED_FOR_ITEMTYPE,
      ERROR_PARAMS( __FUNCTION__, typeid(*this).name() )
    );
  }
  }

  if (force || lossy)
    GET_FACTORY().createDouble(result, doubleValue);
}


/*******************************************************************************
  class UntypedAtomicItem
********************************************************************************/
bool UntypedAtomicItem::castToUri(store::Item_t& result) const
{
  result = NULL;

  try
  {
    URI uriVal(theValue);
    zstring tmp = uriVal.toString();
    return GET_FACTORY().createAnyURI(result, tmp);
  }
  catch (ZorbaException const&)
  {
    result = NULL;
  }

  return false;
}


bool UntypedAtomicItem::castToString(store::Item_t& result) const
{
  zstring tmp = theValue;
  return GET_FACTORY().createString(result, tmp);
}


bool UntypedAtomicItem::castToDateTime(store::Item_t& result) const
{
  return GET_FACTORY().createDateTime(result, theValue.c_str(), theValue.size());
}


bool UntypedAtomicItem::castToDate(store::Item_t& result) const
{
  return GET_FACTORY().createDate(result, theValue.c_str(), theValue.size());
}


bool UntypedAtomicItem::castToTime(store::Item_t& result) const
{
  return GET_FACTORY().createTime(result, theValue.c_str(), theValue.size());
}


bool UntypedAtomicItem::castToGYear(store::Item_t& result) const
{
  return GET_FACTORY().createGYear(result, theValue.c_str(), theValue.size());
}


bool UntypedAtomicItem::castToGYearMonth(store::Item_t& result) const
{
  return GET_FACTORY().createGYearMonth(result, theValue.c_str(), theValue.size());
}


bool UntypedAtomicItem::castToGMonthDay(store::Item_t& result) const
{
  return GET_FACTORY().createGMonthDay(result, theValue.c_str(), theValue.size());
}


bool UntypedAtomicItem::castToGMonth(store::Item_t& result) const
{
  return GET_FACTORY().createGMonth(result, theValue.c_str(), theValue.size());
}


bool UntypedAtomicItem::castToGDay(store::Item_t& result) const
{
  return GET_FACTORY().createGDay(result, theValue.c_str(), theValue.size());
}


bool UntypedAtomicItem::castToDuration(store::Item_t& result) const
{
  return GET_FACTORY().createDuration(result, theValue.c_str(), theValue.size());
}


bool UntypedAtomicItem::castToDouble(store::Item_t& result) const
{
  xs_double doubleValue;
  if (xs_double::parseString(theValue.c_str(), doubleValue))
  {
    return GET_FACTORY().createDouble(result, doubleValue);
  }
  result = NULL;
  return false;
}


bool UntypedAtomicItem::castToDecimal(store::Item_t& result) const
{
  try {
    xs_decimal const decValue(theValue.c_str());
    return GET_FACTORY().createDecimal(result, decValue);
  }
  catch ( std::exception const& ) {
    result = NULL;
    return false;
  }
}


bool UntypedAtomicItem::castToInteger(store::Item_t& result) const
{
  try {
    xs_integer const intValue(theValue.c_str());
    return GET_FACTORY().createInteger(result, intValue);
  }
  catch ( std::exception const& ) {
    result = NULL;
    return false;
  }
}


bool UntypedAtomicItem::castToHexBinary(store::Item_t& result) const
{
  Base16 value;
  if (Base16::parseString(theValue, value))
  {
    return GET_FACTORY().createHexBinary(result, value);
  }
  else
  {
    result = NULL;
    return false;
  }
}


bool UntypedAtomicItem::castToBase64Binary(store::Item_t& result) const
{
  Base64 value;
  if (Base64::parseString(theValue, value))
  {
    return GET_FACTORY().createBase64Binary(result, value);
  }
  else
  {
    result = NULL;
    return false;
  }
}


bool UntypedAtomicItem::castToBoolean(store::Item_t& result) const
{
  zstring str;
  ascii::trim_whitespace(theValue, &str);
  bool value = true;

  if (ZSTREQ(str, "false") || ZSTREQ(str, "0"))
  {
    value = false;
  }
  else if (!ZSTREQ(str, "true") && !ZSTREQ(str, "1"))
  {
    result = NULL;
    return false;
  }

  return GET_FACTORY().createBoolean(result, value);
}


store::Item* UntypedAtomicItem::getType() const
{
  return GET_STORE().theSchemaTypeNames[XS_UNTYPED_ATOMIC];
}


uint32_t UntypedAtomicItem::hash(long timezone, const XQPCollator* aCollation) const
{
  return utf8::hash(theValue);
}


store::Item_t UntypedAtomicItem::getEBV() const
{
  bool b = ! ( theValue == "" );
  store::Item_t bVal;
  CREATE_BOOLITEM(bVal, b);
  return bVal;
}


zstring UntypedAtomicItem::show() const
{
  zstring res("xs:untypedAtomic(");
  res += theValue;
  res += ")";
  return res;
}


/*******************************************************************************
  class QNameItem
********************************************************************************/

QNameItem::~QNameItem()
{
  if (isValid())
  {
    assert(theLocal.empty() || theNormQName == NULL);
  }
}


void QNameItem::free()
{
  GET_STORE().getQNamePool().remove(this);
}

QNameItem* QNameItem::getNormalized() const
{
  return (isNormalized() ? const_cast<QNameItem*>(this) : theNormQName.getp());
}

uint32_t QNameItem::hash(long timezone, const XQPCollator* aCollation) const
{
  const void* tmp = getNormalized();
  return hashfun::h32(&tmp, sizeof(void*), FNV_32_INIT);
}


store::Item* QNameItem::getType() const
{
  return GET_STORE().theSchemaTypeNames[XS_QNAME];
}


store::Item_t QNameItem::getEBV() const
{
  throw XQUERY_EXCEPTION(
    err::FORG0006,
    ERROR_PARAMS(
      ZED( OperationNotDef_23 ), ZED( EffectiveBooleanValue ), "QName"
    )
  );
}

bool QNameItem::equals(
        const store::Item* item,
        long timezone,
        const XQPCollator* aCollation) const
{
  return (getNormalized() == static_cast<const QNameItem*>(item)->getNormalized());
}

zstring QNameItem::getStringValue() const
{
  if (thePrefix.empty())
  {
    return getLocalName();
  }
  else
  {
    return (thePrefix + ":" + getLocalName());
  }
}


void QNameItem::getStringValue2(zstring& val) const
{
  if (thePrefix.empty())
  {
    val = getLocalName();
  }
  else
  {
    val.reserve(thePrefix.size() + getLocalName().size() + 1);
    val = thePrefix;
    val += ":";
    val += getLocalName();
  }
}


void QNameItem::appendStringValue(zstring& buf) const
{
  if (thePrefix.empty())
  {
    buf += getLocalName();
  }
  else
  {
    buf.reserve(buf.size() + thePrefix.size() + getLocalName().size() + 1);
    buf += thePrefix;
    buf += ":";
    buf += getLocalName();
  }
}


bool QNameItem::isIdQName() const
{
  if (ZSTREQ(getLocalName(), "id"))
  {
    if (ZSTREQ(getPrefix(), "xml") ||
        ztd::equals(theNamespace, SimpleStore::XML_URI, SimpleStore::XML_URI_LEN))
      return true;
  }

  return false;
}


bool QNameItem::isBaseUri() const
{
  if (ZSTREQ(getLocalName(), "base"))
  {
    if (ZSTREQ(getPrefix(), "xml") ||
        ztd::equals(getNamespace(), SimpleStore::XML_URI, SimpleStore::XML_URI_LEN))
      return true;
  }

  return false;
}


zstring QNameItem::show() const
{
  zstring res("xs:QName(");
  res += getNamespace();
  res += ",";
  res += getPrefix();
  res += ",";
  res += getLocalName();
  res += ")";
  return res;
}


/*******************************************************************************
  class NotationItem
********************************************************************************/

NotationItem::NotationItem(const zstring& nameSpace,
                           const zstring& prefix,
                           const zstring& localName)
{
  store::Item_t temp;
  GET_FACTORY().createQName(temp, nameSpace, prefix, localName);
  theQName = static_cast<QNameItem*>(temp.getp());
}


NotationItem::NotationItem(store::Item* qname)
{
  theQName = qname;
}


bool NotationItem::equals(const store::Item* item,
                          long timezone,
                          const XQPCollator* aCollation) const
{
  return (theQName->getNormalized() == static_cast<const NotationItem*>(item)->theQName->getNormalized());
}


store::Item* NotationItem::getType() const
{
  return GET_STORE().theSchemaTypeNames[XS_NOTATION];
}


zstring NotationItem::getStringValue() const
{
  return theQName->getStringValue();
}


void NotationItem::getStringValue2(zstring& val) const
{
  theQName->getStringValue2(val);
}


void NotationItem::appendStringValue(zstring& buf) const
{
  theQName->appendStringValue(buf);
}


zstring NotationItem::show() const
{
  zstring res("xs:NOTATION(");
  res += theQName->getStringValue();
  res += ")";
  return res;
}


NotationItem::~NotationItem()
{
}


/*******************************************************************************
  class AnyUriItem
********************************************************************************/
store::Item* AnyUriItem::getType() const
{
  return GET_STORE().theSchemaTypeNames[XS_ANY_URI];
}


uint32_t AnyUriItem::hash(long timezone, const XQPCollator* aCollation) const
{
  return hashfun::h32(theValue.data(), (uint32_t)theValue.size());
}


store::Item_t AnyUriItem::getEBV() const
{
  bool b = ! (theValue == "");
  store::Item_t bVal;
  CREATE_BOOLITEM(bVal, b);
  return bVal;
}


zstring AnyUriItem::show() const
{
  zstring res("xs:anyURI(");
  res += theValue;
  res += ")";
  return res;
}


/*******************************************************************************
  class StringItem
********************************************************************************/
store::Item* StringItem::getType() const
{
  return GET_STORE().theSchemaTypeNames[XS_STRING];
}


uint32_t StringItem::hash(long timezone, const XQPCollator* aCollation) const
{
  return utf8::hash(theValue, aCollation);
}


bool StringItem::equals(
    const store::Item* other,
    long timezone,
    const XQPCollator* aCollation) const
{
  if (aCollation == NULL || aCollation->doMemCmp())
    return theValue == other->getString();

  return (utf8::compare(theValue, other->getString(), aCollation) == 0);
}


long StringItem::compare(
    const Item* other,
    long timezone,
    const XQPCollator* aCollation) const
{
  return utf8::compare(theValue, other->getString(), aCollation);
}


store::Item_t StringItem::getEBV() const
{
  bool b = ! ( theValue == "" );
  store::Item_t bVal;
  CREATE_BOOLITEM(bVal, b);
  return bVal;
}


zstring StringItem::show() const
{
  zstring res("xs:string(");
  res += theValue;
  res += ")";
  return res;
}

#ifndef ZORBA_NO_FULL_TEXT

FTTokenIterator_t StringItem::getDocumentTokens(locale::iso639_1::type lang) const
{
  auto_ptr<Tokenizer> tokenizer( Tokenizer::create() );

  auto_ptr<NaiveFTTokenIterator::container_type> tokens(
    new NaiveFTTokenIterator::container_type
  );

  AtomicItemTokenizerCallback cb( *tokenizer, lang, *tokens );
  cb.tokenize( theValue.data(), theValue.size() );

  return FTTokenIterator_t( new NaiveFTTokenIterator( tokens.release() ) );
}


FTTokenIterator_t StringItem::getQueryTokens(
    locale::iso639_1::type lang,
    bool wildcards ) const
{
  auto_ptr<Tokenizer> tokenizer( Tokenizer::create( wildcards ) );

  auto_ptr<NaiveFTTokenIterator::container_type> tokens(
    new NaiveFTTokenIterator::container_type
  );

  AtomicItemTokenizerCallback cb( *tokenizer, lang, *tokens );
  cb.tokenize( theValue.data(), theValue.size() );

  return FTTokenIterator_t( new NaiveFTTokenIterator( tokens.release() ) );
}

#endif /* ZORBA_NO_FULL_TEXT */


/*******************************************************************************
  class StreamableStringItem
********************************************************************************/
void StreamableStringItem::appendStringValue(zstring& aBuf) const
{
  if (!theIsMaterialized) {
    materialize();
  }
  aBuf += theValue;
}


long StreamableStringItem::compare( 
    const Item* aOther,
    long aTimezone,
    const XQPCollator* aCollator) const
{
  if (!theIsMaterialized) {
    materialize();
  }
  return StringItem::compare(aOther, aTimezone, aCollator);
}


bool StreamableStringItem::equals( 
    store::Item const* aItem,
    long aTimezone,
    XQPCollator const* aCollator) const 
{
  if (!theIsMaterialized) {
    materialize();
  }
  return StringItem::equals(aItem, aTimezone, aCollator);
}


store::Item_t StreamableStringItem::getEBV() const
{
  if (!theIsMaterialized) {
    materialize();
  }
  return StringItem::getEBV();
}


zstring const& StreamableStringItem::getString() const
{
  if (!theIsMaterialized) {
    materialize();
  }
  return theValue;
}


zstring StreamableStringItem::getStringValue() const
{
  if (!theIsMaterialized) {
    materialize();
  }
  return theValue;
}


void StreamableStringItem::getStringValue2(zstring &val) const
{
  if (!theIsMaterialized) {
    materialize();
  }
  val = theValue;
}


uint32_t StreamableStringItem::hash(
    long aTimezone,
    XQPCollator const* aCollator) const
{
  if (!theIsMaterialized) {
    materialize();
  }
  return StringItem::hash(aTimezone, aCollator);
}


zstring StreamableStringItem::show() const
{
  if (!theIsMaterialized) {
    materialize();
  }
  return StringItem::show();
}


bool StreamableStringItem::isStreamable() const
{
  return true;
}


bool StreamableStringItem::isSeekable() const
{
  return theIsSeekable;
}


std::istream& StreamableStringItem::getStream()
{
  // a non-seekable stream can only be consumed once
  // we raise an error if getStream is called twice
  // if a query requires a stream to be consumed more than once,
  // the query needs to make sure that the stream is explicitly
  // materialized before
  if (!theIsSeekable && theIsConsumed) 
  {
    throw ZORBA_EXCEPTION( zerr::ZSTR0055_STREAMABLE_STRING_CONSUMED );
  }
  else
  {
    // if the stream is seekable, we seek to the beginning.
    // We are not using theIstream.seekg because the USER_ERROR that is thrown
    // by Zorba is lost possibly in an internal try/catch of the seekg
    std::streambuf * pbuf;
    pbuf = theIstream.rdbuf();
    pbuf->pubseekoff(0, std::ios::beg);
  }
  theIsConsumed = true;
  return theIstream;
}


void StreamableStringItem::materialize() const
{
  StreamableStringItem* const lSsi = const_cast<StreamableStringItem*>(this);
  std::istream& lStream = lSsi->getStream();

  lSsi->theIsMaterialized = true;
  lSsi->theIsConsumed = true;

  char lBuf[4096];
  while (theIstream) 
  {
    lStream.read(lBuf, sizeof(lBuf));
    lSsi->theValue.append(lBuf, static_cast<unsigned int>(lStream.gcount()));
  }
}


/*******************************************************************************
  class NormalizedStringItem
********************************************************************************/
store::Item* NormalizedStringItem::getType() const
{
  return GET_STORE().theSchemaTypeNames[XS_NORMALIZED_STRING];
}


zstring NormalizedStringItem::show() const
{
  zstring res("xs:NormalizedString(");
  res += theValue;
  res += ")";
  return res;
}


/*******************************************************************************
  class TokenItem
********************************************************************************/
store::Item* TokenItem::getType() const
{
  return GET_STORE().theSchemaTypeNames[XS_TOKEN];
}


zstring TokenItem::show() const
{
  zstring res("xs:TOKEN(");
  res += theValue;
  res += ")";
  return res;
}


/*******************************************************************************
  class LanguageItem
********************************************************************************/
store::Item* LanguageItem::getType() const
{
  return GET_STORE().theSchemaTypeNames[XS_LANGUAGE];
}


zstring LanguageItem::show() const
{
  zstring res("xs:LANGUAGE(");
  res += theValue;
  res += ")";
  return res;
}


/*******************************************************************************
  class NMTOKENItem
********************************************************************************/
store::Item* NMTOKENItem::getType() const
{
  return GET_STORE().theSchemaTypeNames[XS_NMTOKEN];
}


zstring NMTOKENItem::show() const
{
  zstring res("xs:NMTOKEN(");
  res += theValue;
  res += ")";
  return res;
}


/*******************************************************************************
  class NameItem
********************************************************************************/
store::Item* NameItem::getType() const
{
  return GET_STORE().theSchemaTypeNames[XS_NAME];
}


zstring NameItem::show() const
{
  zstring res("xs:NAME(");
  res += theValue;
  res += ")";
  return res;
}


/*******************************************************************************
  class NCNameItem
********************************************************************************/
store::Item* NCNameItem::getType() const
{
  return GET_STORE().theSchemaTypeNames[XS_NCNAME];
}


zstring NCNameItem::show() const
{
  zstring res("xs:NCName(");
  res += theValue;
  res += ")";
  return res;
}


/*******************************************************************************
  class IDItem
********************************************************************************/
store::Item* IDItem::getType() const
{
  return GET_STORE().theSchemaTypeNames[XS_ID];
}


zstring IDItem::show() const
{
  zstring res("xs:ID(");
  res += theValue;
  res += ")";
  return res;
}


/*******************************************************************************
  class IDREFItem
********************************************************************************/
store::Item* IDREFItem::getType() const
{
  return GET_STORE().theSchemaTypeNames[XS_IDREF];
}


zstring IDREFItem::show() const
{
  zstring res("xs:IDREF(");
  res += theValue;
  res += ")";
  return res;
}


/*******************************************************************************
  class ENTITYItem
********************************************************************************/
store::Item* ENTITYItem::getType() const
{
  return GET_STORE().theSchemaTypeNames[XS_ENTITY];
}

zstring ENTITYItem::show() const
{
  zstring res("xs:ENTITY(");
  res += theValue;
  res += ")";
  return res;
}


/*******************************************************************************
  class DateTimeItem
********************************************************************************/
zstring DateTimeItem::getStringValue() const
{
  return theValue.toString();
}


void DateTimeItem::getStringValue2(zstring& val) const
{
  val = theValue.toString();
}

void DateTimeItem::appendStringValue(zstring& buf) const
{
  buf += theValue.toString();
}


SchemaTypeCode DateTimeItem::getTypeCode() const
{
  switch (theValue.getFacet())
  {
  case DateTime::GYEARMONTH_FACET:
    return XS_GYEAR_MONTH;

  case DateTime::GYEAR_FACET:
    return XS_GYEAR;

  case DateTime::GMONTH_FACET:
    return XS_GMONTH;

  case DateTime::GMONTHDAY_FACET:
    return XS_GMONTH_DAY;

  case DateTime::GDAY_FACET:
    return XS_GDAY;

  case DateTime::DATE_FACET:
    return XS_DATE;

  case DateTime::TIME_FACET:
    return XS_TIME;

  case DateTime::DATETIME_FACET:
    return XS_DATETIME;

  default:
    ZORBA_ASSERT(false);
  }
}


store::Item* DateTimeItem::getType() const
{
  return GET_STORE().theSchemaTypeNames[getTypeCode()];
}


bool DateTimeItem::equals(
    const store::Item* aItem,
    long timezone,
    const XQPCollator* coll) const
{
  try
  {
    return 0 == theValue.compare(&aItem->getDateTimeValue(), timezone);
  }
  catch (InvalidTimezoneException const&)
  {
    throw XQUERY_EXCEPTION(err::FODT0003);
  }
}


long DateTimeItem::compare(
    const Item* other,
    long timezone,
    const XQPCollator* aCollation) const
{
  try
  {
    return theValue.compare(&other->getDateTimeValue(), timezone);
  }
  catch (InvalidTimezoneException const&)
  {
    throw XQUERY_EXCEPTION(err::FODT0003);
  }
}


uint32_t DateTimeItem::hash(long timezone, const XQPCollator* aCollation) const
{
  return theValue.hash(0);
}


store::Item_t DateTimeItem::getEBV() const
{
  switch (theValue.getFacet())
  {
    case DateTime::DATE_FACET:
      throw XQUERY_EXCEPTION(
        err::FORG0006,
        ERROR_PARAMS(
          ZED( OperationNotDef_23 ), ZED( EffectiveBooleanValue ), "xs:Date"
        )
      );

    case DateTime::TIME_FACET:
      throw XQUERY_EXCEPTION(
        err::FORG0006,
        ERROR_PARAMS(
          ZED( OperationNotDef_23 ), ZED( EffectiveBooleanValue ), "xs:Time"
        )
      );

    case DateTime::GYEARMONTH_FACET:
      throw XQUERY_EXCEPTION(
        err::FORG0006,
        ERROR_PARAMS(
          ZED( OperationNotDef_23 ), ZED( EffectiveBooleanValue ),
          "xs:GYearMonth"
        )
      );

    case DateTime::GYEAR_FACET:
      throw XQUERY_EXCEPTION(
        err::FORG0006,
        ERROR_PARAMS(
          ZED( OperationNotDef_23 ), ZED( EffectiveBooleanValue ), "xs:GYear"
        )
      );

    case DateTime::GMONTH_FACET:
      throw XQUERY_EXCEPTION(
        err::FORG0006,
        ERROR_PARAMS(
          ZED( OperationNotDef_23 ), ZED( EffectiveBooleanValue ), "xs:GMonth"
        )
      );

    case DateTime::GMONTHDAY_FACET:
      throw XQUERY_EXCEPTION(
        err::FORG0006,
        ERROR_PARAMS(
          ZED( OperationNotDef_23 ), ZED( EffectiveBooleanValue ),
          "xs:GMonthDay"
        )
      );

    case DateTime::GDAY_FACET:
      throw XQUERY_EXCEPTION(
        err::FORG0006,
        ERROR_PARAMS(
          ZED( OperationNotDef_23 ), ZED( EffectiveBooleanValue ), "xs:GDay"
        )
      );

    default:
      throw XQUERY_EXCEPTION(
        err::FORG0006,
        ERROR_PARAMS(
          ZED( OperationNotDef_23 ), ZED( EffectiveBooleanValue ), "dateTime"
        )
      );
  }
  return NULL;
}


zstring DateTimeItem::show() const
{
  return theValue.toString();
}


/*******************************************************************************
  class DurationItem
********************************************************************************/
const xs_duration& DurationItem::getDurationValue() const
{
  return theValue;
}


const xs_dayTimeDuration& DurationItem::getDayTimeDurationValue() const
{
  return theValue;
}


const xs_yearMonthDuration& DurationItem::getYearMonthDurationValue() const
{
  return theValue;
}


zstring DurationItem::getStringValue() const
{
  return theValue.toString();
}


void DurationItem::getStringValue2(zstring& val) const
{
  val = theValue.toString();
}


void DurationItem::appendStringValue(zstring& buf) const
{
  buf += theValue.toString();
}


SchemaTypeCode DurationItem::getTypeCode() const
{
  switch (theValue.getFacet())
  {
  case Duration::DURATION_FACET:
    return XS_DURATION;

  case Duration::DAYTIMEDURATION_FACET:
    return XS_DT_DURATION;

  case Duration::YEARMONTHDURATION_FACET:
  default:
    return XS_YM_DURATION;
  }
}


store::Item* DurationItem::getType() const
{
  return GET_STORE().theSchemaTypeNames[getTypeCode()];
}


store::Item_t DurationItem::getEBV() const
{
  throw XQUERY_EXCEPTION(
    err::FORG0006,
    ERROR_PARAMS(
      ZED( OperationNotDef_23 ), ZED( EffectiveBooleanValue ), "duration"
    )
  );
}


zstring DurationItem::show() const
{
  return theValue.toString();
}



/*******************************************************************************
  class DoubleItem
********************************************************************************/
store::Item* DoubleItem::getType() const
{
  return GET_STORE().theSchemaTypeNames[XS_DOUBLE];
}


store::Item_t DoubleItem::getEBV() const
{
  bool b;
  if (theValue.isNaN())
  {
    b = false;
  }
  else
  {
    b = !theValue.isZero();
  }
  store::Item_t bVal;
  CREATE_BOOLITEM(bVal, b);
  return bVal;
}


zstring DoubleItem::getStringValue() const
{
  return theValue.toString();
}


void DoubleItem::getStringValue2(zstring& val) const
{
  val = theValue.toString();
}


void DoubleItem::appendStringValue(zstring& buf) const
{
  buf += theValue.toString();
}


zstring DoubleItem::show() const
{
  zstring res("xs:double(");
  appendStringValue(res);
  res += ")";
  return res;
}


bool DoubleItem::isNaN() const
{
  return theValue.isNaN();
}


bool DoubleItem::isPosOrNegInf() const
{
  return theValue.isPosInf() || theValue.isNegInf();
}


uint32_t
DoubleItem::hash(long timezone, const XQPCollator* aCollation) const
{
  return theValue.hash();
}


/*******************************************************************************
  class FloatItem
********************************************************************************/
store::Item* FloatItem::getType() const
{
  return GET_STORE().theSchemaTypeNames[XS_FLOAT];
}


store::Item_t FloatItem::getEBV() const
{
  bool b;
  if (theValue.isNaN()) {
    b = false;
  } else {
    b = !theValue.isZero();
  }
  store::Item_t bVal;
  CREATE_BOOLITEM(bVal, b);
  return bVal;
}

zstring FloatItem::getStringValue() const
{
  return theValue.toString();
}


void FloatItem::getStringValue2(zstring& val) const
{
  val = theValue.toString();
}


void FloatItem::appendStringValue(zstring& buf) const
{
  buf += theValue.toString();
}


zstring FloatItem::show() const
{
  zstring res("xs:float(");
  appendStringValue(res);
  res += ")";
  return res;
}


bool FloatItem::isNaN() const
{
  return theValue.isNaN();
}


bool FloatItem::isPosOrNegInf() const
{
  return theValue.isPosInf() || theValue.isNegInf();
}


uint32_t FloatItem::hash(long timezone, const XQPCollator* aCollation) const
{
  return theValue.hash();
}


/*******************************************************************************
  class DecimalItem
********************************************************************************/

store::Item* DecimalItem::getType() const
{
  return GET_STORE().theSchemaTypeNames[XS_DECIMAL];
}


store::Item_t DecimalItem::getEBV() const
{
  bool b = ( theValue != xs_decimal::zero() );
  store::Item_t bVal;
  CREATE_BOOLITEM(bVal, b);
  return bVal;
}

zstring DecimalItem::getStringValue() const
{
  return theValue.toString();
}


void DecimalItem::getStringValue2(zstring& val) const
{
  val = theValue.toString();
}


void DecimalItem::appendStringValue(zstring& buf) const
{
  buf += theValue.toString();
}


bool DecimalItem::isNaN() const
{
  return theValue != theValue;
}


zstring DecimalItem::show() const
{
  zstring res("xs:decimal(");
  appendStringValue(res);
  res +=  ")";
  return res;
}


/*******************************************************************************
  class IntegerItem
********************************************************************************/

long IntegerItem::compare( Item const *other, long, const XQPCollator* ) const {
  try {
    return theValue.compare( other->getIntegerValue() );
  }
  catch ( ZorbaException const& ) {
    return getDecimalValue().compare( other->getDecimalValue() );
  }
}

bool IntegerItem::equals( const store::Item* other, long,
                          const XQPCollator*) const
{
  try
  {
    return theValue == other->getIntegerValue();
  }
  catch (ZorbaException const&)
  {
    return getDecimalValue() == other->getDecimalValue();
  }
}

xs_decimal IntegerItem::getDecimalValue() const
{
  return xs_decimal(theValue);
}


xs_long IntegerItem::getLongValue() const
{
  xs_long longValue;

  if (NumConversions::integerToLong(theValue, longValue))
    return longValue;

  throw XQUERY_EXCEPTION(
    err::FORG0001,
    ERROR_PARAMS( theValue, ZED( CastFromToFailed_34 ), "integer", "long" )
  );
}


store::Item* IntegerItem::getType() const
{
  return GET_STORE().theSchemaTypeNames[XS_INTEGER];
}


store::Item_t IntegerItem::getEBV() const
{
  bool b = ( theValue != xs_integer::zero() );
  store::Item_t bVal;
  CREATE_BOOLITEM(bVal, b);
  return bVal;
}


zstring IntegerItem::getStringValue() const
{
  return theValue.toString();
}


void IntegerItem::getStringValue2(zstring& val) const
{
  val = theValue.toString();
}

uint32_t IntegerItem::hash(long, const XQPCollator*) const
{
  return theValue.hash();
}

void IntegerItem::appendStringValue(zstring& buf) const
{
  buf += theValue.toString();
}


zstring IntegerItem::show() const
{
  zstring res("xs:integer(");
  appendStringValue(res);
  res += ")";
  return res;
}


/*******************************************************************************
  class NonPositiveIntegerItem
********************************************************************************/
store::Item* NonPositiveIntegerItem::getType() const
{
  return GET_STORE().theSchemaTypeNames[XS_NON_POSITIVE_INTEGER];
}

zstring NonPositiveIntegerItem::show() const
{
  zstring res("xs:nonPositiveInteger(");
  appendStringValue(res);
  res += ")";
  return res;
}


/*******************************************************************************
  class NegativeIntegerItem
********************************************************************************/
store::Item* NegativeIntegerItem::getType() const
{
  return GET_STORE().theSchemaTypeNames[XS_NEGATIVE_INTEGER];
}


zstring NegativeIntegerItem::show() const
{
  zstring res("xs:negativeInteger(");
  appendStringValue(res);
  res += ")";
  return res;
}


/*******************************************************************************
  class NonNegativeINtegerItem
********************************************************************************/
store::Item* NonNegativeIntegerItem::getType() const
{
  return GET_STORE().theSchemaTypeNames[XS_NON_NEGATIVE_INTEGER];
}


zstring NonNegativeIntegerItem::show() const
{
  zstring res("xs:nonNegativeInteger(");
  appendStringValue(res);
  res += ")";
  return res;
}


/*******************************************************************************
  class PositiveIntegerItem
********************************************************************************/
store::Item* PositiveIntegerItem::getType() const
{
  return GET_STORE().theSchemaTypeNames[XS_POSITIVE_INTEGER];
}


zstring PositiveIntegerItem::show() const
{
  zstring res("xs:positiveInteger(");
  appendStringValue(res);
  res += ")";
  return res;
}


/*******************************************************************************
  class LongItem
********************************************************************************/
xs_decimal LongItem::getDecimalValue() const
{
  return xs_decimal(theValue);
}


xs_integer LongItem::getIntegerValue() const
{
  return xs_integer(theValue);
}


store::Item* LongItem::getType() const
{
  return GET_STORE().theSchemaTypeNames[XS_LONG];
}


store::Item_t LongItem::getEBV() const
{
  bool b = (theValue != 0);
  store::Item_t bVal;
  CREATE_BOOLITEM(bVal, b);
  return bVal;
}


zstring LongItem::getStringValue() const
{
  zstring result;
  ztd::to_string(theValue, &result);
  return result;
}


void LongItem::getStringValue2(zstring& val) const
{
  ztd::to_string(theValue,&val);
}


void LongItem::appendStringValue(zstring& buf) const
{
  zstring temp;
  ztd::to_string(theValue, &temp);
  buf += temp;
}


zstring LongItem::show() const
{
  zstring res("xs:long(");
  appendStringValue(res);
  res += ")";
  return res;
}


/*******************************************************************************
  class IntItem
********************************************************************************/
xs_decimal IntItem::getDecimalValue() const
{
  return xs_decimal(theValue);
}


xs_integer IntItem::getIntegerValue() const
{
  return Integer( theValue );
}


store::Item* IntItem::getType() const
{
  return GET_STORE().theSchemaTypeNames[XS_INT];
}


store::Item_t IntItem::getEBV() const
{
  bool b = ( theValue != (int32_t)0 );
  store::Item_t bVal;
  CREATE_BOOLITEM(bVal, b);
  return bVal;
}

zstring IntItem::getStringValue() const
{
  zstring result;
  ztd::to_string(theValue, &result);
  return result;
}


void IntItem::getStringValue2(zstring& val) const
{
  ztd::to_string(theValue, &val);
}


void IntItem::appendStringValue(zstring& buf) const
{
  zstring temp;
  ztd::to_string(theValue, &temp);
  buf += temp;
}


zstring IntItem::show() const
{
  zstring res("xs:int(");
  appendStringValue(res);
  res += ")";
  return res;
}


/*******************************************************************************
  class ShortItem
********************************************************************************/
xs_decimal ShortItem::getDecimalValue() const
{
  return xs_decimal( theValue );
}


xs_integer ShortItem::getIntegerValue() const
{
  return Integer(theValue);
}


store::Item* ShortItem::getType() const
{
  return GET_STORE().theSchemaTypeNames[XS_SHORT];
}


store::Item_t ShortItem::getEBV() const
{
  bool b = (theValue != 0);
  store::Item_t bVal;
  CREATE_BOOLITEM(bVal, b);
  return bVal;
}


zstring ShortItem::getStringValue() const
{
  zstring result;
  ztd::to_string(theValue, &result);
  return result;
}


void ShortItem::getStringValue2(zstring& val) const
{
  ztd::to_string(theValue, &val);
}


void ShortItem::appendStringValue(zstring& buf) const
{
  zstring temp;
  ztd::to_string(theValue, &temp);
  buf += temp;
}


zstring ShortItem::show() const
{
  zstring res("xs:short(");
  appendStringValue(res);
  res += ")";
  return res;
}


/*******************************************************************************
  class ByteItem
********************************************************************************/
xs_decimal ByteItem::getDecimalValue() const
{
  return xs_decimal(theValue);
}


xs_integer ByteItem::getIntegerValue() const
{
  return Integer(theValue);
}


store::Item* ByteItem::getType() const
{
  return GET_STORE().theSchemaTypeNames[XS_BYTE];
}


store::Item_t ByteItem::getEBV() const
{
  bool b = (theValue != 0);
  store::Item_t bVal;
  CREATE_BOOLITEM(bVal, b);
  return bVal;
}


zstring ByteItem::getStringValue() const
{
  zstring result;
  ztd::to_string(theValue, &result);
  return result;
}


void ByteItem::getStringValue2(zstring& val) const
{
  ztd::to_string(theValue, &val);
}


void ByteItem::appendStringValue(zstring& buf) const
{
  zstring temp;
  ztd::to_string(theValue, &temp);
  buf += temp;
}


zstring ByteItem::show() const
{
  zstring res("xs:byte(");
  appendStringValue(res);
  res += ")";
  return res;
}


/*******************************************************************************
  class UnsignedLongItem
********************************************************************************/
xs_decimal UnsignedLongItem::getDecimalValue() const
{
  return xs_decimal(theValue);
}


xs_integer UnsignedLongItem::getIntegerValue() const
{
  return xs_integer(theValue);
}


xs_uinteger UnsignedLongItem::getUnsignedIntegerValue() const
{
  return xs_uinteger(theValue);
}


store::Item* UnsignedLongItem::getType() const
{
  return GET_STORE().theSchemaTypeNames[XS_UNSIGNED_LONG];
}


store::Item_t UnsignedLongItem::getEBV() const
{
  bool b = (theValue != 0);
  store::Item_t bVal;
  CREATE_BOOLITEM(bVal, b);
  return bVal;
}


zstring UnsignedLongItem::getStringValue() const
{
  zstring result;
  ztd::to_string(theValue, &result);
  return result;
}


void UnsignedLongItem::getStringValue2(zstring& val) const
{
  ztd::to_string(theValue, &val);
}


void UnsignedLongItem::appendStringValue(zstring& buf) const
{
  zstring temp;
  ztd::to_string(theValue, &temp);
  buf += temp;
}


zstring UnsignedLongItem::show() const
{
  zstring res("xs:unsignedLong(");
  appendStringValue(res);
  res += ")";
  return res;
}


/*******************************************************************************
  class UnsignedIntItem
********************************************************************************/
xs_decimal UnsignedIntItem::getDecimalValue() const
{
  return xs_decimal(theValue);
}


xs_integer UnsignedIntItem::getIntegerValue() const
{
  return Integer(theValue);
}


xs_uinteger UnsignedIntItem::getUnsignedIntegerValue() const
{
  return Integer(theValue);
}


store::Item* UnsignedIntItem::getType() const
{
  return GET_STORE().theSchemaTypeNames[XS_UNSIGNED_INT];
}


store::Item_t UnsignedIntItem::getEBV() const
{
  bool b = (theValue != 0);
  store::Item_t bVal;
  CREATE_BOOLITEM(bVal, b);
  return bVal;
}


zstring UnsignedIntItem::getStringValue() const
{
  zstring result;
  ztd::to_string(theValue, &result);
  return result;
}


void UnsignedIntItem::getStringValue2(zstring& val) const
{
  ztd::to_string(theValue, &val);
}


void UnsignedIntItem::appendStringValue(zstring& buf) const
{
  zstring temp;
  ztd::to_string(theValue, &temp);
  buf += temp;
}


zstring UnsignedIntItem::show() const
{
  zstring res("xs:unsignedInt(");
  appendStringValue(res);
  res += ")";
  return res;
}


/*******************************************************************************
  class UnsignedShortItem
********************************************************************************/
xs_decimal UnsignedShortItem::getDecimalValue() const
{
  return xs_decimal(theValue);
}


xs_integer UnsignedShortItem::getIntegerValue() const
{
  return Integer(theValue);
}


xs_uinteger UnsignedShortItem::getUnsignedIntegerValue() const
{
  return Integer(theValue);
}


store::Item* UnsignedShortItem::getType() const
{
  return GET_STORE().theSchemaTypeNames[XS_UNSIGNED_SHORT];
}


store::Item_t UnsignedShortItem::getEBV() const
{
  bool b = (theValue != 0);
  store::Item_t bVal;
  CREATE_BOOLITEM(bVal, b);
  return bVal;
}


zstring UnsignedShortItem::getStringValue() const
{
  zstring result;
  ztd::to_string(theValue, &result);
  return result;
}


void UnsignedShortItem::getStringValue2(zstring& val) const
{
  ztd::to_string(theValue, &val);
}


void UnsignedShortItem::appendStringValue(zstring& buf) const
{
  zstring temp;
  ztd::to_string(theValue, &temp);
  buf += temp;
}


zstring UnsignedShortItem::show() const
{
  zstring res("xs:unsignedShort(");
  appendStringValue(res);
  res += ")";
  return res;
}


/*******************************************************************************
  class UnsignedByteItem
********************************************************************************/
xs_decimal UnsignedByteItem::getDecimalValue() const
{
  return xs_decimal(theValue);
}


xs_integer UnsignedByteItem::getIntegerValue() const
{
  return Integer((uint32_t)theValue);
}


xs_uinteger UnsignedByteItem::getUnsignedIntegerValue() const
{
  return Integer(theValue);
}


store::Item* UnsignedByteItem::getType() const
{
  return GET_STORE().theSchemaTypeNames[XS_UNSIGNED_BYTE];
}


store::Item_t UnsignedByteItem::getEBV() const
{
  bool b = (theValue != 0);
  store::Item_t bVal;
  CREATE_BOOLITEM(bVal, b);
  return bVal;
}


zstring UnsignedByteItem::getStringValue() const
{
  zstring result;
  ztd::to_string(theValue, &result);
  return result;
}


void UnsignedByteItem::getStringValue2(zstring& val) const
{
  ztd::to_string(theValue, &val);
}


void UnsignedByteItem::appendStringValue(zstring& buf) const
{
  zstring temp;
  ztd::to_string(theValue, &temp);
  buf += temp;
}


zstring UnsignedByteItem::show() const
{
  zstring res("xs:unsignedByte(");
  appendStringValue(res);
  res += ")";
  return res;
}


/*******************************************************************************
  class BooleanItem
********************************************************************************/
store::Item* BooleanItem::getType() const
{
  return GET_STORE().theSchemaTypeNames[XS_BOOLEAN];
}


uint32_t BooleanItem::hash(long timezone, const XQPCollator* aCollation) const
{
  return theValue ? 0 : 1;
}


store::Item_t BooleanItem::getEBV() const
{
  return this->getAtomizationValue();
}


zstring BooleanItem::getStringValue() const
{
  if (theValue)
    return "true";
  else
    return "false";
}


void BooleanItem::getStringValue2(zstring& val) const
{
  if (theValue)
    val = "true";
  else
    val = "false";
}


void BooleanItem::appendStringValue(zstring& buf) const
{
  if (theValue)
    buf += "true";
  else
    buf += "false";
}


zstring BooleanItem::show() const
{
  zstring res("xs:boolean(");
  appendStringValue(res);
  res += ")";
  return res;
}


/*******************************************************************************
  class Base64BinaryItem
********************************************************************************/
store::Item* Base64BinaryItem::getType() const
{
  return GET_STORE().theSchemaTypeNames[XS_BASE64BINARY];
}


zstring Base64BinaryItem::getStringValue() const
{
  return theValue.str();
}


void Base64BinaryItem::getStringValue2(zstring& val) const
{
  val = theValue.str();
}


void Base64BinaryItem::appendStringValue(zstring& buf) const
{
  buf += theValue.str();
}


zstring Base64BinaryItem::show() const
{
  zstring res("xs:base64Binary(");
  appendStringValue(res);
  res += ")";
  return res;
}


uint32_t Base64BinaryItem::hash(long timezone, const XQPCollator* aCollation) const
{
  return theValue.hash();
}


/*******************************************************************************
  class HexBinaryItem
********************************************************************************/
store::Item* HexBinaryItem::getType() const
{
  return GET_STORE().theSchemaTypeNames[XS_HEXBINARY];
}


zstring HexBinaryItem::getStringValue() const
{
  return theValue.str();
}


void HexBinaryItem::getStringValue2(zstring& val) const
{
  val = theValue.str();
}


void HexBinaryItem::appendStringValue(zstring& buf) const
{
  buf += theValue.str();
}


zstring HexBinaryItem::show() const
{
  zstring res("xs:hexBinary(");
  appendStringValue(res);
  res += ")";
  return res;
}


uint32_t HexBinaryItem::hash(long timezone, const XQPCollator* aCollation) const
{
  return theValue.hash();
}


/*******************************************************************************
  class ErrorItem
********************************************************************************/
ErrorItem::~ErrorItem()
{
  if (theError)
  {
    //theError->free();
    theError = NULL;
  }
}


zstring ErrorItem::show() const
{
  return theError->what();
}


/*******************************************************************************
  class AtomicItemTokenizerCallback
********************************************************************************/
#ifndef ZORBA_NO_FULL_TEXT
void AtomicItemTokenizerCallback::operator()(
    char const *utf8_s,
    size_t utf8_len,
    int_t token_no, 
    int_t sent_no,
    int_t para_no,
    void* )
{
  FTToken const t( utf8_s, utf8_len, token_no, lang_ );
  tokens_.push_back( t );
}
#endif /* ZORBA_NO_FULL_TEXT */


} // namespace simplestore
} // namespace zorba
/* vim:set et sw=2 ts=2: */
