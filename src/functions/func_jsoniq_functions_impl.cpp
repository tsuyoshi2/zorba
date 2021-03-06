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
#include "stdafx.h"

#include "functions/func_jsoniq_functions.h"
#include "functions/func_jsoniq_functions_impl.h"

#include "runtime/json/json_constructors.h"
#include "runtime/json/jsoniq_functions.h"
#include "runtime/core/item_iterator.h"

#include "compiler/parser/query_loc.h"
#include "compiler/expression/expr_consts.h"
#include "compiler/expression/fo_expr.h"

#include "types/typeops.h"

namespace zorba
{

/*******************************************************************************

********************************************************************************/
PlanIter_t fn_jsoniq_parse_json::codegen(
    CompilerCB*,
    static_context* sctx,
    const QueryLoc& loc,
    std::vector<PlanIter_t>& argv,
    expr& ann) const
{
  SingletonIterator* lArg = dynamic_cast<SingletonIterator*>(argv[0].getp());
  if (lArg && lArg->getValue()->isAtomic())
  {
    const QueryLoc& lArgLoc = lArg->getLocation();
    return new JSONParseIterator(sctx, loc, argv, lArgLoc);
  }
  else
  {
    QueryLoc lArgLoc = QueryLoc::null;
    return new JSONParseIterator(sctx, loc, argv, lArgLoc);
  }
}


/*******************************************************************************

********************************************************************************/
PlanIter_t fn_jsoniq_keys::codegen(
    CompilerCB*,
    static_context* sctx,
    const QueryLoc& loc,
    std::vector<PlanIter_t>& argv,
    expr& arg) const
{
  if (arg.get_return_type()->max_card() <= 1)
    return new SingleObjectKeysIterator(sctx, loc, argv[0]);

  return new MultiObjectKeysIterator(sctx, loc, argv[0]);
}


/*******************************************************************************

********************************************************************************/
xqtref_t op_zorba_multi_object_lookup::getReturnType(const fo_expr* caller) const
{
  if (caller->get_arg(0)->get_return_type()->max_card() == 0)
    return GENV_TYPESYSTEM.EMPTY_TYPE;

  return theSignature.returnType();
}


PlanIter_t op_zorba_multi_object_lookup::codegen(
  CompilerCB*,
  static_context* sctx,
  const QueryLoc& loc,
  std::vector<PlanIter_t>& argv,
  expr& arg) const
{
  if (arg.get_return_type()->max_card() <= 1)
    return new SingleArrayLookupIterator(sctx, loc, argv[0], argv[1]);

  return new MultiObjectLookupIterator(sctx, loc, argv[0], argv[1]);
}


/*******************************************************************************

********************************************************************************/
xqtref_t op_zorba_single_object_lookup::getReturnType(const fo_expr* caller) const
{
  if (caller->get_arg(0)->get_return_type()->max_card() == 0)
    return GENV_TYPESYSTEM.EMPTY_TYPE;

  return theSignature.returnType();
}


/*******************************************************************************

********************************************************************************/
xqtref_t fn_jsoniq_project::getReturnType(const fo_expr* caller) const
{
  return caller->get_arg(0)->get_return_type();
}


/*******************************************************************************

********************************************************************************/
xqtref_t fn_jsoniq_trim::getReturnType(const fo_expr* caller) const
{
  return caller->get_arg(0)->get_return_type();
}


/*******************************************************************************

********************************************************************************/
PlanIter_t fn_jsoniq_members::codegen(
  CompilerCB*,
  static_context* sctx,
  const QueryLoc& loc,
  std::vector<PlanIter_t>& argv,
  expr& arg) const
{
  if (arg.get_return_type()->max_card() <= 1)
    return new SingleArrayMembersIterator(sctx, loc, argv[0]);

  return new MultiArrayMembersIterator(sctx, loc, argv[0]);
}


/*******************************************************************************

********************************************************************************/
xqtref_t op_zorba_multi_array_lookup::getReturnType(const fo_expr* caller) const
{
  if (caller->get_arg(0)->get_return_type()->max_card() == 0)
    return GENV_TYPESYSTEM.EMPTY_TYPE;

  return theSignature.returnType();
}


PlanIter_t op_zorba_multi_array_lookup::codegen(
  CompilerCB*,
  static_context* sctx,
  const QueryLoc& loc,
  std::vector<PlanIter_t>& argv,
  expr& arg) const
{
  if (arg.get_return_type()->max_card() <= 1)
    return new SingleArrayLookupIterator(sctx, loc, argv[0], argv[1]);

  return new MultiArrayLookupIterator(sctx, loc, argv[0], argv[1]);
}

/*******************************************************************************

********************************************************************************/
xqtref_t op_zorba_single_array_lookup::getReturnType(const fo_expr* caller) const
{
  if (caller->get_arg(0)->get_return_type()->max_card() == 0)
    return GENV_TYPESYSTEM.EMPTY_TYPE;

  return theSignature.returnType();
}


/*******************************************************************************

********************************************************************************/
xqtref_t fn_jsoniq_size::getReturnType(const fo_expr* caller) const
{
  if (caller->get_arg(0)->get_return_type()->get_quantifier() == SequenceType::QUANT_ONE)
    return GENV_TYPESYSTEM.INTEGER_TYPE_ONE;

  return theSignature.returnType();
}


/*******************************************************************************

********************************************************************************/
bool op_zorba_json_array_insert::mustCopyInputNodes(expr* fo, csize producer) const
{
  static_context* sctx = fo->get_sctx();

  if (producer == 2 && sctx->preserve_ns() && sctx->inherit_ns())
  {
    return true;
  }

  return false;
}


/*******************************************************************************

********************************************************************************/
bool op_zorba_json_object_insert::mustCopyInputNodes(expr* fo, csize producer) const
{
  if (producer == 2 && fo->get_sctx()->preserve_ns() && fo->get_sctx()->inherit_ns())
  {
    return true;
  }

  return false;
}


/*******************************************************************************

********************************************************************************/
bool op_zorba_json_replace_value::mustCopyInputNodes(expr* fo, csize producer) const
{
  static_context* sctx = fo->get_sctx();

  if (producer == 2 && sctx->preserve_ns() && sctx->inherit_ns())
  {
    return true;
  }

  return false;
}


/*******************************************************************************

********************************************************************************/
PlanIter_t op_zorba_json_replace_value::codegen(
  CompilerCB*,
  static_context* sctx,
  const QueryLoc& loc,
  std::vector<PlanIter_t>& argv,
  expr& ann) const
{
  return new JSONReplaceValueIterator(sctx, loc, argv, true);
}


/*******************************************************************************

********************************************************************************/
bool op_zorba_json_array_append::mustCopyInputNodes(expr* fo, csize producer) const
{
  static_context* sctx = fo->get_sctx();

  if (producer == 2 && sctx->preserve_ns() && sctx->inherit_ns())
  {
    return true;
  }

  return false;
}


/*******************************************************************************

********************************************************************************/
xqtref_t op_zorba_json_box::getReturnType(const fo_expr* fo) const
{
  xqtref_t type = fo->get_arg(0)->get_return_type();

  if (type->get_quantifier() == SequenceType::QUANT_ONE)
    return type;

  if (type->max_card() == 0)
    return GENV_TYPESYSTEM.JS_NULL_TYPE_ONE;

  return theSignature.returnType();
}


/*******************************************************************************

********************************************************************************/
PlanIter_t jn_object::codegen(
  CompilerCB*,
  static_context* sctx,
  const QueryLoc& loc,
  std::vector<PlanIter_t>& argv,
  expr& ann) const
{
  return new JSONObjectIterator(sctx, loc, argv, true, false);
}


void populate_context_jsoniq_functions_impl(static_context* sctx)
{
  DECL(sctx, jn_object,
       (createQName("http://jsoniq.org/functions", "", "object"),
        GENV_TYPESYSTEM.JSON_OBJECT_TYPE_STAR,
        GENV_TYPESYSTEM.JSON_OBJECT_TYPE_ONE));
}


} /* namespace zorba */

/* vim:set et sw=2 ts=2: */
