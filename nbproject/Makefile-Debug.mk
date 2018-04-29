#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/main.o \
	${OBJECTDIR}/src/resources/core/connection.o \
	${OBJECTDIR}/src/resources/core/connection_FileConn.o \
	${OBJECTDIR}/src/resources/core/connection_MainConn.o \
	${OBJECTDIR}/src/resources/core/connection_Test.o \
	${OBJECTDIR}/src/resources/core/servercore.o \
	${OBJECTDIR}/src/resources/core/servercore_FileConn.o \
	${OBJECTDIR}/src/resources/core/servercore_MainConn.o \
	${OBJECTDIR}/src/resources/core/servercore_Test.o \
	${OBJECTDIR}/src/resources/modules/File.o \
	${OBJECTDIR}/src/resources/modules/Packet.o \
	${OBJECTDIR}/src/resources/modules/Session.o \
	${OBJECTDIR}/src/resources/modules/database.o \
	${OBJECTDIR}/src/resources/modules/fileHandle.o \
	${OBJECTDIR}/src/resources/modules/logger.o \
	${OBJECTDIR}/src/resources/modules/md5.o \
	${OBJECTDIR}/src/resources/modules/ssl.o \
	${OBJECTDIR}/src/srcComponent/Werror.o \
	${OBJECTDIR}/src/srcComponent/WopenSSL.o \
	${OBJECTDIR}/src/srcComponent/Wsock.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/usr/lib -lssl -lcrypto -lmysqlclient -pthread

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/fserver

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/fserver: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/fserver ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/src/main.o: src/main.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/main.o src/main.cpp

${OBJECTDIR}/src/resources/core/connection.o: src/resources/core/connection.cpp
	${MKDIR} -p ${OBJECTDIR}/src/resources/core
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/resources/core/connection.o src/resources/core/connection.cpp

${OBJECTDIR}/src/resources/core/connection_FileConn.o: src/resources/core/connection_FileConn.cpp
	${MKDIR} -p ${OBJECTDIR}/src/resources/core
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/resources/core/connection_FileConn.o src/resources/core/connection_FileConn.cpp

${OBJECTDIR}/src/resources/core/connection_MainConn.o: src/resources/core/connection_MainConn.cpp
	${MKDIR} -p ${OBJECTDIR}/src/resources/core
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/resources/core/connection_MainConn.o src/resources/core/connection_MainConn.cpp

${OBJECTDIR}/src/resources/core/connection_Test.o: src/resources/core/connection_Test.cpp
	${MKDIR} -p ${OBJECTDIR}/src/resources/core
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/resources/core/connection_Test.o src/resources/core/connection_Test.cpp

${OBJECTDIR}/src/resources/core/servercore.o: src/resources/core/servercore.cpp
	${MKDIR} -p ${OBJECTDIR}/src/resources/core
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/resources/core/servercore.o src/resources/core/servercore.cpp

${OBJECTDIR}/src/resources/core/servercore_FileConn.o: src/resources/core/servercore_FileConn.cpp
	${MKDIR} -p ${OBJECTDIR}/src/resources/core
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/resources/core/servercore_FileConn.o src/resources/core/servercore_FileConn.cpp

${OBJECTDIR}/src/resources/core/servercore_MainConn.o: src/resources/core/servercore_MainConn.cpp
	${MKDIR} -p ${OBJECTDIR}/src/resources/core
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/resources/core/servercore_MainConn.o src/resources/core/servercore_MainConn.cpp

${OBJECTDIR}/src/resources/core/servercore_Test.o: src/resources/core/servercore_Test.cpp
	${MKDIR} -p ${OBJECTDIR}/src/resources/core
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/resources/core/servercore_Test.o src/resources/core/servercore_Test.cpp

${OBJECTDIR}/src/resources/modules/File.o: src/resources/modules/File.cpp
	${MKDIR} -p ${OBJECTDIR}/src/resources/modules
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/resources/modules/File.o src/resources/modules/File.cpp

${OBJECTDIR}/src/resources/modules/Packet.o: src/resources/modules/Packet.cpp
	${MKDIR} -p ${OBJECTDIR}/src/resources/modules
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/resources/modules/Packet.o src/resources/modules/Packet.cpp

${OBJECTDIR}/src/resources/modules/Session.o: src/resources/modules/Session.cpp
	${MKDIR} -p ${OBJECTDIR}/src/resources/modules
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/resources/modules/Session.o src/resources/modules/Session.cpp

${OBJECTDIR}/src/resources/modules/database.o: src/resources/modules/database.cpp
	${MKDIR} -p ${OBJECTDIR}/src/resources/modules
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/resources/modules/database.o src/resources/modules/database.cpp

${OBJECTDIR}/src/resources/modules/fileHandle.o: src/resources/modules/fileHandle.cpp
	${MKDIR} -p ${OBJECTDIR}/src/resources/modules
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/resources/modules/fileHandle.o src/resources/modules/fileHandle.cpp

${OBJECTDIR}/src/resources/modules/logger.o: src/resources/modules/logger.cpp
	${MKDIR} -p ${OBJECTDIR}/src/resources/modules
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/resources/modules/logger.o src/resources/modules/logger.cpp

${OBJECTDIR}/src/resources/modules/md5.o: src/resources/modules/md5.cpp
	${MKDIR} -p ${OBJECTDIR}/src/resources/modules
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/resources/modules/md5.o src/resources/modules/md5.cpp

${OBJECTDIR}/src/resources/modules/ssl.o: src/resources/modules/ssl.cpp
	${MKDIR} -p ${OBJECTDIR}/src/resources/modules
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/resources/modules/ssl.o src/resources/modules/ssl.cpp

${OBJECTDIR}/src/srcComponent/Werror.o: src/srcComponent/Werror.cpp
	${MKDIR} -p ${OBJECTDIR}/src/srcComponent
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/srcComponent/Werror.o src/srcComponent/Werror.cpp

${OBJECTDIR}/src/srcComponent/WopenSSL.o: src/srcComponent/WopenSSL.cpp
	${MKDIR} -p ${OBJECTDIR}/src/srcComponent
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/srcComponent/WopenSSL.o src/srcComponent/WopenSSL.cpp

${OBJECTDIR}/src/srcComponent/Wsock.o: src/srcComponent/Wsock.cpp
	${MKDIR} -p ${OBJECTDIR}/src/srcComponent
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/srcComponent/Wsock.o src/srcComponent/Wsock.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
