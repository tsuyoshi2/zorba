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
#pragma once
#ifndef ZORBA_FULL_TEXT_WORDNET_THESAURUS_H
#define ZORBA_FULL_TEXT_WORDNET_THESAURUS_H

#include <deque>
#include <set>
#include <utility>                      /* for pair */

#include "util/mmap_file.h"

#include "../thesaurus.h"
#include "iso2788.h"
#include "wn_db_segment.h"
#include "wn_types.h"

namespace zorba {
namespace wordnet {

///////////////////////////////////////////////////////////////////////////////

/**
 * A %wordnet::thesaurus is a Thesaurus for Wordnet.
 * See: http://wordnet.princeton.edu/
 */
class thesaurus : public internal::Thesaurus {
public:
  thesaurus( zstring const &path, locale::iso639_1::type lang );
  ~thesaurus();

  // inherited
  void destroy() const;
  iterator::ptr lookup( zstring const&, zstring const&, level_type,
                        level_type ) const;

private:
  /**
   * A %wordnet_file_checker is a dummy class that's used in order to be able
   * to perform sanity checking on the WordNet binary thesaurus file in the
   * mem-initializer list of the thesaurus constructor.
   *
   * An instance of this class \e must be declared \e after the instance of
   * mmap_file so that the mmap_file is initialized before the check is
   * performed.
   */
  struct wordnet_file_checker {
    wordnet_file_checker( mmap_file const& );
  };

  //
  // These data members MUST be declared in this order so they're initialized
  // in this order.
  //
  mmap_file const wordnet_file_;
  wordnet_file_checker const wordnet_file_checker_;
  db_segment const wn_lemmas_;
  db_segment const wn_synsets_;
  locale::iso639_1::type const lang_;

  char const* find_lemma( zstring const &phrase ) const;

  // forbid these
  thesaurus( thesaurus const& );
  thesaurus& operator=( thesaurus const& );

  /////////////////////////////////////////////////////////////////////////////

  class iterator : public internal::Thesaurus::iterator {
  public:
    // inherited
    void destroy() const;
    bool next( zstring* );

  private:
    iterator( thesaurus const&, char const *lemma, pointer::type,
              level_type at_least, level_type at_most );
    ~iterator();

    thesaurus const &thesaurus_;

    /**
     * The WordNet pointer type that is the closest equivalent of the
     * "relationship" given in the original query, if any.
     */
    pointer::type query_ptr_type_;
  
    level_type const at_least_, at_most_;
    level_type level_;
  
    typedef std::pair<synset_id_t,iso2788::rel_dir> candidate_t;
    typedef std::deque<candidate_t> candidate_queue_t;
    candidate_queue_t candidate_queue_;
  
    typedef std::deque<lemma_id_t> result_queue_t;
    result_queue_t result_queue_;
  
    typedef std::set<lemma_id_t> seen_set_t;
    seen_set_t synonyms_seen_;

    static candidate_queue_t::value_type const LevelMarker;

    // forbid these
    iterator( iterator const& );
    iterator& operator=( iterator const& );

    friend class thesaurus;
  };

  friend class iterator;
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A %wordnet::provider is a ThesaurusProvider for Wordnet.
 */
class provider : public internal::ThesaurusProvider {
public:
  /**
   * Constructs a %provider.
   *
   * @param path The relative path of where the wordnet-LL.zth file is located
   * (where LL is the ISO 639-1 language code of the language).
   */
  provider( zstring const &path );

  // inherited
  bool getThesaurus( locale::iso639_1::type,
                     internal::Thesaurus::ptr* = nullptr ) const;

private:
  zstring const path_;
};

///////////////////////////////////////////////////////////////////////////////

} // namespace wordnet
} // namespace zorba

#endif  /* ZORBA_FULL_TEXT_WORDNET_THESAURUS_H */
/* vim:set et sw=2 ts=2: */
