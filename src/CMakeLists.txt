# Copyright (c) 2014-2016 Samsung Electronics Co., Ltd All Rights Reserved
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
#
# @file        CMakeLists.txt
# @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
#


SET(COMMON_DEPS)

IF (BUILD_WITH_SYSTEMD)
SET(COMMON_DEPS
    ${COMMON_DEPS}
    libsystemd
    )
ENDIF (BUILD_WITH_SYSTEMD)

IF (CMAKE_BUILD_TYPE MATCHES "DEBUG")
SET(COMMON_DEPS
    ${COMMON_DEPS}
    libunwind
    )
ENDIF (CMAKE_BUILD_TYPE MATCHES "DEBUG")

PKG_CHECK_MODULES(CYNARA_DEP
    REQUIRED
    ${COMMON_DEPS}
    )

INCLUDE_DIRECTORIES(SYSTEM
    ${CYNARA_DEP_INCLUDE_DIRS}
    )

SET(CYNARA_PATH ${PROJECT_SOURCE_DIR}/src)
SET(CYNARA_EXTERNAL_SRC_PATH ${PROJECT_SOURCE_DIR}/src/external)

INCLUDE_DIRECTORIES(
    ${CYNARA_PATH}/common
    )

ADD_SUBDIRECTORY(include)

IF (BUILD_COMMONS)
    ADD_SUBDIRECTORY(common)
    ADD_SUBDIRECTORY(client)
    ADD_SUBDIRECTORY(client-async)
    ADD_SUBDIRECTORY(client-common)
    ADD_SUBDIRECTORY(cyad)
    ADD_SUBDIRECTORY(chsgen)
    ADD_SUBDIRECTORY(admin)
    ADD_SUBDIRECTORY(agent)
    ADD_SUBDIRECTORY(storage)
    ADD_SUBDIRECTORY(monitor)
    ADD_SUBDIRECTORY(helpers/creds-commons)
    ADD_SUBDIRECTORY(helpers/creds-self)
    ADD_SUBDIRECTORY(helpers/creds-socket)
    ADD_SUBDIRECTORY(helpers/session)
ENDIF (BUILD_COMMONS)

IF (BUILD_SERVICE)
    ADD_SUBDIRECTORY(service)
ENDIF (BUILD_SERVICE)

IF (BUILD_DBUS)
    ADD_SUBDIRECTORY(helpers/creds-dbus)
    ADD_SUBDIRECTORY(helpers/creds-gdbus)
ENDIF (BUILD_DBUS)

