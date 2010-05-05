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

#ifndef ZORBA_COMPILER_FTEXPR_H
#define ZORBA_COMPILER_FTEXPR_H

#include "common/shared_types.h"

#include "compiler/expression/expr_base.h"
#include "compiler/expression/fo_expr.h"
#include "compiler/expression/ftexpr_classes.h"
#include "compiler/parser/parse_constants.h"
#include "compiler/parser/ft_types.h"
#include "compiler/parsetree/parsenodes.h"
#include "runtime/base/plan_iterator.h"

namespace zorba {

class ftselection_visitor;


///////////////////////////////////////////////////////////////////////////////

/**
 * An ft_expr is-an expr that is the abstract base class for all full-text
 * expressions.  It is also the interface between the expr tree and the ftexpr
 * tree.
 */
class ft_expr : public expr {
public:
  SERIALIZABLE_ABSTRACT_CLASS(ft_expr)
  SERIALIZABLE_CLASS_CONSTRUCTOR2(ft_expr,expr)
  void serialize( serialization::Archiver& );

  ~ft_expr();

  std::ostream& put( std::ostream& ) const;

protected:
  ft_expr( static_context* sctx, const QueryLoc& );
};


/**
 * An ftcontains_expr is-an ft_expr for the FTContainsExpr.
 */
class ftcontains_expr : public ft_expr 
{
  friend class ExprIterator;

public:
  SERIALIZABLE_ABSTRACT_CLASS(ftcontains_expr)
  SERIALIZABLE_CLASS_CONSTRUCTOR2(ftcontains_expr,ft_expr)
  void serialize( serialization::Archiver& );

  ftcontains_expr(
    static_context* sctx,
    QueryLoc const&,
    range_expr_t range,
    ftexpr *ftselection,
    union_expr_t ftignore
  );

  void compute_scripting_kind();

  range_expr_t get_range() const { return range_; }

  ftexpr* get_ftselection() const { return ftselection_; }

  union_expr_t get_ignore() const { return ftignore_; }

  void accept( expr_visitor& );

  std::ostream& put( std::ostream& ) const;

private:
  range_expr_t         range_;
  ftexpr             * ftselection_;
  union_expr_t         ftignore_;

  std::vector<expr_t>  ftselection_exprs_;
};


///////////////////////////////////////////////////////////////////////////////

/**
 * Base class for full-text expression classes.
 */
class ftexpr : public serialization::SerializeBaseClass {
public:
  SERIALIZABLE_ABSTRACT_CLASS(ftexpr)
  void serialize( serialization::Archiver& );

  virtual ~ftexpr();

  virtual ft_visit_result::type accept( ftselection_visitor& ) = 0;
  QueryLoc const& get_loc() const { return loc_; }

protected:
  ftexpr( QueryLoc const &loc ) : loc_( loc ) {
  }

private:
  QueryLoc loc_;
};


////////// FTMatchOptions /////////////////////////////////////////////////////

class ftmatch_option : public ftexpr {
public:
  SERIALIZABLE_ABSTRACT_CLASS(ftmatch_option)
  SERIALIZABLE_CLASS_CONSTRUCTOR2(ftmatch_option,ftexpr)
  void serialize( serialization::Archiver& );

protected:
  ftmatch_option( QueryLoc const &loc ) : ftexpr( loc ) { }
};


class ftcase_option : public ftmatch_option {
public:
  SERIALIZABLE_CLASS(ftcase_option)
  SERIALIZABLE_CLASS_CONSTRUCTOR2(ftcase_option,ftmatch_option)
  void serialize( serialization::Archiver& );

  ftcase_option(
    QueryLoc const&,
    ft_case_mode::type = ft_case_mode::DEFAULT
  );

  ft_visit_result::type accept( ftselection_visitor& );
  ft_case_mode::type get_mode() const { return mode_; }

private:
  ft_case_mode::type mode_;
};


class ftdiacritics_option : public ftmatch_option {
public:
  SERIALIZABLE_CLASS(ftdiacritics_option)
  SERIALIZABLE_CLASS_CONSTRUCTOR2(ftdiacritics_option,ftmatch_option)
  void serialize( serialization::Archiver& );

  ftdiacritics_option(
    QueryLoc const&,
    ft_diacritics_mode::type = ft_diacritics_mode::DEFAULT
  );

  ft_visit_result::type accept( ftselection_visitor& );
  ft_diacritics_mode::type get_mode() const { return mode_; }

private:
  ft_diacritics_mode::type mode_;
};


class ftextension_option : public ftmatch_option {
public:
  SERIALIZABLE_CLASS(ftextension_option)
  SERIALIZABLE_CLASS_CONSTRUCTOR2(ftextension_option,ftmatch_option)
  void serialize( serialization::Archiver& );

  ftextension_option(
    QueryLoc const&,
    rchandle<QName>,
    std::string const &val
  );

  ft_visit_result::type accept( ftselection_visitor& );
  rchandle<QName> get_qname() const { return qname_; }
  std::string const& get_val() const { return val_; }

private:
  rchandle<QName> qname_;
  std::string val_;
};


class ftlanguage_option : public ftmatch_option {
public:
  SERIALIZABLE_CLASS(ftlanguage_option)
  SERIALIZABLE_CLASS_CONSTRUCTOR2(ftlanguage_option,ftmatch_option)
  void serialize( serialization::Archiver& );

  ftlanguage_option(
    QueryLoc const&,
    std::string const &language
  );

  ft_visit_result::type accept( ftselection_visitor& );
  std::string const& get_language() const { return language_; }

private:
  std::string language_;
};


class ftstem_option : public ftmatch_option {
public:
  SERIALIZABLE_CLASS(ftstem_option)
  SERIALIZABLE_CLASS_CONSTRUCTOR2(ftstem_option,ftmatch_option)
  void serialize( serialization::Archiver& );

  ftstem_option(
    QueryLoc const&,
    ft_stem_mode::type = ft_stem_mode::DEFAULT
  );

  ft_visit_result::type accept( ftselection_visitor& );
  ft_stem_mode::type get_mode() const { return mode_; }

private:
  ft_stem_mode::type mode_;
};


class ftstop_words : public ftexpr {
public:
  SERIALIZABLE_CLASS(ftstop_words)
  SERIALIZABLE_CLASS_CONSTRUCTOR2(ftstop_words,ftexpr)
  void serialize( serialization::Archiver& );

  typedef std::list<std::string> list_t;

  ftstop_words(
    QueryLoc const&,
    std::string const &uri,
    list_t const&,
    ft_stop_words_unex::type = ft_stop_words_unex::union_
  );

  ft_visit_result::type accept( ftselection_visitor& );
  std::string const& get_uri() const { return uri_; }
  list_t const& get_list() const { return list_; }
  ft_stop_words_unex::type get_mode() const { return mode_; }

private:
  std::string uri_;
  list_t list_;
  ft_stop_words_unex::type mode_;
};


class ftstop_word_option : public ftmatch_option {
public:
  SERIALIZABLE_CLASS(ftstop_word_option)
  SERIALIZABLE_CLASS_CONSTRUCTOR2(ftstop_word_option,ftmatch_option)
  void serialize( serialization::Archiver& );

  typedef std::list<ftstop_words*> stop_word_list_t;

  ftstop_word_option(
    QueryLoc const&,
    ft_stop_words_mode::type = ft_stop_words_mode::DEFAULT
  );

  ftstop_word_option(
    QueryLoc const&,
    stop_word_list_t&,
    ft_stop_words_mode::type = ft_stop_words_mode::DEFAULT
  );

  ~ftstop_word_option();

  ft_visit_result::type accept( ftselection_visitor& );
  ft_stop_words_mode::type get_mode() const { return mode_; }
  stop_word_list_t const& get_stop_words() const { return stop_words_; }

private:
  stop_word_list_t stop_words_;
  ft_stop_words_mode::type mode_;
};


class ftthesaurus_id : public ftexpr {
public:
  SERIALIZABLE_CLASS(ftthesaurus_id)
  SERIALIZABLE_CLASS_CONSTRUCTOR2(ftthesaurus_id,ftexpr)
  void serialize( serialization::Archiver& );

  ftthesaurus_id(
    QueryLoc const&,
    std::string const &uri,
    std::string const &relationship,
    ftrange_expr *levels
  );
  ~ftthesaurus_id();

  ft_visit_result::type accept( ftselection_visitor& );
  std::string const& get_uri() const { return uri_; }
  std::string const& get_relationship() const { return relationship_; }
  ftrange_expr const* get_levels() const { return levels_; }

private:
  std::string uri_;
  std::string relationship_;
  ftrange_expr *levels_;
};


class ftthesaurus_option : public ftmatch_option {
public:
  SERIALIZABLE_CLASS(ftthesaurus_option)
  SERIALIZABLE_CLASS_CONSTRUCTOR2(ftthesaurus_option,ftmatch_option)
  void serialize( serialization::Archiver& );

  typedef std::list<ftthesaurus_id*> thesaurus_id_list_t;

  ftthesaurus_option(
    QueryLoc const&,
    thesaurus_id_list_t&,
    bool includes_default,
    bool no_thesaurus
  );
  ~ftthesaurus_option();

  thesaurus_id_list_t const& get_thesaurus_id_list() const
    { return thesaurus_id_list_; }

  ft_visit_result::type accept( ftselection_visitor& );
  bool includes_default() const { return includes_default_; }
  bool no_thesaurus() const { return no_thesaurus_; }

private:
  thesaurus_id_list_t thesaurus_id_list_;
  bool includes_default_;
  bool no_thesaurus_;
};


class ftwild_card_option : public ftmatch_option {
public:
  SERIALIZABLE_CLASS(ftwild_card_option)
  SERIALIZABLE_CLASS_CONSTRUCTOR2(ftwild_card_option,ftmatch_option)
  void serialize( serialization::Archiver& );

  ftwild_card_option(
    QueryLoc const&,
    ft_wild_card_mode::type = ft_wild_card_mode::DEFAULT
  );

  ft_visit_result::type accept( ftselection_visitor& );
  ft_wild_card_mode::type get_mode() const { return mode_; }

private:
  ft_wild_card_mode::type mode_;
};


class ftmatch_options : public ftexpr {
public:
  SERIALIZABLE_CLASS(ftmatch_options)
  SERIALIZABLE_CLASS_CONSTRUCTOR2(ftmatch_options,ftexpr)
  void serialize( serialization::Archiver& );

  ftmatch_options( QueryLoc const& );
  ~ftmatch_options();

  ft_visit_result::type accept( ftselection_visitor& );

  ftcase_option const* get_case_option() const { return case_option_; }

  ftdiacritics_option const* get_diacritics_option() const
    { return diacritics_option_; }

  ftextension_option const* get_extension_option() const
    { return extension_option_; }

  ftlanguage_option const* get_language_option() const
    { return language_option_; }

  ftstem_option const* get_stem_option() const { return stem_option_; }

  ftstop_word_option const* get_stop_word_option() const
    { return stop_word_option_; }

  ftthesaurus_option const* get_thesaurus_option() const
    { return thesaurus_option_; }

  ftwild_card_option const* get_wild_card_option() const
    { return wild_card_option_; }

  void set_case_option( ftcase_option *o ) {
    delete case_option_;
    case_option_ = o;
  }

  void set_diacritics_option( ftdiacritics_option *o ) {
    delete diacritics_option_;
    diacritics_option_ = o;
  }

  void set_extension_option( ftextension_option *o ) {
    delete extension_option_;
    extension_option_ = o;
  }

  void set_language_option( ftlanguage_option *o ) {
    delete language_option_;
    language_option_ = o;
  }

  void set_stem_option( ftstem_option *o ) {
    delete stem_option_;
    stem_option_ = o;
  }

  void set_stop_word_option( ftstop_word_option *o ) {
    delete stop_word_option_;
    stop_word_option_ = o;
  }

  void set_thesaurus_option( ftthesaurus_option *o ) {
    delete thesaurus_option_;
    thesaurus_option_ = o;
  }

  void set_wild_card_option( ftwild_card_option *o ) {
    delete wild_card_option_;
    wild_card_option_ = o;
  }

private:
  ftcase_option *case_option_;
  ftdiacritics_option *diacritics_option_;
  ftextension_option *extension_option_;
  ftlanguage_option *language_option_;
  ftstem_option *stem_option_;
  ftstop_word_option *stop_word_option_;
  ftthesaurus_option *thesaurus_option_;
  ftwild_card_option *wild_card_option_;
};

////////// FTPrimary //////////////////////////////////////////////////////////

class ftprimary_expr : public ftexpr {
public:
  SERIALIZABLE_ABSTRACT_CLASS(ftprimary_expr)
  SERIALIZABLE_CLASS_CONSTRUCTOR2(ftprimary_expr,ftexpr)
  void serialize( serialization::Archiver &ar ) {
    serialize_baseclass( ar, (ftexpr*)this );
  }

protected:
  ftprimary_expr( QueryLoc const &loc ) : ftexpr( loc ) { }
};


class ftextension_selection_expr : public ftprimary_expr {
public:
  SERIALIZABLE_CLASS(ftextension_selection_expr)
  SERIALIZABLE_CLASS_CONSTRUCTOR2(ftextension_selection_expr,ftprimary_expr)
  void serialize( serialization::Archiver& );

  ftextension_selection_expr(
    QueryLoc const&,
    /* TODO: pragma_list, */
    ftselection_expr*
  );

  ft_visit_result::type accept( ftselection_visitor& );
  ftselection_expr const* get_ftselection() const { return ftselection_; }

private:
  ftselection_expr *ftselection_;
};


class ftselection_expr : public ftprimary_expr {
public:
  SERIALIZABLE_CLASS(ftselection_expr)
  SERIALIZABLE_CLASS_CONSTRUCTOR2(ftselection_expr,ftprimary_expr)
  void serialize( serialization::Archiver& );

  typedef std::list<ftpos_filter*> ftpos_filter_list_t;

  ftselection_expr(
    QueryLoc const&,
    ftexpr *ftor,
    ftpos_filter_list_t&
  );
  ~ftselection_expr();

  ft_visit_result::type accept( ftselection_visitor& );
  ftpos_filter_list_t const& get_ftpos_filter_list() const { return list_; }
  ftexpr const* get_ftor() const { return ftor_; };

private:
  ftpos_filter_list_t list_;
  ftexpr *ftor_;
};


class ftwords_times_expr : public ftprimary_expr {
public:
  SERIALIZABLE_CLASS(ftwords_times_expr)
  SERIALIZABLE_CLASS_CONSTRUCTOR2(ftwords_times_expr,ftprimary_expr)
  void serialize( serialization::Archiver& );

  ftwords_times_expr(
    QueryLoc const&,
    ftwords_expr*,
    ftrange_expr* = NULL
  );
  ~ftwords_times_expr();

  ft_visit_result::type accept( ftselection_visitor& );
  ftwords_expr const* get_words() const { return ftwords_; }
  ftrange_expr const* get_times() const { return fttimes_; }

private:
  ftwords_expr *ftwords_;
  ftrange_expr *fttimes_;
};

//////// FTPosFilter //////////////////////////////////////////////////////////

class ftpos_filter : public ftexpr {
public:
  SERIALIZABLE_ABSTRACT_CLASS(ftpos_filter)
  SERIALIZABLE_CLASS_CONSTRUCTOR2(ftpos_filter,ftexpr)
  void serialize( serialization::Archiver& );

protected:
  ftpos_filter( QueryLoc const& );
};


class ftcontent_filter : public ftpos_filter {
public:
  SERIALIZABLE_CLASS(ftcontent_filter)
  SERIALIZABLE_CLASS_CONSTRUCTOR2(ftcontent_filter,ftpos_filter)
  void serialize( serialization::Archiver &ar ) {
    serialize_baseclass( ar, (ftpos_filter*)this );
  }

  ftcontent_filter( QueryLoc const&, ft_content_mode::type );

  ft_visit_result::type accept( ftselection_visitor& );
  ft_content_mode::type get_mode() const { return mode_; }

private:
  ft_content_mode::type mode_;
};


class ftdistance_filter : public ftpos_filter {
public:
  SERIALIZABLE_CLASS(ftdistance_filter)
  SERIALIZABLE_CLASS_CONSTRUCTOR2(ftdistance_filter,ftpos_filter)
  void serialize( serialization::Archiver& );

  ftdistance_filter( QueryLoc const&, ftrange_expr*, ft_unit::type );

  ft_visit_result::type accept( ftselection_visitor& );
  ftrange_expr const* get_range() const { return range_; }
  ft_unit::type get_unit() const { return unit_; }

private:
  ftrange_expr *range_;
  ft_unit::type unit_;
};


class ftorder_filter : public ftpos_filter {
public:
  SERIALIZABLE_CLASS(ftorder_filter)
  SERIALIZABLE_CLASS_CONSTRUCTOR2(ftorder_filter,ftpos_filter)
  void serialize( serialization::Archiver& );

  ft_visit_result::type accept( ftselection_visitor& );
  ftorder_filter( QueryLoc const& );
};


class ftscope_filter : public ftpos_filter {
public:
  SERIALIZABLE_CLASS(ftscope_filter)
  SERIALIZABLE_CLASS_CONSTRUCTOR2(ftscope_filter,ftpos_filter)
  void serialize( serialization::Archiver& );

  ftscope_filter( QueryLoc const&, ft_scope::type, ft_big_unit::type );

  ft_visit_result::type accept( ftselection_visitor& );
  ft_scope::type get_scope() const { return scope_; }
  ft_big_unit::type get_unit() const { return unit_; }

private:
  ft_scope::type scope_;
  ft_big_unit::type unit_;
};


class ftwindow_filter : public ftpos_filter {
public:
  SERIALIZABLE_CLASS(ftwindow_filter)
  SERIALIZABLE_CLASS_CONSTRUCTOR2(ftwindow_filter,ftpos_filter)
  void serialize( serialization::Archiver& );

  ftwindow_filter( QueryLoc const&, additive_expr_t, ft_unit::type );

  ft_visit_result::type accept( ftselection_visitor& );
  ft_unit::type get_unit() const { return unit_; }
  additive_expr_t get_window() const { return window_; }

  PlanIter_t get_plan_iter() const { return plan_iter_; }

  void set_plan_iter( PlanIter_t it ) {
    plan_iter_ = it;
  }

private:
  additive_expr_t window_;
  ft_unit::type unit_;
  PlanIter_t plan_iter_;
};

///////////////////////////////////////////////////////////////////////////////

class ftand_expr : public ftexpr {
public:
  SERIALIZABLE_CLASS(ftand_expr)
  SERIALIZABLE_CLASS_CONSTRUCTOR2(ftand_expr,ftexpr)
  void serialize( serialization::Archiver& );

  typedef std::list<ftexpr*> ftexpr_list_t;

  ftand_expr( QueryLoc const&, ftexpr_list_t& );
  ~ftand_expr();

  ft_visit_result::type accept( ftselection_visitor& );
  ftexpr_list_t& get_expr_list() { return list_; }
  ftexpr_list_t const& get_expr_list() const { return list_; }

private:
  ftexpr_list_t list_;
};


class ftmild_not_expr : public ftexpr {
public:
  SERIALIZABLE_CLASS(ftmild_not_expr)
  SERIALIZABLE_CLASS_CONSTRUCTOR2(ftmild_not_expr,ftexpr)
  void serialize( serialization::Archiver& );

  typedef std::list<ftexpr*> ftexpr_list_t;

  ftmild_not_expr( QueryLoc const&, ftexpr_list_t& );
  ~ftmild_not_expr();

  ft_visit_result::type accept( ftselection_visitor& );
  ftexpr_list_t& get_expr_list() { return list_; }
  ftexpr_list_t const& get_expr_list() const { return list_; }

private:
  ftexpr_list_t list_;
};


class ftor_expr : public ftexpr {
public:
  SERIALIZABLE_CLASS(ftor_expr)
  SERIALIZABLE_CLASS_CONSTRUCTOR2(ftor_expr,ftexpr)
  void serialize( serialization::Archiver& );

  typedef std::list<ftexpr*> ftexpr_list_t;

  ftor_expr( QueryLoc const&, ftexpr_list_t& );
  ~ftor_expr();

  ft_visit_result::type accept( ftselection_visitor& );
  ftexpr_list_t& get_expr_list() { return list_; }
  ftexpr_list_t const& get_expr_list() const { return list_; }

private:
  ftexpr_list_t list_;
};


class ftprimary_with_options_expr : public ftexpr {
public:
  SERIALIZABLE_CLASS(ftprimary_with_options_expr)
  SERIALIZABLE_CLASS_CONSTRUCTOR2(ftprimary_with_options_expr,ftexpr)
  void serialize( serialization::Archiver& );

  ftprimary_with_options_expr( QueryLoc const& );
  ~ftprimary_with_options_expr();

  ft_visit_result::type accept( ftselection_visitor& );
  ftprimary_expr const* get_primary() const { return primary_; }
  ftmatch_options const* get_match_options() const { return match_options_; }
  PlanIter_t get_plan_iter() const { return plan_iter_; }
  expr_t get_weight() const { return weight_; }

  void set_match_options( ftmatch_options *o ) {
    delete match_options_;
    match_options_ = o;
  }

  void set_plan_iter( PlanIter_t it ) {
    plan_iter_ = it;
  }

  void set_primary( ftprimary_expr *e ) {
    delete primary_;
    primary_ = e;
  }

  void set_weight( expr_t e ) {
    weight_ = e;
  }

private:
  ftprimary_expr *primary_;
  ftmatch_options *match_options_;
  expr_t weight_;
  PlanIter_t plan_iter_;
};


class ftrange_expr : public ftexpr {
public:
  SERIALIZABLE_CLASS(ftrange_expr)
  SERIALIZABLE_CLASS_CONSTRUCTOR2(ftrange_expr,ftexpr)
  void serialize( serialization::Archiver& );

  ftrange_expr(
    QueryLoc const&,
    ft_range_mode::type,
    additive_expr_t expr1,
    additive_expr_t expr2 = NULL
  );

  ft_visit_result::type accept( ftselection_visitor& );
  additive_expr_t get_expr1() const { return expr1_; }
  additive_expr_t get_expr2() const { return expr2_; }
  ft_range_mode::type get_mode() const { return mode_; }

  PlanIter_t get_plan_iter1() const { return it1_; }
  PlanIter_t get_plan_iter2() const { return it2_; }

  void set_plan_iters( PlanIter_t it1, PlanIter_t it2 ) {
    it1_ = it1;
    it2_ = it2;
  }

private:
  ft_range_mode::type mode_;
  additive_expr_t expr1_;
  additive_expr_t expr2_;
  PlanIter_t it1_;
  PlanIter_t it2_;
};


class ftunary_not_expr : public ftexpr {
public:
  SERIALIZABLE_CLASS(ftunary_not_expr)
  SERIALIZABLE_CLASS_CONSTRUCTOR2(ftunary_not_expr,ftexpr)
  void serialize( serialization::Archiver& );

  ftunary_not_expr( QueryLoc const&, ftexpr *subexpr );
  ~ftunary_not_expr();

  ft_visit_result::type accept( ftselection_visitor& );
  ftexpr const* get_subexpr() const { return subexpr_; }

private:
  ftexpr *subexpr_;
};


class ftwords_expr : public ftexpr {
public:
  SERIALIZABLE_CLASS(ftwords_expr)
  SERIALIZABLE_CLASS_CONSTRUCTOR2(ftwords_expr,ftexpr)
  void serialize( serialization::Archiver& );

  ftwords_expr(
    QueryLoc const&,
    expr_t,
    ft_anyall_mode::type = ft_anyall_mode::DEFAULT
  );

  ft_visit_result::type accept( ftselection_visitor& );
  expr_t get_expr() const { return expr_; }
  ft_anyall_mode::type get_mode() const { return mode_; }
  PlanIter_t get_plan_iter() const { return plan_iter_; }

  void set_plan_iter( PlanIter_t it ) {
    plan_iter_ = it;
  }

private:
  expr_t expr_;
  ft_anyall_mode::type mode_;
  PlanIter_t plan_iter_;
};

} /* namespace zorba */

#endif /* ZORBA_FTEXPR_H */

/*
 * Local variables:
 * mode: c++
 * End:
 */

/* vim:set et sw=2 ts=2: */