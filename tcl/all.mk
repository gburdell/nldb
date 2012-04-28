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

#Build nl_shell

VARIANT  := debug
DIST_DIR := dist

ifeq ("Darwin","$(shell uname)")
	OS :=  MacOSX
else
	OS :=  linux
endif

LIB_DIRS := ../xyzzy ../slf ../vnl .. . tcl8.4.19

.PHONY: debug release profile clean
debug release profile:
	@for i in ${LIB_DIRS} ; \
	    do \
			${MAKE} -C $$i ${@} ; \
	    done;
	mv -f tcl8.4.19/dist/${@}/${OS}/nl_shell dist/${@}/${OS}

clean:
	@for i in ${LIB_DIRS} ; \
	    do \
			${MAKE} -C $$i ${@} ; \
	    done;

.PHONY: all
all: debug release profile ;
