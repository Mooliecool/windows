#
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

prepbuild: eventlog.rc placepalheader

eventlog.rc : eventlog.mc
        $(MESSAGE_COMPILER) -U eventlog.mc

placepalheader:
            if not exist $(_NTTREE)\sdk\pal\inc mkdir $(_NTTREE)\sdk\pal\inc
            $(COPY_NAME) $(ROTOR_DIR)\pal\inc\rotor_pal.h $(_NTTREE)\sdk\pal\inc
