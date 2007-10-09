/* -*- mode: c++; indent-tabs-mode: nil; tab-width: 2 -*-
 *
 *  $Id: function.h,v 1.1 2006/10/09 07:07:59 Paul Pedersen Exp $
 *
 *	Copyright 2006-2007 FLWOR Foundation.
 *  Authors: John Cowan,Paul Pedersen
 *
 */
 
#ifndef XQP_FUNCTION_H
#define XQP_FUNCTION_H

#include <string>
#include <vector>

#include "functions/signature.h"
#include "context/common.h"
 
namespace xqp {

template <class Object> class rchandle;

typedef rchandle<class BasicIterator> Iterator_t;

class function : public rcobject
{
protected:
	signature sig;
	
public:
	function() {}
	function(const signature& _sig) : sig(_sig) {}
	virtual ~function() {}

public:
	// XQuery signature (name+arity)
	const Item* get_fname() const { return sig.get_name(); }
	void set_signature(signature& _sig) { sig = _sig; }

	// codegen: functor specification
	virtual Iterator_t operator()(const yy::location& loc, std::vector<Iterator_t>& argv) const = 0;

	// polymorphic type inference
	virtual sequence_type_t type_check(signature&) const = 0;

	// runtime arg validation: XXX move this out
	virtual bool validate_args(std::vector<Iterator_t>& argv) const = 0;

};


/*
class user_function : public function
{
private:
	parsenode* body_node;
	expr* body_expr;

};
*/


} /* namespace xqp */
#endif  /* XQP_FUNCTION_H */

