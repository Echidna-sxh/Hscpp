#
#  Copyright 2020-2023 HSCPP
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
#  author: echidna-sxh@qq.com
#

find_path(LibHyperscan_ROOT_DIR
    NAMES include/hs/hs.h
)

find_library(LibHyperscan_LIBRARIES
    NAMES hs
    PATH_SUFFIXES lib64 lib
    PATHS ${LibHyperscan_ROOT_DIR}
)
message(find: ${LibHyperscan_LIBRARIES})

find_path(LibHyperscan_INCLUDE_DIR
    NAMES hs/hs.h
    PATH_SUFFIXES include
    PATHS ${LibHyperscan_ROOT_DIR}
)

message(find: ${LibHyperscan_INCLUDE_DIR}/hs/hs.h)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibHyperscan 
    FOUND_VAR  LibHyperscan_FOUND
    REQUIRED_VARS 
    LibHyperscan_LIBRARIES
    LibHyperscan_INCLUDE_DIR
)

mark_as_advanced(
    LibHyperscan_ROOT_DIR
    LibHyperscan_LIBRARIES
    LibHyperscan_INCLUDE_DIR
)