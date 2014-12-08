# Hack makefile to just remember how to build.
# TODO: needs lots of work

TCL_ROOT ?= /home/gburdell/projects/nldb/tcl/tcl8.4.19

.PHONY: debug %.tgt
debug: debug.tgt ;
clean: clean.tgt ;
%.tgt:
	(cd xyzzy; ${MAKE} ${*})
	(cd vnl; ${MAKE} ${*})
	(cd vnl; ${MAKE} ${*})
	(cd tcl/tcl8.4.19; ${MAKE} TCL_ROOT=${TCL_ROOT} ${*})
