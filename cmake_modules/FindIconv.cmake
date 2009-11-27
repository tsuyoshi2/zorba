# - Try to find Iconv
# Once done this will define
#
#  ICONV_FOUND - system has Iconv
#  ICONV_INCLUDE_DIR - the Iconv include directory
#  ICONV_LIBRARY - Link these to use Iconv
#


IF (ICONV_INCLUDE_DIR AND ICONV_LIBRARY)
   # in cache already
   SET(Iconv_FIND_QUIETLY TRUE)
ENDIF (ICONV_INCLUDE_DIR AND ICONV_LIBRARY)

FIND_PATH(ICONV_INCLUDE_DIR iconv.h
  /usr/include
  /usr/local/include
  DOC "Include directory for the Iconv library linkage (the path to iconv.h file)"
)

FIND_LIBRARY(ICONV_LIBRARY NAMES iconv
  PATHS
  /usr/${LIB_DESTINATION}
  /usr/local/${LIB_DESTINATION}
  DOC "Iconv library that used by Libxml2 (iconv)")
)

IF (ICONV_INCLUDE_DIR AND ICONV_LIBRARY)
   SET(Iconv_FOUND TRUE)
ELSE (ICONV_INCLUDE_DIR AND ICONV_LIBRARY)
   SET(Iconv_FOUND FALSE)
ENDIF (ICONV_INCLUDE_DIR AND ICONV_LIBRARY)

IF(Iconv_FOUND)
  IF(NOT Iconv_FIND_QUIETLY)
    MESSAGE(STATUS "Found Iconv: ${ICONV_LIBRARY}")
  ENDIF(NOT Iconv_FIND_QUIETLY)
ELSE(Iconv_FOUND)
  IF(Iconv_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find Iconv")
  ENDIF(Iconv_FIND_REQUIRED)
ENDIF(Iconv_FOUND)

MARK_AS_ADVANCED(ICONV_INCLUDE_DIR ICONV_LIBRARY)