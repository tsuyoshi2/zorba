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

#include <iostream>

#include <algorithm>                    /* for equal_range */
#include <utility>                      /* for pair */

#include "util/ascii_util.h"
#include "util/less.h"
#include "util/mmap_file.h"

#include "decode_base128.h"
#include "wn_db_segment.h"
#include "wn_synset.h"
#include "wn_thesaurus.h"
#include "wn_types.h"

using namespace std;

namespace zorba {
namespace wordnet {

////////// Helper functions ///////////////////////////////////////////////////

/**
 * Gets a reference to a singleton instance of the WordNet thesaurus database
 * file.
 *
 * @return Returns said reference.
 */
static mmap_file const& get_db_file() {
  static mmap_file thesaurus_file;
  if ( !thesaurus_file )
    thesaurus_file.open( "/usr/local/tmp/zorba/wordnet.zth" ); // TODO
  return thesaurus_file;
}

/**
 * Gets a reference to a singleton instance of a WordNet thesaurus database
 * segment.
 *
 * @return Returns said segment.
 */
template<db_segment::id_t SegID>
static db_segment const& get_segment() {
  static db_segment const segment( get_db_file(), SegID );
  return segment;
}

#define GET_SEGMENT(SEG_ID) get_segment<db_segment::SEG_ID>()

/**
 * Attempts to find a lemma within the WordNet thesaurus.
 *
 * @param phrase The phrase to search for.
 * @return Returns said lemma or \c NULL if not found.
 */
static char const* find_lemma( zstring const &phrase ) {
  typedef pair<db_segment::const_iterator,db_segment::const_iterator>
    lemma_range;

  db_segment const &lemmas = GET_SEGMENT( lemma );
  char const *const c_phrase = phrase.c_str();
  less<char const*> comparator;

  lemma_range const range =
    ::equal_range( lemmas.begin(), lemmas.end(), c_phrase, comparator );
  if ( range.first == lemmas.end() || comparator( c_phrase, *range.first ) )
    return NULL;
  return *range.first;
}

/**
 * Attempts to map an XQuery thesaurus relationship to a WordNet pointer type.
 *
 * @param relationship The XQuery thesaurus relationship.
 * @return Returns the corresponding Wordnet pointer type.
 */
static pointer::type map_xquery_rel( zstring const &relationship ) {
  zstring relationship_lower;
  ascii::to_lower( relationship, &relationship_lower );
  if ( iso2788::rel_type iso_rel = iso2788::find_rel( relationship_lower ) )
    return pointer::map_iso_rel( iso_rel );
  return pointer::find( relationship_lower );
}

///////////////////////////////////////////////////////////////////////////////

synset_id_t const LevelSentinel = ~0u;

thesaurus::thesaurus( zstring const &phrase, zstring const &relationship,
                      ft_int at_least, ft_int at_most ) :
  ptr_type_( map_xquery_rel( relationship ) ),
  at_least_( at_least ), at_most_( at_most ), level_( 0 )
{
  if ( char const *p = find_lemma( phrase ) ) {
    while ( *p++ ) ;                    // skip past lemma
    //
    // Load the synset_id_queue_ will all the synsets for the lemma.
    //
    cout << "thesaurus::thesaurus()" << endl;
    for ( unsigned num_synsets = decode_base128( &p ); num_synsets-- > 0; ) {
      synset_id_t const synset_id = decode_base128( &p );
      cout << "+ synset_id=" << synset_id << endl;
      synset_id_queue_.push_back( synset_id );
    }
  }
}

thesaurus::~thesaurus() {
  // do nothing
}

bool thesaurus::next( zstring *synonym ) {
  while ( synonym_queue_.empty() ) {
    cout << "synonym_queue is empty" << endl;

    if ( synset_id_queue_.empty() ) {
      cout << "synset_id_queue is empty" << endl;
      return false;
    }

    synset_id_t const synset_id = pop_front( synset_id_queue_ );
    if ( synset_id == LevelSentinel ) {
      cout << "+ found LevelSentinel" << endl;
      if ( ++level_ > at_most_ ) {
        cout << "+ level (" << level_ << ") > at_most (" << at_most_ << ')' << endl;
        return false;
      }
      continue;
    }

    cout << "synset_id=" << synset_id << endl;

    synset const ss( GET_SEGMENT( synset )[ synset_id ] );
    if ( level_ >= at_least_ ) {
      cout << "+ level (" << level_ << ") >= at_least (" << at_least_ << ')' << endl;
      FOR_EACH( synset::lemma_id_list, lemma_id, ss.lemma_ids_ ) {
        if ( synonyms_seen_.insert( *lemma_id ).second )
          synonym_queue_.push_back( *lemma_id );
      }

      cout << "+ copying lemmas; synonym_queue is now: ";
      bool comma = false;
      FOR_EACH( synonym_queue, lemma_id, synonym_queue_ ) {
        if ( comma )
          cout << ", ";
        else
          comma = true;
        cout << GET_SEGMENT( lemma )[ *lemma_id ];
      }
      cout << endl;
    }

    FOR_EACH( synset::ptr_list, ptr, ss.ptr_list_ ) {
      if ( ptr_type_ ) {
        //
        // A pointer type (relationship) was given for the thesaurus option: if
        // this ptr's type doesn't match, skip it.
        //
        if ( ptr->type_ != ptr_type_ )
          continue;
      } else {
        switch ( ptr->type_ ) {
          case pointer::antonym:
          case pointer::attribute:
          case pointer::cause:
          case pointer::derivationally_related_form:
          case pointer::derived_from_adjective:
          case pointer::entailment:
          case pointer::member_holonym:
          case pointer::member_meronym:
          case pointer::part_holonym:
          case pointer::part_meronym:
          case pointer::participle_of_verb:
          case pointer::pertainym:
          case pointer::substance_holonym:
          case pointer::substance_meronym:
            continue;
          case pointer::also_see:
          case pointer::hypernym:
          case pointer::hyponym:
          case pointer::instance_hypernym:
          case pointer::instance_hyponym:
          case pointer::similar_to:
          case pointer::verb_group:

          case pointer::domain_of_synset_region: // ?
          case pointer::domain_of_synset_topic: // ?
          case pointer::domain_of_synset_usage: // ?
          case pointer::member_of_domain_region: // ?
          case pointer::member_of_domain_topic: // ?
          case pointer::member_of_domain_usage: // ?

          default:
            cout << "ptr type=" << pointer::string_of[ ptr->type_ ] << endl;
            break;
        }
      }
      synset_id_queue_.push_back( ptr->synset_id_ );

      if ( ptr->source_ ) {
        // TODO
        ptr->target_;
      } else {
        // TODO
      }
    } // FOR_EACH

    //
    // The synset IDs of all the pointers of this synset constitute a "level",
    // so add the sentinel to increment the level.
    //
    synset_id_queue_.push_back( LevelSentinel );
  } // while

  *synonym = GET_SEGMENT( lemma )[ pop_front( synonym_queue_ ) ];
  cout << "--> synonym=" << *synonym << endl;
  return true;
}

///////////////////////////////////////////////////////////////////////////////

} // namespace wordnet
} // namespace zorba
/* vim:set et sw=2 ts=2: */
