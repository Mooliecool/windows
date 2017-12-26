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


var iTestID = 230492;

///////////////////////////////////////////////////////////////////////////////////
//



//          this tescase tests the &= compound assignment op using combinations
//          of hard typed variables

// scenarios:
// 1) sbyte &= byte
// 2) sbyte &= short
// 3) sbyte &= ushort
// 4) sbyte &= int
// 5) sbyte &= uint
// 6) sbyte &= long
// 7) sbyte &= ulong
// 8) sbyte &= char
// 9) byte &= sbyte
// 10) byte &= short
// 11) byte &= ushort
// 12) byte &= int
// 13) byte &= uint
// 14) byte &= long
// 15) byte &= ulong
// 16) byte &= char
// 17) short &= sbyte
// 18) short &= byte
// 19) short &= ushort
// 20) short &= int
// 21) short &= uint
// 22) short &= long
// 23) short &= ulong
// 24) short &= char
// 25) ushort &= sbyte
// 26) ushort &= byte
// 27) ushort &= short
// 28) ushort &= int
// 29) ushort &= uint
// 30) ushort &= long
// 31) ushort &= ulong
// 32) ushort &= char
// 33) int &= sbyte
// 34) int &= byte
// 35) int &= short
// 36) int &= ushort
// 37) int &= uint
// 38) int &= long
// 39) int &= ulong
// 40) int &= char
// 41) uint &= sbyte
// 42) uint &= byte
// 43) uint &= short
// 44) uint &= ushort
// 45) uint &= int
// 46) uint &= long
// 47) uint &= ulong
// 48) uint &= char
// 49) long &= sbyte
// 50) long &= byte
// 51) long &= short
// 52) long &= ushort
// 53) long &= int
// 54) long &= uint
// 55) long &= ulong
// 56) long &= char
// 57) ulong &= sbyte
// 58) ulong &= byte
// 59) ulong &= short
// 60) ulong &= ushort
// 61) ulong &= int
// 62) ulong &= uint
// 63) ulong &= long
// 64) ulong &= char
// 65) char &= sbyte
// 66) char &= byte
// 67) char &= short
// 68) char &= ushort
// 69) char &= int
// 70) char &= uint
// 71) char &= long
// 72) char &= ulong
              






function asgnand02() {

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

apInitScenario("sbyte &= byte")
_byte = 127
_sbyte = 59
_sbyte &= _byte
_tempsbyte = 59
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")

_byte = 14
_sbyte = 127
_sbyte &= _byte
_tempsbyte = 14
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")

_byte = 10
_sbyte = 103
_sbyte &= _byte
_tempsbyte = 2
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")


apInitScenario("sbyte &= short")
_short = 127
_sbyte = 24
_sbyte &= _short
_tempsbyte = 24
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")

_short = 43
_sbyte = 127
_sbyte &= _short
_tempsbyte = 43
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")

_short = 45
_sbyte = 10
_sbyte &= _short
_tempsbyte = 8
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")


apInitScenario("sbyte &= ushort")
_ushort = 127
_sbyte = 64
_sbyte &= _ushort
_tempsbyte = 64
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")

_ushort = 87
_sbyte = 127
_sbyte &= _ushort
_tempsbyte = 87
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")

_ushort = 13
_sbyte = 87
_sbyte &= _ushort
_tempsbyte = 5
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")


apInitScenario("sbyte &= int")
_int = 127
_sbyte = 51
_sbyte &= _int
_tempsbyte = 51
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")

_int = 25
_sbyte = 127
_sbyte &= _int
_tempsbyte = 25
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")

_int = 23
_sbyte = 68
_sbyte &= _int
_tempsbyte = 4
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")


apInitScenario("sbyte &= uint")
_uint = 127
_sbyte = 70
_sbyte &= _uint
_tempsbyte = 70
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")

_uint = 34
_sbyte = 127
_sbyte &= _uint
_tempsbyte = 34
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")

_uint = 105
_sbyte = 51
_sbyte &= _uint
_tempsbyte = 33
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")


apInitScenario("sbyte &= long")
_long = 127
_sbyte = 44
_sbyte &= _long
_tempsbyte = 44
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")

_long = 24
_sbyte = 127
_sbyte &= _long
_tempsbyte = 24
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")

_long = 46
_sbyte = 4
_sbyte &= _long
_tempsbyte = 4
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")


apInitScenario("sbyte &= ulong")
_ulong = 127
_sbyte = 120
_sbyte &= _ulong
_tempsbyte = 120
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")

_ulong = 74
_sbyte = 127
_sbyte &= _ulong
_tempsbyte = 74
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")

_ulong = 59
_sbyte = 114
_sbyte &= _ulong
_tempsbyte = 50
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")


apInitScenario("sbyte &= char")
_char = 127
_sbyte = 103
_sbyte &= _char
_tempsbyte = 103
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")

_char = 27
_sbyte = 127
_sbyte &= _char
_tempsbyte = 27
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")

_char = 84
_sbyte = 75
_sbyte &= _char
_tempsbyte = 64
if (_sbyte !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte, "")


apInitScenario("byte &= sbyte")
_sbyte = 127
_byte = 105
_byte &= _sbyte
_tempbyte = 105
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")

_sbyte = 49
_byte = 127
_byte &= _sbyte
_tempbyte = 49
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")

_sbyte = 69
_byte = 32
_byte &= _sbyte
_tempbyte = 0
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")


apInitScenario("byte &= short")
_short = 255
_byte = 250
_byte &= _short
_tempbyte = 250
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")

_short = 86
_byte = 255
_byte &= _short
_tempbyte = 86
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")

_short = 173
_byte = 108
_byte &= _short
_tempbyte = 44
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")


apInitScenario("byte &= ushort")
_ushort = 255
_byte = 248
_byte &= _ushort
_tempbyte = 248
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")

_ushort = 215
_byte = 255
_byte &= _ushort
_tempbyte = 215
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")

_ushort = 106
_byte = 180
_byte &= _ushort
_tempbyte = 32
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")


apInitScenario("byte &= int")
_int = 255
_byte = 74
_byte &= _int
_tempbyte = 74
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")

_int = 177
_byte = 255
_byte &= _int
_tempbyte = 177
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")

_int = 229
_byte = 253
_byte &= _int
_tempbyte = 229
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")


apInitScenario("byte &= uint")
_uint = 255
_byte = 198
_byte &= _uint
_tempbyte = 198
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")

_uint = 169
_byte = 255
_byte &= _uint
_tempbyte = 169
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")

_uint = 11
_byte = 175
_byte &= _uint
_tempbyte = 11
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")


apInitScenario("byte &= long")
_long = 255
_byte = 145
_byte &= _long
_tempbyte = 145
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")

_long = 77
_byte = 255
_byte &= _long
_tempbyte = 77
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")

_long = 9
_byte = 69
_byte &= _long
_tempbyte = 1
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")


apInitScenario("byte &= ulong")
_ulong = 255
_byte = 192
_byte &= _ulong
_tempbyte = 192
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")

_ulong = 11
_byte = 255
_byte &= _ulong
_tempbyte = 11
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")

_ulong = 220
_byte = 156
_byte &= _ulong
_tempbyte = 156
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")


apInitScenario("byte &= char")
_char = 255
_byte = 223
_byte &= _char
_tempbyte = 223
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")

_char = 116
_byte = 255
_byte &= _char
_tempbyte = 116
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")

_char = 68
_byte = 74
_byte &= _char
_tempbyte = 64
if (_byte !== _tempbyte) apLogFailInfo("Wrong Value", _tempbyte, _byte, "")


apInitScenario("short &= sbyte")
_sbyte = 127
_short = 3
_short &= _sbyte
_tempshort = 3
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")

_sbyte = 64
_short = 127
_short &= _sbyte
_tempshort = 64
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")

_sbyte = 108
_short = 1
_short &= _sbyte
_tempshort = 0
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")


apInitScenario("short &= byte")
_byte = 255
_short = 166
_short &= _byte
_tempshort = 166
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")

_byte = 235
_short = 255
_short &= _byte
_tempshort = 235
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")

_byte = 142
_short = 53
_short &= _byte
_tempshort = 4
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")


apInitScenario("short &= ushort")
_ushort = 32767
_short = 19294
_short &= _ushort
_tempshort = 19294
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")

_ushort = 13273
_short = 32767
_short &= _ushort
_tempshort = 13273
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")

_ushort = 14240
_short = 16565
_short &= _ushort
_tempshort = 160
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")


apInitScenario("short &= int")
_int = 32767
_short = 25521
_short &= _int
_tempshort = 25521
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")

_int = 21465
_short = 32767
_short &= _int
_tempshort = 21465
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")

_int = 24361
_short = 8383
_short &= _int
_tempshort = 41
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")


apInitScenario("short &= uint")
_uint = 32767
_short = 22471
_short &= _uint
_tempshort = 22471
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")

_uint = 15437
_short = 32767
_short &= _uint
_tempshort = 15437
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")

_uint = 4171
_short = 4793
_short &= _uint
_tempshort = 4105
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")


apInitScenario("short &= long")
_long = 32767
_short = 5494
_short &= _long
_tempshort = 5494
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")

_long = 31944
_short = 32767
_short &= _long
_tempshort = 31944
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")

_long = 850
_short = 17185
_short &= _long
_tempshort = 768
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")


apInitScenario("short &= ulong")
_ulong = 32767
_short = 11598
_short &= _ulong
_tempshort = 11598
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")

_ulong = 9083
_short = 32767
_short &= _ulong
_tempshort = 9083
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")

_ulong = 16546
_short = 30799
_short &= _ulong
_tempshort = 16386
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")


apInitScenario("short &= char")
_char = 32767
_short = 15632
_short &= _char
_tempshort = 15632
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")

_char = 2247
_short = 32767
_short &= _char
_tempshort = 2247
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")

_char = 18038
_short = 16974
_short &= _char
_tempshort = 16966
if (_short !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short, "")


apInitScenario("ushort &= sbyte")
_sbyte = 127
_ushort = 73
_ushort &= _sbyte
_tempushort = 73
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")

_sbyte = 15
_ushort = 127
_ushort &= _sbyte
_tempushort = 15
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")

_sbyte = 86
_ushort = 99
_ushort &= _sbyte
_tempushort = 66
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")


apInitScenario("ushort &= byte")
_byte = 255
_ushort = 213
_ushort &= _byte
_tempushort = 213
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")

_byte = 225
_ushort = 255
_ushort &= _byte
_tempushort = 225
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")

_byte = 87
_ushort = 217
_ushort &= _byte
_tempushort = 81
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")


apInitScenario("ushort &= short")
_short = 32767
_ushort = 3559
_ushort &= _short
_tempushort = 3559
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")

_short = 27394
_ushort = 32767
_ushort &= _short
_tempushort = 27394
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")

_short = 3211
_ushort = 5472
_ushort &= _short
_tempushort = 1024
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")


apInitScenario("ushort &= int")
_int = 65535
_ushort = 57093
_ushort &= _int
_tempushort = 57093
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")

_int = 34922
_ushort = 65535
_ushort &= _int
_tempushort = 34922
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")

_int = 32430
_ushort = 6515
_ushort &= _int
_tempushort = 6178
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")


apInitScenario("ushort &= uint")
_uint = 65535
_ushort = 19672
_ushort &= _uint
_tempushort = 19672
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")

_uint = 39861
_ushort = 65535
_ushort &= _uint
_tempushort = 39861
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")

_uint = 60719
_ushort = 26443
_ushort &= _uint
_tempushort = 25867
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")


apInitScenario("ushort &= long")
_long = 65535
_ushort = 21510
_ushort &= _long
_tempushort = 21510
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")

_long = 7954
_ushort = 65535
_ushort &= _long
_tempushort = 7954
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")

_long = 8065
_ushort = 3599
_ushort &= _long
_tempushort = 3585
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")


apInitScenario("ushort &= ulong")
_ulong = 65535
_ushort = 31529
_ushort &= _ulong
_tempushort = 31529
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")

_ulong = 21850
_ushort = 65535
_ushort &= _ulong
_tempushort = 21850
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")

_ulong = 43232
_ushort = 24618
_ushort &= _ulong
_tempushort = 8224
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")


apInitScenario("ushort &= char")
_char = 65535
_ushort = 27332
_ushort &= _char
_tempushort = 27332
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")

_char = 40495
_ushort = 65535
_ushort &= _char
_tempushort = 40495
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")

_char = 48062
_ushort = 34095
_ushort &= _char
_tempushort = 33070
if (_ushort !== _tempushort) apLogFailInfo("Wrong Value", _tempushort, _ushort, "")


apInitScenario("int &= sbyte")
_sbyte = 127
_int = 108
_int &= _sbyte
_tempint = 108
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")

_sbyte = 116
_int = 127
_int &= _sbyte
_tempint = 116
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")

_sbyte = 117
_int = 83
_int &= _sbyte
_tempint = 81
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")


apInitScenario("int &= byte")
_byte = 255
_int = 249
_int &= _byte
_tempint = 249
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")

_byte = 244
_int = 255
_int &= _byte
_tempint = 244
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")

_byte = 106
_int = 204
_int &= _byte
_tempint = 72
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")


apInitScenario("int &= short")
_short = 32767
_int = 4233
_int &= _short
_tempint = 4233
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")

_short = 8929
_int = 32767
_int &= _short
_tempint = 8929
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")

_short = 1233
_int = 10867
_int &= _short
_tempint = 81
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")


apInitScenario("int &= ushort")
_ushort = 65535
_int = 57467
_int &= _ushort
_tempint = 57467
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")

_ushort = 56290
_int = 65535
_int &= _ushort
_tempint = 56290
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")

_ushort = 42813
_int = 53810
_int &= _ushort
_tempint = 33328
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")


apInitScenario("int &= uint")
_uint = 2147483647
_int = 1680071090
_int &= _uint
_tempint = 1680071090
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")

_uint = 870950657
_int = 2147483647
_int &= _uint
_tempint = 870950657
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")

_uint = 1373871654
_int = 1865790151
_int &= _uint
_tempint = 1092719110
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")


apInitScenario("int &= long")
_long = 2147483647
_int = 305078311
_int &= _long
_tempint = 305078311
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")

_long = 315713822
_int = 2147483647
_int &= _long
_tempint = 315713822
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")

_long = 407575437
_int = 981990625
_int &= _long
_tempint = 402856065
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")


apInitScenario("int &= ulong")
_ulong = 2147483647
_int = 4026242
_int &= _ulong
_tempint = 4026242
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")

_ulong = 1337530221
_int = 2147483647
_int &= _ulong
_tempint = 1337530221
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")

_ulong = 655073869
_int = 1080065176
_int &= _ulong
_tempint = 8200
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")


apInitScenario("int &= char")
_char = 65535
_int = 14740
_int &= _char
_tempint = 14740
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")

_char = 63325
_int = 65535
_int &= _char
_tempint = 63325
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")

_char = 28684
_int = 32158
_int &= _char
_tempint = 28684
if (_int !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int, "")


apInitScenario("uint &= sbyte")
_sbyte = 127
_uint = 13
_uint &= _sbyte
_tempuint = 13
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")

_sbyte = 73
_uint = 127
_uint &= _sbyte
_tempuint = 73
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")

_sbyte = 39
_uint = 121
_uint &= _sbyte
_tempuint = 33
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")


apInitScenario("uint &= byte")
_byte = 255
_uint = 154
_uint &= _byte
_tempuint = 154
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")

_byte = 214
_uint = 255
_uint &= _byte
_tempuint = 214
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")

_byte = 31
_uint = 87
_uint &= _byte
_tempuint = 23
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")


apInitScenario("uint &= short")
_short = 32767
_uint = 6185
_uint &= _short
_tempuint = 6185
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")

_short = 4106
_uint = 32767
_uint &= _short
_tempuint = 4106
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")

_short = 8987
_uint = 1825
_uint &= _short
_tempuint = 769
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")


apInitScenario("uint &= ushort")
_ushort = 65535
_uint = 5487
_uint &= _ushort
_tempuint = 5487
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")

_ushort = 51865
_uint = 65535
_uint &= _ushort
_tempuint = 51865
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")

_ushort = 11666
_uint = 40774
_uint &= _ushort
_tempuint = 3330
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")


apInitScenario("uint &= int")
_int = 2147483647
_uint = 1460529880
_uint &= _int
_tempuint = 1460529880
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")

_int = 1800836265
_uint = 2147483647
_uint &= _int
_tempuint = 1800836265
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")

_int = 1274126536
_uint = 24682284
_uint &= _int
_tempuint = 24150024
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")


apInitScenario("uint &= long")
_long = 4294967295
_uint = 4201193748
_uint &= _long
_tempuint = 4201193748
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")

_long = 3713278476
_uint = 4294967295
_uint &= _long
_tempuint = 3713278476
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")

_long = 1624359952
_uint = 3038956106
_uint &= _long
_tempuint = 536920064
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")


apInitScenario("uint &= ulong")
_ulong = 4294967295
_uint = 3121535170
_uint &= _ulong
_tempuint = 3121535170
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")

_ulong = 2017093856
_uint = 4294967295
_uint &= _ulong
_tempuint = 2017093856
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")

_ulong = 2800510108
_uint = 1522551560
_uint &= _ulong
_tempuint = 46153736
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")


apInitScenario("uint &= char")
_char = 65535
_uint = 42747
_uint &= _char
_tempuint = 42747
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")

_char = 60065
_uint = 65535
_uint &= _char
_tempuint = 60065
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")

_char = 37766
_uint = 10677
_uint &= _char
_tempuint = 388
if (_uint !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint, "")


apInitScenario("long &= sbyte")
_sbyte = 127
_long = 106
_long &= _sbyte
_templong = 106
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")

_sbyte = 69
_long = 127
_long &= _sbyte
_templong = 69
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")

_sbyte = 29
_long = 37
_long &= _sbyte
_templong = 5
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")


apInitScenario("long &= byte")
_byte = 255
_long = 139
_long &= _byte
_templong = 139
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")

_byte = 178
_long = 255
_long &= _byte
_templong = 178
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")

_byte = 54
_long = 240
_long &= _byte
_templong = 48
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")


apInitScenario("long &= short")
_short = 32767
_long = 9288
_long &= _short
_templong = 9288
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")

_short = 9728
_long = 32767
_long &= _short
_templong = 9728
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")

_short = 22283
_long = 548
_long &= _short
_templong = 512
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")


apInitScenario("long &= ushort")
_ushort = 65535
_long = 57194
_long &= _ushort
_templong = 57194
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")

_ushort = 8787
_long = 65535
_long &= _ushort
_templong = 8787
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")

_ushort = 24480
_long = 13792
_long &= _ushort
_templong = 5536
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")


apInitScenario("long &= int")
_int = 2147483647
_long = 955249107
_long &= _int
_templong = 955249107
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")

_int = 1466431774
_long = 2147483647
_long &= _int
_templong = 1466431774
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")

_int = 1767018943
_long = 829663304
_long &= _int
_templong = 559054856
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")


apInitScenario("long &= uint")
_uint = 4294967295
_long = 1601890962
_long &= _uint
_templong = 1601890962
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")

_uint = 1830555826
_long = 4294967295
_long &= _uint
_templong = 1830555826
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")

_uint = 2201367968
_long = 1022566366
_long &= _uint
_templong = 3282304
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")


apInitScenario("long &= ulong")
_ulong = 9223372036854775807
_long = 1840469594013520000
_long &= _ulong
_templong = 1840469594013520000
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")

_ulong = 5579013338707870000
_long = 9223372036854775807
_long &= _ulong
_templong = 5579013338707870000
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")

_ulong = 2101264397804660000
_long = 1257321216581430000
_long &= _ulong
_templong = 1234022393379315744
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")


apInitScenario("long &= char")
_char = 65535
_long = 12379
_long &= _char
_templong = 12379
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")

_char = 50303
_long = 65535
_long &= _char
_templong = 50303
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")

_char = 45256
_long = 17839
_long &= _char
_templong = 136
if (_long !== _templong) apLogFailInfo("Wrong Value", _templong, _long, "")


apInitScenario("ulong &= sbyte")
_sbyte = 127
_ulong = 69
_ulong &= _sbyte
_tempulong = 69
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")

_sbyte = 14
_ulong = 127
_ulong &= _sbyte
_tempulong = 14
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")

_sbyte = 28
_ulong = 68
_ulong &= _sbyte
_tempulong = 4
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")


apInitScenario("ulong &= byte")
_byte = 255
_ulong = 143
_ulong &= _byte
_tempulong = 143
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")

_byte = 224
_ulong = 255
_ulong &= _byte
_tempulong = 224
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")

_byte = 19
_ulong = 250
_ulong &= _byte
_tempulong = 18
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")


apInitScenario("ulong &= short")
_short = 32767
_ulong = 20428
_ulong &= _short
_tempulong = 20428
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")

_short = 21361
_ulong = 32767
_ulong &= _short
_tempulong = 21361
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")

_short = 23854
_ulong = 22322
_ulong &= _short
_tempulong = 21794
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")


apInitScenario("ulong &= ushort")
_ushort = 65535
_ulong = 12091
_ulong &= _ushort
_tempulong = 12091
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")

_ushort = 23688
_ulong = 65535
_ulong &= _ushort
_tempulong = 23688
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")

_ushort = 54710
_ulong = 38842
_ulong &= _ushort
_tempulong = 38322
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")


apInitScenario("ulong &= int")
_int = 2147483647
_ulong = 206374006
_ulong &= _int
_tempulong = 206374006
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")

_int = 948308117
_ulong = 2147483647
_ulong &= _int
_tempulong = 948308117
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")

_int = 1953510320
_ulong = 2081830039
_ulong &= _int
_tempulong = 1947218064
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")


apInitScenario("ulong &= uint")
_uint = 4294967295
_ulong = 402478008
_ulong &= _uint
_tempulong = 402478008
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")

_uint = 815754382
_ulong = 4294967295
_ulong &= _uint
_tempulong = 815754382
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")

_uint = 3392843830
_ulong = 1754998450
_ulong &= _uint
_tempulong = 1209671730
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")


apInitScenario("ulong &= long")
_long = 9223372036854775807
_ulong = 317128032958370000
_ulong &= _long
_tempulong = 317128032958370000
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")

_long = 9139611013378520000
_ulong = 9223372036854775807
_ulong &= _long
_tempulong = 9139611013378520000
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")

_long = 918787393027334000
_ulong = 8684874276891640000
_ulong &= _long
_tempulong = 612507141793660992
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")


apInitScenario("ulong &= char")
_char = 65535
_ulong = 30858
_ulong &= _char
_tempulong = 30858
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")

_char = 5165
_ulong = 65535
_ulong &= _char
_tempulong = 5165
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")

_char = 49315
_ulong = 33809
_ulong &= _char
_tempulong = 32769
if (_ulong !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong, "")


apInitScenario("char &= sbyte")
_sbyte = 127
_char = 77
_char &= _sbyte
_tempchar = 77
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")

_sbyte = 51
_char = 127
_char &= _sbyte
_tempchar = 51
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")

_sbyte = 59
_char = 96
_char &= _sbyte
_tempchar = 32
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")


apInitScenario("char &= byte")
_byte = 255
_char = 232
_char &= _byte
_tempchar = 232
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")

_byte = 213
_char = 255
_char &= _byte
_tempchar = 213
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")

_byte = 165
_char = 144
_char &= _byte
_tempchar = 128
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")


apInitScenario("char &= short")
_short = 32767
_char = 19895
_char &= _short
_tempchar = 19895
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")

_short = 27570
_char = 32767
_char &= _short
_tempchar = 27570
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")

_short = 24998
_char = 23627
_char &= _short
_tempchar = 16386
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")


apInitScenario("char &= ushort")
_ushort = 65535
_char = 45756
_char &= _ushort
_tempchar = 45756
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")

_ushort = 54478
_char = 65535
_char &= _ushort
_tempchar = 54478
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")

_ushort = 3511
_char = 54923
_char &= _ushort
_tempchar = 1155
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")


apInitScenario("char &= int")
_int = 65535
_char = 50464
_char &= _int
_tempchar = 50464
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")

_int = 41622
_char = 65535
_char &= _int
_tempchar = 41622
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")

_int = 2635
_char = 48974
_char &= _int
_tempchar = 2634
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")


apInitScenario("char &= uint")
_uint = 65535
_char = 56223
_char &= _uint
_tempchar = 56223
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")

_uint = 47259
_char = 65535
_char &= _uint
_tempchar = 47259
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")

_uint = 11698
_char = 23534
_char &= _uint
_tempchar = 2466
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")


apInitScenario("char &= long")
_long = 65535
_char = 21063
_char &= _long
_tempchar = 21063
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")

_long = 53231
_char = 65535
_char &= _long
_tempchar = 53231
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")

_long = 32926
_char = 37453
_char &= _long
_tempchar = 32780
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")


apInitScenario("char &= ulong")
_ulong = 65535
_char = 51067
_char &= _ulong
_tempchar = 51067
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")

_ulong = 8786
_char = 65535
_char &= _ulong
_tempchar = 8786
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")

_ulong = 33458
_char = 35691
_char &= _ulong
_tempchar = 33314
if (_char !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char, "")





    apEndTest();
}


asgnand02();


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
