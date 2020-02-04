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
CND_PLATFORM=MinGW-Windows
CND_DLIB_EXT=dll
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/zlib/adler32.o \
	${OBJECTDIR}/zlib/compress.o \
	${OBJECTDIR}/zlib/crc32.o \
	${OBJECTDIR}/zlib/deflate.o \
	${OBJECTDIR}/zlib/gzclose.o \
	${OBJECTDIR}/zlib/gzlib.o \
	${OBJECTDIR}/zlib/gzread.o \
	${OBJECTDIR}/zlib/gzwrite.o \
	${OBJECTDIR}/zlib/infback.o \
	${OBJECTDIR}/zlib/inffast.o \
	${OBJECTDIR}/zlib/inflate.o \
	${OBJECTDIR}/zlib/inftrees.o \
	${OBJECTDIR}/zlib/trees.o \
	${OBJECTDIR}/zlib/uncompr.o \
	${OBJECTDIR}/zlib/zutil.o


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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/myromfs.exe

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/myromfs.exe: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/myromfs ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/main.o: main.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.c

${OBJECTDIR}/zlib/adler32.o: zlib/adler32.c
	${MKDIR} -p ${OBJECTDIR}/zlib
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/zlib/adler32.o zlib/adler32.c

${OBJECTDIR}/zlib/compress.o: zlib/compress.c
	${MKDIR} -p ${OBJECTDIR}/zlib
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/zlib/compress.o zlib/compress.c

${OBJECTDIR}/zlib/crc32.o: zlib/crc32.c
	${MKDIR} -p ${OBJECTDIR}/zlib
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/zlib/crc32.o zlib/crc32.c

${OBJECTDIR}/zlib/deflate.o: zlib/deflate.c
	${MKDIR} -p ${OBJECTDIR}/zlib
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/zlib/deflate.o zlib/deflate.c

${OBJECTDIR}/zlib/gzclose.o: zlib/gzclose.c
	${MKDIR} -p ${OBJECTDIR}/zlib
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/zlib/gzclose.o zlib/gzclose.c

${OBJECTDIR}/zlib/gzlib.o: zlib/gzlib.c
	${MKDIR} -p ${OBJECTDIR}/zlib
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/zlib/gzlib.o zlib/gzlib.c

${OBJECTDIR}/zlib/gzread.o: zlib/gzread.c
	${MKDIR} -p ${OBJECTDIR}/zlib
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/zlib/gzread.o zlib/gzread.c

${OBJECTDIR}/zlib/gzwrite.o: zlib/gzwrite.c
	${MKDIR} -p ${OBJECTDIR}/zlib
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/zlib/gzwrite.o zlib/gzwrite.c

${OBJECTDIR}/zlib/infback.o: zlib/infback.c
	${MKDIR} -p ${OBJECTDIR}/zlib
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/zlib/infback.o zlib/infback.c

${OBJECTDIR}/zlib/inffast.o: zlib/inffast.c
	${MKDIR} -p ${OBJECTDIR}/zlib
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/zlib/inffast.o zlib/inffast.c

${OBJECTDIR}/zlib/inflate.o: zlib/inflate.c
	${MKDIR} -p ${OBJECTDIR}/zlib
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/zlib/inflate.o zlib/inflate.c

${OBJECTDIR}/zlib/inftrees.o: zlib/inftrees.c
	${MKDIR} -p ${OBJECTDIR}/zlib
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/zlib/inftrees.o zlib/inftrees.c

${OBJECTDIR}/zlib/trees.o: zlib/trees.c
	${MKDIR} -p ${OBJECTDIR}/zlib
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/zlib/trees.o zlib/trees.c

${OBJECTDIR}/zlib/uncompr.o: zlib/uncompr.c
	${MKDIR} -p ${OBJECTDIR}/zlib
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/zlib/uncompr.o zlib/uncompr.c

${OBJECTDIR}/zlib/zutil.o: zlib/zutil.c
	${MKDIR} -p ${OBJECTDIR}/zlib
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/zlib/zutil.o zlib/zutil.c

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
