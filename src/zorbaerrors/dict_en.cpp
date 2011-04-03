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

#include "dict_impl.h"

namespace zorba {
namespace err {
namespace dict {

///////////////////////////////////////////////////////////////////////////////

extern entry const dict_en[] = {

  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // !!!!! THIS MUST BE ASCII SORTED BY KEY !!!!!
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  { "API0001", "empty query" },
  { "API0002", "XQuery compilation failed${: 1}" },
  { "API0003", "XQuery not compiled" },
  { "API0004", "XQuery already compiled" },
  { "API0005", "XQuery already executing" },
  { "API0006", "XQuery already closed" },
  { "API0007", "cannot serialize pul" },
  { "API0008", "not an update XQuery" },
  { "API0009", "XQuery not compiled in debug mode" },
  { "API0010", "XQuery has iterator already" },
  { "API0040", "iterator is not open" },
  { "API0041", "iterator is open already" },
  { "API0042", "iterator query is closed" },
  { "API0010", "thread not initialized" },
  { "API0011", "\"$1\": invalid variable QName" },
  { "API0012", "invalid index" },
  { "API0013", "\"$1\": inappropriate variable value" },
  { "API0014", "\"$1\": invalid argument${: 2}" },
  { "API0015", "\"$1\": cannot open file" },
  { "API0016", "cannot create collection" },
  { "API0017", "cannot load document" },
  { "API0018", "\"$1\": cannot access file" },
  { "API0019", "\"$1\": function already registered" },
  { "API0020", "document already exists" },
  { "API0021", "item to load is not node" },
  { "API0022", "\"$1\": not plain file" },
  { "API0025", "end of sequence" },
  { "API0027", "cannot update dctx with iterators" },
  { "API0028", "invalid node URI" },
  { "API0030", "no node at given position" },
  { "API0033", "\"$1\": file or folder does not exist" },
  { "API0034", "\"$1\": file or folder cannot be opened" },
  { "API0035", "collection cannot be serialized" },
  { "API0050", "REST error header" },
  { "API0051", "REST error payload" },
  { "API0060", "CONV JSON PARSE" },
  { "API0061", "\"$1\": could not serialize element" },
  { "API0062", "CONV JSON PARAM" },
  { "API0063", "CONV JSON ML PARSE" },
  { "API0064", "CONV JSON ML SERIALIZE" },
  { "API0065", "CONV JSON ML PARAM" },
  { "API0066", "JSON sequence cannot be serialized" },
  { "API0070", "invalid serialization method for SAX" },

  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // !!!!! THIS MUST BE ASCII SORTED BY KEY !!!!!
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  { "FOAR0001", "division by zero" },
  { "FOAR0002", "numeric operation overflow/underflow" },
  { "FOCA0001", "\"$1\": value too large for decimal" },
  { "FOCA0002", "\"$1\": invalid lexical value" },
  { "FOCA0003", "\"$1\": value too large for integer" },
  { "FOCA0005", "NaN supplied as float/double value" },
  { "FOCA0006", "\"$1\": string to be cast to decimal has too many digits of precision" },
  { "FOCH0001", "\"$1\": invalid code-point" },
  { "FOCH0002", "\"$1\": unsuported collation" },
  { "FOCH0003", "\"$1\": unsupported normalization form" },
  { "FOCH0004", "collation does not support collation units" },
  { "FODC0001", "no context document" },
  { "FODC0002", "\"$1\": error retrieving resource" },
  { "FODC0003", "function stability not defined" },
  { "FODC0004", "\"$1\": invalid argument to fn:collection()" },
  { "FODC0005", "\"$1\": invalid argument to fn:doc() or fn:doc-available()" },
  { "FODC0006", "\"$1\": invalid content passed to fn:parse()" },
  { "FODC0007", "\"$1\": base URI passed to fn:parse() is not a valid absolute URI" },
  { "FODF1280", "\"$1\": invalid decimal format name for fn:format-number()" },
  { "FODF1310", "\"$1\": invalid fn:format-number() picture string" },
  { "FODT0001", "overflow/underflow in date/time operation" },
  { "FODT0002", "overflow/underflow in duration operation" },
  { "FODT0003", "\"$1\": invalid timezone value" },
  { "FOER0000", "unidentifier error" },
  { "FOFI0001", "$language argument is not castable to xs:language" },
  { "FOFI0002", "invalid argument in $1" },
  { "FONS0004", "\"$1\": no namespace found for prefix" },
  { "FONS0005", "base-URI not defined in the static context" },
  { "FORG0001", "\"$1\": invalid value for cast/constructor${: 2}" },
  { "FORG0002", "\"$1\": invalid argument to fn:resolve-uri()" },
  { "FORG0003", "fn:zero-or-one() called with a sequnce containing more than one item" },
  { "FORG0004", "fn:one-or-more() called with a sequence containing no items" },
  { "FORG0005", "fn:exactly-one() called with a sequence containing zero or more than one item" },
  { "FORG0006", "invalid argument type${: 1}" },
  { "FORG0008", "the two arguments to fn:dateTime() have inconsistent timezones" },
  { "FORG0009", "error in resolving a relative URI against a base URI in fn:resolve-uri()" },
  { "FORX0001", "'$1': invalid regular expression flag" },
  { "FORX0002", "\"$1\": invalid regular expression${: 2}" },
  { "FORX0003", "regular expression matches zero-length string" },
  { "FORX0004", "invalid replacement string${: 1}" },
  { "FOTY0012", "argument node does not have a typed value" },
  { "FOTY0013", "argument to fn:data() contains untyped node value" },
  { "FOTY0014", "argument to fn:string() is a function item" },
  { "FOTY0015", "argument to fn:deep-equal() contains a function item" },
  { "FOUP0001", "first operand of fn:put() is not a node of a supported kind" },
  { "FOUP0002", "second operand of fn:put() is not a valid lexical representation of the xs:anyURI type" },

  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // !!!!! THIS MUST BE ASCII SORTED BY KEY !!!!!
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#ifndef ZORBA_NO_FULL_TEXT
  { "FTDY0016", "\"$1\": invalid weight: absolute value must be in [0,1000]" },
  { "FTDY0017", "mild-not contains StringExclude" },
  { "FTDY0020", "invalid wildcard syntax${: 1}" },
  { "FTST0008", "\"$1\": unknown stop-word list" },
  { "FTST0009", "\"$1\": unsupported language" },
  { "FTST0018", "\"$1\": unknown thesaurus" },
  { "FTST0019", "\"$1\": match option specified more than once" },
#endif /* ZORBA_NO_FULL_TEXT */

  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // !!!!! THIS MUST BE ASCII SORTED BY KEY !!!!!
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  { "SENR0001", "an item in S6 in sequence normalization is an attribute node or a namespace node" },
  { "SEPM0004", "doctype-system parameter, or standalone parameter with a value other than \"omit\", specified" },
  { "SEPM0009", "the omit-xml-declaration parameter has the value \"yes\" and the standalone attribute has a value other than \"omit\"; or the version parameter has a value other than \"1.0\" and the doctype-system parameter is specified" },
  { "SEPM0010", "the output method is \"xml\", the value of the undeclare-prefixes parameter is \"yes\", and the value of the version parameter is \"1.0\"" },
  { "SEPM0016", "\"$1\": parameter value invalid for defined domain" },
  { "SERE0003", "serializer can't satisfy the rules for either a well-formed XML document entity or a well-formed XML external general parsed entity" },
  { "SERE0005", "invalid NCName character" },
  { "SERE0006", "\"$1\": invalid character for XML version $2" },
  { "SERE0008", "\"$1\": can not encode character as character reference here" },
  { "SERE0012", "the value of the normalization-form parameter is fully-normalized and any relevant construct of the result begins with a combining character" },
  { "SERE0014", "can not use HTML output method" },
  { "SERE0015", "can not use '>' within processing instruction for HTML output method" },
  { "SESU0007", "\"$1\": unsupported encoding" },
  { "SESU0011", "\"$1\": unsupported normalization form" },
  { "SESU0013", "\"$1\": unsupported XML or HTML version" },

  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // !!!!! THIS MUST BE ASCII SORTED BY KEY !!!!!
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  { "SRL0001", "\"$1\": nonexistent input field" },
  { "SRL0002", "\"$1\": incompatible input field" },
  { "SRL0003", "\"$1\": unrecognized class field" },
  { "SRL0004", "\"$1\": unresolved field reference" },
  { "SRL0005", "\"$1\": class version is too new" },
  { "SRL0006", "\"$1\": class version is too old" },
  { "SRL0007", "input archive used for out serialization" },
  { "SRL0008", "output archive used for in serialization" },
  { "SRL0009", "class not serializable" },
  { "SRL0010", "\"$1\": item type not serializable" },
  { "SRL0011", "input archive not zorba archive" },
  { "SRL0012", "\"$1\": incompatible archive version" },
  { "SRL0013", "unable to load query" },
  { "SRL0014", "infinite circular dependencies" },
  { "SRL0015", "incompatible between 32/64 bits or little/big-endian" },

  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // !!!!! THIS MUST BE ASCII SORTED BY KEY !!!!!
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  { "STR0001", "\"$1\": index already exists" },
  { "STR0002", "\"$1\": index does not exist" },
  { "STR0003", "\"$1\": partial key insertion into index \"$2\"" },
  { "STR0004", "partial key deletion from index $1" },
  { "STR0005", "partial key probe into index $1" },
  { "STR0006", "\"$1\": invalid box probe condition for index \"$2\"" },
  { "STR0007", "index unsupported probe condition" },
  { "STR0008", "\"$1\": collection already exists" },
  { "STR0009", "\"$1\": collection not found" },
  { "STR0010", "can not insert node into colletion \"$1\" because it already belongs to collection \"$2\"" },
  { "STR0011", "non-root node can not be inserted into collection \"$1\"" },
  { "STR0012", "non-node item used with collection \"$1\"" },
  { "STR0015", "integrity constraint already exists" },
  { "STR0016", "integrity constraint does not exist" },
  { "STR0020", "loader I/O error${: 1}" },
  { "STR0021", "loader parsing error${: 1}" },
  { "STR0030", "NodeID error${: 1}" },
  { "STR0040", "type error${: 1}" },
  { "STR0041", "NaN comparison" },
  { "STR0045", "duplicate node found in sequence" },
  { "STR0050", "\"$1\": function not implemented for item type $2" },
  { "STR0055", "the given streamable string has already been consumed" },

  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // !!!!! THIS MUST BE ASCII SORTED BY KEY !!!!!
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  { "XDDY0001", "\"$1\": collection not declared" },
  { "XDDY0002", "\"$1\": collection already exists" },
  { "XDDY0003", "\"$1\": collection does not exist" },
  { "XDDY0004", "\"$1\": can not update constant collection" },
  { "XDDY0005", "\"$1\": illegal insert into append-only collection" },
  { "XDDY0006", "\"$1\": illegal insert into queue collection" },
  { "XDDY0007", "\"$1\": illegal delete from append-only collection" },
  { "XDDY0008", "\"$1\": illegal delete from queue collection" },
  { "XDDY0009", "\"$1\": not all nodes to delete are at the beginning of this queue collection" },
  { "XDDY0010", "\"$1\": illegal update of read-only nodes" },
  { "XDDY0011", "\"$1\": collection does not contain node" },
  { "XDDY0012", "\"$1\": illegal insert into unordered collection" },
  { "XDDY0013", "\"$1\": can not delete collection because indexes reference it" },
  { "XDDY0014", "\"$1\": can not delete collection because integrity constraits reference it" },
  { "XDDY0015", "\"$1\": can not delete collection because there are references to its nodes" },
  { "XDDY0016", "\"$1\": multiple calls to create-collection function in same snapshot" },
  { "XDDY0020", "\"$1\": index domain expression yields nodes that are not in collection" },
  { "XDDY0021", "\"$1\": undeclared index" },
  { "XDDY0022", "\"$1\": index exists already" },
  { "XDDY0023", "\"$1\": index does not exist" },
  { "XDDY0024", "\"$1\": index uniqueness violation" },
  { "XDDY0025", "\"$1\": invalid number of arguments in probe" },
  { "XDDY0026", "\"$1\": index range probe not allowed" },
  { "XDDY0027", "\"$1\": index multiple creates" },
  { "XDDY0028", "\"$1\": index domain has duplicate nodes" },
  { "XDDY0029", "\"$1\": index general probe not allowed" },
  { "XDDY0031", "\"$1\": integrity constraint is not declared" },
  { "XDDY0032", "\"$1\": integrity constraint is not activated" },
  { "XDDY0033", "\"$1\": integrity constraint not met" },
  { "XDST0001", "\"$1\": collection already declared" },
  { "XDST0002", "\"$1\": collection already imported" },
  { "XDST0003", "\"$1\": collection defl in main module" },
  { "XDST0004", "collection multiple property values" },
  { "XDST0005", "collection properties conflict" },
  { "XDST0006", "collection invalid property value" },
  { "XDST0007", "collection defl in foreign module" },
  { "XDST0021", "index already declared" },
  { "XDST0022", "index already imported" },
  { "XDST0023", "index defl in main module" },
  { "XDST0024", "index multiple property values" },
  { "XDST0025", "index properties conflict" },
  { "XDST0026", "index invalid property value" },
  { "XDST0027", "index bad key type" },
  { "XDST0028", "index not deterministic" },
  { "XDST0029", "index invalid data source" },
  { "XDST0030", "index non const data source" },
  { "XDST0031", "index has free vars" },
  { "XDST0032", "index references ctx item" },
  { "XDST0033", "index non simple expr" },
  { "XDST0034", "index cannot do automatic maintenance" },
  { "XDST0035", "index general multikey" },
  { "XDST0036", "index defl in foreign module" },
  { "XDST0041", "integrity constraint is already declared" },
  { "XDST0042", "multi-valued integrity constraint key" },
  { "XDST0043", "non-atomic integrity constraint key" },
  { "XDST0044", "integrity constraint defl in main module" },
  { "XDST0045", "integrity constraint has free vars" },
  { "XDST0046", "integrity constraint has invalid data source${: 1}" },
  { "XDST0047", "integrity constraint not deterministic" },
  { "XDST0048", "integrity constraint defl in foreign module" },
  { "XDTY0001", "collection invalid node type" },
  { "XDTY0010", "\"$1\": index domain expression yields a non-node item" },
  { "XDTY0011", "result of some key expression of index $1 does not match its declared type" },
  { "XDTY0012", "\"$1\": general range index key item has a type for which no ordering relationship exists" },

  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // !!!!! THIS MUST BE ASCII SORTED BY KEY !!!!!
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  { "XPDY0002", "\"$1\": undefined value" },
  { "XPDY0050", "invalid treat expression type" },
  { "XPST0001", "\"$1\": undefined value" },
  { "XPST0003", "invalid expression${: 1}" },
  { "XPST0005", "static type must not be empty-sequence()" },
  { "XPST0008", "\"$1\": undefined name" },
  { "XPST0010", "\"$1\": unsupported axis" },
  { "XPST0017", "function call does not match signature" },
  { "XPST0051", "\"$1\": not defined as atomic type" },
  { "XPST0080", "\"cast\" or \"castable\" exression must not have xs:anyAtomicType or xs:NOTATION type" },
  { "XPST0081", "\"$1\": can not expand namespace prefix to URI" },
  { "XPST0083", "\"$1\": not a string literal" },
  { "XPTY0004", "\"$1\": invalid type${: 2}" },
  { "XPTY0018", "path expression last step must not have nodes and atomic values" },
  { "XPTY0019", "path expression non-last step must not be an atomic value" },
  { "XPTY0020", "axis step context item is not a node" },

  { "XQD0000", "\"$1\": prefix not declared" },
  { "XQD0001", "document not valid" },
  { "XQDY0025", "\"$1\": duplicate attribute name" },
  { "XQDY0026", "computed processing instrucion must not contain \"?>\"" },
  { "XQDY0027", "\"$1\": unexpected validity property" },
  { "XQDY0041", "\"$1\": can not cast to xs:NCName" },
  { "XQDY0044", "\"$1\": invalid attibute node-name" },
  { "XQDY0061", "invalid validate expression operand" },
  { "XQDY0064", "\"XML\": invalid name expression" },
  { "XQDY0072", "comment must not contain \"--\" or end with \"-\"" },
  { "XQDY0074", "\"$1\": can not convert to expanded QName" },
  { "XQDY0084", "validated element does not have a top-level element declaration in the in-scope element declarations, if validation mode is strict" },
  { "XQDY0091", "\"xml:id\" encountered" },
  { "XQDY0092", "\"$1\": must be either \"preserve\" or \"default\"" },
  { "XQDY0096", "\"$1\": invalid node-name" },

  { "XQP0000", "no error" },
  { "XQP0001", "dynamic runtime error${: 1}" },
  { "XQP0002", "dynamic iterator overrun" },
  { "XQP0003", "illegal node child" },
  { "XQP0004", "\"$1\": target namespace not found" },
  { "XQP0005", "\"$1\": not supported" },
  { "XQP0006", "\"$1\": assertion failed" },
  { "XQP0007", "hash error: KeyLen exceeds MaxKeyLen" },
  { "XQP0008", "vector out of range" },
  { "XQP0009", "read lock failed" },
  { "XQP0010", "write lock failed" },
  { "XQP0010", "pop from empty list" },
  { "XQP0011", "file error in function" },
  { "XQP0012", "FxCharHeap I/O error" },
  { "XQP0013", "mmfile I/O error" },
  { "XQP0015", "not yet implemented: $1" },
  { "XQP0016", "\"$1\": reserved module target namespace" },
  { "XQP0017", "file access disabled" },
  { "XQP0019", "internal error${: 1 }" },
  { "XQP0020", "\"$1\": invalid URI${: 2}" },
  { "XQP0021", "user error" },
  { "XQP0022", "get results as DOM failed" },
  { "XQP0023", "get results as SAX failed" },
  { "XQP0024", "XML does not match schema${: \"1\"}${ 2}" },
  { "XQP0025", "could not create item" },
  { "XQP0026", "can not initialize store" },
  { "XQP0026", "could not create thread" },
  { "XQP0027", "error from MS DOM engine" },
  { "XQP0028", "\"$1\": external function not bound" },
  { "XQP0029", "module import not allowed" },
  { "XQP0030", "deadlock" },
  { "XQP0031", "malformed XQueryX input" },
  { "XQP0032", "error transforming XQueryX to XQuery${: 1}" },
  { "XQP0034", "\"$1\": illegal UTF-8 byte" },
  { "XQP0035", "\"$1\": illegal UTF-16 byte" },
  { "XQP0036", "BreakIterator creation failed" },
  { "XQP8401", "\"$1\": wrong WordNet file version; should be \"$2\"" },
  { "XQP8402", "thesaurus data error${: 1}" },

  { "XQST0009", "schema import not supported" },
  { "XQST0012", "invalid schema definitions" },
  { "XQST0013", "invalid pragma content" },
  { "XQST0016", "invalid pragma content" },
  { "XQST0022", "namespace declaration attribute is not a URI literal" },
  { "XQST0031", "\"$1\": unsupported version" },
  { "XQST0032", "multiple base URI declarations" },
  { "XQST0033", "\"$1\": namespace prefix already bound to \"$2\"" },
  { "XQST0034", "\"$1\": duplicate function declaration" },
  { "XQST0035", "\"$1\": name previosly imported" },
  { "XQST0036", "incomplete in-scope schema definitions" },
  { "XQST0038", "multiple default collation declarations" },
  { "XQST0039", "\"$1\": duplicate parameter name" },
  { "XQST0040", "\"$1\": non-distinct expanded attribute QName" },
  { "XQST0045", "\"$1\": invalid function namespace" },
  { "XQST0046", "\"$1\": invalid URI literal${: 2}" },
  { "XQST0047", "\"$1\": duplicate target namespace" },
  { "XQST0048", "\"$1\": not in library namespace" },
  { "XQST0049", "\"$1\": duplicate variable declaration" },
  { "XQST0054", "\"$1\": variable must not depend on itself" },
  { "XQST0055", "multiple copy-namespaces declarations" },
  { "XQST0057", "empty target namespace" },
  { "XQST0058", "\"$1\": duplicate target namespace" },
  { "XQST0059", "\"$1\": target namespace not found" },
  { "XQST0060", "\"$1\": function name has a null namespace URI" },
  { "XQST0065", "multiple ordering mode declaraions" },
  { "XQST0066", "multiple element/type/function namespace declarations" },
  { "XQST0067", "multiple construction declarations" },
  { "XQST0068", "multiple boundary-space declarations" },
  { "XQST0069", "multiple empty order declarations" },
  { "XQST0070", "can not reuse \"xml\" or \"xmlns\" prefixes" },
  { "XQST0071", "\"$1\": duplicate namespace declaration attribute" },
  { "XQST0075", "\"validate\" expression not supported" },
  { "XQST0076", "unknown collation" },
  { "XQST0079", "unknown pragma or empty expression" },
  { "XQST0085", "namespace URI is empty" },
  { "XQST0087", "\"$1\": invalid encoding" },
  { "XQST0088", "empty target namespace literal" },
  { "XQST0089", "\"$1\": duplicate variable name" },
  { "XQST0090", "\"$1\": invalid character reference" },
  { "XQST0093", "\"$1\": module must not depend on itself" },
  { "XQST0097", "the decimal format declaration specifies a value that is not valid for a property" },
  { "XQST0098", "the properties representing characters used in a picture string do not each have distinct values" },
  { "XQST0106", "\"$1\": multiple function annotations with this name" },
  { "XQST0111", "the query contains two decimal formats with the same name, or two default decimal formats" },
  { "XQTY0024", "element constructor content sequence must not have an attribute node following a non-attribute node" },
  { "XQTY0030", "validate argument must be exactly one document or element node" },
  { "XQTY0086", "typed value of copied element or attribute node is namespace-sensitive when construction mode is preserve and copy-namespaces mode is no-preserve" },
  { "XTDE1310", "\"$1\": picture string does not satisfy format-number function rules" },
  { "XTDE1340", "\"$1\": invalid picture string" },
  { "XTDE1350", "\"$1\": component specifier not available" },

  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // !!!!! THIS MUST BE ASCII SORTED BY KEY !!!!!
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  { "XUDY0009", "TODO" },
  { "XUDY0014", "TODO" },
  { "XUDY0015", "TODO" },
  { "XUDY0016", "TODO" },
  { "XUDY0017", "TODO" },
  { "XUDY0018", "TODO" },
  { "XUDY0019", "TODO" },
  { "XUDY0020", "TODO" },
  { "XUDY0021", "TODO" },
  { "XUDY0023", "TODO" },
  { "XUDY0024", "TODO" },
  { "XUDY0025", "TODO" },
  { "XUDY0027", "TODO" },
  { "XUDY0029", "TODO" },
  { "XUDY0030", "TODO" },
  { "XUDY0031", "TODO" },
  { "XUST0001", "updating expression illegal here" },
  { "XUST0002", "simple expression illegal here" },
  { "XUST0003", "multiple revalidation declarations" },
  { "XUST0026", "TODO" },
  { "XUST0028", "TODO" },
  { "XUTY0004", "attribute node follows non-attribute node" },
  { "XUTY0005", "target expression not a single element or document node" },
  { "XUTY0006", "target expression not a single element, text, comment, or processing instruction node" },
  { "XUTY0007", "target expression does not return a sequence of zero or more nodes" },
  { "XUTY0008", "target expression not a single element, attribute, text, comment, or processing instruction node" },
  { "XUTY0010", "TODO" },
  { "XUTY0011", "TODO" },
  { "XUTY0012", "TODO" },
  { "XUTY0013", "TODO" },
  { "XUTY0022", "TODO" },

  /////////////////////////////////////////////////////////////////////////////

  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // !!!!! THIS MUST BE ASCII SORTED BY KEY !!!!!
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  // Note: parameter numbers for sub-messages start with at LEAST $2.

  { "~AllMatchesHasExcludes", "AllMatches contains StringExclude" },
  { "~AlreadySpecified", "already specified" },
  { "~BackRef0Illegal", "\"\\0\": illegal backreference" },
  { "~BackRefIllegalInCharClass", "backreference illegal in character class" },
  { "~BadCharInBraces", "'$2': illegal character within { }" },
  { "~BadDecDigit", "'$2': invalid decimal digit" },
  { "~BadFileURIAuthority", "\"$2\": invalid authority for \"file\" scheme" },
  { "~BadHexDigit", "\"$2\": invalid hexedecimal digit" },
  { "~BadLibraryModule", "invalid library module" },
  { "~BadMathTypes", "arithmetic operation not defined with type $3" },
  { "~BadPath", "invalid path" },
  { "~BadTokenInBraces", "\"$2\": illegal token within { }" },
  { "~BadUnicodeChar", "\"$2\": invalid character code-point" },
  { "~BadWordNetPartOfSpeech", "\"$2\": invalid part-of-speech" },
  { "~BadWordNetPtr", "\"$2\": invalid pointer type" },
  { "~CastFailed", "$2 to $3 cast failed" },
  { "~ClosingBraceWithoutOpen", "'}' encountered without '{' first" },
  { "~CommaExpected", "',' expected" },
  { "~CountClause11", "count clause only available in XQuery 1.1 or later" },
  { "~EBVNotDef", "effective boolean value not defined for \"$2\"" },
  { "~EmptyPath", "empty path" },
  { "~Eval11", "eval only available in XQuery 1.1 or later" },
  { "~ExternFnDeterministic", "only external functions may be declared deterministic" },
  { "~ExternFnNondeterministic", "only external functions may be declared nondeterministic" },
  { "~FullTextNotEnabled", "full-text was not enabled in this build" },
  { "~IncompleteKeyInIndexBuild", "incomplete key during index build" },
  { "~ModuleDeclNotInMain", "module declaration must not be in main module" },
  { "~ModuleNotFound", "module not found" },
  { "~MustBeAbsoluteURI", "must be absolute" },
  { "~NoCastTo", "can not cast to $3" },
  { "~NoDriveSpecification", "missing drive specification" },
  { "~NoSourceURI", "no URI" },
  { "~NoURIAuthority", "no authority" },
  { "~NoURIScheme", "no scheme" },
  { "~NonClosedBackRef", "non-closed backreference" },
  { "~NonFileThesaurusURI", "non-file thesaurus URI" },
  { "~NonLocalhostAuthority", "non-localhost authority" },
  { "~NonexistantBackRef", "non-existant backreference" },
  { "~NotAllowedForTypeName", "not allowed for typeName (use xsd:untyped instead)" },
  { "~NotAmongInScopeSchemaTypes", "not among in-scope schema types" },
  { "~NotSpecified", "not specified" },
  { "~OuterForClause11", "outer-for clause only available in XQuery 1.1 or later" },
  { "~SeqFnBody", "only a function declared as sequential can have a body that is a sequential expression" },
  { "~StartEndTagMismatch", "start tag \"$2\" does not match end tag \"$3\"" },
  { "~SwitchExpr11", "switch expressions only available in XQuery 1.1 or later" },
  { "~TrailingBackslash", "trailing backslash '\\'" },
  { "~TryCatchExpr11", "try/catch expressions only available in XQuery 1.1 or later" },
  { "~U_REGEX_BAD_ESCAPE_SEQUENCE", "unrecognized backslash escape sequence" },
  { "~U_REGEX_BAD_INTERVAL", "error in {min,max} interval" },
  { "~U_REGEX_INTERNAL_ERROR", "an internal ICU error (bug) was detected" },
  { "~U_REGEX_INVALID_BACK_REF", "backreference to a non-existent capture group" },
  { "~U_REGEX_INVALID_FLAG", "invalid value for match mode flags" },
  { "~U_REGEX_INVALID_RANGE", "in a character range [x-y], x is greater than y" },
  { "~U_REGEX_INVALID_STATE", "RegexMatcher in invalid state for requested operation" },
  { "~U_REGEX_LOOK_BEHIND_LIMIT", "look-behind pattern matches must have a bounded maximum length" },
  { "~U_REGEX_MAX_LT_MIN", "in {min,max}, max is less than min" },
  { "~U_REGEX_MISMATCHED_PAREN", "incorrectly nested parentheses" },
  { "~U_REGEX_MISSING_CLOSE_BRACKET", "missing closing bracket on a bracket expression" },
  { "~U_REGEX_NUMBER_TOO_BIG", "decimal number is too large" },
  { "~U_REGEX_OCTAL_TOO_BIG", "octal character constants must be <= 0377" },
  { "~U_REGEX_PROPERTY_SYNTAX", "incorrect Unicode property" },
  { "~U_REGEX_RULE_SYNTAX", "syntax error" },
  { "~U_REGEX_SET_CONTAINS_STRING", "can not have UnicodeSets containing strings" },
  { "~U_REGEX_STACK_OVERFLOW", "backtrack stack overflow" },
  { "~U_REGEX_STOPPED_BY_CALLER", "matching operation aborted by user callback fn" },
  { "~U_REGEX_TIME_OUT", "maximum allowed match time exceeded" },
  { "~U_REGEX_UNIMPLEMENTED", "use of regular expression feature that is not yet implemented" },
  { "~UnbalancedBrace", "missing '}'" },
  { "~UnbalancedParen", "unbalanced ')'" },
  { "~UnexpectedElement", "unexpected element" },
  { "~WindowClause11", "window clause only available in XQuery 1.1 or later" },
  { "~WrongWordNetEndianness", "wrong endianness" },
  { "~Zorba API error", "Zorba API error" },
  { "~Zorba data-definition error", "Zorba data-definition error" },
  { "~Zorba error", "Zorba error" },
  { "~Zorba serialization error", "Zorba serialization error" },
  { "~Zorba store error", "Zorba store error" },
  { "~dynamic error", "dynamic error" },
  { "~error", "error" },
  { "~format-integer: $language parameter is not castable to xs:language", "format-integer: $language parameter is not castable to xs:language" },
  { "~format-integer: $picture parameter should not be empty", "format-integer: $picture parameter should not be empty" },
  { "~format-integer: $value should be between 1 and 10 for this formatting picture", "format-integer: $value should be between 1 and 10 for this formatting picture" },
  { "~format-integer: $value should be between 1 and 20 for this formatting picture", "format-integer: $value should be between 1 and 20 for this formatting picture" },
  { "~format-integer: $value should be less than 3000 for Roman representation", "format-integer: $value should be less than 3000 for Roman representation" },
  { "~format-integer: bad $picture format: ", "format-integer: bad $picture format: $2" },
  { "~format-integer: bad optional format modifier, cannot find closing ')' ", "format-integer: bad optional format modifier, cannot find closing ')' " },
  { "~format-integer: duplicated optional format modifier ", "format-integer: duplicated optional format modifier $2" },
  { "~format-integer: unknown optional format modifier character", "format-integer: unknown optional format modifier character $2" },
  { "~serialization error", "serialization error" },
  { "~static error", "static error" },
  { "~type error", "type error" },
  { "~user-defined error", "user-defined error" },

};
DEF_DICT_END(en);

///////////////////////////////////////////////////////////////////////////////

} // namespace dict
} // namespace err
} // namespace zorba
/* vim:set et sw=2 ts=2: */
