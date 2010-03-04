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

#include "zorbacmdproperties.h"

#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#ifdef WIN32
#include <windows.h>
#define sleep(s) Sleep(s*1000)
#endif

#include <zorba/zorba.h>

#ifndef ZORBA_MINIMAL_STORE
#include <zorba/store_manager.h>
#else
#include "store/minimal/min_store.h"
#endif

#include <zorba/debugger_client.h>
#include "debugger_server_runnable.h"
#include "debugger_handler.h"

#include "error_printer.h"
#include "util.h"
#include "module_path.h"

// For setting the base URI from the current directory
#include <zorba/util/path.h>

// Non-API includes -- handy, but should be rewritten eventually

// We use zorba::DateTime from the zorbatypes library to measure wallclock time.
// This is a highly non-portable problem; see zorba::DateTime::getLocalTime().
#include <zorbatypes/zorbatypes_decl.h>
#include <zorbatypes/datetime.h>
#include <zorbatypes/duration.h>
#include <zorbatypes/floatimpl.h>

// Simple wrappers around get_rusage() / clock() for measuring CPU time.
// Could be moved to include/util.
#include "util/time.h"

// toggle this to allow configuration via a system properties file
// (see src/util/properties.*)
#define ZORBACMD_LOAD_SYSTEM_PROPERTIES 1

#if ZORBACMD_LOAD_SYSTEM_PROPERTIES
#  include "util/properties.h"
#endif

using namespace zorba;
namespace zorbatm = zorba::time;

const char *copyright_str =
  "Copyright 2006-2009 The FLWOR Foundation.\n"
  "License: Apache License 2.0: <http://www.apache.org/licenses/LICENSE-2.0>";

#define PATH_SEP (zorba::filesystem_path::get_path_separator ())



bool
populateStaticContext(
    zorba::StaticContext_t& aStaticContext,
    const ZorbaCMDProperties& aProperties)
{
  try {

    // add the following module path to the static context (in this order)
    // 1. command-line properties
    // 2. environment ZORBA_MODULE_PATH
    // 3. current working directory
    {
      std::vector<String> lModulePaths;
      ModulePath::getModulePaths(aProperties, lModulePaths);

      aStaticContext->setModulePaths(lModulePaths);
    }

    if (aProperties.boundarySpace().size() != 0 )
      aStaticContext->setBoundarySpacePolicy(
                         (aProperties.boundarySpace().compare("preserve") == 0
                          ? preserve_space 
                          : strip_space));

    if (aProperties.constructionMode().size() != 0)
      aStaticContext->setConstructionMode( aProperties.boundarySpace().compare("preserve") == 0
                                           ? preserve_cons 
                                           : strip_cons );

    if (aProperties.orderingMode().size() != 0 )
    {
      aStaticContext->setOrderingMode( aProperties.boundarySpace().compare("ordered") == 0
                                       ? ordered 
                                       : unordered );
    }

    if (aProperties.baseUri().size() != 0 )
      aStaticContext->setBaseURI( aProperties.baseUri() );
  }
  catch (zorba::ZorbaException& ze) {
    std::cerr << ze << std::endl;
    return false;
  }

  if (aProperties.defaultCollation().size() != 0 )
  {
    try {
      aStaticContext->addCollation( aProperties.defaultCollation() );
    } catch (zorba::ZorbaException&) {
      std::cerr << "the given collation {" << aProperties.defaultCollation() << "} is not a valid collation." << std::endl;
      return false;
    }
    aStaticContext->setDefaultCollation( aProperties.defaultCollation() );
  }
  return true;
}


bool populateDynamicContext(
    zorba::DynamicContext* aDynamicContext,
    const ZorbaCMDProperties& aProperties)
{
  if ( aProperties.contextItem().size() != 0 ) {
    std::ifstream* lInStream = new std::ifstream(aProperties.contextItem().c_str());
    aDynamicContext->setContextItemAsDocument(aProperties.contextItem(), 
                                              std::auto_ptr<std::istream>(lInStream));
  }

  ZorbaCMDProperties::ExternalVars_t::const_iterator lIter;
  ZorbaCMDProperties::ExternalVars_t::const_iterator end = aProperties.externalVarsEnd();
  for (lIter = aProperties.externalVarsBegin();
       lIter != end;
       ++lIter)
  {
    if ((*lIter).inline_file) {
      std::ifstream* lInStream = new std::ifstream((*lIter).var_value.c_str());
      aDynamicContext->setVariableAsDocument((*lIter).var_name, (*lIter).var_value, 
                                             std::auto_ptr<std::istream>(lInStream));
    } else {
      zorba::Item lItem = zorba::Zorba::getInstance(NULL)
                            ->getItemFactory()->createString((*lIter).var_value);
      aDynamicContext->setVariable((*lIter).var_name, lItem);
    }
  }
  return true;
}


bool createSerializerOptions(
    Zorba_SerializerOptions_t& lSerOptions,
    const ZorbaCMDProperties& aProperties)
{
  if ( aProperties.indent() )
    lSerOptions.indent = ZORBA_INDENT_YES;
  
  if ( aProperties.omitXmlDeclaration() )
    lSerOptions.omit_xml_declaration = ZORBA_OMIT_XML_DECLARATION_YES;
  
  if ( aProperties.byteOrderMark() )
    lSerOptions.byte_order_mark = ZORBA_BYTE_ORDER_MARK_YES;
  
  if ( aProperties.serializeHtml() )
    lSerOptions.ser_method = ZORBA_SERIALIZATION_METHOD_HTML;
  else if ( aProperties.serializeText() )
    lSerOptions.ser_method = ZORBA_SERIALIZATION_METHOD_TEXT;
  
  return true;
}


/// Fullfills the command-line "as-file" (-f) switch,
/// or if not requested, infers -f for file:// queries.
/// Returns an URI or the empty string.
std::string parseFileURI (bool asPath, const std::string &str) 
{
  if (asPath)
    return str;

  // otherwise, the user still might have meant a file
#ifdef WIN32
  // file:///c:/ returns c:<backslash>
  // file://localhost returns \\localhost
  // BUG: it seems that <a>/x returns <a>\x
  static const char *file3 = "file:///";
  static const char *file2 = "file://";
  std::string fpath;
  if(str.compare(0, strlen(file3), file3) == 0) {
    fpath = str.substr(strlen(file3));
  } else if(str.compare(0, strlen(file2), file2) == 0) {
    fpath = PATH_SEP;
    fpath += str.substr(strlen(file2));
  }
  // replace all slash with backslash
  std::string::size_type off=0;
  while ((off=fpath.find('/', off)) != std::string::npos)
    fpath.replace(off, 1, PATH_SEP);
  return fpath;

#else // for UNIX

  static const char *pfx = "file://";
  static unsigned plen = strlen (pfx);
  if (str.compare (0, plen, pfx) == 0)
    return str.substr (plen);
  else
    return "";
#endif
}


struct TimingInfo
{
  typedef enum
  {
    INIT_TIMER,
    DEINIT_TIMER,
    COMP_TIMER,
    EXEC_TIMER,
    UNLOAD_TIMER,
    TOTAL_TIMER
  } TimerKind;

  ulong             numExecs;

  zorba::DateTime   startInitTime;
  zorba::DateTime   stopInitTime;
  zorba::DateTime   startDeInitTime;
  zorba::DateTime   stopDeInitTime;
  zorba::DateTime   startCompTime;
  zorba::DateTime   stopCompTime;
  zorba::DateTime   startExecTime;
  zorba::DateTime   stopExecTime;
  zorba::DateTime   startUnloadTime;
  zorba::DateTime   stopUnloadTime;
  zorba::DateTime   startTotalTime;
  zorba::DateTime   stopTotalTime;

  long              initTime;
  long              deinitTime;
  long              compTime;
  long              execTime;
  long              loadTime;
  long              unloadTime;
  long              totalTime;

  zorbatm::timeinfo startInitClock;
  zorbatm::timeinfo stopInitClock;
  zorbatm::timeinfo startDeInitClock;
  zorbatm::timeinfo stopDeInitClock;
  zorbatm::timeinfo startCompClock;
  zorbatm::timeinfo stopCompClock;
  zorbatm::timeinfo startExecClock;
  zorbatm::timeinfo stopExecClock;
  zorbatm::timeinfo startUnloadClock;
  zorbatm::timeinfo stopUnloadClock;
  zorbatm::timeinfo startTotalClock;
  zorbatm::timeinfo stopTotalClock;

  double            initClock;
  double            deinitClock;
  double            compClock;
  double            execClock;
  double            loadClock;
  double            unloadClock;
  double            totalClock;

  TimingInfo(ulong num)
    :
    numExecs(num),
    initTime(0),
    deinitTime(0),
    compTime(0),
    execTime(0),
    loadTime(0),
    unloadTime(0),
    totalTime(0),
    initClock(0),
    deinitClock(0),
    compClock(0),
    execClock(0),
    loadClock(0),
    unloadClock(0),
    totalClock(0)
  {
  }

  void startTimer(TimerKind kind, ulong iteration);
  void stopTimer(TimerKind kind, ulong iteration);

  std::ostream& print(std::ostream& os);

#define START_TIMER(kind)                             \
  zorba::DateTime::getLocalTime(start##kind##Time);   \
  zorbatm::get_timeinfo(start##kind##Clock);
  
#define STOP_TIMER(kind)                                                \
  zorba::DateTime::getLocalTime(stop##kind##Time);                      \
  std::auto_ptr<zorba::Duration> diffTime;                              \
  diffTime.reset(stop##kind##Time.subtractDateTime(&start##kind##Time, 0)); \
                                                                        \
  zorbatm::get_timeinfo(stop##kind##Clock);                             \
  double diffClock = zorbatm::get_time_elapsed                          \
    (zorbatm::extract_user_time_detail(start##kind##Clock),             \
     zorbatm::extract_user_time_detail(stop##kind##Clock));
};


void TimingInfo::startTimer(TimerKind kind, ulong iteration)
{
  if (iteration == 0 && numExecs > 1)
    return;

  switch (kind)
  {
  case INIT_TIMER:
    START_TIMER(Init);
    break;

  case DEINIT_TIMER:
    START_TIMER(DeInit);
    break;

  case TOTAL_TIMER:
    START_TIMER(Total);
    break;

  case COMP_TIMER:
    START_TIMER(Comp);
    break;

  case EXEC_TIMER:
    START_TIMER(Exec);
    break;

  case UNLOAD_TIMER:
    START_TIMER(Unload);
    break;
  }
}


void TimingInfo::stopTimer(TimerKind kind, ulong iteration)
{
  if (iteration == 0 && numExecs > 1)
    return;

  switch (kind)
  {
  case INIT_TIMER: 
  {
    STOP_TIMER(Init);
    initTime += diffTime->getTotalMilliseconds();
    initClock += diffClock;
    break;
  }
  case DEINIT_TIMER: 
  {
    STOP_TIMER(DeInit);
    deinitTime += diffTime->getTotalMilliseconds();
    deinitClock += diffClock;
    break;
  }
  case TOTAL_TIMER: 
  {
    STOP_TIMER(Total);
    totalTime += diffTime->getTotalMilliseconds();
    totalClock += diffClock;
    break;
  }
  case COMP_TIMER: 
  {
    STOP_TIMER(Comp);
    compTime += diffTime->getTotalMilliseconds();
    compClock += diffClock;
    break;
  }
  case EXEC_TIMER: 
  {
    STOP_TIMER(Exec);
    execTime += diffTime->getTotalMilliseconds();
    execClock += diffClock;
    break;
  }
  case UNLOAD_TIMER: 
  {
    STOP_TIMER(Unload);
    unloadTime += diffTime->getTotalMilliseconds();
    unloadClock += diffClock;
    break;
  }
  }
}


std::ostream& TimingInfo::print(std::ostream& os)
{
  os.precision(3); 
  os.setf(std::ios::fixed);

  os << "\nNumber of executions = " << numExecs << std::endl;

  ulong timeDiv = numExecs == 1 ? 1 : (numExecs - 1);
  long cTime = compTime / timeDiv;
  long eTime = execTime / timeDiv;
  long lTime = loadTime / timeDiv;
  long uTime = unloadTime / timeDiv;
  long tTime = totalTime / timeDiv;
  
  double cClock = compClock / timeDiv;
  double eClock = execClock / timeDiv;
  double lClock = loadClock / timeDiv;
  double uClock = unloadClock / timeDiv;
  double tClock = totalClock / timeDiv;
  
  os << "Engine Startup Time     : " << initTime
     << " (user: " << initClock << ")"
     << " milliseconds" << std::endl;

  os << "Average Compilation Time: " << cTime
     << " (user: " << cClock << ")"
     << " milliseconds" << std::endl;
  
  os << "Average Execution Time  : " << eTime - lTime
     << " (user: " << eClock - lClock  << ")"
     << " milliseconds" << std::endl;
    
  os << "Average Loading Time    : " << lTime
     << " (user: " << lClock << ")"
     << " milliseconds" << std::endl;

  os << "Average Unloading Time  : " << uTime
     << " (user: " << uClock << ")"
     << " milliseconds" << std::endl;

  os << "Average Total Time      : " << tTime
     << " (user: " << tClock << ")"
     << " milliseconds" << std::endl;

  return os;
}


int executeQueryWithTiming(
    zorba::Zorba* zorbaInstance,
    const ZorbaCMDProperties& properties,
    zorba::StaticContext_t& staticContext,
    const std::string& qfilepath,
    std::istream& qfile,
    std::ostream& outputStream,
    TimingInfo& timing)
{
  int lNumExecutions = properties.multiple();

  // default is O1
  Zorba_CompilerHints lHints;
  if (properties.optimizationLevel() == "O0")
    lHints.opt_level = ZORBA_OPT_LEVEL_O0;

  //default is false
  if (properties.libModule())
    lHints.lib_module = true;

  bool indent = properties.indent();

  XmlDataManager* store = zorbaInstance->getXmlDataManager();

  Zorba_SerializerOptions lSerOptions = Zorba_SerializerOptions::SerializerOptionsFromStringParams(properties.getSerializerParameters());
  createSerializerOptions(lSerOptions, properties);

  for (ulong i = 0; i < timing.numExecs; ++i) 
  {
    qfile.clear();
    qfile.seekg(0); // go back to the beginning
    assert (qfile.tellg() >= 0);

    timing.startTimer(TimingInfo::TOTAL_TIMER, i);

    zorba::XQuery_t query;

    //
    // Compile the query
    //
    try 
    {
      timing.startTimer(TimingInfo::COMP_TIMER, i);

      query = zorbaInstance->createQuery();

      query->setFileName(qfilepath);

      query->compile(qfile, staticContext, lHints);

      timing.stopTimer(TimingInfo::COMP_TIMER, i);
    }
    catch (zorba::QueryException& qe) 
    {
      ErrorPrinter::print(qe,
                          std::cerr,
                          properties.printErrorsAsXml(),
                          properties.indent());
      return 5;
    }

    //
    // Create and populate the dynamic context
    //
    zorba::DynamicContext* lDynamicContext = query->getDynamicContext();
    try 
    {
      if ( ! populateDynamicContext(lDynamicContext, properties) )
      {
        properties.printHelp(std::cout);
        return 4;
      }
    }
    catch (zorba::QueryException& qe) 
    {
      ErrorPrinter::print(qe, std::cerr, properties.printErrorsAsXml(), indent);
      return 5;
    }
    catch (zorba::ZorbaException& ze) 
    {
      std::cerr << ze << std::endl;
      return 6;
    }

    //libModule assumes compileOnly even if compileOnly is false
    if( ! properties.compileOnly() && ! properties.libModule() ) 
    {
    //
    // Run the query
    //
    try 
    {
      timing.startTimer(TimingInfo::EXEC_TIMER, i);

      if (properties.noSerializer())
        query->executeSAX();
      else
        query->execute(outputStream, &lSerOptions);
      
      timing.stopTimer(TimingInfo::EXEC_TIMER, i);

      if (i > 0 || lNumExecutions == 1) 
      {
        timing.loadTime += query->getDocLoadingTime();
        timing.loadClock += query->getDocLoadingUserTime();
      }
    }
    catch (zorba::QueryException& qe) 
    {
      ErrorPrinter::print(qe, std::cerr, properties.printErrorsAsXml(), indent);
      return 5;
    }
    catch (zorba::ZorbaException& ze)
    {
      std::cerr << ze << std::endl;
      return 6;
    }
    }

    //
    // Delete all loaded docs from the store
    //
    timing.startTimer(TimingInfo::UNLOAD_TIMER, i);

    store->deleteAllDocuments();

    timing.stopTimer(TimingInfo::UNLOAD_TIMER, i);

    timing.stopTimer(TimingInfo::TOTAL_TIMER, i);
  }

  return 0;
}


int executeQuery(
    ZorbaCMDProperties& properties,
    zorba::XQuery_t& query,
    zorba::StaticContext_t& staticContext,
    std::istream& qfile,
    std::ostream& outputStream)
{
  int lNumExecutions = properties.multiple();

  // default is O1
  Zorba_CompilerHints lHints;
  if (properties.optimizationLevel() == "O0")
    lHints.opt_level = ZORBA_OPT_LEVEL_O0;

  //default is false
  if (properties.libModule())
    lHints.lib_module = true;


    Zorba_SerializerOptions lSerOptions = Zorba_SerializerOptions::SerializerOptionsFromStringParams(properties.getSerializerParameters());
    createSerializerOptions(lSerOptions, properties);

  //
  // Create and compile the query
  //
  try {
    query->compile(qfile, staticContext, lHints);
  } catch (zorba::QueryException& qe) {
    ErrorPrinter::print(qe, std::cerr, properties.printErrorsAsXml(), properties.indent());
    return 5;
  } catch (zorba::ZorbaException& ze) {
    std::cerr << ze << std::endl;
    return 5;
  }

  //
  // Create and populate the dynamic context
  //
  zorba::DynamicContext* lDynamicContext = query->getDynamicContext();
  try 
  {
    if ( ! populateDynamicContext(lDynamicContext, properties) )
    {
      properties.printHelp(std::cout);
      return 4;
    }
  }
  catch (zorba::QueryException& qe) 
  {
    ErrorPrinter::print(qe, std::cerr, properties.printErrorsAsXml(), properties.indent());
    return 5;
  }
  catch (zorba::ZorbaException& ze) 
  {
    std::cerr << ze << std::endl;
    return 6;
  }

  //libModule assumes compileOnly even if compileOnly is false
  if( ! properties.compileOnly() && ! properties.libModule() ) 
  {
    //
    // Run the query N times
    //
    try 
    {
      for (int i = 0; i < lNumExecutions; ++i) 
      {
        if (properties.noSerializer ())
          query->executeSAX ();
        else
          query->execute(outputStream, &lSerOptions);
      }
    }
    catch (zorba::QueryException& qe) 
    {
      ErrorPrinter::print(qe, std::cerr, properties.printErrorsAsXml(), properties.indent());
      return 5;
    }
    catch (zorba::ZorbaException& ze) 
    {
      std::cerr << ze << std::endl;
      return 6;
    }
  }

  return 0;
}

/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
//  Main                                                                       //
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////

#ifndef _WIN32_WCE
int main(int argc, char* argv[])
#else
int _tmain(int argc, _TCHAR* argv[])
#endif
{
#if ZORBACMD_LOAD_SYSTEM_PROPERTIES
  // only configurable via a config file or environment vars
  zorba::Properties::load (0, NULL);
#endif

  // parse the command line and/or the properties file
  ZorbaCMDProperties lProperties;
  if (!lProperties.loadProperties(argc, argv)) {
    return 1;
  }

  TimingInfo timing(lProperties.multiple());

  bool doTiming = lProperties.timing();
  bool debug = (lProperties.debugServer() || lProperties.debug());

  // libModule assumes compileOnly even if compileOnly is false
  bool compileOnly = (lProperties.compileOnly() || lProperties.libModule() );

  // write to file or standard out
  std::auto_ptr<std::ostream> lFileStream(lProperties.outputFile().size() > 0 ?
                                           new std::ofstream(lProperties.outputFile().c_str()) 
                                          : 0 );
  std::ostream* lOutputStream = lFileStream.get();
  if ( lOutputStream == 0 ) 
  {
    lOutputStream = &std::cout;
  }
  else if ( !lOutputStream->good() )
  {
    std::cerr << "could not write to output file {" << lProperties.outputFile()
              << "}" << std::endl;
    return 2;
  }

  if(lProperties.queriesOrFilesBegin() == lProperties.queriesOrFilesEnd()) 
  {
    std::cerr << "no queries submitted." << std::endl;
    lProperties.printHelp(std::cout);
    return 3;
  }


  // Start the engine

  timing.startTimer(TimingInfo::INIT_TIMER, 2);

#ifndef ZORBA_MINIMAL_STORE
  zorba::simplestore::SimpleStore* store = zorba::StoreManager::getStore();
#else
  zorba::storeminimal::SimpleStore* store =
  zorba::storeminimal::SimpleStoreManager::getStore();
#endif

  zorba::Zorba* lZorbaInstance = zorba::Zorba::getInstance(store);

  {
  timing.stopTimer(TimingInfo::INIT_TIMER, 2);

  // For each query ...

  int queryNo;
  ZorbaCMDProperties::QueriesOrFiles_t::const_iterator lIter;
  for (queryNo = 1, lIter = lProperties.queriesOrFilesBegin();
       lIter != lProperties.queriesOrFilesEnd();
       ++queryNo, ++lIter)
  {
    //
    // Read the query (either from a file or given as parameter)
    //
    std::string fURI = *lIter;
    std::string fname = parseFileURI (lProperties.asFiles (), fURI);
    zorba::filesystem_path path (fname);
    bool asFile = ! fname.empty ();
    std::auto_ptr<std::istream> qfile;
    
    if (asFile) {
      path.resolve_relative ();
      qfile.reset (new std::ifstream (path.c_str ()));
    } else {
      qfile.reset (new std::istringstream(fURI));
    }
    
    if ( asFile && (!qfile->good() || qfile->eof()) ) {
      std::cerr << "file {" << fname << "} not found or not readable." << std::endl;
      lProperties.printHelp(std::cout);
      return 3;
    } else if (fURI.empty ()) {
      std::cerr << "empty query." << std::endl;
      lProperties.printHelp(std::cout);
      return 3;
    }

    //
    // Print the query if requested
    //
    if ( lProperties.printQuery() ) 
    {
      *lOutputStream << "\nQuery number " << queryNo << " :\n";
      std::copy (std::istreambuf_iterator<char> (*qfile),
                 std::istreambuf_iterator<char> (),
                 std::ostreambuf_iterator<char> (*lOutputStream));
      *lOutputStream << std::endl;
      qfile->seekg(0); // go back to the beginning
    }

    //
    // Create the static context and populate it with info taken from the properties
    //
    zorba::StaticContext_t lStaticContext = lZorbaInstance->createStaticContext();
    if (! populateStaticContext(lStaticContext, lProperties) ) 
    {
      lProperties.printHelp(std::cout);
      return 3;
    }

    if (!asFile && lProperties.baseUri().size() == 0 ) 
    {
      // No user set base URI. Set the cwd to be used as base-uri in order
      // to make the doc function doc("mydoc.xml") work
      zorba::filesystem_path p;
      std::stringstream lTmp;
      std::vector<std::string> lTokens;
      Util::tokenize(p.c_str(), PATH_SEP, lTokens);

      lTmp << "file:///";
      for (std::vector<std::string>::const_iterator lIter = lTokens.begin();
           lIter != lTokens.end(); ++lIter) 
      {
        zorba::String lTmpString(*lIter);
        lTmp << "/" << lTmpString.encodeForUri();
      }

      lTmp << "/";

      lStaticContext->setBaseURI(lTmp.str());
    }

    //
    // Parse the query
    //
    if (lProperties.parseOnly()) {
      try {
        zorba::XQuery_t lQuery = lZorbaInstance->createQuery();
        if (asFile) {
          lQuery->setFileName(path.get_path());
        }

        lQuery->parse (*qfile);
      }
      catch (zorba::ZorbaException& ze) {
        std::cerr << ze << std::endl;
        return 6;
      }
    }

    //
    // Compile and run it if necessary.
    // Print timing information if requested.
    //
    else if (!debug) {

      if(compileOnly) {
        try {
          zorba::XQuery_t aQuery = lZorbaInstance->createQuery();
          if (asFile) {
            aQuery->setFileName(path.get_path());
          }
          aQuery->parse(*qfile);
          qfile->clear();
          qfile->seekg(0); // go back to the beginning
        }
        catch (zorba::QueryException& qe) {
          ErrorPrinter::print(qe, std::cerr, lProperties.printErrorsAsXml(), lProperties.indent());
          return 6;
        }
      } 

      int status = 0;

      if (doTiming) {
        status = executeQueryWithTiming(lZorbaInstance,
                                        lProperties,
                                        lStaticContext,
                                        path.get_path(),
                                        *qfile,
                                        *lOutputStream,
                                        timing);
      } else {
        zorba::XQuery_t lQuery = lZorbaInstance->createQuery();
        lQuery->setTimeout(lProperties.timeout());

        if (asFile) {
          lQuery->setFileName (path.get_path ());
        }

        status = executeQuery(lProperties,
                              lQuery,
                              lStaticContext,
                              *qfile,
                              *lOutputStream);
      }

      if (status != 0) {
        return status;
      }

      if (doTiming) {
        timing.print(std::cout);
      }
    }

    //
    // Debug the query. Do not allow "compileOnly" flags and inline queries
    //
    else if (debug) {
      if (compileOnly) {
        std::cerr << "cannot debug a query if the compileOnly option is specified"
                  << std::endl;
        return 7;
      }

      if (!asFile) {
        std::cerr << "Cannot debug inline queries." << std::endl;
        return 8;
      }

      std::auto_ptr<std::istream> lXQ(new std::ifstream(path.c_str()));
      std::string lFileName(path.get_path());
      
      zorba::XQuery_t lQuery;

      try {
        lQuery = lZorbaInstance->createQuery();
        lQuery->setFileName(lFileName);
        lQuery->setDebugMode(true);

        Zorba_CompilerHints lHints;
        lHints.opt_level = ZORBA_OPT_LEVEL_O0;

        lQuery->compile(*lXQ.get(), lHints);
        zorba::DynamicContext* lDynamicContext = lQuery->getDynamicContext();
        if (!populateDynamicContext(lDynamicContext, lProperties)) {
          return 9;
        }

        std::auto_ptr<zorba::DebuggerServerRunnable> lServer;
        std::auto_ptr<ZorbaDebuggerClient>           lClient;
        std::auto_ptr<DebuggerHandler>               lHandler;

        if (lProperties.getRequestPort() == lProperties.getEventPort()) {
          std::cout << "Command and event ports have to be different"
                    << std::endl;
          return 10;
        }
        
        std::string lHost = lProperties.debugServerHost();
        if (lHost == "") {
          lHost = "127.0.0.1";
        }

        if (lProperties.debugServer() || lProperties.debug()) {
          Zorba_SerializerOptions lSerOptions = 
              Zorba_SerializerOptions::SerializerOptionsFromStringParams(
              lProperties.getSerializerParameters());
          createSerializerOptions(lSerOptions, lProperties);
          lServer.reset(new zorba::DebuggerServerRunnable(
                            lQuery,
                            *lOutputStream,                                                          
                            lHost,
                            lProperties.getRequestPort(),
                            lProperties.getEventPort(),
                            lSerOptions));
          if (!lProperties.hasNoLogo() && !lProperties.debug()) {
            std::cout << "Zorba XQuery Debugger Server\n" << copyright_str << std::endl;
          }
          lServer->start();
        }
                
        if (lProperties.debug()) {
          if (!lProperties.hasNoLogo() ) {
            std::cout << "Zorba XQuery Debugger\n" << copyright_str << std::endl;
          }
          
          // Try to connect 3 times on the server thread
          for (unsigned int i = 0; i < 3; i++) {
            try {
              // wait 1 second before trying to reconnect
              sleep(1);
              lClient.reset(ZorbaDebuggerClient::createClient(
                lHost, lProperties.getRequestPort(), lProperties.getEventPort()));
              lHandler.reset(new DebuggerHandler(lZorbaInstance, lClient.get(), lFileName));
              lClient->registerEventHandler( lHandler.get() );
              break;
            } catch( std::exception &e ) {
              if ( i < 2 ){ continue; }
              std::cerr << "Could not start the debugger: {" << e.what() << "}" << std::endl;
            }
            return 1;
          }
        }

        // important to destroy the client before joining the server
        lClient.reset(0);
        lHandler.reset(0);

        if (lProperties.debugServer() || lProperties.debug()) {
          lServer->join();
          lServer.reset(0);
        }

      } catch (zorba::QueryException& qe) {
        ErrorPrinter::print(qe, std::cerr, lProperties.printErrorsAsXml(), lProperties.indent());
        return 5;
      } catch (zorba::ZorbaException& ze) {
        std::cerr << ze << std::endl;
        return 6;
      }
    } // else if (debug) 

  } // for each query

  }

  if (doTiming)
    timing.startTimer(TimingInfo::DEINIT_TIMER, 2);

  lZorbaInstance->shutdown();
  zorba::StoreManager::shutdownStore(store);

  if (doTiming)
  {
    timing.stopTimer(TimingInfo::DEINIT_TIMER, 2);

    std::cout << std::endl << "Engine Shutdown Time     : " << timing.deinitTime
              << " (user: " << timing.deinitClock << ")"
              << " milliseconds" << std::endl;
  }

  return 0;
}

/* vim:set ts=2 sw=2: */