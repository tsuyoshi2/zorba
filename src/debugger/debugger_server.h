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
#ifndef ZORBA_DEBUGGER_SERVER_H
#define ZORBA_DEBUGGER_SERVER_H

#include <string>
#include <ostream>
#include <list>

#include "zorbatypes/zstring.h"

#include "debugger_common.h"
#include "debugger_commons.h"
#include "debugger_protocol.h"


struct Zorba_SerializerOptions;
typedef struct Zorba_SerializerOptions Zorba_SerializerOptions_t;

namespace zorba {

class DebuggerCommunicator;
class DebuggerRuntime;
class XQueryImpl;
typedef Zorba_SerializerOptions_t* (*itemHandler)(void* aUserData);

class DebuggerServer {
  public:
    DebuggerServer(
      XQueryImpl*               query,
      Zorba_SerializerOptions&  serializerOptions,
      std::ostream&             ostream,
      itemHandler               handler,
      void*                     callbackData,
      const std::string&        host,
      unsigned short            port);

    virtual ~DebuggerServer();

  public:
    virtual bool
    run();

    void
    throwError();

  private:

#ifdef WIN32
    static BOOL WINAPI
    ctrlC_Handler(DWORD aCtrlType);
#else
    static void
    ctrlC_Handler(int lParam);
#endif


    std::string
    processCommand(DebuggerCommand command);

    std::string
    buildErrorResponse(
      int transactionID,
      std::string commandName,
      int errorCode,
      std::string errorMessage);

    std::string
    getVariableName(std::string& aFullName);

    void
    buildStackFrame(
      StackFrame& frame,
      int stackFrameNumber,
      std::ostream& stream);

    void
    buildBreakpoint(
      Breakable& breakpoint,
      int breakpointID,
      std::ostream& stream);

    void
    buildProperty(
      std::string& fullName,
      std::string& name,
      std::string& type,
      std::ostream& stream);

    void
    buildChildProperties(
      std::string& name,
      std::list<std::pair<zstring, zstring> >& children,
      std::ostream& stream);

    bool
    getEnvVar(const std::string& name, std::string& value);

    void
    init();

  private:

    DebuggerCommunicator* theCommunicator;
    DebuggerRuntime*      theRuntime;
    std::string           theFileName;
    bool                  theStopping;
  };
}

#endif // ZORBA_DEBUGGER_SERVER_H
