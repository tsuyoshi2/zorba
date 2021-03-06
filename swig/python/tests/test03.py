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

class MyDiagnosticHandler(zorba_api.DiagnosticHandler): 
  def error(self, *args):
    print "Error args: ", args

def test(zorba):
  diagnosticHandler = MyDiagnosticHandler()
  try:
    print "Compiling 1 div 0"
    xquery = zorba.compileQuery("1 div 0", diagnosticHandler)
    print xquery.execute()
  except Exception, e:
    print "Caught error: ", e
  return


store = zorba_api.InMemoryStore_getInstance()
zorba = zorba_api.Zorba_getInstance(store)

print "Running: Compile query string using Diagnostic Handler"
test(zorba)
print "Success"


zorba.shutdown()
zorba_api.InMemoryStore_shutdown(store)
