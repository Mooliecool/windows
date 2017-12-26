###############################################################
#
# Rotor sample projects mk file 
#
#
###############################################################

!IF 0

Copyright (c) Microsoft Corporation.  All rights reserved.

Abstract:

    This file specifies the target component being built and the list of
    sources files needed to build that component.  Also specifies optional
    compiler switches and libraries that are unique for the component being
    built.

!ENDIF

# Should have already been included !include $(DEVDIV_TOOLS)\sources.inc

_PROJECT_=rotor_sample

INCLUDES=$(INCLUDES);$(_NTROOT)\ndp\inc\version

EXCLUDE_ASSEMBLY_ATTRIBUTES=1

