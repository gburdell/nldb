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

APP       := nl_shell
LIBNM     := libtcl8.4.a
BUILD_DIR := unix

ifeq ("debug","${VARIANT}")
	CFLAGS += -DDEBUG -g
	STRIP =
endif
ifeq ("release","${VARIANT}")
	CFLAGS += -O3
	STRIP = strip ${@}
endif
ifeq ("profile","${VARIANT}")
	CFLAGS += -DDEBUG -pg
	STRIP =
endif

CFLAGS += -DTCL_LOCAL_APPINIT=Nlsh_AppInit

#
#BEGIN: common make
#

.SUFFIXES:

VARIANT  = debug
OBJ_DIR  = build/${VARIANT}
DIST_DIR = dist/${VARIANT}

ifeq ("Darwin","$(shell uname)")
	OS :=  MacOSX
else
	OS :=  linux
endif

###TODO
.PHONY: debug release profile
debug release profile:
	${MAKE} lib.a VARIANT=${@}
	${MAKE} app   VARIANT=${@}

.PHONY: all
all: debug release profile ;

.PHONY: lib.a
lib.a: ${OBJ_DIR}/${OS}/${LIBNM};

#TODO: add dependencies for libtcl.a
${OBJ_DIR}/${OS}/${LIBNM}:
	test -d ${@D} || mkdir -p ${@D}
	${MAKE} -C ${BUILD_DIR} clean ${LIBNM} CFLAGS="${CFLAGS}"
	mv -f ${BUILD_DIR}/${LIBNM} ${@}

.PHONY: app
app: ${DIST_DIR}/${OS}/${APP};

${DIST_DIR}/${OS}/${APP}: _ALWAYS
	test -d ${@D} || mkdir -p ${@D}
	${MAKE} -C ${BUILD_DIR} ${APP} CFLAGS="${CFLAGS}" VARIANT="${VARIANT}/${OS}"
	mv -f ${BUILD_DIR}/${APP} ${@}
	${STRIP}
	
_ALWAYS:

.PHONY: clean
clean:
	-rm -rf build/*/${OS}
	-rm -rf dist/*/${OS}
	${MAKE} -C ${BUILD_DIR} ${@}

