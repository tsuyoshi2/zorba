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

#include <sstream>
#include <map>

#include <zorba/zorba.h>
#include <zorba/external_module.h>
#include <zorba/external_function.h>
#include <zorba/error_list.h>
#include <zorba/user_exception.h>
#include <zorba/item_factory.h>
#include <zorba/singleton_item_sequence.h>
#include <zorba/base64.h>

#include "sha1.h"
#include "md5_impl.h"

namespace zorba { namespace security {

zorba::String getOneStringArgument(const StatelessExternalFunction::Arguments_t& aArgs, int aIndex)
{
  zorba::Item lItem;
  Iterator_t args_iter = aArgs[aIndex]->getIterator();
  args_iter->open();
  if (!(args_iter->next(lItem))) {
    std::stringstream lErrorMessage;
    lErrorMessage << "An empty-sequence is not allowed as "
                  << aIndex << ". parameter.";
    Item lQName = Zorba::getInstance(0)->getItemFactory()->createQName("http://www.zorba-xquery.com/modules/security/sign",
        "XPTY0004");
    throw USER_EXCEPTION(lQName, lErrorMessage.str());
  }
  zorba::String lTmpString = lItem.getStringValue();
  if (args_iter->next(lItem)) {
    std::stringstream lErrorMessage;
    lErrorMessage << "A sequence of more then one item is not allowed as "
      << aIndex << ". parameter.";
    Item lQName = Zorba::getInstance(0)->getItemFactory()->createQName("http://www.zorba-xquery.com/modules/security/sign",
        "XPTY0004");
    throw USER_EXCEPTION(lQName, lErrorMessage.str());
  }
  args_iter->close();
  return lTmpString;
}

static zorba::String getNodeText(
    const StatelessExternalFunction::Arguments_t&  aArgs,
    int                 aArgumentIndex)
{
  zorba::Item lItem;
  Iterator_t args_iter = aArgs[aArgumentIndex]->getIterator();
  args_iter->open();
  if (!(args_iter->next(lItem))) {
    std::stringstream lErrorMessage;
    lErrorMessage << "An empty-sequence is not allowed as " << aArgumentIndex << ". parameter.";
    Item lQName = Zorba::getInstance(0)->getItemFactory()->createQName("http://www.zorba-xquery.com/modules/security/sign",
        "XPTY0004");
    throw USER_EXCEPTION(lQName, lErrorMessage.str());
  }
  std::stringstream lTmpStream;
  zorba::String lText = lItem.getStringValue();
  if (args_iter->next(lItem)) {
    std::stringstream lErrorMessage;
    lErrorMessage << "A sequence of more then one item is not allowed as " << aArgumentIndex << ". parameter.";
    Item lQName = Zorba::getInstance(0)->getItemFactory()->createQName("http://www.zorba-xquery.com/modules/security/sign",
        "XPTY0004");
    throw USER_EXCEPTION(lQName, lErrorMessage.str());
  }
  args_iter->close();
  return lText;
}


class HashModule : public ExternalModule
{
private:
  static ItemFactory* theFactory;

protected:
  class ltstr
  {
  public:
    bool operator()(const String& s1, const String& s2) const
    {
      return s1.compare(s2) < 0;
    }
  };

  typedef std::map<String, StatelessExternalFunction*, ltstr> FuncMap_t;
  FuncMap_t theFunctions;

public:
  virtual ~HashModule()
  {
    for (FuncMap_t::const_iterator lIter = theFunctions.begin();
         lIter != theFunctions.end(); ++lIter) {
      delete lIter->second;
    }
    theFunctions.clear();
  }

  virtual String
  getURI() const { return "http://www.zorba-xquery.com/modules/security/sign"; }

  virtual StatelessExternalFunction*
  getExternalFunction(const String& aLocalname);

  virtual void
  destroy()
  {
    if (!dynamic_cast<HashModule*>(this)) {
      return;
    }
    delete this;
  }

  static ItemFactory*
  getItemFactory()
  {
    if(!theFactory)
      theFactory = Zorba::getInstance(0)->getItemFactory();
    return theFactory;
  }

};

class HashFunction : public PureStatelessExternalFunction
{
protected:
  const HashModule* theModule;

public:
  HashFunction(const HashModule* aModule): theModule(aModule){}
  ~HashFunction(){}

  virtual String
  getLocalName() const { return "sign-internal"; }

  virtual zorba::ItemSequence_t
  evaluate(const Arguments_t& aArgs) const
  {
    std::string lValue = (getNodeText(aArgs, 0)).c_str();
    std::string lKey = (getNodeText(aArgs, 1)).c_str();
    std::string lAlg = (getOneStringArgument(aArgs, 2)).c_str();
    std::string lText = lValue + lKey;
    zorba::String lHash;
    if (lAlg == "sha1") {
      CSHA1 lSha1;
      const unsigned char* lData = (const unsigned char*) lText.c_str();
      lSha1.Update(lData, lText.size());
      lSha1.Final();
      char lRes[65];
      lSha1.GetHash((UINT_8 *)lRes);
      zorba::String lDigest(lRes);
      lHash = zorba::encoding::Base64::encode(lDigest);
    } else {
      lHash = md5(lText);
    }
    zorba::Item lItem;
    lItem = theModule->getItemFactory()->createString(lHash);
    return zorba::ItemSequence_t(new zorba::SingletonItemSequence(lItem));
  }

  virtual String
  getURI() const
  {
    return theModule->getURI();
  }

};

StatelessExternalFunction* HashModule::getExternalFunction(
    const String& aLocalname)
{
  StatelessExternalFunction*& lFunc = theFunctions[aLocalname];
  if (!lFunc) {
    if (aLocalname.equals("sign-internal")) {
      lFunc = new HashFunction(this);
    }
  }
  return lFunc;
}

ItemFactory* HashModule::theFactory = 0;


} /* namespace security */ } /* namespace zorba */

#ifdef WIN32
#  define DLL_EXPORT __declspec(dllexport)
#else
#  define DLL_EXPORT __attribute__ ((visibility("default")))
#endif

extern "C" DLL_EXPORT zorba::ExternalModule* createModule() {
  return new zorba::security::HashModule();
}
