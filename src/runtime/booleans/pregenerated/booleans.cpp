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
 
// ******************************************
// *                                        *
// * THIS IS A GENERATED FILE. DO NOT EDIT! *
// * SEE .xml FILE WITH SAME NAME           *
// *                                        *
// ******************************************


#include "stdafx.h"
#include "zorbatypes/rchandle.h"
#include "zorbatypes/zstring.h"
#include "runtime/visitors/planiter_visitor.h"
#include "runtime/booleans/booleans.h"
#include "system/globalenv.h"



namespace zorba {

// <IsSameNodeIterator>
SERIALIZABLE_CLASS_VERSIONS(IsSameNodeIterator)

void IsSameNodeIterator::serialize(::zorba::serialization::Archiver& ar)
{
  serialize_baseclass(ar,
  (NaryBaseIterator<IsSameNodeIterator, PlanIteratorState>*)this);
}


void IsSameNodeIterator::accept(PlanIterVisitor& v) const
{
  if (!v.hasToVisit(this))
    return;

  v.beginVisit(*this);

  std::vector<PlanIter_t>::const_iterator lIter = theChildren.begin();
  std::vector<PlanIter_t>::const_iterator lEnd = theChildren.end();
  for ( ; lIter != lEnd; ++lIter ){
    (*lIter)->accept(v);
  }

  v.endVisit(*this);
}

IsSameNodeIterator::~IsSameNodeIterator() {}


zstring IsSameNodeIterator::getNameAsString() const {
  return "op:is-same-node";
}
// </IsSameNodeIterator>


// <NodeBeforeIterator>
SERIALIZABLE_CLASS_VERSIONS(NodeBeforeIterator)

void NodeBeforeIterator::serialize(::zorba::serialization::Archiver& ar)
{
  serialize_baseclass(ar,
  (NaryBaseIterator<NodeBeforeIterator, PlanIteratorState>*)this);
}


void NodeBeforeIterator::accept(PlanIterVisitor& v) const
{
  if (!v.hasToVisit(this))
    return;

  v.beginVisit(*this);

  std::vector<PlanIter_t>::const_iterator lIter = theChildren.begin();
  std::vector<PlanIter_t>::const_iterator lEnd = theChildren.end();
  for ( ; lIter != lEnd; ++lIter ){
    (*lIter)->accept(v);
  }

  v.endVisit(*this);
}

NodeBeforeIterator::~NodeBeforeIterator() {}


zstring NodeBeforeIterator::getNameAsString() const {
  return "op:node-before";
}
// </NodeBeforeIterator>


// <NodeAfterIterator>
SERIALIZABLE_CLASS_VERSIONS(NodeAfterIterator)

void NodeAfterIterator::serialize(::zorba::serialization::Archiver& ar)
{
  serialize_baseclass(ar,
  (NaryBaseIterator<NodeAfterIterator, PlanIteratorState>*)this);
}


void NodeAfterIterator::accept(PlanIterVisitor& v) const
{
  if (!v.hasToVisit(this))
    return;

  v.beginVisit(*this);

  std::vector<PlanIter_t>::const_iterator lIter = theChildren.begin();
  std::vector<PlanIter_t>::const_iterator lEnd = theChildren.end();
  for ( ; lIter != lEnd; ++lIter ){
    (*lIter)->accept(v);
  }

  v.endVisit(*this);
}

NodeAfterIterator::~NodeAfterIterator() {}


zstring NodeAfterIterator::getNameAsString() const {
  return "op:node-after";
}
// </NodeAfterIterator>



}


