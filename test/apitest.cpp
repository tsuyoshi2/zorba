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

#include <zorba/zorba.h>

#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <memory>

// tests are allowed to use internals
#include "api/unmarshaller.h"
#include "zorbatypes/xqpstring.h"
#include "util/properties.h"

#include <zorba/util/path.h>

#include "store/api/item.h"
#include "zorba_test_setting.h"
#ifndef ZORBA_MINIMAL_STORE
#include <zorba/store_manager.h>
#else
#include "store/minimal/min_store.h"
#endif

// Global variable g_abort_on_error is used to generate an abort() when an 
// error is encountered, to aid debugging
#ifndef NDEBUG
#ifdef BUILDING_ZORBA_STATIC
extern bool g_abort_on_error;
#else
ZORBA_DLL_PUBLIC bool g_abort_on_error;
#endif
#endif


using namespace zorba;
using namespace std;


void set_var (string name, string val, DynamicContext* dctx)
{
  if (name [name.size () - 1] == ':') 
  {
    name = name.substr (0, name.size () - 1);
    Item lItem = Zorba::getInstance(NULL)->getItemFactory()->createString(val);
    if(name != ".") {
      dctx->setVariable(name, lItem);
    } else
      dctx->setContextItem(lItem);
  }
  else if (name[name.size () - 1] != ':')
  {
    ifstream* is = new ifstream(val.c_str ());
    assert (*is);
    if(name != ".")
      dctx->setVariableAsDocument(name, val.c_str(), std::auto_ptr<std::istream>(is));
    else
      dctx->setContextItemAsDocument(val.c_str(), std::auto_ptr<std::istream>(is));
  }
}


#ifndef _WIN32_WCE
int main(int argc, char* argv[])
#else
int _tmain(int argc, _TCHAR* argv[])
#endif
{
  // read the command file properties
  if (! Properties::load(argc,argv))
    return 4;

  Properties* lProp = Properties::instance();
  if (! lProp->hasSingleQuery ()) {
    cout << "Error: either a single inline query or a single query file must be supplied" << endl;
    return 4;
  }
  Zorba_CompilerHints chints;
  chints.opt_level = (lProp->optimizer () ?
                      ZORBA_OPT_LEVEL_O1:
                      ZORBA_OPT_LEVEL_O0);

  // output file (either a file or the standard out if no file is specified)
  auto_ptr<ostream> outputFile (lProp->resultFile ().empty () 
                                ? NULL : new ofstream (lProp->resultFile().c_str()));
  ostream *resultFile = outputFile.get ();
  if (resultFile == NULL)
    resultFile = &cout;

  // input file (either from a file or given as parameter)
  auto_ptr<istream> qfile;
  filesystem_path path;
  bool  is_xqueryx = false;

  if (! lProp->inlineQuery()) {
    path = lProp->queryFile ();
    path.resolve_relative ();
    std::string fname = path.get_path ();
    if(fname.substr(fname.length()-4) == ".xqx")
      is_xqueryx = true;
    qfile.reset (new ifstream (fname.c_str ()));
    if (!qfile->good() || qfile->eof()) {
      cerr << "no query given or not readable " << fname  << endl;
      return 3;
    }
  } else {
    qfile.reset (new istringstream(lProp->query ()));
  }
  
  // print the query if requested
  if (lProp->printQuery()) {
    lProp->debug_out ()<< "Query text:\n";
    copy (istreambuf_iterator<char> (*qfile), istreambuf_iterator<char> (), ostreambuf_iterator<char> (lProp->debug_out ()));
    lProp->debug_out () << "\n" << endl;
    qfile->seekg(0); // go back to the beginning
  }

  // Instantiate the simple store
#ifdef ZORBA_MINIMAL_STORE
  zorba::storeminimal::SimpleStore* store = 
        zorba::storeminimal::SimpleStore::getInstance();
#else
  zorba::simplestore::SimpleStore* store = zorba::StoreManager::getStore();
#endif
      
  // Set the g_abort_on_exception flag in error_manager.cpp
#ifndef NDEBUG
  if (lProp->abort())
    g_abort_on_error = true;
#endif

  // start processing
  Zorba* zengine = Zorba::getInstance(store);

  // start parsing the query
  XQuery_t query = zengine->createQuery ();
  if (lProp->debug()) {
    query->setDebugMode(lProp->debug());
    Zorba_CompilerHints lHints;
    lHints.opt_level = ZORBA_OPT_LEVEL_O0;
  }
  if (! lProp->inlineQuery())
    query->setFileName (path.get_path ());
  
  try {
    query->compile(*qfile, chints);
  } catch (ZorbaException &e) {
    // no need to close because the object is not valid
    cerr << "Compilation error: " << e << endl;
    return 1;
  }

  // set external variables
  vector<pair <string, string> > ext_vars = lProp->getExternalVars ();
  DynamicContext* dctx = query->getDynamicContext ();
  dctx->setImplicitTimezone (lProp->tz ());
  for (vector<pair <string, string> >::const_iterator iter = ext_vars.begin ();
       iter != ext_vars.end (); iter++) {
    set_var (iter->first, iter->second, dctx);
  }

  //if you want to print the plan into a file
  if( ! lProp->dotPlanFile().empty () ) {
    auto_ptr<ostream> planFile (new ofstream (lProp->dotPlanFile().c_str()));
    ostream *printPlanFile = planFile.get ();

    query->printPlan(*printPlanFile, true);
  }

  if (! lProp->compileOnly ()) {
    // output the result (either using xml serialization or using show)
  
    try {
      if (lProp->useSerializer()) {
        Zorba_SerializerOptions opts = Zorba_SerializerOptions::SerializerOptionsFromStringParams(lProp->getSerializerParameters());
        query->execute(*resultFile, &opts);
        // *resultFile << query;
      } else {
        Iterator_t result = query->iterator();
        result->open();
        Item lItem;
        while (result->next(lItem)) {
          // unmarshall the store item from the api item
          store::Item_t lStoreItem = Unmarshaller::getInternalItem(lItem);
          *resultFile << lStoreItem->show() << endl;
        }
        result->close();
      }
    } catch (ZorbaException &e) {
      query->close();
      zengine->shutdown();
      zorba::StoreManager::shutdownStore(store);
      cerr << "Execution error: ";
      if (dynamic_cast<QueryException *> (&e) != NULL)
        cerr << (QueryException &) e;
      else
        cerr << e;
      cerr << endl;
      return 2;
    }
  }

  query->close();
  zengine->shutdown();
  zorba::StoreManager::shutdownStore(store);
  return 0;
}