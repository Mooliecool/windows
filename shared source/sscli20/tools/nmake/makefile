# /*++
# 
#  Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
# 
#  The use and distribution terms for this software are contained in the file
#  named license.txt, which can be found in the root of this distribution.
#  By using this software in any fashion, you are agreeing to be bound by the
#  terms of this license.
# 
#  You must not remove this notice, or any other, from this software.
# 
#
# Module Name : makefile
#
# This file is NOT generated
#
# --*/

SHELL = /bin/bash

# It's very easy here -> just redirect all the targets to the obj subdirectories

all: check_env
	cd obj${BUILD_ALT_DIR}/${_BUILDARCH} && \
	${MAKE} all

clean: check_env
	cd obj${BUILD_ALT_DIR}/${_BUILDARCH} && \
	${MAKE} clean

depend: check_env
	cd obj${BUILD_ALT_DIR}/${_BUILDARCH} && \
	${MAKE} depend

check_env:
        if test X"${_NTTREE}" = "X"; \
        then \
                echo "ERROR: Set environment before running this."; \
                exit 1; \
        fi

