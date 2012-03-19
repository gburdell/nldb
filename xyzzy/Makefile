#The MIT License
#
#Copyright (c) 2006-2010  Karl W. Pfalzer
#
#Permission is hereby granted, free of charge, to any person obtaining a copy
#of this software and associated documentation files (the "Software"), to deal
#in the Software without restriction, including without limitation the rights
#to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#copies of the Software, and to permit persons to whom the Software is
#furnished to do so, subject to the following conditions:
#
#The above copyright notice and this permission notice shall be included in
#all copies or substantial portions of the Software.
#
#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
#THE SOFTWARE.

LIBNM    = xyzzy
SRC_DIR  = src/${LIBNM}

#CFLAGS := -DM32 -m32 -Isrc
CFLAGS := -Isrc
ifeq ("debug","${VARIANT}")
	CFLAGS += -DDEBUG -g
endif
ifeq ("release","${VARIANT}")
	CFLAGS += -O3
endif
ifeq ("profile","${VARIANT}")
	#CFLAGS += -DDEBUG -pg
	CFLAGS += -pg
endif

#
#BEGIN: common make
#

.SUFFIXES:

VARIANT  = debug
OBJ_DIR  = build/${VARIANT}
DIST_DIR = dist

ifeq ("Darwin","$(shell uname)")
	OS :=  MacOSX
else
	OS :=  linux
endif

LIBDIR = ${VARIANT}/${OS}

CXX_FILES  := $(wildcard ${SRC_DIR}/*.cxx)
MODULES    := ${CXX_FILES:${SRC_DIR}/%=%}
MODULES    := ${MODULES:%.cxx=%}
O_FILES     = ${MODULES:%=${OBJ_DIR}/%.o}
#$(info VARIANT=${VARIANT} OBJ_DIR=${OBJ_DIR})
#$(info O_FILES=${O_FILES})

.PHONY: debug release profile
debug release profile:
	${MAKE} ${@}.objdir VARIANT=${@}
	@${MAKE} ${MODULES:%=%.doto} VARIANT=${@} 
	${MAKE} ${DIST_DIR}/${@}/${OS}/${LIBNM}.a VARIANT=${@}

.PHONY: %.objdir
%.objdir: ${OBJ_DIR} ;
${OBJ_DIR}:
	mkdir -p ${@}

.PHONY: all
all: debug release profile ;

.PRECIOUS: ${OBJ_DIR}/%.d
${OBJ_DIR}/%.d: ${SRC_DIR}/%.cxx
	-@gcc ${CFLAGS} -MMD -MF ${@} ${<} -c -o ${@D}/${*}.phonyo
	@cat /dev/null > ${@D}/${*}.phonyo
	@echo ",touch \$${@}" | tr ',' '\t' >> ${@}

PHONY: %.doto
%.doto: ${OBJ_DIR}/%.d
	@${MAKE} -f ${<}

${OBJ_DIR}/%.o: ${OBJ_DIR}/%.phonyo
	gcc ${CFLAGS} -c -o ${@} ${SRC_DIR}/${*}.cxx

${DIST_DIR}/${VARIANT}/${OS}/${LIBNM}.a: ${O_FILES}
	test -d ${@D} || mkdir -p ${@D}
	ar -rs ${@} ${O_FILES}

.PHONY: clean
clean:
	-rm -rf build
	-find ${DIST_DIR}/*/${OS} -name '*.a' -exec rm -f {} \;

