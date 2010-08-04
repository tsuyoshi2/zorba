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
#include "functions/external_function_adapters.h"

#include "runtime/core/fncall_iterator.h"

namespace zorba 
{

SERIALIZABLE_CLASS_VERSIONS(external_function)
END_SERIALIZABLE_CLASS_VERSIONS(external_function)


external_function::external_function(
    const QueryLoc& loc,
    static_context* modSctx,
    const xqpStringStore_t& ns,
    const signature& sig,
    expr_script_kind_t scriptingType,
    bool deterministic,
    StatelessExternalFunction* impl) 
  :
  function(sig, FunctionConsts::FN_UNKNOWN),
  theLoc(loc),
  theModuleSctx(modSctx),
  theNamespace(ns),
  theScriptingKind(scriptingType),
  theImpl(impl)
{
  setDeterministic(deterministic);
}


void external_function::serialize(::zorba::serialization::Archiver& ar)
{
  zorba::serialization::serialize_baseclass(ar, (function*)this);
  
  if(!ar.is_serializing_out())
    theImpl = NULL;//don't serialize this for now

  ar & theLoc;
  ar & theModuleSctx;
  ar & theNamespace;
  SERIALIZE_ENUM(expr_script_kind_t, theScriptingKind);
}


bool external_function::accessesDynCtx() const
{
  return theImpl->isContextual();
}


PlanIter_t external_function::codegen(
    CompilerCB* /*cb*/,
    static_context* sctx,
    const QueryLoc& loc,
    std::vector<PlanIter_t>& argv,
    AnnotationHolder& ann) const
{
  return new StatelessExtFunctionCallIterator(sctx,
                                              loc,
                                              argv,
                                              theImpl,
                                              isUpdating(),
                                              theNamespace,
                                              theModuleSctx);
}


}
