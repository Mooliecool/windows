; /*
;    
;     Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
;    
;     The use and distribution terms for this software are contained in the file
;     named license.txt, which can be found in the root of this distribution.
;     By using this software in any fashion, you are agreeing to be bound by the
;     terms of this license.
;    
;     You must not remove this notice, or any other, from this software.
;    
;
;    MODULE: eventlog.mc
;
;    This file contains the message definition the Rotor PAL
; */

MessageId=0x01
;Severity=Error
;Facility=Application
SymbolicName=MSG_ERROR
Language=English
%1
.

MessageId=0x02
;Severity=Warning
;Facility=Application
SymbolicName=MSG_WARNING
Language=English
%1
.

MessageId=0x04
;Severity=Informational
;Facility=Application
SymbolicName=MSG_INFORMATIONAL
Language=English
%1
.


