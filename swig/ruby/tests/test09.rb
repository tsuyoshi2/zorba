# Copyright 2006-2016 zorba.io
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
# http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import sys
sys.path.insert(0, '@pythonPath@')
import zorba_api


def test(zorba)
  xquery = zorba.compileQuery("<a>text-a1<b at1='att1' at2='{1+2}'>text-b1</b>text-a2</a>") 
   
  saxHandler = zorba_api.SAX2ContentHandlerProxy()
  saxHandler.setStartDocumentHandler(startDocumentHandler);
  saxHandler.setEndDocumentHandler(endDocumentHandler);
  saxHandler.setStartElementHandler(startElementHandler);
  saxHandler.setEndElementHandler(endElementHandler);
  saxHandler.setCharactersHandler(charactersHandler);
  saxHandler.setProcessingInstructionHandler(processingInstructionHandler);
  saxHandler.setIgnorableWhitespaceHandler(ignorableWhitespaceHandler);
  saxHandler.setStartPrefixMappingHandler(startPrefixMappingHandler);
  saxHandler.setEndPrefixMappingHandler(endPrefixMappingHandler);
  saxHandler.setSkippedEntityHandler(skippedEntityHandler);
  xquery.executeSAX(saxHandler)

  return


def startDocumentHandler():
  print "Start Document"
  return
def endDocumentHandler():
  print "End Document"
  return
def startElementHandler(URI, localName, QName, SAXAttributes):
  print "Start Element - ", QName
  return
def endElementHandler(URI, localName, QName):
  print "End Element - ", QName
  return
def charactersHandler(text):
  print "Characters - ", text
  return
def processingInstructionHandler(target, data):
  print "Processing Instruction"
  return
def ignorableWhitespaceHandler(text):
  print "Ignorable Whitespace - ", text
  return
def startPrefixMappingHandler(prefix, URI):
  print "Start Prefix Mapping - ", prefix
  return
def endPrefixMappingHandler(prefix):
  print "End Prefix Mapping - ", prefix
  return
def skippedEntityHandler(name):
  print "Skipped Entity - ", name
  return


store = zorba_api.InMemoryStore_getInstance()
zorba = zorba_api.Zorba_getInstance(store)

print "Running: XQuery execute - executeSAX"
test(zorba)
print "Success"


zorba.shutdown()
zorba_api.InMemoryStore_shutdown(store)

