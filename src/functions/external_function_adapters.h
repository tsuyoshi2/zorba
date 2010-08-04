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
#ifndef ZORBA_EXTERNAL_FUNCTION_ADAPTERS_H
#define ZORBA_EXTERNAL_FUNCTION_ADAPTERS_H

#include <vector>
#include <zorba/external_function.h>

#include "common/shared_types.h"

#include "functions/function.h"

namespace zorba 
{

/*******************************************************************************

********************************************************************************/
class external_function : public function 
{
protected:
  QueryLoc                    theLoc;
  static_context            * theModuleSctx;
  xqpStringStore_t            theNamespace;
  expr_script_kind_t          theScriptingKind;
  StatelessExternalFunction * theImpl;

public:
  SERIALIZABLE_CLASS(external_function)
  SERIALIZABLE_CLASS_CONSTRUCTOR2(external_function, function)
  void serialize(::zorba::serialization::Archiver& ar);

public:
  external_function(
        const QueryLoc& loc,
        static_context* modSctx,
        const xqpStringStore_t& ns,
        const signature& sig,
        expr_script_kind_t scriptingType,
        bool deterministic,
        StatelessExternalFunction* impl);

  ~external_function() { }

  bool isBuiltin() const { return false; }

  bool isExternal() const { return true; }

  bool isUdf() const { return false; }

  expr_script_kind_t getUpdateType() const { return theScriptingKind; }

  bool accessesDynCtx() const;

  PlanIter_t codegen(
        CompilerCB* /*cb*/,
        static_context* sctx,
        const QueryLoc& loc,
        std::vector<PlanIter_t>& argv,
        AnnotationHolder& ann) const;
};


} /* namespace zorba */
#endif  /* ZORBA_EXTERNAL_FUNCTION_ADAPTERS_H */

/* vim:set ts=2 sw=2: */

/*
 * Local variables:
 * mode: c++
 * End:
 */
