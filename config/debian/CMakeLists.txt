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


###############################################################################
# Debian package dependencies
SET(debian-required-packages "libxml2,libicu38,libxerces28")
IF(CURL_FOUND)
  SET(debian-required-packages "${debian-required-packages},libcurl3")
ENDIF(CURL_FOUND)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/ZorbaDebianDependencies.cmake.in
  ${CMAKE_BINARY_DIR}/ZorbaDebianDependencies.cmake)

