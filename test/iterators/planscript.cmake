# Copyright 2006-2008 The FLWOR Foundation.
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

#$TESTFILE is a filepath relative to the test/iterators dir.

GET_FILENAME_COMPONENT(EXTENSION "${TESTFILE}" EXT)
STRING (LENGTH ${TESTFILE} TESTFILE_LENGTH)
STRING (LENGTH ${EXTENSION} EXTENSION_LENGTH)
MATH (EXPR FORMAT_LENGTH "${EXTENSION_LENGTH} - 6")
MATH (EXPR BASENAME_LENGTH "${TESTFILE_LENGTH} - ${EXTENSION_LENGTH}")
STRING (SUBSTRING ${EXTENSION} 6 ${FORMAT_LENGTH} FORMAT)
STRING (SUBSTRING ${TESTFILE} 0 "${BASENAME_LENGTH}" BASENAME)

SET (APITEST_RESULT_FILEPATH ${CMAKE_ZORBA_BINARY_DIR}/test/iterators/${TESTFILE})
SET (APITEST_EXPECTED_FILEPATH ${CMAKE_ZORBA_SOURCE_DIR}/test/iterators/${TESTFILE})
SET (APITEST_QUERY ${CMAKE_ZORBA_SOURCE_DIR}/test/rbkt/Queries/${BASENAME}.xq)

SET (RBKT_SRC_DIR ${CMAKE_ZORBA_SOURCE_DIR}/test/rbkt)
SET (RBKT_BINARY_DIR ${CMAKE_ZORBA_BINARY_DIR}/test/rbkt)

SET (APITEST_SPEC ${CMAKE_ZORBA_SOURCE_DIR}/test/rbkt/Queries/${BASENAME}.spec)
MESSAGE(STATUS ${APITEST_SPEC})
IF (EXISTS ${APITEST_SPEC})
  MESSAGE(STATUS "${APITEST_SPEC} EXISTS")
  FILE (READ ${APITEST_SPEC} APITEST_SPEC_CONTENTS)
  MESSAGE(STATUS "CONTENTS ${APITEST_SPEC_CONTENTS}")
  #STRING (REGEX MATCH "Args:(.*)" APITEST_ARGS ${APITEST_SPEC_CONTENTS})
  IF (${APITEST_SPEC_CONTENTS} MATCHES "Args:(.*)")
    SET (APITEST_ARGS ${CMAKE_MATCH_1})
    MESSAGE(STATUS "SPEC CONTAINS ARGS SPECIFICATION ${APITEST_ARGS}")
    IF (${APITEST_ARGS} MATCHES "(.*)(Options:|Serialization:|Result:|InputQuery:|Comparison:|DefaultCollection:|Error:|Date:|Timezone:)")
      SET (APITEST_ARGS ${CMAKE_MATCH_1})
      MESSAGE(STATUS "SPEC CONTAINS ARGS SPECIFICATION2 ${APITEST_ARGS}")      
    ENDIF (${APITEST_ARGS} MATCHES "(.*)(Options:|Serialization:|Result:|InputQuery:|Comparison:|DefaultCollection:|Error:|Date:|Timezone:)")
    STRING(REPLACE "$RBKT_SRC_DIR" "${RBKT_SRC_DIR}" APITEST_ARGS ${APITEST_ARGS})
    STRING(REPLACE "$RBKT_BINARY_DIR" "${RBKT_BINARY_DIR}" APITEST_ARGS ${APITEST_ARGS})
    STRING(REPLACE "\n" " " APITEST_ARGS ${APITEST_ARGS})
    #zorba::ascii::replace_all(*it, "$RBKT_SRC_DIR", one_space + rbkt_src_dir);
    #zorba::ascii::replace_all(*it, "$RBKT_BINARY_DIR", rbkt_binary_dir);
    MESSAGE(STATUS "SPEC CONTAINS ARGS SPECIFICATION3 ${APITEST_ARGS}")  
  ENDIF (${APITEST_SPEC_CONTENTS} MATCHES "Args:(.*)")
ENDIF (EXISTS ${APITEST_SPEC})


GET_FILENAME_COMPONENT (APITEST_OUT_DIRPATH ${APITEST_RESULT_FILEPATH} PATH)    
FILE (MAKE_DIRECTORY ${APITEST_OUT_DIRPATH})

FILE (TO_NATIVE_PATH ${ZORBA_EXE} NATIVE_ZORBA_EXE)

STRING(REGEX MATCHALL "[^ ]+" APITEST_ARGS_LIST "${APITEST_ARGS}")    
MESSAGE (STATUS "LIST: ${APITEST_ARGS_LIST}") 
MESSAGE (STATUS "${NATIVE_ZORBA_EXE}
    ${APITEST_ARGS_LIST} 
    --iterator-tree ${FORMAT}
    --no-tree-ids
    --no-serializer    
    ${APITEST_QUERY}")
     
EXECUTE_PROCESS (
  COMMAND
    ${NATIVE_ZORBA_EXE} 
    --iterator-tree ${FORMAT}
    --no-tree-ids
    --no-serializer
    ${APITEST_ARGS_LIST}
    ${APITEST_QUERY}
  RESULT_VARIABLE EXIT_CODE
  OUTPUT_FILE ${APITEST_RESULT_FILEPATH}
) 

IF ("${EXIT_CODE}" STREQUAL "0")
  FILE (TO_NATIVE_PATH ${CMAKE_ZORBA_BINARY_DIR}/test/iterators/plantest.xq PLANTEST_XQ_PATH)

  EXECUTE_PROCESS (
    COMMAND
      ${NATIVE_ZORBA_EXE}
      -f -q ${PLANTEST_XQ_PATH}
      -e command:="${NATIVE_ZORBA_EXE} --profile ${FORMAT} --no-tree-ids --no-serializer ${APITEST_ARGS} ${APITEST_QUERY}"
      -e format:=${FORMAT}
      -e expected-filepath:=${APITEST_EXPECTED_FILEPATH}
      -e result-filepath:=${APITEST_RESULT_FILEPATH} 
      -z method=text
    OUTPUT_VARIABLE out_var
  )
              
  MESSAGE(STATUS ${out_var})
ELSE ("${EXIT_CODE}" STREQUAL "0")
  MESSAGE(STATUS "Profile generation failed.")
ENDIF ("${EXIT_CODE}" STREQUAL "0")