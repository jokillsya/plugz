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
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/serv_zmq_layer.o \
	${OBJECTDIR}/src/main.o \
	${OBJECTDIR}/src/thpool.o \
	${OBJECTDIR}/src/serv_stdio_layer.o \
	${OBJECTDIR}/src/plugz_db.o \
	${OBJECTDIR}/src/lock_util.o \
	${OBJECTDIR}/src/server.o


# C Compiler Flags
CFLAGS=-Wall

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=`pkg-config --libs libzmq` `pkg-config --libs argtable2` `pkg-config --libs sqlite3` -lzdb  

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/plugz

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/plugz: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/plugz ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/src/serv_zmq_layer.o: src/serv_zmq_layer.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.c) -Wall `pkg-config --cflags libzmq` `pkg-config --cflags argtable2` `pkg-config --cflags sqlite3`    -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/serv_zmq_layer.o src/serv_zmq_layer.c

${OBJECTDIR}/src/main.o: src/main.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.c) -Wall `pkg-config --cflags libzmq` `pkg-config --cflags argtable2` `pkg-config --cflags sqlite3`    -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/main.o src/main.c

${OBJECTDIR}/src/thpool.o: src/thpool.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.c) -Wall `pkg-config --cflags libzmq` `pkg-config --cflags argtable2` `pkg-config --cflags sqlite3`    -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/thpool.o src/thpool.c

${OBJECTDIR}/src/serv_stdio_layer.o: src/serv_stdio_layer.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.c) -Wall `pkg-config --cflags libzmq` `pkg-config --cflags argtable2` `pkg-config --cflags sqlite3`    -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/serv_stdio_layer.o src/serv_stdio_layer.c

${OBJECTDIR}/src/plugz_db.o: src/plugz_db.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.c) -Wall `pkg-config --cflags libzmq` `pkg-config --cflags argtable2` `pkg-config --cflags sqlite3`    -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/plugz_db.o src/plugz_db.c

${OBJECTDIR}/src/lock_util.o: src/lock_util.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.c) -Wall `pkg-config --cflags libzmq` `pkg-config --cflags argtable2` `pkg-config --cflags sqlite3`    -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/lock_util.o src/lock_util.c

${OBJECTDIR}/src/server.o: src/server.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.c) -Wall `pkg-config --cflags libzmq` `pkg-config --cflags argtable2` `pkg-config --cflags sqlite3`    -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/server.o src/server.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/plugz

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
