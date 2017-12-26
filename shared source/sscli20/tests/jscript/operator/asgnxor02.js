//# ==++== 
//# 
//#   
//#    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//#   
//#    The use and distribution terms for this software are contained in the file
//#    named license.txt, which can be found in the root of this distribution.
//#    By using this software in any fashion, you are agreeing to be bound by the
//#    terms of this license.
//#   
//#    You must not remove this notice, or any other, from this software.
//#   
//# 
//# ==--== 
//####################################################################################
@cc_on


import System;

var NULL_DISPATCH = null;
var apGlobalObj;
var apPlatform;
var lFailCount;


var iTestID = 230537;

///////////////////////////////////////////////////////////////////////////////////
//



//          this tescase tests the ^= compound assignment op using combinations
//          of hard typed variables
// 
// Scenarios:
// 1) sbyte ^= byte
// 2) sbyte ^= short
// 3) sbyte ^= ushort
// 4) sbyte ^= int
// 5) sbyte ^= uint
// 6) sbyte ^= long
// 7) sbyte ^= ulong
// 8) sbyte ^= char
// 9) byte ^= sbyte
// 10) byte ^= short
// 11) byte ^= ushort
// 12) byte ^= int
// 13) byte ^= uint
// 14) byte ^= long
// 15) byte ^= ulong
// 16) byte ^= char
// 17) short ^= sbyte
// 18) short ^= byte
// 19) short ^= ushort
// 20) short ^= int
// 21) short ^= uint
// 22) short ^= long
// 23) short ^= ulong
// 24) short ^= char
// 25) ushort ^= sbyte
// 26) ushort ^= byte
// 27) ushort ^= short
// 28) ushort ^= int
// 29) ushort ^= uint
// 30) ushort ^= long
// 31) ushort ^= ulong
// 32) ushort ^= char
// 33) int ^= sbyte
// 34) int ^= byte
// 35) int ^= short
// 36) int ^= ushort
// 37) int ^= uint
// 38) int ^= long
// 39) int ^= ulong
// 40) int ^= char
// 41) uint ^= sbyte
// 42) uint ^= byte
// 43) uint ^= short
// 44) uint ^= ushort
// 45) uint ^= int
// 46) uint ^= long
// 47) uint ^= ulong
// 48) uint ^= char
// 49) long ^= sbyte
// 50) long ^= byte
// 51) long ^= short
// 52) long ^= ushort
// 53) long ^= int
// 54) long ^= uint
// 55) long ^= ulong
// 56) long ^= char
// 57) ulong ^= sbyte
// 58) ulong ^= byte
// 59) ulong ^= short
// 60) ulong ^= ushort
// 61) ulong ^= int
// 62) ulong ^= uint
// 63) ulong ^= long
// 64) ulong ^= char
// 65) char ^= sbyte
// 66) char ^= byte
// 67) char ^= short
// 68) char ^= ushort
// 69) char ^= int
// 70) char ^= uint
// 71) char ^= long
// 72) char ^= ulong

              






function asgnxor02() {

var x
var _sbyte : sbyte
var _tempsbyte : sbyte
var _byte : byte
var _tempbyte : byte
var _short : short
var _tempshort : short
var _ushort : ushort
var _tempushort : ushort
var _int : int
var _tempint : int
var _uint : uint
var _tempuint : uint
var _long : long
var _templong : long
var _ulong : ulong
var _tempulong : ulong
var _char : char
var _tempchar : char

apInitScenario("sbyte ^= byte")
_byte = 127
_sbyte = 18
_sbyte ^= _byte
_tempsbyte = 109
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")

_byte = 32
_sbyte = 127
_sbyte ^= _byte
_tempsbyte = 95
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")

_byte = 49
_sbyte = 77
_sbyte ^= _byte
_tempsbyte = 124
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")


apInitScenario("sbyte ^= short")
_short = 127
_sbyte = 107
_sbyte ^= _short
_tempsbyte = 20
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")

_short = 0
_sbyte = 127
_sbyte ^= _short
_tempsbyte = 127
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")

_short = 41
_sbyte = 69
_sbyte ^= _short
_tempsbyte = 108
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")


apInitScenario("sbyte ^= ushort")
_ushort = 127
_sbyte = 110
_sbyte ^= _ushort
_tempsbyte = 17
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")

_ushort = 110
_sbyte = 127
_sbyte ^= _ushort
_tempsbyte = 17
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")

_ushort = 120
_sbyte = 91
_sbyte ^= _ushort
_tempsbyte = 35
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")


apInitScenario("sbyte ^= int")
_int = 127
_sbyte = 3
_sbyte ^= _int
_tempsbyte = 124
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")

_int = 103
_sbyte = 127
_sbyte ^= _int
_tempsbyte = 24
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")

_int = 56
_sbyte = 12
_sbyte ^= _int
_tempsbyte = 52
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")


apInitScenario("sbyte ^= uint")
_uint = 127
_sbyte = 68
_sbyte ^= _uint
_tempsbyte = 59
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")

_uint = 79
_sbyte = 127
_sbyte ^= _uint
_tempsbyte = 48
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")

_uint = 54
_sbyte = 6
_sbyte ^= _uint
_tempsbyte = 48
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")


apInitScenario("sbyte ^= long")
_long = 127
_sbyte = 108
_sbyte ^= _long
_tempsbyte = 19
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")

_long = 113
_sbyte = 127
_sbyte ^= _long
_tempsbyte = 14
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")

_long = 67
_sbyte = 113
_sbyte ^= _long
_tempsbyte = 50
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")


apInitScenario("sbyte ^= ulong")
_ulong = 127
_sbyte = 108
_sbyte ^= _ulong
_tempsbyte = 19
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")

_ulong = 52
_sbyte = 127
_sbyte ^= _ulong
_tempsbyte = 75
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")

_ulong = 121
_sbyte = 0
_sbyte ^= _ulong
_tempsbyte = 121
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")


apInitScenario("sbyte ^= char")
_char = 127
_sbyte = 78
_sbyte ^= _char
_tempsbyte = 49
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")

_char = 84
_sbyte = 127
_sbyte ^= _char
_tempsbyte = 43
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")

_char = 124
_sbyte = 42
_sbyte ^= _char
_tempsbyte = 86
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")


apInitScenario("byte ^= sbyte")
_sbyte = 127
_byte = 99
_byte ^= _sbyte
_tempbyte = 28
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")

_sbyte = 48
_byte = 127
_byte ^= _sbyte
_tempbyte = 79
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")

_sbyte = 86
_byte = 2
_byte ^= _sbyte
_tempbyte = 84
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")


apInitScenario("byte ^= short")
_short = 255
_byte = 251
_byte ^= _short
_tempbyte = 4
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")

_short = 133
_byte = 255
_byte ^= _short
_tempbyte = 122
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")

_short = 129
_byte = 37
_byte ^= _short
_tempbyte = 164
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")


apInitScenario("byte ^= ushort")
_ushort = 255
_byte = 189
_byte ^= _ushort
_tempbyte = 66
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")

_ushort = 235
_byte = 255
_byte ^= _ushort
_tempbyte = 20
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")

_ushort = 160
_byte = 139
_byte ^= _ushort
_tempbyte = 43
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")


apInitScenario("byte ^= int")
_int = 255
_byte = 219
_byte ^= _int
_tempbyte = 36
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")

_int = 9
_byte = 255
_byte ^= _int
_tempbyte = 246
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")

_int = 76
_byte = 212
_byte ^= _int
_tempbyte = 152
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")


apInitScenario("byte ^= uint")
_uint = 255
_byte = 66
_byte ^= _uint
_tempbyte = 189
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")

_uint = 251
_byte = 255
_byte ^= _uint
_tempbyte = 4
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")

_uint = 43
_byte = 5
_byte ^= _uint
_tempbyte = 46
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")


apInitScenario("byte ^= long")
_long = 255
_byte = 37
_byte ^= _long
_tempbyte = 218
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")

_long = 233
_byte = 255
_byte ^= _long
_tempbyte = 22
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")

_long = 45
_byte = 63
_byte ^= _long
_tempbyte = 18
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")


apInitScenario("byte ^= ulong")
_ulong = 255
_byte = 185
_byte ^= _ulong
_tempbyte = 70
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")

_ulong = 125
_byte = 255
_byte ^= _ulong
_tempbyte = 130
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")

_ulong = 193
_byte = 111
_byte ^= _ulong
_tempbyte = 174
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")


apInitScenario("byte ^= char")
_char = 255
_byte = 13
_byte ^= _char
_tempbyte = 242
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")

_char = 82
_byte = 255
_byte ^= _char
_tempbyte = 173
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")

_char = 235
_byte = 51
_byte ^= _char
_tempbyte = 216
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")


apInitScenario("short ^= sbyte")
_sbyte = 127
_short = 113
_short ^= _sbyte
_tempshort = 14
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")

_sbyte = 78
_short = 127
_short ^= _sbyte
_tempshort = 49
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")

_sbyte = 119
_short = 81
_short ^= _sbyte
_tempshort = 38
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")


apInitScenario("short ^= byte")
_byte = 255
_short = 32
_short ^= _byte
_tempshort = 223
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")

_byte = 110
_short = 255
_short ^= _byte
_tempshort = 145
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")

_byte = 29
_short = 4
_short ^= _byte
_tempshort = 25
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")


apInitScenario("short ^= ushort")
_ushort = 32767
_short = 3874
_short ^= _ushort
_tempshort = 28893
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")

_ushort = 9215
_short = 32767
_short ^= _ushort
_tempshort = 23552
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")

_ushort = 10191
_short = 30486
_short ^= _ushort
_tempshort = 20697
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")


apInitScenario("short ^= int")
_int = 32767
_short = 8677
_short ^= _int
_tempshort = 24090
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")

_int = 32382
_short = 32767
_short ^= _int
_tempshort = 385
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")

_int = 1098
_short = 26750
_short ^= _int
_tempshort = 27700
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")


apInitScenario("short ^= uint")
_uint = 32767
_short = 3643
_short ^= _uint
_tempshort = 29124
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")

_uint = 3864
_short = 32767
_short ^= _uint
_tempshort = 28903
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")

_uint = 24363
_short = 20804
_short ^= _uint
_tempshort = 3695
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")


apInitScenario("short ^= long")
_long = 32767
_short = 16279
_short ^= _long
_tempshort = 16488
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")

_long = 31339
_short = 32767
_short ^= _long
_tempshort = 1428
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")

_long = 6153
_short = 28430
_short ^= _long
_tempshort = 30471
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")


apInitScenario("short ^= ulong")
_ulong = 32767
_short = 6624
_short ^= _ulong
_tempshort = 26143
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")

_ulong = 14247
_short = 32767
_short ^= _ulong
_tempshort = 18520
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")

_ulong = 9505
_short = 16260
_short ^= _ulong
_tempshort = 6821
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")


apInitScenario("short ^= char")
_char = 32767
_short = 25076
_short ^= _char
_tempshort = 7691
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")

_char = 2297
_short = 32767
_short ^= _char
_tempshort = 30470
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")

_char = 2968
_short = 13771
_short ^= _char
_tempshort = 15955
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")


apInitScenario("ushort ^= sbyte")
_sbyte = 127
_ushort = 0
_ushort ^= _sbyte
_tempushort = 127
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")

_sbyte = 54
_ushort = 127
_ushort ^= _sbyte
_tempushort = 73
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")

_sbyte = 83
_ushort = 31
_ushort ^= _sbyte
_tempushort = 76
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")


apInitScenario("ushort ^= byte")
_byte = 255
_ushort = 25
_ushort ^= _byte
_tempushort = 230
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")

_byte = 166
_ushort = 255
_ushort ^= _byte
_tempushort = 89
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")

_byte = 180
_ushort = 210
_ushort ^= _byte
_tempushort = 102
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")


apInitScenario("ushort ^= short")
_short = 32767
_ushort = 28577
_ushort ^= _short
_tempushort = 4190
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")

_short = 5017
_ushort = 32767
_ushort ^= _short
_tempushort = 27750
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")

_short = 29611
_ushort = 28326
_ushort ^= _short
_tempushort = 7437
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")


apInitScenario("ushort ^= int")
_int = 65535
_ushort = 39598
_ushort ^= _int
_tempushort = 25937
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")

_int = 16369
_ushort = 65535
_ushort ^= _int
_tempushort = 49166
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")

_int = 64537
_ushort = 48366
_ushort ^= _int
_tempushort = 16631
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")


apInitScenario("ushort ^= uint")
_uint = 65535
_ushort = 29954
_ushort ^= _uint
_tempushort = 35581
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")

_uint = 61664
_ushort = 65535
_ushort ^= _uint
_tempushort = 3871
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")

_uint = 21307
_ushort = 61402
_ushort ^= _uint
_tempushort = 48353
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")


apInitScenario("ushort ^= long")
_long = 65535
_ushort = 37884
_ushort ^= _long
_tempushort = 27651
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")

_long = 18785
_ushort = 65535
_ushort ^= _long
_tempushort = 46750
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")

_long = 46280
_ushort = 61276
_ushort ^= _long
_tempushort = 23444
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")


apInitScenario("ushort ^= ulong")
_ulong = 65535
_ushort = 28007
_ushort ^= _ulong
_tempushort = 37528
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")

_ulong = 4651
_ushort = 65535
_ushort ^= _ulong
_tempushort = 60884
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")

_ulong = 29009
_ushort = 45462
_ushort ^= _ulong
_tempushort = 49351
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")


apInitScenario("ushort ^= char")
_char = 65535
_ushort = 9280
_ushort ^= _char
_tempushort = 56255
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")

_char = 40526
_ushort = 65535
_ushort ^= _char
_tempushort = 25009
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")

_char = 16503
_ushort = 3152
_ushort ^= _char
_tempushort = 19495
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")


apInitScenario("int ^= sbyte")
_sbyte = 127
_int = 24
_int ^= _sbyte
_tempint = 103
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")

_sbyte = 113
_int = 127
_int ^= _sbyte
_tempint = 14
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")

_sbyte = 117
_int = 106
_int ^= _sbyte
_tempint = 31
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")


apInitScenario("int ^= byte")
_byte = 255
_int = 85
_int ^= _byte
_tempint = 170
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")

_byte = 200
_int = 255
_int ^= _byte
_tempint = 55
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")

_byte = 182
_int = 116
_int ^= _byte
_tempint = 194
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")


apInitScenario("int ^= short")
_short = 32767
_int = 13485
_int ^= _short
_tempint = 19282
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")

_short = 30023
_int = 32767
_int ^= _short
_tempint = 2744
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")

_short = 24994
_int = 11261
_int ^= _short
_tempint = 19039
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")


apInitScenario("int ^= ushort")
_ushort = 65535
_int = 3455
_int ^= _ushort
_tempint = 62080
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")

_ushort = 21189
_int = 65535
_int ^= _ushort
_tempint = 44346
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")

_ushort = 17262
_int = 62414
_int ^= _ushort
_tempint = 45216
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")


apInitScenario("int ^= uint")
_uint = 2147483647
_int = 966421004
_int ^= _uint
_tempint = 1181062643
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")

_uint = 1185544890
_int = 2147483647
_int ^= _uint
_tempint = 961938757
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")

_uint = 84123301
_int = 1770340078
_int ^= _uint
_tempint = 1820762187
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")


apInitScenario("int ^= long")
_long = 2147483647
_int = 1599865857
_int ^= _long
_tempint = 547617790
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")

_long = 329991965
_int = 2147483647
_int ^= _long
_tempint = 1817491682
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")

_long = 1808636501
_int = 1539047825
_int ^= _long
_tempint = 812751812
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")


apInitScenario("int ^= ulong")
_ulong = 2147483647
_int = 1544921566
_int ^= _ulong
_tempint = 602562081
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")

_ulong = 1543414528
_int = 2147483647
_int ^= _ulong
_tempint = 604069119
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")

_ulong = 1760409866
_int = 62874871
_int ^= _ulong
_tempint = 1800592893
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")


apInitScenario("int ^= char")
_char = 65535
_int = 13890
_int ^= _char
_tempint = 51645
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")

_char = 830
_int = 65535
_int ^= _char
_tempint = 64705
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")

_char = 44935
_int = 16630
_int ^= _char
_tempint = 61297
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")


apInitScenario("uint ^= sbyte")
_sbyte = 127
_uint = 114
_uint ^= _sbyte
_tempuint = 13
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")

_sbyte = 108
_uint = 127
_uint ^= _sbyte
_tempuint = 19
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")

_sbyte = 85
_uint = 90
_uint ^= _sbyte
_tempuint = 15
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")


apInitScenario("uint ^= byte")
_byte = 255
_uint = 82
_uint ^= _byte
_tempuint = 173
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")

_byte = 37
_uint = 255
_uint ^= _byte
_tempuint = 218
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")

_byte = 102
_uint = 171
_uint ^= _byte
_tempuint = 205
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")


apInitScenario("uint ^= short")
_short = 32767
_uint = 7366
_uint ^= _short
_tempuint = 25401
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")

_short = 28460
_uint = 32767
_uint ^= _short
_tempuint = 4307
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")

_short = 17214
_uint = 21686
_uint ^= _short
_tempuint = 6024
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")


apInitScenario("uint ^= ushort")
_ushort = 65535
_uint = 34331
_uint ^= _ushort
_tempuint = 31204
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")

_ushort = 23756
_uint = 65535
_uint ^= _ushort
_tempuint = 41779
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")

_ushort = 57820
_uint = 6817
_uint ^= _ushort
_tempuint = 64381
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")


apInitScenario("uint ^= int")
_int = 2147483647
_uint = 1734224860
_uint ^= _int
_tempuint = 413258787
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")

_int = 1052861957
_uint = 2147483647
_uint ^= _int
_tempuint = 1094621690
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")

_int = 523326931
_uint = 1266054622
_uint ^= _int
_tempuint = 1413947405
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")


apInitScenario("uint ^= long")
_long = 4294967295
_uint = 2487723916
_uint ^= _long
_tempuint = 1807243379
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")

_long = 1835905656
_uint = 4294967295
_uint ^= _long
_tempuint = 2459061639
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")

_long = 1301844084
_uint = 158889638
_uint ^= _long
_tempuint = 1155589842
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")


apInitScenario("uint ^= ulong")
_ulong = 4294967295
_uint = 208417882
_uint ^= _ulong
_tempuint = 4086549413
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")

_ulong = 903630988
_uint = 4294967295
_uint ^= _ulong
_tempuint = 3391336307
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")

_ulong = 514558688
_uint = 2645001112
_uint ^= _ulong
_tempuint = 2198604152
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")


apInitScenario("uint ^= char")
_char = 65535
_uint = 63350
_uint ^= _char
_tempuint = 2185
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")

_char = 45012
_uint = 65535
_uint ^= _char
_tempuint = 20523
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")

_char = 16030
_uint = 26141
_uint ^= _char
_tempuint = 22659
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")


apInitScenario("long ^= sbyte")
_sbyte = 127
_long = 29
_long ^= _sbyte
_templong = 98
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")

_sbyte = 64
_long = 127
_long ^= _sbyte
_templong = 63
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")

_sbyte = 56
_long = 25
_long ^= _sbyte
_templong = 33
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")


apInitScenario("long ^= byte")
_byte = 255
_long = 165
_long ^= _byte
_templong = 90
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")

_byte = 140
_long = 255
_long ^= _byte
_templong = 115
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")

_byte = 160
_long = 77
_long ^= _byte
_templong = 237
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")


apInitScenario("long ^= short")
_short = 32767
_long = 4868
_long ^= _short
_templong = 27899
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")

_short = 12074
_long = 32767
_long ^= _short
_templong = 20693
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")

_short = 19646
_long = 28718
_long ^= _short
_templong = 15504
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")


apInitScenario("long ^= ushort")
_ushort = 65535
_long = 41175
_long ^= _ushort
_templong = 24360
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")

_ushort = 11821
_long = 65535
_long ^= _ushort
_templong = 53714
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")

_ushort = 12635
_long = 23389
_long ^= _ushort
_templong = 27142
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")


apInitScenario("long ^= int")
_int = 2147483647
_long = 1796191535
_long ^= _int
_templong = 351292112
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")

_int = 1537004124
_long = 2147483647
_long ^= _int
_templong = 610479523
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")

_int = 476133657
_long = 1549797597
_long ^= _int
_templong = 1073820100
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")


apInitScenario("long ^= uint")
_uint = 4294967295
_long = 3302732694
_long ^= _uint
_templong = 992234601
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")

_uint = 412801108
_long = 4294967295
_long ^= _uint
_templong = 3882166187
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")

_uint = 2897145882
_long = 2035134700
_long ^= _uint
_templong = 3588440310
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")


apInitScenario("long ^= ulong")
_ulong = 9223372036854775807
_long = 5095873803090680000
_long ^= _ulong
_templong = 4127498233764095807
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")

_ulong = 5873619718501740000
_long = 9223372036854775807
_long ^= _ulong
_templong = 3349752318353035807
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")

_ulong = 6133271249242780000
_long = 1036045149928530000
_long ^= _ulong
_templong = 6592429998160400688
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")


apInitScenario("long ^= char")
_char = 65535
_long = 1687
_long ^= _char
_templong = 63848
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")

_char = 51618
_long = 65535
_long ^= _char
_templong = 13917
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")

_char = 46136
_long = 35255
_long ^= _char
_templong = 15759
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")


apInitScenario("ulong ^= sbyte")
_sbyte = 127
_ulong = 79
_ulong ^= _sbyte
_tempulong = 48
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")

_sbyte = 16
_ulong = 127
_ulong ^= _sbyte
_tempulong = 111
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")

_sbyte = 54
_ulong = 1
_ulong ^= _sbyte
_tempulong = 55
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")


apInitScenario("ulong ^= byte")
_byte = 255
_ulong = 234
_ulong ^= _byte
_tempulong = 21
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")

_byte = 174
_ulong = 255
_ulong ^= _byte
_tempulong = 81
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")

_byte = 115
_ulong = 65
_ulong ^= _byte
_tempulong = 50
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")


apInitScenario("ulong ^= short")
_short = 32767
_ulong = 28199
_ulong ^= _short
_tempulong = 4568
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")

_short = 30775
_ulong = 32767
_ulong ^= _short
_tempulong = 1992
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")

_short = 14449
_ulong = 6904
_ulong ^= _short
_tempulong = 8841
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")


apInitScenario("ulong ^= ushort")
_ushort = 65535
_ulong = 54255
_ulong ^= _ushort
_tempulong = 11280
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")

_ushort = 27962
_ulong = 65535
_ulong ^= _ushort
_tempulong = 37573
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")

_ushort = 26784
_ulong = 56893
_ulong ^= _ushort
_tempulong = 46749
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")


apInitScenario("ulong ^= int")
_int = 2147483647
_ulong = 37761601
_ulong ^= _int
_tempulong = 2109722046
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")

_int = 1638311143
_ulong = 2147483647
_ulong ^= _int
_tempulong = 509172504
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")

_int = 1673792666
_ulong = 538895729
_ulong ^= _int
_tempulong = 1138420203
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")


apInitScenario("ulong ^= uint")
_uint = 4294967295
_ulong = 1997701072
_ulong ^= _uint
_tempulong = 2297266223
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")

_uint = 2245972320
_ulong = 4294967295
_ulong ^= _uint
_tempulong = 2048994975
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")

_uint = 2705470048
_ulong = 577600132
_ulong ^= _uint
_tempulong = 2200914148
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")


apInitScenario("ulong ^= long")
_long = 9223372036854775807
_ulong = 7696677493614330000
_ulong ^= _long
_tempulong = 1526694543240445807
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")

_long = 8933288240445840000
_ulong = 9223372036854775807
_ulong ^= _long
_tempulong = 290083796408935807
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")

_long = 5945624707813750000
_ulong = 117136222211559000
_ulong ^= _long
_tempulong = 5990698920727444136
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")


apInitScenario("ulong ^= char")
_char = 65535
_ulong = 57994
_ulong ^= _char
_tempulong = 7541
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")

_char = 33903
_ulong = 65535
_ulong ^= _char
_tempulong = 31632
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")

_char = 18265
_ulong = 23653
_ulong ^= _char
_tempulong = 6972
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")


apInitScenario("char ^= sbyte")
_sbyte = 127
_char = 84
_char ^= _sbyte
_tempchar = 43
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")

_sbyte = 7
_char = 127
_char ^= _sbyte
_tempchar = 120
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")

_sbyte = 32
_char = 63
_char ^= _sbyte
_tempchar = 31
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")


apInitScenario("char ^= byte")
_byte = 255
_char = 191
_char ^= _byte
_tempchar = 64
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")

_byte = 45
_char = 255
_char ^= _byte
_tempchar = 210
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")

_byte = 111
_char = 38
_char ^= _byte
_tempchar = 73
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")


apInitScenario("char ^= short")
_short = 32767
_char = 8559
_char ^= _short
_tempchar = 24208
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")

_short = 31589
_char = 32767
_char ^= _short
_tempchar = 1178
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")

_short = 28214
_char = 27188
_char ^= _short
_tempchar = 1026
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")


apInitScenario("char ^= ushort")
_ushort = 65535
_char = 42934
_char ^= _ushort
_tempchar = 22601
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")

_ushort = 30397
_char = 65535
_char ^= _ushort
_tempchar = 35138
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")

_ushort = 42332
_char = 8245
_char ^= _ushort
_tempchar = 34153
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")


apInitScenario("char ^= int")
_int = 65535
_char = 14949
_char ^= _int
_tempchar = 50586
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")

_int = 52220
_char = 65535
_char ^= _int
_tempchar = 13315
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")

_int = 6209
_char = 17226
_char ^= _int
_tempchar = 23307
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")


apInitScenario("char ^= uint")
_uint = 65535
_char = 539
_char ^= _uint
_tempchar = 64996
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")

_uint = 29690
_char = 65535
_char ^= _uint
_tempchar = 35845
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")

_uint = 4773
_char = 6620
_char ^= _uint
_tempchar = 2937
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")


apInitScenario("char ^= long")
_long = 65535
_char = 32542
_char ^= _long
_tempchar = 32993
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")

_long = 19447
_char = 65535
_char ^= _long
_tempchar = 46088
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")

_long = 11820
_char = 62360
_char ^= _long
_tempchar = 56756
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")


apInitScenario("char ^= ulong")
_ulong = 65535
_char = 2657
_char ^= _ulong
_tempchar = 62878
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")

_ulong = 28954
_char = 65535
_char ^= _ulong
_tempchar = 36581
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")

_ulong = 24424
_char = 22497
_char ^= _ulong
_tempchar = 2185
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")





    apEndTest();
}


asgnxor02();


if(lFailCount >= 0) System.Environment.ExitCode = lFailCount;
else System.Environment.ExitCode = 0;

function apInitTest(stTestName) {
    lFailCount = 0;

    apGlobalObj = new Object();
    apGlobalObj.apGetPlatform = function Funca() { return "Rotor" }
    apGlobalObj.LangHost = function Funcb() { return 1033;}
    apGlobalObj.apGetLangExt = function Funcc(num) { return "EN"; }

    apPlatform = apGlobalObj.apGetPlatform();
    var sVer = "1.0";  //navigator.appVersion.toUpperCase().charAt(navigator.appVersion.toUpperCase().indexOf("MSIE")+5);
    apGlobalObj.apGetHost = function Funcp() { return "Rotor " + sVer; }
    print ("apInitTest: " + stTestName);
}

function apInitScenario(stScenarioName) {print( "\tapInitScenario: " + stScenarioName);}

function apLogFailInfo(stMessage, stExpected, stActual, stBugNum) {
    lFailCount = lFailCount + 1;
    print ("***** FAILED:");
    print ("\t\t" + stMessage);
    print ("\t\tExpected: " + stExpected);
    print ("\t\tActual: " + stActual);
}

function apGetLocale(){ return 1033; }
function apWriteDebug(s) { print("dbg ---> " + s) }
function apEndTest() {}
