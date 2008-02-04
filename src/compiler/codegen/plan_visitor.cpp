/* -*- mode: c++; indent-tabs-mode: nil; tab-width: 2 -*-
 *
 *  $Id: plan_visitor.cpp,v 1.1 2006/10/09 07:07:59 Paul Pedersen Exp $
 *
 *  Copyright 2006-2007 FLWOR Foundation.
 *  Author: John Cowan, Paul Pedersen
 *
 */

#include "plan_visitor.h"

#include <memory>
#include "context/namespace_context.h"
#include "compiler/expression/expr.h"
#include "compiler/expression/expr_visitor.h"
#include "runtime/sequences/SequencesImpl.h"
#include "runtime/core/sequencetypes.h"
#include "runtime/core/item_iterator.h"
#include "runtime/core/var_iterators.h"
#include "runtime/core/constructors.h"
#include "runtime/core/path_iterators.h"
#include "runtime/core/nodeid_iterators.h"
#include "runtime/core/flwor_iterator.h"
#include "util/tracer.h"
#include "functions/function.h"
#include "util/stl_extra.h"
#include "util/hashmap.h"

#ifndef NDEBUG
#  include "zorba/util/properties.h"
#endif

#include <iostream>
#include <vector>
#include <stack>


#ifndef NDEBUG
#define CODEGEN_TRACE(msg)                      \
  if (Properties::instance()->traceCodegen())   \
    std::cout << (msg) << TRACE << std::endl;
#define CODEGEN_TRACE_IN( msg )  \
  CODEGEN_TRACE (std::string (++depth, ' ') + msg)
#define CODEGEN_TRACE_OUT( msg ) \
  CODEGEN_TRACE (std::string (depth--, ' ') + msg)
#else
#define CODEGEN_TRACE(msg)
#define CODEGEN_TRACE_IN(msg)
#define CODEGEN_TRACE_OUT(msg)
#endif

using namespace std;
namespace xqp 
{

#define ITEM_FACTORY (Store::getInstance().getItemFactory())

  template <typename T> T pop_stack (stack<T> &stk) {
    assert (! stk.empty ());
    T x = stk.top ();
    stk.pop ();
    return x;
  }

  template<class T> T &peek_stack (stack<T> &stk) {
    ZORBA_ASSERT (! stk.empty ());
    return stk.top ();
  }

template <typename V>
struct vector_destroyer {
    void operator()(const struct hash64map<std::vector<V> *>::entry& entry)
    {
        delete (const_cast<struct hash64map<std::vector<V> *>::entry&>(entry)).val;
    }
};
  
class plan_visitor : public expr_visitor
{
public:
	typedef rchandle<expr> expr_h_t;
  typedef rchandle<ForVarIterator> var_iter_t;
  typedef rchandle<LetVarIterator> ref_iter_t;

protected:
  uint32_t depth;
  
  std::stack<PlanIter_t>                    itstack;

  std::stack<expr*>                         theConstructorsStack;
  std::stack<bool>                          theAttrContentStack;

  namespace_context                       * theLastNSCtx;
  std::stack<namespace_context *>           theNSCtxStack;
  std::stack<namespace_context::bindings_t> theFlatBindingsStack;

  hash64map<std::vector<var_iter_t> *>      fvar_iter_map;
  hash64map<std::vector<var_iter_t> *>      pvar_iter_map;
  hash64map<std::vector<ref_iter_t> *>      lvar_iter_map;
  hash64map<std::vector<ref_iter_t> *>    * param_var_iter_map;

public:
	plan_visitor(hash64map<std::vector<ref_iter_t> *> *param_var_map = NULL)
        : depth (0), theLastNSCtx(NULL), param_var_iter_map(param_var_map) {}
	~plan_visitor()
    {
        std::for_each(fvar_iter_map.begin(), fvar_iter_map.end(), vector_destroyer<var_iter_t>());
        std::for_each(pvar_iter_map.begin(), pvar_iter_map.end(), vector_destroyer<var_iter_t>());
        std::for_each(lvar_iter_map.begin(), lvar_iter_map.end(), vector_destroyer<ref_iter_t>());
    }

public:
	PlanIter_t pop_itstack()
	{
    return pop_stack (itstack);
	}

  bool is_enclosed_expr(expr* e)
  {
    fo_expr* foe = dynamic_cast<fo_expr*>(e);
    if (foe != NULL && foe->get_fname()->getLocalName() == ":enclosed-expr")
      return true;

    return false;
  }

/*..........................................
 :  begin visit                            :
 :.........................................*/
bool begin_visit(expr& v)
{
  CODEGEN_TRACE_IN("");
  return true;
}


void end_visit(expr& v)
{
  CODEGEN_TRACE_OUT("");
}


bool begin_visit(var_expr& v)
{
  CODEGEN_TRACE_IN("");
  return true;
}


void end_visit(var_expr& v)
{
  CODEGEN_TRACE_OUT("");

  switch (v.kind) 
  {
  case var_expr::for_var:
  {
    ForVarIterator *v_p = new ForVarIterator(v.get_varname()->getLocalName(),
                                             v.get_loc(),
                                             (void *) &v);
    vector<var_iter_t> *map = NULL;
    
    ZORBA_ASSERT (fvar_iter_map.get ((uint64_t) &v, map));
    map->push_back (v_p);
    itstack.push(v_p);
  }
  break;
  case var_expr::pos_var:
  {
    ForVarIterator *v_p = new ForVarIterator(v.get_varname ()->getLocalName(),
                                             v.get_loc(),
                                             (void *) &v);
    vector<var_iter_t> *map = NULL;
    ZORBA_ASSERT (pvar_iter_map.get ((uint64_t) &v, map));
    map->push_back (v_p);
    itstack.push(v_p);
  }
  break;
  case var_expr::let_var:
  {
    LetVarIterator *v_p = new LetVarIterator(v.get_varname()->getLocalName(),
                                             v.get_loc(),
                                             (void *) &v);
    vector<ref_iter_t> *map = NULL;
    
    ZORBA_ASSERT (lvar_iter_map.get ((uint64_t) &v, map));
    map->push_back (v_p);
    itstack.push(v_p);
  }
    break;
  case var_expr::param_var:
  {
    LetVarIterator *v_p = new LetVarIterator(v.get_varname()->getLocalName(),
                                             v.get_loc(),
                                             (void *) &v);
    vector<ref_iter_t> *map = NULL;
    
    ZORBA_ASSERT (param_var_iter_map->get ((uint64_t) &v, map));
    map->push_back (v_p);
    itstack.push(v_p);
  }
    break;
  case var_expr::unknown_var:
    assert (false);
    break;
  default:
    assert (false);
    break;
  }
}

bool begin_visit(order_modifier& v)
{
  CODEGEN_TRACE_IN("");
  // not implemented, but this is a performance, not a conformance issue
  return true;
}

void end_visit(order_modifier& v)
{
  CODEGEN_TRACE_OUT("");
}

bool begin_visit(flwor_expr& v)
{
  CODEGEN_TRACE_IN("");
  for (vector<rchandle<forlet_clause> >::const_iterator it = v.clause_begin ();
       it != v.clause_end();
       ++it) 
  {
    rchandle<var_expr> var = (*it)->var_h;
    uint64_t k = (uint64_t) &*var;

    if (var->kind == var_expr::for_var)
    {
      fvar_iter_map.put(k, new vector<var_iter_t>());
      var_expr* pos_var = (*it)->get_pos_var().getp();
      if (pos_var != NULL)
        pvar_iter_map.put((uint64_t)pos_var, new vector<var_iter_t>());
    }
    else if (var->kind == var_expr::let_var)
    {
      lvar_iter_map.put(k, new vector<ref_iter_t>());
    }
    else
      ZORBA_ASSERT (false);
  }
  return true;
}


void end_visit(flwor_expr& v)
{
  CODEGEN_TRACE_OUT("");

  PlanIter_t ret = pop_itstack ();

  vector<FLWORIterator::OrderSpec> orderSpecs;
  for (vector<flwor_expr::orderspec_t>::const_iterator i = v.orderspec_begin ();
       i != v.orderspec_end ();
       i++) 
  {
    flwor_expr::orderspec_t spec = *i;
    orderSpecs.push_back(FLWORIterator::OrderSpec(pop_itstack (),
																									spec.second->empty_mode == StaticQueryContext::empty_least,
                                                  spec.second->dir == dir_descending));
  }

  std::auto_ptr<FLWORIterator::OrderByClause> orderby(orderSpecs.empty() ? NULL : new FLWORIterator::OrderByClause(orderSpecs, v.get_order_stable ()));

  PlanIter_t where = NULL;
  if (v.get_where () != NULL)
    where = pop_itstack ();

  vector<FLWORIterator::ForLetClause> clauses;
  stack<PlanIter_t> inputs;
  for (vector<rchandle<forlet_clause> >::const_iterator it = v.clause_begin ();
       it != v.clause_end();
       ++it)
  {
    inputs.push(pop_itstack());
  }

  for (vector<rchandle<forlet_clause> >::const_iterator it = v.clause_begin ();
       it != v.clause_end();
       ++it) 
  {
    PlanIter_t input = pop_stack(inputs);

    if ((*it)->type == forlet_clause::for_clause)
    {
      vector<var_iter_t> *var_iters = NULL, *pvar_iters = NULL;
      var_expr* var = (*it)->var_h.getp();
      var_expr* pos_var = (*it)->get_pos_var().getp();
      ZORBA_ASSERT( fvar_iter_map.get((uint64_t)var, var_iters) );
      if (pos_var == NULL)
      {
        clauses.push_back(FLWORIterator::ForLetClause(var, *var_iters, input));
      }
      else 
      {
        ZORBA_ASSERT( pvar_iter_map.get((uint64_t) pos_var, pvar_iters) );
        clauses.push_back(FLWORIterator::ForLetClause(var, *var_iters, *pvar_iters, input));
      }
    }
    else if ((*it)->type == forlet_clause::let_clause)
    {
      vector<ref_iter_t> *var_iters = NULL;
      var_expr* var = (*it)->var_h;
      ZORBA_ASSERT( lvar_iter_map.get((uint64_t)var, var_iters) );
      clauses.push_back(FLWORIterator::ForLetClause(var, *var_iters, input, true));
    }
  }

  FLWORIterator *iter = new FLWORIterator(v.get_loc(), clauses, where, orderby.release(), ret, false);
  itstack.push(iter);
}


bool begin_visit(case_clause& v)
{
  CODEGEN_TRACE_IN("");
  // TODO: Not implemented
  return true;
}


void end_visit(case_clause& v)
{
  CODEGEN_TRACE_OUT("");
}

bool begin_visit(promote_expr& v)
{
  CODEGEN_TRACE_IN("");
  return true;
}

void end_visit(promote_expr& v)
{
  CODEGEN_TRACE_OUT("");
  PlanIter_t lChild = pop_itstack();
  // TODO: Currently we use cast. Promotion may be more efficient.
  itstack.push(new PromoteIterator(v.get_loc(), lChild, v.get_target_type()));
}

bool begin_visit(typeswitch_expr& v)
{
  CODEGEN_TRACE_IN("");
  ZORBA_NOT_IMPLEMENTED ("typeswitch codegen");
  return true;
}

void end_visit(typeswitch_expr& v)
{
  CODEGEN_TRACE_OUT("");
}


bool begin_visit(if_expr& v)
{
  CODEGEN_TRACE_IN("");
  return true;
}


void end_visit(if_expr& v)
{
  CODEGEN_TRACE_OUT("");
  PlanIter_t iterElse = pop_itstack();
  PlanIter_t iterThen = pop_itstack();
  PlanIter_t iterCond = pop_itstack();
  PlanIter_t iterIfThenElse = new IfThenElseIterator(v.get_loc(), iterCond, iterThen, iterElse);
  itstack.push(&*iterIfThenElse);
}


bool begin_visit(fo_expr& v)
{
  CODEGEN_TRACE_IN ("");

  // If the function is an enclosed expression, push it in the constructors
  // stack to "hide" the current constructor context, if any. This way, a new
  // constructor context can be started if a node constructor exists inside
  // the enclosed expr.
  if (is_enclosed_expr(&v))
    theConstructorsStack.push(&v);

	return true;
}


void end_visit(fo_expr& v) 
{
  CODEGEN_TRACE_OUT("");

  const function* func = v.get_func();
  ZORBA_ASSERT (func != NULL);

  vector<PlanIter_t> argv (v.size ());
  generate (argv.rbegin (), argv.rend (), stack_to_generator (itstack));

  const yy::location& loc = v.get_loc ();

  if (func->validate_args (argv))
  {
    PlanIter_t iter = (*func) (loc, argv);
    ZORBA_ASSERT(iter != NULL);
    itstack.push(iter);

    if (is_enclosed_expr(&v))
    {
      expr *e = pop_stack(theConstructorsStack);
      ZORBA_ASSERT(e == &v);

      if (!theAttrContentStack.empty() && theAttrContentStack.top() == true)
        dynamic_cast<EnclosedIterator*>(iter.getp())->setAttrContent();
    }
  }
  else 
  {
    ZORBA_ERROR_ALERT_OSS(ZorbaError::XPST0017, &loc, false,
                          func->get_signature().get_name()->getStringValue(),
                          argv.size());
  }
}


bool begin_visit(ft_select_expr& v)
{
  CODEGEN_TRACE_IN("");
  return true;
}

bool begin_visit(ft_contains_expr& v)
{
  CODEGEN_TRACE_IN("");
  return true;
}

bool begin_visit(instanceof_expr& v)
{
  CODEGEN_TRACE_IN("");
  return true;
}

void end_visit(instanceof_expr& v)
{
  CODEGEN_TRACE_OUT("");
  PlanIter_t p = pop_itstack ();
  itstack.push (new InstanceOfIterator (v.get_loc (), p, v.get_type (), v.isForced ()));
}

bool begin_visit(treat_expr& v)
{
  CODEGEN_TRACE_IN("");
  return true;
}

void end_visit(treat_expr& v)
{
  CODEGEN_TRACE_OUT("");
  ZORBA_ASSERT (false);
}

bool begin_visit(castable_expr& v)
{
  CODEGEN_TRACE_IN("");
  return true;
}

void end_visit(castable_expr& v)
{
  CODEGEN_TRACE_OUT("");
  PlanIter_t lChild = pop_itstack();
  itstack.push(new CastableIterator(v.get_loc(), lChild, v.get_type()));
}

bool begin_visit(cast_expr& v)
{
  CODEGEN_TRACE_IN("");
  return true;
}

void end_visit(cast_expr& v)
{
  CODEGEN_TRACE_OUT("");
  PlanIter_t lChild = pop_itstack();
  itstack.push(new CastIterator(v.get_loc(), lChild, v.get_type()));
}

bool begin_visit(validate_expr& v)
{
  CODEGEN_TRACE_IN("");
  return true;
}

bool begin_visit(extension_expr& v)
{
  CODEGEN_TRACE_IN("");
  return true;
}


/*******************************************************************************

********************************************************************************/
bool begin_visit(relpath_expr& v)
{
  CODEGEN_TRACE_IN("");
  // Done in axis itself
  return true;
}


void end_visit(relpath_expr& v)
{
  CODEGEN_TRACE_OUT("");
}


/*******************************************************************************

********************************************************************************/
bool begin_visit(axis_step_expr& v)
{
  CODEGEN_TRACE_IN("");

  bool result = true;

  PlanIter_t input = pop_itstack();

  Assert(input != NULL);

  PlanIter_t axisIte;

  switch (v.getAxis())
  {
  case axis_kind_self:
  {
    axisIte = new SelfAxisIterator(v.get_loc(), input);
    break;
  }
  case axis_kind_child:
  {
    axisIte = new ChildAxisIterator(v.get_loc(), input);
    break;
  }
  case axis_kind_parent:
  {
    axisIte = new ParentAxisIterator(v.get_loc(), input);
    break;
  }
  case axis_kind_descendant:
  {
    axisIte = new DescendantAxisIterator(v.get_loc(), input);
    break;
  }
  case axis_kind_descendant_or_self:
  {
    axisIte = new DescendantSelfAxisIterator(v.get_loc(), input);
    break;
  }
  case axis_kind_ancestor:
  {
    axisIte = new AncestorAxisIterator(v.get_loc(), input);
    break;
  }
  case axis_kind_ancestor_or_self:
  {
    axisIte = new AncestorSelfAxisIterator(v.get_loc(), input);
    break;
  }
  case axis_kind_following_sibling:
  {
    axisIte = new RSiblingAxisIterator(v.get_loc(), input);
    break;
  }
  case axis_kind_following:
  {
    axisIte = new FollowingAxisIterator(v.get_loc(), input);
    break;
  }
  case axis_kind_preceding_sibling:
  {
    axisIte = new LSiblingAxisIterator(v.get_loc(), input);
    break;
  }
  case axis_kind_preceding:
  {
    axisIte = new PrecedingAxisIterator(v.get_loc(), input);
    break;
  }
  case axis_kind_attribute:
  {
    if (v.getTest()->getTestKind() == match_name_test ||
        v.getTest()->getTestKind() == match_attr_test ||
        v.getTest()->getTestKind() == match_xs_attr_test ||
        v.getTest()->getTestKind() == match_anykind_test)
    {
      axisIte = new AttributeAxisIterator(v.get_loc(), input);
    }
    else
    {
      axisIte = new EmptyIterator(v.get_loc());
      result = false;
    }

    break;
  }
  default:
    ZORBA_ASSERT (false && "Unknown axis kind");
  }

  itstack.push(axisIte);

  return result;
}


void end_visit(axis_step_expr& v)
{
  CODEGEN_TRACE_OUT("");
}


/*******************************************************************************

********************************************************************************/
bool begin_visit(match_expr& v)
{
  CODEGEN_TRACE_IN ("");

  PlanIter_t axisIte = pop_itstack();
  AxisIteratorHelper* axisItep = dynamic_cast<AxisIteratorHelper*>(&*axisIte);
  ZORBA_ASSERT(axisItep != NULL);

  PlanIter_t matchIte;
  Item_t qname;
  Item_t tname;

  ItemFactory& iFactory = Store::getInstance().getItemFactory();

  if (v.getTestKind() == match_name_test)
  {
    // Note: the attribute axis iterator does not do any filtering based on
    // the node kind, so it is ok to set the principal node kind to elementNode
    // in all cases.
    axisItep->setNodeKind(StoreConsts::elementNode);

    match_wild_t wildKind = v.getWildKind();

    if (wildKind == match_no_wild)
    {
      matchIte = new NameTestIterator(v.get_loc(), axisIte, v.getQName(), wildKind);
    }
    else if (wildKind == match_prefix_wild)
    {
      qname = iFactory.createQName("", "wildcard", v.getWildName().c_str());

      matchIte = new NameTestIterator(v.get_loc(), axisIte, qname, wildKind);
    }
    else if (wildKind == match_name_wild)
    {
      qname = iFactory.createQName("", v.getWildName().c_str(), "wildcard");

      matchIte = new NameTestIterator(v.get_loc(), axisIte, qname, wildKind);
    }
    else
    {
      matchIte = axisIte;
    }
  }
  else
  {
    axisItep->setNodeKind(v.getNodeKind());

    matchIte = new KindTestIterator(v.get_loc(), axisIte,
                                    v.getQName(), v.getTypeName(),
                                    v.getTestKind(), v.getDocTestKind(),
                                    v.getNilledAllowed());
  }

  itstack.push(matchIte);

  return true;
}


void end_visit(match_expr& v)
{
  CODEGEN_TRACE_OUT("");
}


/*******************************************************************************

  Node Constructors

********************************************************************************/

bool begin_visit(doc_expr& v)
{
  CODEGEN_TRACE_IN("");

  theConstructorsStack.push(&v);
  theAttrContentStack.push(false);

  return true;
}


void end_visit(doc_expr& v)
{
  CODEGEN_TRACE_OUT("");
  
  PlanIter_t lContent = pop_itstack();
  PlanIter_t lContIter = new DocumentContentIterator(v.get_loc(), lContent);
  PlanIter_t lDocIter = new DocumentIterator(v.get_loc(), lContIter);
  itstack.push(lDocIter);

  theAttrContentStack.pop();
  expr* e = pop_stack(theConstructorsStack);
  ZORBA_ASSERT(e == &v);
}


bool begin_visit(elem_expr& v)
{
  CODEGEN_TRACE_IN ("");

  if (theConstructorsStack.empty() || is_enclosed_expr(theConstructorsStack.top()))
  {
    if (theConstructorsStack.empty())
      theLastNSCtx = v.getNSCtx()->get_parent().getp();

    theNSCtxStack.push(theLastNSCtx);
    theLastNSCtx = v.getNSCtx().getp();
  }

  theConstructorsStack.push(&v);
  theAttrContentStack.push(false);

  return true;
}


void end_visit(elem_expr& v)
{
  CODEGEN_TRACE_OUT ("");

  PlanIter_t lQNameIter = 0;
  PlanIter_t lContentIter = 0;
  PlanIter_t lAttrsIter = 0;

  ItemFactory& iFactory = Store::getInstance().getItemFactory();

  if ( v.getContent() != 0 )
  {
    lContentIter = pop_itstack();
    std::vector<PlanIter_t> lArgs;
    lArgs.push_back(lContentIter);
    lContentIter = new ElementContentIterator(v.get_loc(), lArgs);
  }

  if ( v.getAttrs() != 0 )
    lAttrsIter = pop_itstack();

  lQNameIter = pop_itstack();

  bool isRoot = false;
  theAttrContentStack.pop();
  expr* e = pop_stack(theConstructorsStack);
  ZORBA_ASSERT(e == &v);
  if (theConstructorsStack.empty() || is_enclosed_expr(theConstructorsStack.top()))
  {
    isRoot = true;
    theLastNSCtx = theNSCtxStack.top();
    theNSCtxStack.pop();
  }

	PlanIter_t iter = new ElementIterator(v.get_loc(),
                                        lQNameIter,
                                        lAttrsIter,
                                        lContentIter,
                                        theLastNSCtx,
                                        v.getNSCtx().getp(),
                                        isRoot);
  itstack.push(iter);
}


bool begin_visit(attr_expr& v)
{
  CODEGEN_TRACE_IN("");

  theConstructorsStack.push(&v);
  theAttrContentStack.push(true);

	return true;
}


void end_visit(attr_expr& v)
{
  CODEGEN_TRACE_OUT("");

  PlanIter_t lQNameIter = 0;
  PlanIter_t lVarIter = 0;
  PlanIter_t lContentIter = 0;
  
  ItemFactory& iFactory = Store::getInstance().getItemFactory();

  if (v.getValueExpr() != 0) {
    lVarIter = pop_itstack();
  }
  else
  {
    lVarIter = new EmptyIterator(v.get_loc());
  }
  
  PlanIter_t lAttrIter = 0;
  lQNameIter = pop_itstack();
  
  bool isRoot = false;
  theAttrContentStack.pop();
  expr* e = pop_stack(theConstructorsStack);
  ZORBA_ASSERT(e = &v);
  if (theConstructorsStack.empty() || is_enclosed_expr(theConstructorsStack.top()))
  {
    isRoot = true;
  }

  lAttrIter = new AttributeIterator(v.get_loc(), lQNameIter, lVarIter, isRoot);
  
  itstack.push(lAttrIter);
}


bool begin_visit(text_expr& v)
{
  CODEGEN_TRACE_IN ("");

  theConstructorsStack.push(&v);
  theAttrContentStack.push(true);

	return true;
}


void end_visit(text_expr& v)
{
  CODEGEN_TRACE_OUT ("");

  PlanIter_t content = pop_itstack ();

  bool isRoot = false;
  theAttrContentStack.pop();
  expr* e = pop_stack(theConstructorsStack);
  ZORBA_ASSERT(e = &v);
  if (theConstructorsStack.empty() || is_enclosed_expr(theConstructorsStack.top()))
  {
    isRoot = true;
  }

  switch (v.get_type ()) {
  case text_expr::text_constructor:
    itstack.push (new TextIterator(v.get_loc(), content, isRoot));
    break;

  case text_expr::comment_constructor:
    itstack.push (new CommentIterator (v.get_loc(), content, isRoot));
    break;

  default:
    break;
  }
}


bool begin_visit(pi_expr& v)
{
  CODEGEN_TRACE_IN("");

  theConstructorsStack.push(&v);
  theAttrContentStack.push(true);

  return true;
}


void end_visit(pi_expr& v)
{
  CODEGEN_TRACE_OUT("");

  bool isRoot = false;
  theAttrContentStack.pop();
  expr* e = pop_stack(theConstructorsStack);
  ZORBA_ASSERT(e = &v);
  if (theConstructorsStack.empty() || is_enclosed_expr(theConstructorsStack.top()))
  {
    isRoot = true;
  }

  PlanIter_t content = pop_itstack ();
  PlanIter_t target = pop_itstack ();
  itstack.push (new PiIterator (v.get_loc (), target, content, isRoot));
}


bool begin_visit (function_def_expr &v) {
  CODEGEN_TRACE_IN("");
  return true;
}

void end_visit (function_def_expr &v) {
  CODEGEN_TRACE_OUT("");
}


bool begin_visit(const_expr& v)
{
  CODEGEN_TRACE_IN ("");
  return true;
}

void end_visit(const_expr& v)
{
  CODEGEN_TRACE_OUT("");
  PlanIter_t it = new SingletonIterator (v.get_loc (), v.get_val ());
  itstack.push (it);
}


bool begin_visit(order_expr& v)
{
  CODEGEN_TRACE_IN("");
  return true;
}

void end_visit(order_expr& v)
{
  CODEGEN_TRACE_OUT("");
}


/*******************************************************************************


********************************************************************************/

/*..........................................
 :  end visit                              :
 :.........................................*/

void end_visit(ft_select_expr& v)
{
  CODEGEN_TRACE_OUT("");
}

void end_visit(ft_contains_expr& v)
{
  CODEGEN_TRACE_OUT("");
}

void end_visit(validate_expr& v)
{
  CODEGEN_TRACE_OUT("");
}

void end_visit(extension_expr& v)
{
  CODEGEN_TRACE_OUT("");
}


};

PlanIter_t codegen (expr *root, hash64map<std::vector<ref_iter_t> *> *param_var_map) {
  plan_visitor c(param_var_map);
  root->accept (c);
  return c.pop_itstack ();
}

} /* namespace xqp */
