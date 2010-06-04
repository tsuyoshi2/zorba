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

#include "compiler/expression/ftnode.h"
#include "runtime/full_text/ft_stop_words_set.h"
#include "store/api/ft_token.h"
#include "zorbautils/stl_util.h"

using namespace std;

namespace zorba {

/**
 * Test whether the 2 given tokens match given the match options.
 *
 * @param dts         The document token string.
 * @param qts         The query token string.
 * @param options     The match options to use.
 * @param stop_words  The stop words to use.
 */
bool match_tokens( FTToken::string_t const &dts, FTToken::string_t const &qts,
                   ftmatch_options const &options,
                   ft_stop_words_set const *stop_words ) {
  FTToken::string_t dts2, qts2;
  FTToken::string_t const *pdts = &dts, *pqts = &qts;
  bool qt_is_lower = false;

  if ( ftcase_option const *const c = options.get_case_option() ) {
    switch ( c->get_mode() ) {
      case ft_case_mode::insensitive:
        dts2 = *pdts->lowercase(), pdts = &dts2;
        qts2 = *pqts->lowercase(), pqts = &qts2;
        qt_is_lower = true;
        break;
      case ft_case_mode::sensitive:
        // do nothing
        break;
      case ft_case_mode::lower:
        qts2 = *pqts->lowercase(); pqts = &qts2;
        qt_is_lower = true;
        break;
      case ft_case_mode::upper:
        qts2 = *pqts->uppercase(); pqts = &qts2;
        break;
    }
  }

  if ( ftdiacritics_option const *const d = options.get_diacritics_option() ) {
    switch ( d->get_mode() ) {
      case ft_diacritics_mode::insensitive:
        // TODO
        break;
      case ft_diacritics_mode::sensitive:
        // do nothing
        break;
    }
  }

  if ( ftextension_option const *const e = options.get_extension_option() ) {
    // TODO
  }

  if ( ftlanguage_option const *const l = options.get_language_option() ) {
    // string const &lang = l->get_language();
    // TODO
  }

  if ( ftstem_option const *const s = options.get_stem_option() ) {
    switch ( s->get_mode() ) {
      case ft_stem_mode::with:
        // TODO
        break;
      case ft_stem_mode::without:
        // do nothing
        break;
    }
  }

  if ( stop_words ) {
    //
    // Perform stop-word comparison case-insensitively.  If the query token has
    // already been converted to lower-case above, just use it as-is; otherwise
    // convert to lower-case here.
    //
    // Note, however, that the spec isn't clear on whether this should be done
    // case-insensitively.
    //
    FTToken::string_t lcqts;            // lower case query token string
    FTToken::string_t const *const plcqts = qt_is_lower ?
      pqts : (lcqts = *pqts->lowercase(), &lcqts);

    if ( stop_words->contains( plcqts->str() ) )
      return true;
  }

  if ( ftthesaurus_option const *const t = options.get_thesaurus_option() ) {
    if ( !t->no_thesaurus() ) {
      // TODO
    }
  }

  if ( ftwild_card_option const *const wc = options.get_wild_card_option() ) {
    switch ( wc->get_mode() ) {
      case ft_wild_card_mode::with:
        // TODO
        break;
      case ft_wild_card_mode::without:
        // do nothing
        break;
    }
  }

  return *pdts == *pqts;
}

} // namespace zorba
/* vim:set et sw=2 ts=2: */
