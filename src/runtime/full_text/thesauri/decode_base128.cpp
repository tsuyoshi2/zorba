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

namespace zorba {

unsigned decode_base128( char const **p ) {
  unsigned char const *&u = *reinterpret_cast<unsigned char const**>( p );
  unsigned n = 0;
  do {
    n = (n << 7) | (*u & 0x7F);
  } while ( *u++ & 0x80 );
  return n;
}

} // namespace zorba
/* vim:set et sw=2 ts=2: */
