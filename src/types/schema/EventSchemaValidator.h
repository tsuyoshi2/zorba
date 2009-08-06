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

/*
 * Code modified from Apache Xerces's SchemaValidator.
 *   - added validateProcessorStipulatedTypeName
 *      - a XMLSchema 1.0 feature not implemented by Xerces
 */

#ifndef ZORBA_EVENTSCHEMAVALIDATOR_H_
#define ZORBA_EVENTSCHEMAVALIDATOR_H_

#include "common/common.h"
#ifndef ZORBA_NO_XMLSCHEMA

//#include "types/schema/SchemaValidatorFilter.h"
//#include "xercesIncludes.h"
//#include "types/schema/StrX.h"
#include "types/schema/ValidationEventHandler.h"
//#include "store/api/item.h"
#include "common/shared_types.h"

namespace XERCES_CPP_NAMESPACE {
  class GrammarResolver;
  class XMLGrammarPool;
}

namespace zorba
{

  class TypeManager;
  class SchemaValidatorFilter;


class EventSchemaValidator
{
private:
  TypeManager                            * _typeManager;
  SchemaValidatorFilter                  * _schemaValidatorFilter;
  XERCES_CPP_NAMESPACE::GrammarResolver  * _grammarResolver;
  ValidationEventHandler                   _validationEventHandler;
    
public:
  EventSchemaValidator(
        TypeManager* typeManager,
        XERCES_CPP_NAMESPACE::XMLGrammarPool* grammarPool,
        bool isLax,
        const QueryLoc& loc);

  virtual ~EventSchemaValidator();

  void startDoc();
  void endDoc();

  void startElem(store::Item_t elemName);
  void endElem(store::Item_t elemName);

  // Must be called only after all ns() have been send if any otherwise only after startElem()
  void attr(store::Item_t attrName, xqpStringStore_t textValue);
  void endAttrs();
  void text(xqpStringStore_t textValue);
    
  // Must be called only after startElem()
  void ns(xqpStringStore_t prefix, xqpStringStore_t uri);

  // Must be called only after all attr() have been sent
  store::Item_t getTypeQName();
  xqtref_t      getType();
  store::Item_t getSubstitutedElemQName();
    
  // Must be called only after all attr() have been sent and after getTypeQName() has been called
  std::list<AttributeValidationInfo*>* getAttributeList()
  {
    return _validationEventHandler.getAttributeList();
  }
    
  // Must be called only after text() has been called
  TextValidationInfo* getTextInfo()
  {
    return _validationEventHandler.getTextInfo();
  }

  // for validating content of a schema type
  void startType(store::Item_t typeName);
  void endType();
};

} // namespace zorba

#endif //ZORBA_NO_XMLSCHEMA
#endif /*ZORBA_EVENTSCHEMAVALIDATOR_H_*/

/*
 * Local variables:
 * mode: c++
 * End:
 */
