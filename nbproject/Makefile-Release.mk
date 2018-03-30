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
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/resources/core/connection.o \
	${OBJECTDIR}/resources/core/connection_FileConn.o \
	${OBJECTDIR}/resources/core/connection_MainConn.o \
	${OBJECTDIR}/resources/core/connection_Test.o \
	${OBJECTDIR}/resources/core/servercore.o \
	${OBJECTDIR}/resources/core/servercore_FileConn.o \
	${OBJECTDIR}/resources/core/servercore_MainConn.o \
	${OBJECTDIR}/resources/core/servercore_Test.o \
	${OBJECTDIR}/resources/modules/File.o \
	${OBJECTDIR}/resources/modules/Packet.o \
	${OBJECTDIR}/resources/modules/Session.o \
	${OBJECTDIR}/resources/modules/database.o \
	${OBJECTDIR}/resources/modules/fileHandle.o \
	${OBJECTDIR}/resources/modules/logger.o \
	${OBJECTDIR}/resources/modules/md5.o \
	${OBJECTDIR}/resources/modules/ssl.o \
	${OBJECTDIR}/srcComponent/Werror.o \
	${OBJECTDIR}/srcComponent/WopenSSL.o \
	${OBJECTDIR}/srcComponent/Wsock.o


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
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/fileserver

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/fileserver: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/fileserver ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/main.o: main.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/resources/core/connection.o: resources/core/connection.cpp
	${MKDIR} -p ${OBJECTDIR}/resources/core
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/resources/core/connection.o resources/core/connection.cpp

${OBJECTDIR}/resources/core/connection_FileConn.o: resources/core/connection_FileConn.cpp
	${MKDIR} -p ${OBJECTDIR}/resources/core
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/resources/core/connection_FileConn.o resources/core/connection_FileConn.cpp

${OBJECTDIR}/resources/core/connection_MainConn.o: resources/core/connection_MainConn.cpp
	${MKDIR} -p ${OBJECTDIR}/resources/core
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/resources/core/connection_MainConn.o resources/core/connection_MainConn.cpp

${OBJECTDIR}/resources/core/connection_Test.o: resources/core/connection_Test.cpp
	${MKDIR} -p ${OBJECTDIR}/resources/core
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/resources/core/connection_Test.o resources/core/connection_Test.cpp

${OBJECTDIR}/resources/core/servercore.o: resources/core/servercore.cpp
	${MKDIR} -p ${OBJECTDIR}/resources/core
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/resources/core/servercore.o resources/core/servercore.cpp

${OBJECTDIR}/resources/core/servercore_FileConn.o: resources/core/servercore_FileConn.cpp
	${MKDIR} -p ${OBJECTDIR}/resources/core
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/resources/core/servercore_FileConn.o resources/core/servercore_FileConn.cpp

${OBJECTDIR}/resources/core/servercore_MainConn.o: resources/core/servercore_MainConn.cpp
	${MKDIR} -p ${OBJECTDIR}/resources/core
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/resources/core/servercore_MainConn.o resources/core/servercore_MainConn.cpp

${OBJECTDIR}/resources/core/servercore_Test.o: resources/core/servercore_Test.cpp
	${MKDIR} -p ${OBJECTDIR}/resources/core
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/resources/core/servercore_Test.o resources/core/servercore_Test.cpp

${OBJECTDIR}/resources/modules/File.o: resources/modules/File.cpp
	${MKDIR} -p ${OBJECTDIR}/resources/modules
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/resources/modules/File.o resources/modules/File.cpp

${OBJECTDIR}/resources/modules/Packet.o: resources/modules/Packet.cpp
	${MKDIR} -p ${OBJECTDIR}/resources/modules
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/resources/modules/Packet.o resources/modules/Packet.cpp

${OBJECTDIR}/resources/modules/Session.o: resources/modules/Session.cpp
	${MKDIR} -p ${OBJECTDIR}/resources/modules
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/resources/modules/Session.o resources/modules/Session.cpp

${OBJECTDIR}/resources/modules/database.o: resources/modules/database.cpp
	${MKDIR} -p ${OBJECTDIR}/resources/modules
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/resources/modules/database.o resources/modules/database.cpp

${OBJECTDIR}/resources/modules/fileHandle.o: resources/modules/fileHandle.cpp
	${MKDIR} -p ${OBJECTDIR}/resources/modules
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/resources/modules/fileHandle.o resources/modules/fileHandle.cpp

${OBJECTDIR}/resources/modules/logger.o: resources/modules/logger.cpp
	${MKDIR} -p ${OBJECTDIR}/resources/modules
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/resources/modules/logger.o resources/modules/logger.cpp

${OBJECTDIR}/resources/modules/md5.o: resources/modules/md5.cpp
	${MKDIR} -p ${OBJECTDIR}/resources/modules
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/resources/modules/md5.o resources/modules/md5.cpp

${OBJECTDIR}/resources/modules/ssl.o: resources/modules/ssl.cpp
	${MKDIR} -p ${OBJECTDIR}/resources/modules
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/resources/modules/ssl.o resources/modules/ssl.cpp

${OBJECTDIR}/srcComponent/Werror.o: srcComponent/Werror.cpp
	${MKDIR} -p ${OBJECTDIR}/srcComponent
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/srcComponent/Werror.o srcComponent/Werror.cpp

${OBJECTDIR}/srcComponent/WopenSSL.o: srcComponent/WopenSSL.cpp
	${MKDIR} -p ${OBJECTDIR}/srcComponent
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/srcComponent/WopenSSL.o srcComponent/WopenSSL.cpp

${OBJECTDIR}/srcComponent/Wsock.o: srcComponent/Wsock.cpp
	${MKDIR} -p ${OBJECTDIR}/srcComponent
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/srcComponent/Wsock.o srcComponent/Wsock.cpp

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
