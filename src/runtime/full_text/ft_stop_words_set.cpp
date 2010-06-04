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

#include "runtime/full_text/ft_stop_words_set.h"
#include "zorbautils/stl_util.h"

using namespace std;

namespace zorba {

extern char const *const stop_words_de[];
extern char const *const stop_words_en[];
extern char const *const stop_words_es[];
extern char const *const stop_words_fr[];

typedef char const *const *ft_stop_table;

///////////////////////////////////////////////////////////////////////////////

static ft_stop_table get_table( lang::iso639_1 code ) {
  switch ( code ) {
    case lang::de:
      return stop_words_de;
    case lang::fr:
      return stop_words_fr;
    case lang::en:
      return stop_words_en;
    case lang::es:
      return stop_words_es;
#if 0
    case land::da:
      return stop_words_da;
    case land::fi:
      return stop_words_fi;
    case land::hu:
      return stop_words_hu;
    case land::nl:
      return stop_words_nl;
    case land::no:
      return stop_words_no;
    case land::pt:
      return stop_words_pt;
    case land::ro:
      return stop_words_ro;
    case land::ru:
      return stop_words_ru;
    case land::sv:
      return stop_words_sv;
#endif
    default:
      return 0;
  }
}

///////////////////////////////////////////////////////////////////////////////

ft_stop_words_set const*
ft_stop_words_set::construct( ftstop_word_option const &option,
                              lang::iso639_1 code ) {
  bool must_delete;
  set_t *word_set;

  switch ( option.get_mode() ) {
    case ft_stop_words_mode::with:
      word_set = new set_t;
      must_delete = true;
      break;
    case ft_stop_words_mode::with_default:
      word_set = get_default_word_set_for( code );
      if ( !word_set ) {
        // TODO: throw exception?
        return 0;
      }
      must_delete = false;
      break;
    case ft_stop_words_mode::without:
      return 0;
  }

  FOR_EACH( ftstop_word_option::list_t, ftsw, option.get_stop_words() ) {
    ftstop_words::list_t const *words = &(*ftsw)->get_list();
    if ( words->empty() ) {
      //string const &uri = (*ftsw)->get_uri();
      // TODO: fetch words from URI
      //words = ???
    }
    if ( words && !words->empty() ) {
      if ( !must_delete ) {
        word_set = new set_t( *word_set );
        must_delete = true;
      }
      ft_stop_words_unex::type const mode = (*ftsw)->get_mode();
      FOR_EACH( ftstop_words::list_t, word, *words ) {
        switch ( mode ) {
          case ft_stop_words_unex::union_:
            word_set->insert( *word );
            break;
          case ft_stop_words_unex::except:
            word_set->erase( *word );
            break;
        }
      }
    }
  }
  return new ft_stop_words_set( word_set, must_delete );
}

ft_stop_words_set::set_t*
ft_stop_words_set::get_default_word_set_for( lang::iso639_1 code ) {
  static set_t* cached_word_sets[ lang::unknown_639_1 ];
  if ( code == lang::unknown_639_1 )
    return 0;
  set_t *&word_set = cached_word_sets[ code ];
  if ( !word_set ) {
    if ( ft_stop_table const table = get_table( code ) ) {
      word_set = new set_t;
      for ( ft_stop_table word = table; *word; ++word ) {
        word_set->insert( *word );
      }
    }
  }
  return word_set;
}

///////////////////////////////////////////////////////////////////////////////

} // namespace zorba
/* vim:set et sw=2 ts=2: */
