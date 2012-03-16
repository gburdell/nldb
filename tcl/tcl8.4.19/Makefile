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

CFLAGS += -DTCL_LOCAL_APPINIT=Nldbsh_AppInit

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

