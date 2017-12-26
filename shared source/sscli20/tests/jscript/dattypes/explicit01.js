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


var iTestID = 228578;

/*






*/
///////////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
function explicit01() {

  apInitTest("explicit01");

var x
var _sbyte : sbyte
var _byte : byte
var _short : short
var _ushort : ushort
var _int : int
var _uint : uint
var _long : long
var _ulong : ulong
var _char : char
var _decimal : decimal

apInitScenario("sbyte <-- sbyte")

_sbyte = 127
_sbyte = _sbyte
if (_sbyte != 127) apLogFailInfo("Wrong Value", 127, _sbyte, "")
_sbyte = sbyte(127)
if (_sbyte != 127) apLogFailInfo("Wrong Value", 127, _sbyte, "")
x = sbyte(_sbyte)
if (x != 127) apLogFailInfo("Wrong Value", 127, _sbyte, "")
x = sbyte(sbyte(127))
if (x != 127) apLogFailInfo("Wrong Value", 127, _sbyte, "")

_sbyte = -128
_sbyte = _sbyte
if (_sbyte != -128) apLogFailInfo("Wrong Value", -128, _sbyte, "")
_sbyte = sbyte(-128)
if (_sbyte != -128) apLogFailInfo("Wrong Value", -128, _sbyte, "")
x = sbyte(_sbyte)
if (x != -128) apLogFailInfo("Wrong Value", -128, _sbyte, "")
x = sbyte(sbyte(-128))
if (x != -128) apLogFailInfo("Wrong Value", -128, _sbyte, "")

_sbyte = 101
_sbyte = _sbyte
if (_sbyte != 101) apLogFailInfo("Wrong Value", 101, _sbyte, "")
_sbyte = sbyte(101)
if (_sbyte != 101) apLogFailInfo("Wrong Value", 101, _sbyte, "")
x = sbyte(_sbyte)
if (x != 101) apLogFailInfo("Wrong Value", 101, _sbyte, "")
x = sbyte(sbyte(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _sbyte, "")



apInitScenario("sbyte <-- byte")

_byte = 127
_sbyte = _byte
if (_sbyte != 127) apLogFailInfo("Wrong Value", 127, _sbyte, "")
_sbyte = byte(127)
if (_sbyte != 127) apLogFailInfo("Wrong Value", 127, _sbyte, "")
x = sbyte(_byte)
if (x != 127) apLogFailInfo("Wrong Value", 127, _sbyte, "")
x = sbyte(byte(127))
if (x != 127) apLogFailInfo("Wrong Value", 127, _sbyte, "")

_byte = 0
_sbyte = _byte
if (_sbyte != 0) apLogFailInfo("Wrong Value", 0, _sbyte, "")
_sbyte = byte(0)
if (_sbyte != 0) apLogFailInfo("Wrong Value", 0, _sbyte, "")
x = sbyte(_byte)
if (x != 0) apLogFailInfo("Wrong Value", 0, _sbyte, "")
x = sbyte(byte(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _sbyte, "")

_byte = 101
_sbyte = _byte
if (_sbyte != 101) apLogFailInfo("Wrong Value", 101, _sbyte, "")
_sbyte = byte(101)
if (_sbyte != 101) apLogFailInfo("Wrong Value", 101, _sbyte, "")
x = sbyte(_byte)
if (x != 101) apLogFailInfo("Wrong Value", 101, _sbyte, "")
x = sbyte(byte(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _sbyte, "")



apInitScenario("sbyte <-- short")

_short = 127
_sbyte = _short
if (_sbyte != 127) apLogFailInfo("Wrong Value", 127, _sbyte, "")
_sbyte = short(127)
if (_sbyte != 127) apLogFailInfo("Wrong Value", 127, _sbyte, "")
x = sbyte(_short)
if (x != 127) apLogFailInfo("Wrong Value", 127, _sbyte, "")
x = sbyte(short(127))
if (x != 127) apLogFailInfo("Wrong Value", 127, _sbyte, "")

_short = -128
_sbyte = _short
if (_sbyte != -128) apLogFailInfo("Wrong Value", -128, _sbyte, "")
_sbyte = short(-128)
if (_sbyte != -128) apLogFailInfo("Wrong Value", -128, _sbyte, "")
x = sbyte(_short)
if (x != -128) apLogFailInfo("Wrong Value", -128, _sbyte, "")
x = sbyte(short(-128))
if (x != -128) apLogFailInfo("Wrong Value", -128, _sbyte, "")

_short = 101
_sbyte = _short
if (_sbyte != 101) apLogFailInfo("Wrong Value", 101, _sbyte, "")
_sbyte = short(101)
if (_sbyte != 101) apLogFailInfo("Wrong Value", 101, _sbyte, "")
x = sbyte(_short)
if (x != 101) apLogFailInfo("Wrong Value", 101, _sbyte, "")
x = sbyte(short(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _sbyte, "")



apInitScenario("sbyte <-- ushort")

_ushort = 127
_sbyte = _ushort
if (_sbyte != 127) apLogFailInfo("Wrong Value", 127, _sbyte, "")
_sbyte = ushort(127)
if (_sbyte != 127) apLogFailInfo("Wrong Value", 127, _sbyte, "")
x = sbyte(_ushort)
if (x != 127) apLogFailInfo("Wrong Value", 127, _sbyte, "")
x = sbyte(ushort(127))
if (x != 127) apLogFailInfo("Wrong Value", 127, _sbyte, "")

_ushort = 0
_sbyte = _ushort
if (_sbyte != 0) apLogFailInfo("Wrong Value", 0, _sbyte, "")
_sbyte = ushort(0)
if (_sbyte != 0) apLogFailInfo("Wrong Value", 0, _sbyte, "")
x = sbyte(_ushort)
if (x != 0) apLogFailInfo("Wrong Value", 0, _sbyte, "")
x = sbyte(ushort(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _sbyte, "")

_ushort = 101
_sbyte = _ushort
if (_sbyte != 101) apLogFailInfo("Wrong Value", 101, _sbyte, "")
_sbyte = ushort(101)
if (_sbyte != 101) apLogFailInfo("Wrong Value", 101, _sbyte, "")
x = sbyte(_ushort)
if (x != 101) apLogFailInfo("Wrong Value", 101, _sbyte, "")
x = sbyte(ushort(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _sbyte, "")



apInitScenario("sbyte <-- int")

_int = 127
_sbyte = _int
if (_sbyte != 127) apLogFailInfo("Wrong Value", 127, _sbyte, "")
_sbyte = int(127)
if (_sbyte != 127) apLogFailInfo("Wrong Value", 127, _sbyte, "")
x = sbyte(_int)
if (x != 127) apLogFailInfo("Wrong Value", 127, _sbyte, "")
x = sbyte(int(127))
if (x != 127) apLogFailInfo("Wrong Value", 127, _sbyte, "")

_int = -128
_sbyte = _int
if (_sbyte != -128) apLogFailInfo("Wrong Value", -128, _sbyte, "")
_sbyte = int(-128)
if (_sbyte != -128) apLogFailInfo("Wrong Value", -128, _sbyte, "")
x = sbyte(_int)
if (x != -128) apLogFailInfo("Wrong Value", -128, _sbyte, "")
x = sbyte(int(-128))
if (x != -128) apLogFailInfo("Wrong Value", -128, _sbyte, "")

_int = 101
_sbyte = _int
if (_sbyte != 101) apLogFailInfo("Wrong Value", 101, _sbyte, "")
_sbyte = int(101)
if (_sbyte != 101) apLogFailInfo("Wrong Value", 101, _sbyte, "")
x = sbyte(_int)
if (x != 101) apLogFailInfo("Wrong Value", 101, _sbyte, "")
x = sbyte(int(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _sbyte, "")



apInitScenario("sbyte <-- uint")

_uint = 127
_sbyte = _uint
if (_sbyte != 127) apLogFailInfo("Wrong Value", 127, _sbyte, "")
_sbyte = uint(127)
if (_sbyte != 127) apLogFailInfo("Wrong Value", 127, _sbyte, "")
x = sbyte(_uint)
if (x != 127) apLogFailInfo("Wrong Value", 127, _sbyte, "")
x = sbyte(uint(127))
if (x != 127) apLogFailInfo("Wrong Value", 127, _sbyte, "")

_uint = 0
_sbyte = _uint
if (_sbyte != 0) apLogFailInfo("Wrong Value", 0, _sbyte, "")
_sbyte = uint(0)
if (_sbyte != 0) apLogFailInfo("Wrong Value", 0, _sbyte, "")
x = sbyte(_uint)
if (x != 0) apLogFailInfo("Wrong Value", 0, _sbyte, "")
x = sbyte(uint(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _sbyte, "")

_uint = 101
_sbyte = _uint
if (_sbyte != 101) apLogFailInfo("Wrong Value", 101, _sbyte, "")
_sbyte = uint(101)
if (_sbyte != 101) apLogFailInfo("Wrong Value", 101, _sbyte, "")
x = sbyte(_uint)
if (x != 101) apLogFailInfo("Wrong Value", 101, _sbyte, "")
x = sbyte(uint(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _sbyte, "")



apInitScenario("sbyte <-- long")

_long = 127
_sbyte = _long
if (_sbyte != 127) apLogFailInfo("Wrong Value", 127, _sbyte, "")
_sbyte = long(127)
if (_sbyte != 127) apLogFailInfo("Wrong Value", 127, _sbyte, "")
x = sbyte(_long)
if (x != 127) apLogFailInfo("Wrong Value", 127, _sbyte, "")
x = sbyte(long(127))
if (x != 127) apLogFailInfo("Wrong Value", 127, _sbyte, "")

_long = -128
_sbyte = _long
if (_sbyte != -128) apLogFailInfo("Wrong Value", -128, _sbyte, "")
_sbyte = long(-128)
if (_sbyte != -128) apLogFailInfo("Wrong Value", -128, _sbyte, "")
x = sbyte(_long)
if (x != -128) apLogFailInfo("Wrong Value", -128, _sbyte, "")
x = sbyte(long(-128))
if (x != -128) apLogFailInfo("Wrong Value", -128, _sbyte, "")

_long = 101
_sbyte = _long
if (_sbyte != 101) apLogFailInfo("Wrong Value", 101, _sbyte, "")
_sbyte = long(101)
if (_sbyte != 101) apLogFailInfo("Wrong Value", 101, _sbyte, "")
x = sbyte(_long)
if (x != 101) apLogFailInfo("Wrong Value", 101, _sbyte, "")
x = sbyte(long(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _sbyte, "")



apInitScenario("sbyte <-- ulong")

_ulong = 127
_sbyte = _ulong
if (_sbyte != 127) apLogFailInfo("Wrong Value", 127, _sbyte, "")
_sbyte = ulong(127)
if (_sbyte != 127) apLogFailInfo("Wrong Value", 127, _sbyte, "")
x = sbyte(_ulong)
if (x != 127) apLogFailInfo("Wrong Value", 127, _sbyte, "")
x = sbyte(ulong(127))
if (x != 127) apLogFailInfo("Wrong Value", 127, _sbyte, "")

_ulong = 0
_sbyte = _ulong
if (_sbyte != 0) apLogFailInfo("Wrong Value", 0, _sbyte, "")
_sbyte = ulong(0)
if (_sbyte != 0) apLogFailInfo("Wrong Value", 0, _sbyte, "")
x = sbyte(_ulong)
if (x != 0) apLogFailInfo("Wrong Value", 0, _sbyte, "")
x = sbyte(ulong(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _sbyte, "")

_ulong = 101
_sbyte = _ulong
if (_sbyte != 101) apLogFailInfo("Wrong Value", 101, _sbyte, "")
_sbyte = ulong(101)
if (_sbyte != 101) apLogFailInfo("Wrong Value", 101, _sbyte, "")
x = sbyte(_ulong)
if (x != 101) apLogFailInfo("Wrong Value", 101, _sbyte, "")
x = sbyte(ulong(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _sbyte, "")



apInitScenario("sbyte <-- char")
/*  PGMTODOPGM
_char = 127
_sbyte = _char
if (_sbyte != 127) apLogFailInfo("Wrong Value", 127, _sbyte, "")
_sbyte = char(127)
if (_sbyte != 127) apLogFailInfo("Wrong Value", 127, _sbyte, "")
x = sbyte(_char)
if (x != 127) apLogFailInfo("Wrong Value", 127, _sbyte, "")
x = sbyte(char(127))
if (x != 127) apLogFailInfo("Wrong Value", 127, _sbyte, "")

_char = 0
_sbyte = _char
if (_sbyte != 0) apLogFailInfo("Wrong Value", 0, _sbyte, "")
_sbyte = char(0)
if (_sbyte != 0) apLogFailInfo("Wrong Value", 0, _sbyte, "")
x = sbyte(_char)
if (x != 0) apLogFailInfo("Wrong Value", 0, _sbyte, "")
x = sbyte(char(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _sbyte, "")

_char = 101
_sbyte = _char
if (_sbyte != 101) apLogFailInfo("Wrong Value", 101, _sbyte, "")
_sbyte = char(101)
if (_sbyte != 101) apLogFailInfo("Wrong Value", 101, _sbyte, "")
x = sbyte(_char)
if (x != 101) apLogFailInfo("Wrong Value", 101, _sbyte, "")
x = sbyte(char(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _sbyte, "")
*/


apInitScenario("sbyte <-- decimal")

_decimal = 127
_sbyte = _decimal
if (_sbyte != 127) apLogFailInfo("Wrong Value", 127, _sbyte, "")
_sbyte = decimal(127)
if (_sbyte != 127) apLogFailInfo("Wrong Value", 127, _sbyte, "")
x = sbyte(_decimal)
if (x != 127) apLogFailInfo("Wrong Value", 127, _sbyte, "")
x = sbyte(decimal(127))
if (x != 127) apLogFailInfo("Wrong Value", 127, _sbyte, "")

_decimal = -128
_sbyte = _decimal
if (_sbyte != -128) apLogFailInfo("Wrong Value", -128, _sbyte, "")
_sbyte = decimal(-128)
if (_sbyte != -128) apLogFailInfo("Wrong Value", -128, _sbyte, "")
x = sbyte(_decimal)
if (x != -128) apLogFailInfo("Wrong Value", -128, _sbyte, "")
x = sbyte(decimal(-128))
if (x != -128) apLogFailInfo("Wrong Value", -128, _sbyte, "")

_decimal = 101
_sbyte = _decimal
if (_sbyte != 101) apLogFailInfo("Wrong Value", 101, _sbyte, "")
_sbyte = decimal(101)
if (_sbyte != 101) apLogFailInfo("Wrong Value", 101, _sbyte, "")
x = sbyte(_decimal)
if (x != 101) apLogFailInfo("Wrong Value", 101, _sbyte, "")
x = sbyte(decimal(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _sbyte, "")



apInitScenario("byte <-- sbyte")

_sbyte = 127
_byte = _sbyte
if (_byte != 127) apLogFailInfo("Wrong Value", 127, _byte, "")
_byte = sbyte(127)
if (_byte != 127) apLogFailInfo("Wrong Value", 127, _byte, "")
x = byte(_sbyte)
if (x != 127) apLogFailInfo("Wrong Value", 127, _byte, "")
x = byte(sbyte(127))
if (x != 127) apLogFailInfo("Wrong Value", 127, _byte, "")

_sbyte = 0
_byte = _sbyte
if (_byte != 0) apLogFailInfo("Wrong Value", 0, _byte, "")
_byte = sbyte(0)
if (_byte != 0) apLogFailInfo("Wrong Value", 0, _byte, "")
x = byte(_sbyte)
if (x != 0) apLogFailInfo("Wrong Value", 0, _byte, "")
x = byte(sbyte(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _byte, "")

_sbyte = 101
_byte = _sbyte
if (_byte != 101) apLogFailInfo("Wrong Value", 101, _byte, "")
_byte = sbyte(101)
if (_byte != 101) apLogFailInfo("Wrong Value", 101, _byte, "")
x = byte(_sbyte)
if (x != 101) apLogFailInfo("Wrong Value", 101, _byte, "")
x = byte(sbyte(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _byte, "")



apInitScenario("byte <-- byte")

_byte = 255
_byte = _byte
if (_byte != 255) apLogFailInfo("Wrong Value", 255, _byte, "")
_byte = byte(255)
if (_byte != 255) apLogFailInfo("Wrong Value", 255, _byte, "")
x = byte(_byte)
if (x != 255) apLogFailInfo("Wrong Value", 255, _byte, "")
x = byte(byte(255))
if (x != 255) apLogFailInfo("Wrong Value", 255, _byte, "")

_byte = 0
_byte = _byte
if (_byte != 0) apLogFailInfo("Wrong Value", 0, _byte, "")
_byte = byte(0)
if (_byte != 0) apLogFailInfo("Wrong Value", 0, _byte, "")
x = byte(_byte)
if (x != 0) apLogFailInfo("Wrong Value", 0, _byte, "")
x = byte(byte(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _byte, "")

_byte = 101
_byte = _byte
if (_byte != 101) apLogFailInfo("Wrong Value", 101, _byte, "")
_byte = byte(101)
if (_byte != 101) apLogFailInfo("Wrong Value", 101, _byte, "")
x = byte(_byte)
if (x != 101) apLogFailInfo("Wrong Value", 101, _byte, "")
x = byte(byte(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _byte, "")



apInitScenario("byte <-- short")

_short = 255
_byte = _short
if (_byte != 255) apLogFailInfo("Wrong Value", 255, _byte, "")
_byte = short(255)
if (_byte != 255) apLogFailInfo("Wrong Value", 255, _byte, "")
x = byte(_short)
if (x != 255) apLogFailInfo("Wrong Value", 255, _byte, "")
x = byte(short(255))
if (x != 255) apLogFailInfo("Wrong Value", 255, _byte, "")

_short = 0
_byte = _short
if (_byte != 0) apLogFailInfo("Wrong Value", 0, _byte, "")
_byte = short(0)
if (_byte != 0) apLogFailInfo("Wrong Value", 0, _byte, "")
x = byte(_short)
if (x != 0) apLogFailInfo("Wrong Value", 0, _byte, "")
x = byte(short(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _byte, "")

_short = 101
_byte = _short
if (_byte != 101) apLogFailInfo("Wrong Value", 101, _byte, "")
_byte = short(101)
if (_byte != 101) apLogFailInfo("Wrong Value", 101, _byte, "")
x = byte(_short)
if (x != 101) apLogFailInfo("Wrong Value", 101, _byte, "")
x = byte(short(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _byte, "")



apInitScenario("byte <-- ushort")

_ushort = 255
_byte = _ushort
if (_byte != 255) apLogFailInfo("Wrong Value", 255, _byte, "")
_byte = ushort(255)
if (_byte != 255) apLogFailInfo("Wrong Value", 255, _byte, "")
x = byte(_ushort)
if (x != 255) apLogFailInfo("Wrong Value", 255, _byte, "")
x = byte(ushort(255))
if (x != 255) apLogFailInfo("Wrong Value", 255, _byte, "")

_ushort = 0
_byte = _ushort
if (_byte != 0) apLogFailInfo("Wrong Value", 0, _byte, "")
_byte = ushort(0)
if (_byte != 0) apLogFailInfo("Wrong Value", 0, _byte, "")
x = byte(_ushort)
if (x != 0) apLogFailInfo("Wrong Value", 0, _byte, "")
x = byte(ushort(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _byte, "")

_ushort = 101
_byte = _ushort
if (_byte != 101) apLogFailInfo("Wrong Value", 101, _byte, "")
_byte = ushort(101)
if (_byte != 101) apLogFailInfo("Wrong Value", 101, _byte, "")
x = byte(_ushort)
if (x != 101) apLogFailInfo("Wrong Value", 101, _byte, "")
x = byte(ushort(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _byte, "")



apInitScenario("byte <-- int")

_int = 255
_byte = _int
if (_byte != 255) apLogFailInfo("Wrong Value", 255, _byte, "")
_byte = int(255)
if (_byte != 255) apLogFailInfo("Wrong Value", 255, _byte, "")
x = byte(_int)
if (x != 255) apLogFailInfo("Wrong Value", 255, _byte, "")
x = byte(int(255))
if (x != 255) apLogFailInfo("Wrong Value", 255, _byte, "")

_int = 0
_byte = _int
if (_byte != 0) apLogFailInfo("Wrong Value", 0, _byte, "")
_byte = int(0)
if (_byte != 0) apLogFailInfo("Wrong Value", 0, _byte, "")
x = byte(_int)
if (x != 0) apLogFailInfo("Wrong Value", 0, _byte, "")
x = byte(int(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _byte, "")

_int = 101
_byte = _int
if (_byte != 101) apLogFailInfo("Wrong Value", 101, _byte, "")
_byte = int(101)
if (_byte != 101) apLogFailInfo("Wrong Value", 101, _byte, "")
x = byte(_int)
if (x != 101) apLogFailInfo("Wrong Value", 101, _byte, "")
x = byte(int(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _byte, "")



apInitScenario("byte <-- uint")

_uint = 255
_byte = _uint
if (_byte != 255) apLogFailInfo("Wrong Value", 255, _byte, "")
_byte = uint(255)
if (_byte != 255) apLogFailInfo("Wrong Value", 255, _byte, "")
x = byte(_uint)
if (x != 255) apLogFailInfo("Wrong Value", 255, _byte, "")
x = byte(uint(255))
if (x != 255) apLogFailInfo("Wrong Value", 255, _byte, "")

_uint = 0
_byte = _uint
if (_byte != 0) apLogFailInfo("Wrong Value", 0, _byte, "")
_byte = uint(0)
if (_byte != 0) apLogFailInfo("Wrong Value", 0, _byte, "")
x = byte(_uint)
if (x != 0) apLogFailInfo("Wrong Value", 0, _byte, "")
x = byte(uint(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _byte, "")

_uint = 101
_byte = _uint
if (_byte != 101) apLogFailInfo("Wrong Value", 101, _byte, "")
_byte = uint(101)
if (_byte != 101) apLogFailInfo("Wrong Value", 101, _byte, "")
x = byte(_uint)
if (x != 101) apLogFailInfo("Wrong Value", 101, _byte, "")
x = byte(uint(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _byte, "")



apInitScenario("byte <-- long")

_long = 255
_byte = _long
if (_byte != 255) apLogFailInfo("Wrong Value", 255, _byte, "")
_byte = long(255)
if (_byte != 255) apLogFailInfo("Wrong Value", 255, _byte, "")
x = byte(_long)
if (x != 255) apLogFailInfo("Wrong Value", 255, _byte, "")
x = byte(long(255))
if (x != 255) apLogFailInfo("Wrong Value", 255, _byte, "")

_long = 0
_byte = _long
if (_byte != 0) apLogFailInfo("Wrong Value", 0, _byte, "")
_byte = long(0)
if (_byte != 0) apLogFailInfo("Wrong Value", 0, _byte, "")
x = byte(_long)
if (x != 0) apLogFailInfo("Wrong Value", 0, _byte, "")
x = byte(long(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _byte, "")

_long = 101
_byte = _long
if (_byte != 101) apLogFailInfo("Wrong Value", 101, _byte, "")
_byte = long(101)
if (_byte != 101) apLogFailInfo("Wrong Value", 101, _byte, "")
x = byte(_long)
if (x != 101) apLogFailInfo("Wrong Value", 101, _byte, "")
x = byte(long(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _byte, "")



apInitScenario("byte <-- ulong")

_ulong = 255
_byte = _ulong
if (_byte != 255) apLogFailInfo("Wrong Value", 255, _byte, "")
_byte = ulong(255)
if (_byte != 255) apLogFailInfo("Wrong Value", 255, _byte, "")
x = byte(_ulong)
if (x != 255) apLogFailInfo("Wrong Value", 255, _byte, "")
x = byte(ulong(255))
if (x != 255) apLogFailInfo("Wrong Value", 255, _byte, "")

_ulong = 0
_byte = _ulong
if (_byte != 0) apLogFailInfo("Wrong Value", 0, _byte, "")
_byte = ulong(0)
if (_byte != 0) apLogFailInfo("Wrong Value", 0, _byte, "")
x = byte(_ulong)
if (x != 0) apLogFailInfo("Wrong Value", 0, _byte, "")
x = byte(ulong(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _byte, "")

_ulong = 101
_byte = _ulong
if (_byte != 101) apLogFailInfo("Wrong Value", 101, _byte, "")
_byte = ulong(101)
if (_byte != 101) apLogFailInfo("Wrong Value", 101, _byte, "")
x = byte(_ulong)
if (x != 101) apLogFailInfo("Wrong Value", 101, _byte, "")
x = byte(ulong(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _byte, "")



apInitScenario("byte <-- char")

_char = 255
_byte = _char
if (_byte != 255) apLogFailInfo("Wrong Value", 255, _byte, "")
_byte = char(255)
if (_byte != 255) apLogFailInfo("Wrong Value", 255, _byte, "")
x = byte(_char)
if (x != 255) apLogFailInfo("Wrong Value", 255, _byte, "")
x = byte(char(255))
if (x != 255) apLogFailInfo("Wrong Value", 255, _byte, "")

_char = 0
_byte = _char
if (_byte != 0) apLogFailInfo("Wrong Value", 0, _byte, "")
_byte = char(0)
if (_byte != 0) apLogFailInfo("Wrong Value", 0, _byte, "")
x = byte(_char)
if (x != 0) apLogFailInfo("Wrong Value", 0, _byte, "")
x = byte(char(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _byte, "")

_char = 101
_byte = _char
if (_byte != 101) apLogFailInfo("Wrong Value", 101, _byte, "")
_byte = char(101)
if (_byte != 101) apLogFailInfo("Wrong Value", 101, _byte, "")
x = byte(_char)
if (x != 101) apLogFailInfo("Wrong Value", 101, _byte, "")
x = byte(char(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _byte, "")



apInitScenario("byte <-- decimal")

_decimal = 255
_byte = _decimal
if (_byte != 255) apLogFailInfo("Wrong Value", 255, _byte, "")
_byte = decimal(255)
if (_byte != 255) apLogFailInfo("Wrong Value", 255, _byte, "")
x = byte(_decimal)
if (x != 255) apLogFailInfo("Wrong Value", 255, _byte, "")
x = byte(decimal(255))
if (x != 255) apLogFailInfo("Wrong Value", 255, _byte, "")

_decimal = 0
_byte = _decimal
if (_byte != 0) apLogFailInfo("Wrong Value", 0, _byte, "")
_byte = decimal(0)
if (_byte != 0) apLogFailInfo("Wrong Value", 0, _byte, "")
x = byte(_decimal)
if (x != 0) apLogFailInfo("Wrong Value", 0, _byte, "")
x = byte(decimal(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _byte, "")

_decimal = 101
_byte = _decimal
if (_byte != 101) apLogFailInfo("Wrong Value", 101, _byte, "")
_byte = decimal(101)
if (_byte != 101) apLogFailInfo("Wrong Value", 101, _byte, "")
x = byte(_decimal)
if (x != 101) apLogFailInfo("Wrong Value", 101, _byte, "")
x = byte(decimal(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _byte, "")



apInitScenario("short <-- sbyte")

_sbyte = 127
_short = _sbyte
if (_short != 127) apLogFailInfo("Wrong Value", 127, _short, "")
_short = sbyte(127)
if (_short != 127) apLogFailInfo("Wrong Value", 127, _short, "")
x = short(_sbyte)
if (x != 127) apLogFailInfo("Wrong Value", 127, _short, "")
x = short(sbyte(127))
if (x != 127) apLogFailInfo("Wrong Value", 127, _short, "")

_sbyte = -128
_short = _sbyte
if (_short != -128) apLogFailInfo("Wrong Value", -128, _short, "")
_short = sbyte(-128)
if (_short != -128) apLogFailInfo("Wrong Value", -128, _short, "")
x = short(_sbyte)
if (x != -128) apLogFailInfo("Wrong Value", -128, _short, "")
x = short(sbyte(-128))
if (x != -128) apLogFailInfo("Wrong Value", -128, _short, "")

_sbyte = 101
_short = _sbyte
if (_short != 101) apLogFailInfo("Wrong Value", 101, _short, "")
_short = sbyte(101)
if (_short != 101) apLogFailInfo("Wrong Value", 101, _short, "")
x = short(_sbyte)
if (x != 101) apLogFailInfo("Wrong Value", 101, _short, "")
x = short(sbyte(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _short, "")



apInitScenario("short <-- byte")

_byte = 255
_short = _byte
if (_short != 255) apLogFailInfo("Wrong Value", 255, _short, "")
_short = byte(255)
if (_short != 255) apLogFailInfo("Wrong Value", 255, _short, "")
x = short(_byte)
if (x != 255) apLogFailInfo("Wrong Value", 255, _short, "")
x = short(byte(255))
if (x != 255) apLogFailInfo("Wrong Value", 255, _short, "")

_byte = 0
_short = _byte
if (_short != 0) apLogFailInfo("Wrong Value", 0, _short, "")
_short = byte(0)
if (_short != 0) apLogFailInfo("Wrong Value", 0, _short, "")
x = short(_byte)
if (x != 0) apLogFailInfo("Wrong Value", 0, _short, "")
x = short(byte(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _short, "")

_byte = 101
_short = _byte
if (_short != 101) apLogFailInfo("Wrong Value", 101, _short, "")
_short = byte(101)
if (_short != 101) apLogFailInfo("Wrong Value", 101, _short, "")
x = short(_byte)
if (x != 101) apLogFailInfo("Wrong Value", 101, _short, "")
x = short(byte(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _short, "")



apInitScenario("short <-- short")

_short = 32767
_short = _short
if (_short != 32767) apLogFailInfo("Wrong Value", 32767, _short, "")
_short = short(32767)
if (_short != 32767) apLogFailInfo("Wrong Value", 32767, _short, "")
x = short(_short)
if (x != 32767) apLogFailInfo("Wrong Value", 32767, _short, "")
x = short(short(32767))
if (x != 32767) apLogFailInfo("Wrong Value", 32767, _short, "")

_short = -32768
_short = _short
if (_short != -32768) apLogFailInfo("Wrong Value", -32768, _short, "")
_short = short(-32768)
if (_short != -32768) apLogFailInfo("Wrong Value", -32768, _short, "")
x = short(_short)
if (x != -32768) apLogFailInfo("Wrong Value", -32768, _short, "")
x = short(short(-32768))
if (x != -32768) apLogFailInfo("Wrong Value", -32768, _short, "")

_short = 101
_short = _short
if (_short != 101) apLogFailInfo("Wrong Value", 101, _short, "")
_short = short(101)
if (_short != 101) apLogFailInfo("Wrong Value", 101, _short, "")
x = short(_short)
if (x != 101) apLogFailInfo("Wrong Value", 101, _short, "")
x = short(short(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _short, "")



apInitScenario("short <-- ushort")

_ushort = 32767
_short = _ushort
if (_short != 32767) apLogFailInfo("Wrong Value", 32767, _short, "")
_short = ushort(32767)
if (_short != 32767) apLogFailInfo("Wrong Value", 32767, _short, "")
x = short(_ushort)
if (x != 32767) apLogFailInfo("Wrong Value", 32767, _short, "")
x = short(ushort(32767))
if (x != 32767) apLogFailInfo("Wrong Value", 32767, _short, "")

_ushort = 0
_short = _ushort
if (_short != 0) apLogFailInfo("Wrong Value", 0, _short, "")
_short = ushort(0)
if (_short != 0) apLogFailInfo("Wrong Value", 0, _short, "")
x = short(_ushort)
if (x != 0) apLogFailInfo("Wrong Value", 0, _short, "")
x = short(ushort(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _short, "")

_ushort = 101
_short = _ushort
if (_short != 101) apLogFailInfo("Wrong Value", 101, _short, "")
_short = ushort(101)
if (_short != 101) apLogFailInfo("Wrong Value", 101, _short, "")
x = short(_ushort)
if (x != 101) apLogFailInfo("Wrong Value", 101, _short, "")
x = short(ushort(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _short, "")



apInitScenario("short <-- int")

_int = 32767
_short = _int
if (_short != 32767) apLogFailInfo("Wrong Value", 32767, _short, "")
_short = int(32767)
if (_short != 32767) apLogFailInfo("Wrong Value", 32767, _short, "")
x = short(_int)
if (x != 32767) apLogFailInfo("Wrong Value", 32767, _short, "")
x = short(int(32767))
if (x != 32767) apLogFailInfo("Wrong Value", 32767, _short, "")

_int = -32768
_short = _int
if (_short != -32768) apLogFailInfo("Wrong Value", -32768, _short, "")
_short = int(-32768)
if (_short != -32768) apLogFailInfo("Wrong Value", -32768, _short, "")
x = short(_int)
if (x != -32768) apLogFailInfo("Wrong Value", -32768, _short, "")
x = short(int(-32768))
if (x != -32768) apLogFailInfo("Wrong Value", -32768, _short, "")

_int = 101
_short = _int
if (_short != 101) apLogFailInfo("Wrong Value", 101, _short, "")
_short = int(101)
if (_short != 101) apLogFailInfo("Wrong Value", 101, _short, "")
x = short(_int)
if (x != 101) apLogFailInfo("Wrong Value", 101, _short, "")
x = short(int(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _short, "")



apInitScenario("short <-- uint")

_uint = 32767
_short = _uint
if (_short != 32767) apLogFailInfo("Wrong Value", 32767, _short, "")
_short = uint(32767)
if (_short != 32767) apLogFailInfo("Wrong Value", 32767, _short, "")
x = short(_uint)
if (x != 32767) apLogFailInfo("Wrong Value", 32767, _short, "")
x = short(uint(32767))
if (x != 32767) apLogFailInfo("Wrong Value", 32767, _short, "")

_uint = 0
_short = _uint
if (_short != 0) apLogFailInfo("Wrong Value", 0, _short, "")
_short = uint(0)
if (_short != 0) apLogFailInfo("Wrong Value", 0, _short, "")
x = short(_uint)
if (x != 0) apLogFailInfo("Wrong Value", 0, _short, "")
x = short(uint(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _short, "")

_uint = 101
_short = _uint
if (_short != 101) apLogFailInfo("Wrong Value", 101, _short, "")
_short = uint(101)
if (_short != 101) apLogFailInfo("Wrong Value", 101, _short, "")
x = short(_uint)
if (x != 101) apLogFailInfo("Wrong Value", 101, _short, "")
x = short(uint(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _short, "")



apInitScenario("short <-- long")

_long = 32767
_short = _long
if (_short != 32767) apLogFailInfo("Wrong Value", 32767, _short, "")
_short = long(32767)
if (_short != 32767) apLogFailInfo("Wrong Value", 32767, _short, "")
x = short(_long)
if (x != 32767) apLogFailInfo("Wrong Value", 32767, _short, "")
x = short(long(32767))
if (x != 32767) apLogFailInfo("Wrong Value", 32767, _short, "")

_long = -32768
_short = _long
if (_short != -32768) apLogFailInfo("Wrong Value", -32768, _short, "")
_short = long(-32768)
if (_short != -32768) apLogFailInfo("Wrong Value", -32768, _short, "")
x = short(_long)
if (x != -32768) apLogFailInfo("Wrong Value", -32768, _short, "")
x = short(long(-32768))
if (x != -32768) apLogFailInfo("Wrong Value", -32768, _short, "")

_long = 101
_short = _long
if (_short != 101) apLogFailInfo("Wrong Value", 101, _short, "")
_short = long(101)
if (_short != 101) apLogFailInfo("Wrong Value", 101, _short, "")
x = short(_long)
if (x != 101) apLogFailInfo("Wrong Value", 101, _short, "")
x = short(long(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _short, "")



apInitScenario("short <-- ulong")

_ulong = 32767
_short = _ulong
if (_short != 32767) apLogFailInfo("Wrong Value", 32767, _short, "")
_short = ulong(32767)
if (_short != 32767) apLogFailInfo("Wrong Value", 32767, _short, "")
x = short(_ulong)
if (x != 32767) apLogFailInfo("Wrong Value", 32767, _short, "")
x = short(ulong(32767))
if (x != 32767) apLogFailInfo("Wrong Value", 32767, _short, "")

_ulong = 0
_short = _ulong
if (_short != 0) apLogFailInfo("Wrong Value", 0, _short, "")
_short = ulong(0)
if (_short != 0) apLogFailInfo("Wrong Value", 0, _short, "")
x = short(_ulong)
if (x != 0) apLogFailInfo("Wrong Value", 0, _short, "")
x = short(ulong(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _short, "")

_ulong = 101
_short = _ulong
if (_short != 101) apLogFailInfo("Wrong Value", 101, _short, "")
_short = ulong(101)
if (_short != 101) apLogFailInfo("Wrong Value", 101, _short, "")
x = short(_ulong)
if (x != 101) apLogFailInfo("Wrong Value", 101, _short, "")
x = short(ulong(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _short, "")



apInitScenario("short <-- char")
/*  PGMTODOPGM
_char = 32767
_short = _char
if (_short != 32767) apLogFailInfo("Wrong Value", 32767, _short, "")
_short = char(32767)
if (_short != 32767) apLogFailInfo("Wrong Value", 32767, _short, "")
x = short(_char)
if (x != 32767) apLogFailInfo("Wrong Value", 32767, _short, "")
x = short(char(32767))
if (x != 32767) apLogFailInfo("Wrong Value", 32767, _short, "")

_char = 0
_short = _char
if (_short != 0) apLogFailInfo("Wrong Value", 0, _short, "")
_short = char(0)
if (_short != 0) apLogFailInfo("Wrong Value", 0, _short, "")
x = short(_char)
if (x != 0) apLogFailInfo("Wrong Value", 0, _short, "")
x = short(char(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _short, "")

_char = 101
_short = _char
if (_short != 101) apLogFailInfo("Wrong Value", 101, _short, "")
_short = char(101)
if (_short != 101) apLogFailInfo("Wrong Value", 101, _short, "")
x = short(_char)
if (x != 101) apLogFailInfo("Wrong Value", 101, _short, "")
x = short(char(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _short, "")
*/


apInitScenario("short <-- decimal")

_decimal = 32767
_short = _decimal
if (_short != 32767) apLogFailInfo("Wrong Value", 32767, _short, "")
_short = decimal(32767)
if (_short != 32767) apLogFailInfo("Wrong Value", 32767, _short, "")
x = short(_decimal)
if (x != 32767) apLogFailInfo("Wrong Value", 32767, _short, "")
x = short(decimal(32767))
if (x != 32767) apLogFailInfo("Wrong Value", 32767, _short, "")

_decimal = -32768
_short = _decimal
if (_short != -32768) apLogFailInfo("Wrong Value", -32768, _short, "")
_short = decimal(-32768)
if (_short != -32768) apLogFailInfo("Wrong Value", -32768, _short, "")
x = short(_decimal)
if (x != -32768) apLogFailInfo("Wrong Value", -32768, _short, "")
x = short(decimal(-32768))
if (x != -32768) apLogFailInfo("Wrong Value", -32768, _short, "")

_decimal = 101
_short = _decimal
if (_short != 101) apLogFailInfo("Wrong Value", 101, _short, "")
_short = decimal(101)
if (_short != 101) apLogFailInfo("Wrong Value", 101, _short, "")
x = short(_decimal)
if (x != 101) apLogFailInfo("Wrong Value", 101, _short, "")
x = short(decimal(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _short, "")



apInitScenario("ushort <-- sbyte")

_sbyte = 127
_ushort = _sbyte
if (_ushort != 127) apLogFailInfo("Wrong Value", 127, _ushort, "")
_ushort = sbyte(127)
if (_ushort != 127) apLogFailInfo("Wrong Value", 127, _ushort, "")
x = ushort(_sbyte)
if (x != 127) apLogFailInfo("Wrong Value", 127, _ushort, "")
x = ushort(sbyte(127))
if (x != 127) apLogFailInfo("Wrong Value", 127, _ushort, "")

_sbyte = 0
_ushort = _sbyte
if (_ushort != 0) apLogFailInfo("Wrong Value", 0, _ushort, "")
_ushort = sbyte(0)
if (_ushort != 0) apLogFailInfo("Wrong Value", 0, _ushort, "")
x = ushort(_sbyte)
if (x != 0) apLogFailInfo("Wrong Value", 0, _ushort, "")
x = ushort(sbyte(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _ushort, "")

_sbyte = 101
_ushort = _sbyte
if (_ushort != 101) apLogFailInfo("Wrong Value", 101, _ushort, "")
_ushort = sbyte(101)
if (_ushort != 101) apLogFailInfo("Wrong Value", 101, _ushort, "")
x = ushort(_sbyte)
if (x != 101) apLogFailInfo("Wrong Value", 101, _ushort, "")
x = ushort(sbyte(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _ushort, "")



apInitScenario("ushort <-- byte")

_byte = 255
_ushort = _byte
if (_ushort != 255) apLogFailInfo("Wrong Value", 255, _ushort, "")
_ushort = byte(255)
if (_ushort != 255) apLogFailInfo("Wrong Value", 255, _ushort, "")
x = ushort(_byte)
if (x != 255) apLogFailInfo("Wrong Value", 255, _ushort, "")
x = ushort(byte(255))
if (x != 255) apLogFailInfo("Wrong Value", 255, _ushort, "")

_byte = 0
_ushort = _byte
if (_ushort != 0) apLogFailInfo("Wrong Value", 0, _ushort, "")
_ushort = byte(0)
if (_ushort != 0) apLogFailInfo("Wrong Value", 0, _ushort, "")
x = ushort(_byte)
if (x != 0) apLogFailInfo("Wrong Value", 0, _ushort, "")
x = ushort(byte(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _ushort, "")

_byte = 101
_ushort = _byte
if (_ushort != 101) apLogFailInfo("Wrong Value", 101, _ushort, "")
_ushort = byte(101)
if (_ushort != 101) apLogFailInfo("Wrong Value", 101, _ushort, "")
x = ushort(_byte)
if (x != 101) apLogFailInfo("Wrong Value", 101, _ushort, "")
x = ushort(byte(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _ushort, "")



apInitScenario("ushort <-- short")

_short = 32767
_ushort = _short
if (_ushort != 32767) apLogFailInfo("Wrong Value", 32767, _ushort, "")
_ushort = short(32767)
if (_ushort != 32767) apLogFailInfo("Wrong Value", 32767, _ushort, "")
x = ushort(_short)
if (x != 32767) apLogFailInfo("Wrong Value", 32767, _ushort, "")
x = ushort(short(32767))
if (x != 32767) apLogFailInfo("Wrong Value", 32767, _ushort, "")

_short = 0
_ushort = _short
if (_ushort != 0) apLogFailInfo("Wrong Value", 0, _ushort, "")
_ushort = short(0)
if (_ushort != 0) apLogFailInfo("Wrong Value", 0, _ushort, "")
x = ushort(_short)
if (x != 0) apLogFailInfo("Wrong Value", 0, _ushort, "")
x = ushort(short(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _ushort, "")

_short = 101
_ushort = _short
if (_ushort != 101) apLogFailInfo("Wrong Value", 101, _ushort, "")
_ushort = short(101)
if (_ushort != 101) apLogFailInfo("Wrong Value", 101, _ushort, "")
x = ushort(_short)
if (x != 101) apLogFailInfo("Wrong Value", 101, _ushort, "")
x = ushort(short(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _ushort, "")



apInitScenario("ushort <-- ushort")

_ushort = 65535
_ushort = _ushort
if (_ushort != 65535) apLogFailInfo("Wrong Value", 65535, _ushort, "")
_ushort = ushort(65535)
if (_ushort != 65535) apLogFailInfo("Wrong Value", 65535, _ushort, "")
x = ushort(_ushort)
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _ushort, "")
x = ushort(ushort(65535))
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _ushort, "")

_ushort = 0
_ushort = _ushort
if (_ushort != 0) apLogFailInfo("Wrong Value", 0, _ushort, "")
_ushort = ushort(0)
if (_ushort != 0) apLogFailInfo("Wrong Value", 0, _ushort, "")
x = ushort(_ushort)
if (x != 0) apLogFailInfo("Wrong Value", 0, _ushort, "")
x = ushort(ushort(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _ushort, "")

_ushort = 101
_ushort = _ushort
if (_ushort != 101) apLogFailInfo("Wrong Value", 101, _ushort, "")
_ushort = ushort(101)
if (_ushort != 101) apLogFailInfo("Wrong Value", 101, _ushort, "")
x = ushort(_ushort)
if (x != 101) apLogFailInfo("Wrong Value", 101, _ushort, "")
x = ushort(ushort(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _ushort, "")



apInitScenario("ushort <-- int")

_int = 65535
_ushort = _int
if (_ushort != 65535) apLogFailInfo("Wrong Value", 65535, _ushort, "")
_ushort = int(65535)
if (_ushort != 65535) apLogFailInfo("Wrong Value", 65535, _ushort, "")
x = ushort(_int)
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _ushort, "")
x = ushort(int(65535))
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _ushort, "")

_int = 0
_ushort = _int
if (_ushort != 0) apLogFailInfo("Wrong Value", 0, _ushort, "")
_ushort = int(0)
if (_ushort != 0) apLogFailInfo("Wrong Value", 0, _ushort, "")
x = ushort(_int)
if (x != 0) apLogFailInfo("Wrong Value", 0, _ushort, "")
x = ushort(int(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _ushort, "")

_int = 101
_ushort = _int
if (_ushort != 101) apLogFailInfo("Wrong Value", 101, _ushort, "")
_ushort = int(101)
if (_ushort != 101) apLogFailInfo("Wrong Value", 101, _ushort, "")
x = ushort(_int)
if (x != 101) apLogFailInfo("Wrong Value", 101, _ushort, "")
x = ushort(int(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _ushort, "")



apInitScenario("ushort <-- uint")

_uint = 65535
_ushort = _uint
if (_ushort != 65535) apLogFailInfo("Wrong Value", 65535, _ushort, "")
_ushort = uint(65535)
if (_ushort != 65535) apLogFailInfo("Wrong Value", 65535, _ushort, "")
x = ushort(_uint)
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _ushort, "")
x = ushort(uint(65535))
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _ushort, "")

_uint = 0
_ushort = _uint
if (_ushort != 0) apLogFailInfo("Wrong Value", 0, _ushort, "")
_ushort = uint(0)
if (_ushort != 0) apLogFailInfo("Wrong Value", 0, _ushort, "")
x = ushort(_uint)
if (x != 0) apLogFailInfo("Wrong Value", 0, _ushort, "")
x = ushort(uint(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _ushort, "")

_uint = 101
_ushort = _uint
if (_ushort != 101) apLogFailInfo("Wrong Value", 101, _ushort, "")
_ushort = uint(101)
if (_ushort != 101) apLogFailInfo("Wrong Value", 101, _ushort, "")
x = ushort(_uint)
if (x != 101) apLogFailInfo("Wrong Value", 101, _ushort, "")
x = ushort(uint(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _ushort, "")



apInitScenario("ushort <-- long")

_long = 65535
_ushort = _long
if (_ushort != 65535) apLogFailInfo("Wrong Value", 65535, _ushort, "")
_ushort = long(65535)
if (_ushort != 65535) apLogFailInfo("Wrong Value", 65535, _ushort, "")
x = ushort(_long)
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _ushort, "")
x = ushort(long(65535))
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _ushort, "")

_long = 0
_ushort = _long
if (_ushort != 0) apLogFailInfo("Wrong Value", 0, _ushort, "")
_ushort = long(0)
if (_ushort != 0) apLogFailInfo("Wrong Value", 0, _ushort, "")
x = ushort(_long)
if (x != 0) apLogFailInfo("Wrong Value", 0, _ushort, "")
x = ushort(long(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _ushort, "")

_long = 101
_ushort = _long
if (_ushort != 101) apLogFailInfo("Wrong Value", 101, _ushort, "")
_ushort = long(101)
if (_ushort != 101) apLogFailInfo("Wrong Value", 101, _ushort, "")
x = ushort(_long)
if (x != 101) apLogFailInfo("Wrong Value", 101, _ushort, "")
x = ushort(long(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _ushort, "")



apInitScenario("ushort <-- ulong")

_ulong = 65535
_ushort = _ulong
if (_ushort != 65535) apLogFailInfo("Wrong Value", 65535, _ushort, "")
_ushort = ulong(65535)
if (_ushort != 65535) apLogFailInfo("Wrong Value", 65535, _ushort, "")
x = ushort(_ulong)
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _ushort, "")
x = ushort(ulong(65535))
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _ushort, "")

_ulong = 0
_ushort = _ulong
if (_ushort != 0) apLogFailInfo("Wrong Value", 0, _ushort, "")
_ushort = ulong(0)
if (_ushort != 0) apLogFailInfo("Wrong Value", 0, _ushort, "")
x = ushort(_ulong)
if (x != 0) apLogFailInfo("Wrong Value", 0, _ushort, "")
x = ushort(ulong(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _ushort, "")

_ulong = 101
_ushort = _ulong
if (_ushort != 101) apLogFailInfo("Wrong Value", 101, _ushort, "")
_ushort = ulong(101)
if (_ushort != 101) apLogFailInfo("Wrong Value", 101, _ushort, "")
x = ushort(_ulong)
if (x != 101) apLogFailInfo("Wrong Value", 101, _ushort, "")
x = ushort(ulong(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _ushort, "")



apInitScenario("ushort <-- char")

_char = 65535
_ushort = _char
if (_ushort != 65535) apLogFailInfo("Wrong Value", 65535, _ushort, "")
_ushort = char(65535)
if (_ushort != 65535) apLogFailInfo("Wrong Value", 65535, _ushort, "")
x = ushort(_char)
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _ushort, "")
x = ushort(char(65535))
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _ushort, "")

_char = 0
_ushort = _char
if (_ushort != 0) apLogFailInfo("Wrong Value", 0, _ushort, "")
_ushort = char(0)
if (_ushort != 0) apLogFailInfo("Wrong Value", 0, _ushort, "")
x = ushort(_char)
if (x != 0) apLogFailInfo("Wrong Value", 0, _ushort, "")
x = ushort(char(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _ushort, "")

_char = 101
_ushort = _char
if (_ushort != 101) apLogFailInfo("Wrong Value", 101, _ushort, "")
_ushort = char(101)
if (_ushort != 101) apLogFailInfo("Wrong Value", 101, _ushort, "")
x = ushort(_char)
if (x != 101) apLogFailInfo("Wrong Value", 101, _ushort, "")
x = ushort(char(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _ushort, "")



apInitScenario("ushort <-- decimal")

_decimal = 65535
_ushort = _decimal
if (_ushort != 65535) apLogFailInfo("Wrong Value", 65535, _ushort, "")
_ushort = decimal(65535)
if (_ushort != 65535) apLogFailInfo("Wrong Value", 65535, _ushort, "")
x = ushort(_decimal)
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _ushort, "")
x = ushort(decimal(65535))
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _ushort, "")

_decimal = 0
_ushort = _decimal
if (_ushort != 0) apLogFailInfo("Wrong Value", 0, _ushort, "")
_ushort = decimal(0)
if (_ushort != 0) apLogFailInfo("Wrong Value", 0, _ushort, "")
x = ushort(_decimal)
if (x != 0) apLogFailInfo("Wrong Value", 0, _ushort, "")
x = ushort(decimal(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _ushort, "")

_decimal = 101
_ushort = _decimal
if (_ushort != 101) apLogFailInfo("Wrong Value", 101, _ushort, "")
_ushort = decimal(101)
if (_ushort != 101) apLogFailInfo("Wrong Value", 101, _ushort, "")
x = ushort(_decimal)
if (x != 101) apLogFailInfo("Wrong Value", 101, _ushort, "")
x = ushort(decimal(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _ushort, "")



apInitScenario("int <-- sbyte")

_sbyte = 127
_int = _sbyte
if (_int != 127) apLogFailInfo("Wrong Value", 127, _int, "")
_int = sbyte(127)
if (_int != 127) apLogFailInfo("Wrong Value", 127, _int, "")
x = int(_sbyte)
if (x != 127) apLogFailInfo("Wrong Value", 127, _int, "")
x = int(sbyte(127))
if (x != 127) apLogFailInfo("Wrong Value", 127, _int, "")

_sbyte = -128
_int = _sbyte
if (_int != -128) apLogFailInfo("Wrong Value", -128, _int, "")
_int = sbyte(-128)
if (_int != -128) apLogFailInfo("Wrong Value", -128, _int, "")
x = int(_sbyte)
if (x != -128) apLogFailInfo("Wrong Value", -128, _int, "")
x = int(sbyte(-128))
if (x != -128) apLogFailInfo("Wrong Value", -128, _int, "")

_sbyte = 101
_int = _sbyte
if (_int != 101) apLogFailInfo("Wrong Value", 101, _int, "")
_int = sbyte(101)
if (_int != 101) apLogFailInfo("Wrong Value", 101, _int, "")
x = int(_sbyte)
if (x != 101) apLogFailInfo("Wrong Value", 101, _int, "")
x = int(sbyte(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _int, "")



apInitScenario("int <-- byte")

_byte = 255
_int = _byte
if (_int != 255) apLogFailInfo("Wrong Value", 255, _int, "")
_int = byte(255)
if (_int != 255) apLogFailInfo("Wrong Value", 255, _int, "")
x = int(_byte)
if (x != 255) apLogFailInfo("Wrong Value", 255, _int, "")
x = int(byte(255))
if (x != 255) apLogFailInfo("Wrong Value", 255, _int, "")

_byte = 0
_int = _byte
if (_int != 0) apLogFailInfo("Wrong Value", 0, _int, "")
_int = byte(0)
if (_int != 0) apLogFailInfo("Wrong Value", 0, _int, "")
x = int(_byte)
if (x != 0) apLogFailInfo("Wrong Value", 0, _int, "")
x = int(byte(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _int, "")

_byte = 101
_int = _byte
if (_int != 101) apLogFailInfo("Wrong Value", 101, _int, "")
_int = byte(101)
if (_int != 101) apLogFailInfo("Wrong Value", 101, _int, "")
x = int(_byte)
if (x != 101) apLogFailInfo("Wrong Value", 101, _int, "")
x = int(byte(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _int, "")



apInitScenario("int <-- short")

_short = 32767
_int = _short
if (_int != 32767) apLogFailInfo("Wrong Value", 32767, _int, "")
_int = short(32767)
if (_int != 32767) apLogFailInfo("Wrong Value", 32767, _int, "")
x = int(_short)
if (x != 32767) apLogFailInfo("Wrong Value", 32767, _int, "")
x = int(short(32767))
if (x != 32767) apLogFailInfo("Wrong Value", 32767, _int, "")

_short = -32768
_int = _short
if (_int != -32768) apLogFailInfo("Wrong Value", -32768, _int, "")
_int = short(-32768)
if (_int != -32768) apLogFailInfo("Wrong Value", -32768, _int, "")
x = int(_short)
if (x != -32768) apLogFailInfo("Wrong Value", -32768, _int, "")
x = int(short(-32768))
if (x != -32768) apLogFailInfo("Wrong Value", -32768, _int, "")

_short = 101
_int = _short
if (_int != 101) apLogFailInfo("Wrong Value", 101, _int, "")
_int = short(101)
if (_int != 101) apLogFailInfo("Wrong Value", 101, _int, "")
x = int(_short)
if (x != 101) apLogFailInfo("Wrong Value", 101, _int, "")
x = int(short(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _int, "")



apInitScenario("int <-- ushort")

_ushort = 65535
_int = _ushort
if (_int != 65535) apLogFailInfo("Wrong Value", 65535, _int, "")
_int = ushort(65535)
if (_int != 65535) apLogFailInfo("Wrong Value", 65535, _int, "")
x = int(_ushort)
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _int, "")
x = int(ushort(65535))
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _int, "")

_ushort = 0
_int = _ushort
if (_int != 0) apLogFailInfo("Wrong Value", 0, _int, "")
_int = ushort(0)
if (_int != 0) apLogFailInfo("Wrong Value", 0, _int, "")
x = int(_ushort)
if (x != 0) apLogFailInfo("Wrong Value", 0, _int, "")
x = int(ushort(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _int, "")

_ushort = 101
_int = _ushort
if (_int != 101) apLogFailInfo("Wrong Value", 101, _int, "")
_int = ushort(101)
if (_int != 101) apLogFailInfo("Wrong Value", 101, _int, "")
x = int(_ushort)
if (x != 101) apLogFailInfo("Wrong Value", 101, _int, "")
x = int(ushort(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _int, "")



apInitScenario("int <-- int")

_int = 2147483647
_int = _int
if (_int != 2147483647) apLogFailInfo("Wrong Value", 2147483647, _int, "")
_int = int(2147483647)
if (_int != 2147483647) apLogFailInfo("Wrong Value", 2147483647, _int, "")
x = int(_int)
if (x != 2147483647) apLogFailInfo("Wrong Value", 2147483647, _int, "")
x = int(int(2147483647))
if (x != 2147483647) apLogFailInfo("Wrong Value", 2147483647, _int, "")

_int = -2147483648
_int = _int
if (_int != -2147483648) apLogFailInfo("Wrong Value", -2147483648, _int, "")
_int = int(-2147483648)
if (_int != -2147483648) apLogFailInfo("Wrong Value", -2147483648, _int, "")
x = int(_int)
if (x != -2147483648) apLogFailInfo("Wrong Value", -2147483648, _int, "")
x = int(int(-2147483648))
if (x != -2147483648) apLogFailInfo("Wrong Value", -2147483648, _int, "")

_int = 101
_int = _int
if (_int != 101) apLogFailInfo("Wrong Value", 101, _int, "")
_int = int(101)
if (_int != 101) apLogFailInfo("Wrong Value", 101, _int, "")
x = int(_int)
if (x != 101) apLogFailInfo("Wrong Value", 101, _int, "")
x = int(int(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _int, "")



apInitScenario("int <-- uint")

_uint = 2147483647
_int = _uint
if (_int != 2147483647) apLogFailInfo("Wrong Value", 2147483647, _int, "")
_int = uint(2147483647)
if (_int != 2147483647) apLogFailInfo("Wrong Value", 2147483647, _int, "")
x = int(_uint)
if (x != 2147483647) apLogFailInfo("Wrong Value", 2147483647, _int, "")
x = int(uint(2147483647))
if (x != 2147483647) apLogFailInfo("Wrong Value", 2147483647, _int, "")

_uint = 0
_int = _uint
if (_int != 0) apLogFailInfo("Wrong Value", 0, _int, "")
_int = uint(0)
if (_int != 0) apLogFailInfo("Wrong Value", 0, _int, "")
x = int(_uint)
if (x != 0) apLogFailInfo("Wrong Value", 0, _int, "")
x = int(uint(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _int, "")

_uint = 101
_int = _uint
if (_int != 101) apLogFailInfo("Wrong Value", 101, _int, "")
_int = uint(101)
if (_int != 101) apLogFailInfo("Wrong Value", 101, _int, "")
x = int(_uint)
if (x != 101) apLogFailInfo("Wrong Value", 101, _int, "")
x = int(uint(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _int, "")



apInitScenario("int <-- long")

_long = 2147483647
_int = _long
if (_int != 2147483647) apLogFailInfo("Wrong Value", 2147483647, _int, "")
_int = long(2147483647)
if (_int != 2147483647) apLogFailInfo("Wrong Value", 2147483647, _int, "")
x = int(_long)
if (x != 2147483647) apLogFailInfo("Wrong Value", 2147483647, _int, "")
x = int(long(2147483647))
if (x != 2147483647) apLogFailInfo("Wrong Value", 2147483647, _int, "")

_long = -2147483648
_int = _long
if (_int != -2147483648) apLogFailInfo("Wrong Value", -2147483648, _int, "")
_int = long(-2147483648)
if (_int != -2147483648) apLogFailInfo("Wrong Value", -2147483648, _int, "")
x = int(_long)
if (x != -2147483648) apLogFailInfo("Wrong Value", -2147483648, _int, "")
x = int(long(-2147483648))
if (x != -2147483648) apLogFailInfo("Wrong Value", -2147483648, _int, "")

_long = 101
_int = _long
if (_int != 101) apLogFailInfo("Wrong Value", 101, _int, "")
_int = long(101)
if (_int != 101) apLogFailInfo("Wrong Value", 101, _int, "")
x = int(_long)
if (x != 101) apLogFailInfo("Wrong Value", 101, _int, "")
x = int(long(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _int, "")



apInitScenario("int <-- ulong")

_ulong = 2147483647
_int = _ulong
if (_int != 2147483647) apLogFailInfo("Wrong Value", 2147483647, _int, "")
_int = ulong(2147483647)
if (_int != 2147483647) apLogFailInfo("Wrong Value", 2147483647, _int, "")
x = int(_ulong)
if (x != 2147483647) apLogFailInfo("Wrong Value", 2147483647, _int, "")
x = int(ulong(2147483647))
if (x != 2147483647) apLogFailInfo("Wrong Value", 2147483647, _int, "")

_ulong = 0
_int = _ulong
if (_int != 0) apLogFailInfo("Wrong Value", 0, _int, "")
_int = ulong(0)
if (_int != 0) apLogFailInfo("Wrong Value", 0, _int, "")
x = int(_ulong)
if (x != 0) apLogFailInfo("Wrong Value", 0, _int, "")
x = int(ulong(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _int, "")

_ulong = 101
_int = _ulong
if (_int != 101) apLogFailInfo("Wrong Value", 101, _int, "")
_int = ulong(101)
if (_int != 101) apLogFailInfo("Wrong Value", 101, _int, "")
x = int(_ulong)
if (x != 101) apLogFailInfo("Wrong Value", 101, _int, "")
x = int(ulong(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _int, "")



apInitScenario("int <-- char")

_char = 65535
_int = _char
if (_int != 65535) apLogFailInfo("Wrong Value", 65535, _int, "")
_int = char(65535)
if (_int != 65535) apLogFailInfo("Wrong Value", 65535, _int, "")
x = int(_char)
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _int, "")
x = int(char(65535))
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _int, "")

_char = 0
_int = _char
if (_int != 0) apLogFailInfo("Wrong Value", 0, _int, "")
_int = char(0)
if (_int != 0) apLogFailInfo("Wrong Value", 0, _int, "")
x = int(_char)
if (x != 0) apLogFailInfo("Wrong Value", 0, _int, "")
x = int(char(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _int, "")

_char = 101
_int = _char
if (_int != 101) apLogFailInfo("Wrong Value", 101, _int, "")
_int = char(101)
if (_int != 101) apLogFailInfo("Wrong Value", 101, _int, "")
x = int(_char)
if (x != 101) apLogFailInfo("Wrong Value", 101, _int, "")
x = int(char(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _int, "")



apInitScenario("int <-- decimal")

_decimal = 2147483647
_int = _decimal
if (_int != 2147483647) apLogFailInfo("Wrong Value", 2147483647, _int, "")
_int = decimal(2147483647)
if (_int != 2147483647) apLogFailInfo("Wrong Value", 2147483647, _int, "")
x = int(_decimal)
if (x != 2147483647) apLogFailInfo("Wrong Value", 2147483647, _int, "")
x = int(decimal(2147483647))
if (x != 2147483647) apLogFailInfo("Wrong Value", 2147483647, _int, "")

_decimal = -2147483648
_int = _decimal
if (_int != -2147483648) apLogFailInfo("Wrong Value", -2147483648, _int, "")
_int = decimal(-2147483648)
if (_int != -2147483648) apLogFailInfo("Wrong Value", -2147483648, _int, "")
x = int(_decimal)
if (x != -2147483648) apLogFailInfo("Wrong Value", -2147483648, _int, "")
x = int(decimal(-2147483648))
if (x != -2147483648) apLogFailInfo("Wrong Value", -2147483648, _int, "")

_decimal = 101
_int = _decimal
if (_int != 101) apLogFailInfo("Wrong Value", 101, _int, "")
_int = decimal(101)
if (_int != 101) apLogFailInfo("Wrong Value", 101, _int, "")
x = int(_decimal)
if (x != 101) apLogFailInfo("Wrong Value", 101, _int, "")
x = int(decimal(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _int, "")



apInitScenario("uint <-- sbyte")

_sbyte = 127
_uint = _sbyte
if (_uint != 127) apLogFailInfo("Wrong Value", 127, _uint, "")
_uint = sbyte(127)
if (_uint != 127) apLogFailInfo("Wrong Value", 127, _uint, "")
x = uint(_sbyte)
if (x != 127) apLogFailInfo("Wrong Value", 127, _uint, "")
x = uint(sbyte(127))
if (x != 127) apLogFailInfo("Wrong Value", 127, _uint, "")

_sbyte = 0
_uint = _sbyte
if (_uint != 0) apLogFailInfo("Wrong Value", 0, _uint, "")
_uint = sbyte(0)
if (_uint != 0) apLogFailInfo("Wrong Value", 0, _uint, "")
x = uint(_sbyte)
if (x != 0) apLogFailInfo("Wrong Value", 0, _uint, "")
x = uint(sbyte(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _uint, "")

_sbyte = 101
_uint = _sbyte
if (_uint != 101) apLogFailInfo("Wrong Value", 101, _uint, "")
_uint = sbyte(101)
if (_uint != 101) apLogFailInfo("Wrong Value", 101, _uint, "")
x = uint(_sbyte)
if (x != 101) apLogFailInfo("Wrong Value", 101, _uint, "")
x = uint(sbyte(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _uint, "")



apInitScenario("uint <-- byte")

_byte = 255
_uint = _byte
if (_uint != 255) apLogFailInfo("Wrong Value", 255, _uint, "")
_uint = byte(255)
if (_uint != 255) apLogFailInfo("Wrong Value", 255, _uint, "")
x = uint(_byte)
if (x != 255) apLogFailInfo("Wrong Value", 255, _uint, "")
x = uint(byte(255))
if (x != 255) apLogFailInfo("Wrong Value", 255, _uint, "")

_byte = 0
_uint = _byte
if (_uint != 0) apLogFailInfo("Wrong Value", 0, _uint, "")
_uint = byte(0)
if (_uint != 0) apLogFailInfo("Wrong Value", 0, _uint, "")
x = uint(_byte)
if (x != 0) apLogFailInfo("Wrong Value", 0, _uint, "")
x = uint(byte(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _uint, "")

_byte = 101
_uint = _byte
if (_uint != 101) apLogFailInfo("Wrong Value", 101, _uint, "")
_uint = byte(101)
if (_uint != 101) apLogFailInfo("Wrong Value", 101, _uint, "")
x = uint(_byte)
if (x != 101) apLogFailInfo("Wrong Value", 101, _uint, "")
x = uint(byte(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _uint, "")



apInitScenario("uint <-- short")

_short = 32767
_uint = _short
if (_uint != 32767) apLogFailInfo("Wrong Value", 32767, _uint, "")
_uint = short(32767)
if (_uint != 32767) apLogFailInfo("Wrong Value", 32767, _uint, "")
x = uint(_short)
if (x != 32767) apLogFailInfo("Wrong Value", 32767, _uint, "")
x = uint(short(32767))
if (x != 32767) apLogFailInfo("Wrong Value", 32767, _uint, "")

_short = 0
_uint = _short
if (_uint != 0) apLogFailInfo("Wrong Value", 0, _uint, "")
_uint = short(0)
if (_uint != 0) apLogFailInfo("Wrong Value", 0, _uint, "")
x = uint(_short)
if (x != 0) apLogFailInfo("Wrong Value", 0, _uint, "")
x = uint(short(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _uint, "")

_short = 101
_uint = _short
if (_uint != 101) apLogFailInfo("Wrong Value", 101, _uint, "")
_uint = short(101)
if (_uint != 101) apLogFailInfo("Wrong Value", 101, _uint, "")
x = uint(_short)
if (x != 101) apLogFailInfo("Wrong Value", 101, _uint, "")
x = uint(short(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _uint, "")



apInitScenario("uint <-- ushort")

_ushort = 65535
_uint = _ushort
if (_uint != 65535) apLogFailInfo("Wrong Value", 65535, _uint, "")
_uint = ushort(65535)
if (_uint != 65535) apLogFailInfo("Wrong Value", 65535, _uint, "")
x = uint(_ushort)
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _uint, "")
x = uint(ushort(65535))
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _uint, "")

_ushort = 0
_uint = _ushort
if (_uint != 0) apLogFailInfo("Wrong Value", 0, _uint, "")
_uint = ushort(0)
if (_uint != 0) apLogFailInfo("Wrong Value", 0, _uint, "")
x = uint(_ushort)
if (x != 0) apLogFailInfo("Wrong Value", 0, _uint, "")
x = uint(ushort(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _uint, "")

_ushort = 101
_uint = _ushort
if (_uint != 101) apLogFailInfo("Wrong Value", 101, _uint, "")
_uint = ushort(101)
if (_uint != 101) apLogFailInfo("Wrong Value", 101, _uint, "")
x = uint(_ushort)
if (x != 101) apLogFailInfo("Wrong Value", 101, _uint, "")
x = uint(ushort(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _uint, "")



apInitScenario("uint <-- int")

_int = 2147483647
_uint = _int
if (_uint != 2147483647) apLogFailInfo("Wrong Value", 2147483647, _uint, "")
_uint = int(2147483647)
if (_uint != 2147483647) apLogFailInfo("Wrong Value", 2147483647, _uint, "")
x = uint(_int)
if (x != 2147483647) apLogFailInfo("Wrong Value", 2147483647, _uint, "")
x = uint(int(2147483647))
if (x != 2147483647) apLogFailInfo("Wrong Value", 2147483647, _uint, "")

_int = 0
_uint = _int
if (_uint != 0) apLogFailInfo("Wrong Value", 0, _uint, "")
_uint = int(0)
if (_uint != 0) apLogFailInfo("Wrong Value", 0, _uint, "")
x = uint(_int)
if (x != 0) apLogFailInfo("Wrong Value", 0, _uint, "")
x = uint(int(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _uint, "")

_int = 101
_uint = _int
if (_uint != 101) apLogFailInfo("Wrong Value", 101, _uint, "")
_uint = int(101)
if (_uint != 101) apLogFailInfo("Wrong Value", 101, _uint, "")
x = uint(_int)
if (x != 101) apLogFailInfo("Wrong Value", 101, _uint, "")
x = uint(int(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _uint, "")



apInitScenario("uint <-- uint")

_uint = uint.MaxValue
_uint = _uint
if (_uint != uint.MaxValue) apLogFailInfo("Wrong Value", uint.MaxValue, _uint, "")
_uint = uint(uint.MaxValue)
if (_uint != uint.MaxValue) apLogFailInfo("Wrong Value", uint.MaxValue, _uint, "")
x = uint(_uint)
if (x != uint.MaxValue) apLogFailInfo("Wrong Value", uint.MaxValue, _uint, "")
x = uint(uint(uint.MaxValue))
if (x != uint.MaxValue) apLogFailInfo("Wrong Value", uint.MaxValue, _uint, "")

_uint = 0
_uint = _uint
if (_uint != 0) apLogFailInfo("Wrong Value", 0, _uint, "")
_uint = uint(0)
if (_uint != 0) apLogFailInfo("Wrong Value", 0, _uint, "")
x = uint(_uint)
if (x != 0) apLogFailInfo("Wrong Value", 0, _uint, "")
x = uint(uint(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _uint, "")

_uint = 101
_uint = _uint
if (_uint != 101) apLogFailInfo("Wrong Value", 101, _uint, "")
_uint = uint(101)
if (_uint != 101) apLogFailInfo("Wrong Value", 101, _uint, "")
x = uint(_uint)
if (x != 101) apLogFailInfo("Wrong Value", 101, _uint, "")
x = uint(uint(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _uint, "")



apInitScenario("uint <-- long")

_long = uint.MaxValue 
_uint = _long
if (_uint != uint.MaxValue ) apLogFailInfo("Wrong Value", uint.MaxValue , _uint, "")
_uint = long(uint.MaxValue )
if (_uint != uint.MaxValue ) apLogFailInfo("Wrong Value", uint.MaxValue , _uint, "")
x = uint(_long)
if (x != uint.MaxValue ) apLogFailInfo("Wrong Value", uint.MaxValue , _uint, "")
x = uint(long(uint.MaxValue ))
if (x != uint.MaxValue ) apLogFailInfo("Wrong Value", uint.MaxValue , _uint, "")

_long = 0
_uint = _long
if (_uint != 0) apLogFailInfo("Wrong Value", 0, _uint, "")
_uint = long(0)
if (_uint != 0) apLogFailInfo("Wrong Value", 0, _uint, "")
x = uint(_long)
if (x != 0) apLogFailInfo("Wrong Value", 0, _uint, "")
x = uint(long(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _uint, "")

_long = 101
_uint = _long
if (_uint != 101) apLogFailInfo("Wrong Value", 101, _uint, "")
_uint = long(101)
if (_uint != 101) apLogFailInfo("Wrong Value", 101, _uint, "")
x = uint(_long)
if (x != 101) apLogFailInfo("Wrong Value", 101, _uint, "")
x = uint(long(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _uint, "")



apInitScenario("uint <-- ulong")

_ulong = uint.MaxValue 
_uint = _ulong
if (_uint != uint.MaxValue ) apLogFailInfo("Wrong Value", uint.MaxValue , _uint, "")
_uint = ulong(uint.MaxValue )
if (_uint != uint.MaxValue ) apLogFailInfo("Wrong Value", uint.MaxValue , _uint, "")
x = uint(_ulong)
if (x != uint.MaxValue ) apLogFailInfo("Wrong Value", uint.MaxValue , _uint, "")
x = uint(ulong(uint.MaxValue ))
if (x != uint.MaxValue ) apLogFailInfo("Wrong Value", uint.MaxValue , _uint, "")

_ulong = 0
_uint = _ulong
if (_uint != 0) apLogFailInfo("Wrong Value", 0, _uint, "")
_uint = ulong(0)
if (_uint != 0) apLogFailInfo("Wrong Value", 0, _uint, "")
x = uint(_ulong)
if (x != 0) apLogFailInfo("Wrong Value", 0, _uint, "")
x = uint(ulong(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _uint, "")

_ulong = 101
_uint = _ulong
if (_uint != 101) apLogFailInfo("Wrong Value", 101, _uint, "")
_uint = ulong(101)
if (_uint != 101) apLogFailInfo("Wrong Value", 101, _uint, "")
x = uint(_ulong)
if (x != 101) apLogFailInfo("Wrong Value", 101, _uint, "")
x = uint(ulong(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _uint, "")



apInitScenario("uint <-- char")

_char = 65535
_uint = _char
if (_uint != 65535) apLogFailInfo("Wrong Value", 65535, _uint, "")
_uint = char(65535)
if (_uint != 65535) apLogFailInfo("Wrong Value", 65535, _uint, "")
x = uint(_char)
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _uint, "")
x = uint(char(65535))
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _uint, "")

_char = 0
_uint = _char
if (_uint != 0) apLogFailInfo("Wrong Value", 0, _uint, "")
_uint = char(0)
if (_uint != 0) apLogFailInfo("Wrong Value", 0, _uint, "")
x = uint(_char)
if (x != 0) apLogFailInfo("Wrong Value", 0, _uint, "")
x = uint(char(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _uint, "")

_char = 101
_uint = _char
if (_uint != 101) apLogFailInfo("Wrong Value", 101, _uint, "")
_uint = char(101)
if (_uint != 101) apLogFailInfo("Wrong Value", 101, _uint, "")
x = uint(_char)
if (x != 101) apLogFailInfo("Wrong Value", 101, _uint, "")
x = uint(char(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _uint, "")



apInitScenario("uint <-- decimal")

_decimal = uint.MaxValue 
_uint = _decimal
if (_uint != uint.MaxValue ) apLogFailInfo("Wrong Value", uint.MaxValue , _uint, "")
_uint = decimal(uint.MaxValue )
if (_uint != uint.MaxValue ) apLogFailInfo("Wrong Value", uint.MaxValue , _uint, "")
x = uint(_decimal)
if (x != uint.MaxValue ) apLogFailInfo("Wrong Value", uint.MaxValue , _uint, "")
x = uint(decimal(uint.MaxValue ))
if (x != uint.MaxValue ) apLogFailInfo("Wrong Value", uint.MaxValue , _uint, "")

_decimal = 0
_uint = _decimal
if (_uint != 0) apLogFailInfo("Wrong Value", 0, _uint, "")
_uint = decimal(0)
if (_uint != 0) apLogFailInfo("Wrong Value", 0, _uint, "")
x = uint(_decimal)
if (x != 0) apLogFailInfo("Wrong Value", 0, _uint, "")
x = uint(decimal(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _uint, "")

_decimal = 101
_uint = _decimal
if (_uint != 101) apLogFailInfo("Wrong Value", 101, _uint, "")
_uint = decimal(101)
if (_uint != 101) apLogFailInfo("Wrong Value", 101, _uint, "")
x = uint(_decimal)
if (x != 101) apLogFailInfo("Wrong Value", 101, _uint, "")
x = uint(decimal(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _uint, "")



apInitScenario("long <-- sbyte")

_sbyte = 127
_long = _sbyte
if (_long != 127) apLogFailInfo("Wrong Value", 127, _long, "")
_long = sbyte(127)
if (_long != 127) apLogFailInfo("Wrong Value", 127, _long, "")
x = long(_sbyte)
if (x != 127) apLogFailInfo("Wrong Value", 127, _long, "")
x = long(sbyte(127))
if (x != 127) apLogFailInfo("Wrong Value", 127, _long, "")

_sbyte = -128
_long = _sbyte
if (_long != -128) apLogFailInfo("Wrong Value", -128, _long, "")
_long = sbyte(-128)
if (_long != -128) apLogFailInfo("Wrong Value", -128, _long, "")
x = long(_sbyte)
if (x != -128) apLogFailInfo("Wrong Value", -128, _long, "")
x = long(sbyte(-128))
if (x != -128) apLogFailInfo("Wrong Value", -128, _long, "")

_sbyte = 101
_long = _sbyte
if (_long != 101) apLogFailInfo("Wrong Value", 101, _long, "")
_long = sbyte(101)
if (_long != 101) apLogFailInfo("Wrong Value", 101, _long, "")
x = long(_sbyte)
if (x != 101) apLogFailInfo("Wrong Value", 101, _long, "")
x = long(sbyte(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _long, "")



apInitScenario("long <-- byte")

_byte = 255
_long = _byte
if (_long != 255) apLogFailInfo("Wrong Value", 255, _long, "")
_long = byte(255)
if (_long != 255) apLogFailInfo("Wrong Value", 255, _long, "")
x = long(_byte)
if (x != 255) apLogFailInfo("Wrong Value", 255, _long, "")
x = long(byte(255))
if (x != 255) apLogFailInfo("Wrong Value", 255, _long, "")

_byte = 0
_long = _byte
if (_long != 0) apLogFailInfo("Wrong Value", 0, _long, "")
_long = byte(0)
if (_long != 0) apLogFailInfo("Wrong Value", 0, _long, "")
x = long(_byte)
if (x != 0) apLogFailInfo("Wrong Value", 0, _long, "")
x = long(byte(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _long, "")

_byte = 101
_long = _byte
if (_long != 101) apLogFailInfo("Wrong Value", 101, _long, "")
_long = byte(101)
if (_long != 101) apLogFailInfo("Wrong Value", 101, _long, "")
x = long(_byte)
if (x != 101) apLogFailInfo("Wrong Value", 101, _long, "")
x = long(byte(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _long, "")



apInitScenario("long <-- short")

_short = 32767
_long = _short
if (_long != 32767) apLogFailInfo("Wrong Value", 32767, _long, "")
_long = short(32767)
if (_long != 32767) apLogFailInfo("Wrong Value", 32767, _long, "")
x = long(_short)
if (x != 32767) apLogFailInfo("Wrong Value", 32767, _long, "")
x = long(short(32767))
if (x != 32767) apLogFailInfo("Wrong Value", 32767, _long, "")

_short = -32768
_long = _short
if (_long != -32768) apLogFailInfo("Wrong Value", -32768, _long, "")
_long = short(-32768)
if (_long != -32768) apLogFailInfo("Wrong Value", -32768, _long, "")
x = long(_short)
if (x != -32768) apLogFailInfo("Wrong Value", -32768, _long, "")
x = long(short(-32768))
if (x != -32768) apLogFailInfo("Wrong Value", -32768, _long, "")

_short = 101
_long = _short
if (_long != 101) apLogFailInfo("Wrong Value", 101, _long, "")
_long = short(101)
if (_long != 101) apLogFailInfo("Wrong Value", 101, _long, "")
x = long(_short)
if (x != 101) apLogFailInfo("Wrong Value", 101, _long, "")
x = long(short(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _long, "")



apInitScenario("long <-- ushort")

_ushort = 65535
_long = _ushort
if (_long != 65535) apLogFailInfo("Wrong Value", 65535, _long, "")
_long = ushort(65535)
if (_long != 65535) apLogFailInfo("Wrong Value", 65535, _long, "")
x = long(_ushort)
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _long, "")
x = long(ushort(65535))
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _long, "")

_ushort = 0
_long = _ushort
if (_long != 0) apLogFailInfo("Wrong Value", 0, _long, "")
_long = ushort(0)
if (_long != 0) apLogFailInfo("Wrong Value", 0, _long, "")
x = long(_ushort)
if (x != 0) apLogFailInfo("Wrong Value", 0, _long, "")
x = long(ushort(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _long, "")

_ushort = 101
_long = _ushort
if (_long != 101) apLogFailInfo("Wrong Value", 101, _long, "")
_long = ushort(101)
if (_long != 101) apLogFailInfo("Wrong Value", 101, _long, "")
x = long(_ushort)
if (x != 101) apLogFailInfo("Wrong Value", 101, _long, "")
x = long(ushort(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _long, "")



apInitScenario("long <-- int")

_int = 2147483647
_long = _int
if (_long != 2147483647) apLogFailInfo("Wrong Value", 2147483647, _long, "")
_long = int(2147483647)
if (_long != 2147483647) apLogFailInfo("Wrong Value", 2147483647, _long, "")
x = long(_int)
if (x != 2147483647) apLogFailInfo("Wrong Value", 2147483647, _long, "")
x = long(int(2147483647))
if (x != 2147483647) apLogFailInfo("Wrong Value", 2147483647, _long, "")

_int = -2147483648
_long = _int
if (_long != -2147483648) apLogFailInfo("Wrong Value", -2147483648, _long, "")
_long = int(-2147483648)
if (_long != -2147483648) apLogFailInfo("Wrong Value", -2147483648, _long, "")
x = long(_int)
if (x != -2147483648) apLogFailInfo("Wrong Value", -2147483648, _long, "")
x = long(int(-2147483648))
if (x != -2147483648) apLogFailInfo("Wrong Value", -2147483648, _long, "")

_int = 101
_long = _int
if (_long != 101) apLogFailInfo("Wrong Value", 101, _long, "")
_long = int(101)
if (_long != 101) apLogFailInfo("Wrong Value", 101, _long, "")
x = long(_int)
if (x != 101) apLogFailInfo("Wrong Value", 101, _long, "")
x = long(int(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _long, "")



apInitScenario("long <-- uint")

_uint = uint.MaxValue 
_long = _uint
_long = uint(uint.MaxValue )
if (_long != uint.MaxValue ) apLogFailInfo("2 Wrong Value", uint.MaxValue , _long, "")
x = long(_uint)
x = long(uint(uint.MaxValue ))
if (x != uint.MaxValue ) apLogFailInfo("4 Wrong Value", uint.MaxValue , _long, "")

_uint = 0
_long = _uint
if (_long != 0) apLogFailInfo("Wrong Value", 0, _long, "")
_long = uint(0)
if (_long != 0) apLogFailInfo("Wrong Value", 0, _long, "")
x = long(_uint)
if (x != 0) apLogFailInfo("Wrong Value", 0, _long, "")
x = long(uint(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _long, "")

_uint = 101
_long = _uint
if (_long != 101) apLogFailInfo("Wrong Value", 101, _long, "")
_long = uint(101)
if (_long != 101) apLogFailInfo("Wrong Value", 101, _long, "")
x = long(_uint)
if (x != 101) apLogFailInfo("Wrong Value", 101, _long, "")
x = long(uint(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _long, "")



apInitScenario("long <-- long")

_long = long.MaxValue
_long = _long
if (_long != long.MaxValue) apLogFailInfo("Wrong Value", long.MaxValue, _long, "")
_long = long(long.MaxValue)
if (_long != long.MaxValue) apLogFailInfo("Wrong Value", long.MaxValue, _long, "")
x = long(_long)
if (x != long.MaxValue) apLogFailInfo("Wrong Value", long.MaxValue, _long, "")
x = long(long(long.MaxValue))
if (x != long.MaxValue) apLogFailInfo("Wrong Value", long.MaxValue, _long, "")

_long = -9223372036854775808
_long = _long
if (_long != -9223372036854775808) apLogFailInfo("Wrong Value", -9223372036854775808, _long, "")
_long = long(-9223372036854775808)
if (_long != -9223372036854775808) apLogFailInfo("Wrong Value", -9223372036854775808, _long, "")
x = long(_long)
if (x != -9223372036854775808) apLogFailInfo("Wrong Value", -9223372036854775808, _long, "")
x = long(long(-9223372036854775808))
if (x != -9223372036854775808) apLogFailInfo("Wrong Value", -9223372036854775808, _long, "")

_long = 101
_long = _long
if (_long != 101) apLogFailInfo("Wrong Value", 101, _long, "")
_long = long(101)
if (_long != 101) apLogFailInfo("Wrong Value", 101, _long, "")
x = long(_long)
if (x != 101) apLogFailInfo("Wrong Value", 101, _long, "")
x = long(long(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _long, "")



apInitScenario("long <-- ulong")

_ulong = long.MaxValue
_long = _ulong
if (_long != long.MaxValue) apLogFailInfo("Wrong Value", long.MaxValue, _long, "")
_long = ulong(long.MaxValue)
if (_long != long.MaxValue) apLogFailInfo("Wrong Value", long.MaxValue, _long, "")
x = long(_ulong)
if (x != long.MaxValue) apLogFailInfo("Wrong Value", long.MaxValue, _long, "")
x = long(ulong(long.MaxValue))
if (x != long.MaxValue) apLogFailInfo("Wrong Value", long.MaxValue, _long, "")

_ulong = 0
_long = _ulong
if (_long != 0) apLogFailInfo("Wrong Value", 0, _long, "")
_long = ulong(0)
if (_long != 0) apLogFailInfo("Wrong Value", 0, _long, "")
x = long(_ulong)
if (x != 0) apLogFailInfo("Wrong Value", 0, _long, "")
x = long(ulong(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _long, "")

_ulong = 101
_long = _ulong
if (_long != 101) apLogFailInfo("Wrong Value", 101, _long, "")
_long = ulong(101)
if (_long != 101) apLogFailInfo("Wrong Value", 101, _long, "")
x = long(_ulong)
if (x != 101) apLogFailInfo("Wrong Value", 101, _long, "")
x = long(ulong(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _long, "")



apInitScenario("long <-- char")

_char = 65535
_long = _char
if (_long != 65535) apLogFailInfo("Wrong Value", 65535, _long, "")
_long = char(65535)
if (_long != 65535) apLogFailInfo("Wrong Value", 65535, _long, "")
x = long(_char)
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _long, "")
x = long(char(65535))
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _long, "")

_char = 0
_long = _char
if (_long != 0) apLogFailInfo("Wrong Value", 0, _long, "")
_long = char(0)
if (_long != 0) apLogFailInfo("Wrong Value", 0, _long, "")
x = long(_char)
if (x != 0) apLogFailInfo("Wrong Value", 0, _long, "")
x = long(char(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _long, "")

_char = 101
_long = _char
if (_long != 101) apLogFailInfo("Wrong Value", 101, _long, "")
_long = char(101)
if (_long != 101) apLogFailInfo("Wrong Value", 101, _long, "")
x = long(_char)
if (x != 101) apLogFailInfo("Wrong Value", 101, _long, "")
x = long(char(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _long, "")



apInitScenario("long <-- decimal")
/*  PGMTODOPGM
_decimal = long.MaxValue 
_long = _decimal
if (_long != long.MaxValue ) apLogFailInfo("Wrong Value", long.MaxValue , _long, "")
_long = decimal(long.MaxValue )
if (_long != long.MaxValue ) apLogFailInfo("Wrong Value", long.MaxValue , _long, "")
x = long(_decimal)
if (x != long.MaxValue ) apLogFailInfo("Wrong Value", long.MaxValue , _long, "")
x = long(decimal(long.MaxValue ))
if (x != long.MaxValue ) apLogFailInfo("Wrong Value", long.MaxValue , _long, "")

_decimal = -9223372036854775808
_long = _decimal
if (_long != -9223372036854775808) apLogFailInfo("Wrong Value", -9223372036854775808, _long, "")
_long = decimal(-9223372036854775808)
if (_long != -9223372036854775808) apLogFailInfo("Wrong Value", -9223372036854775808, _long, "")
x = long(_decimal)
if (x != -9223372036854775808) apLogFailInfo("Wrong Value", -9223372036854775808, _long, "")
x = long(decimal(-9223372036854775808))
if (x != -9223372036854775808) apLogFailInfo("Wrong Value", -9223372036854775808, _long, "")

_decimal = 101
_long = _decimal
if (_long != 101) apLogFailInfo("Wrong Value", 101, _long, "")
_long = decimal(101)
if (_long != 101) apLogFailInfo("Wrong Value", 101, _long, "")
x = long(_decimal)
if (x != 101) apLogFailInfo("Wrong Value", 101, _long, "")
x = long(decimal(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _long, "")
*/


apInitScenario("ulong <-- sbyte")

_sbyte = 127
_ulong = _sbyte
if (_ulong != 127) apLogFailInfo("Wrong Value", 127, _ulong, "")
_ulong = sbyte(127)
if (_ulong != 127) apLogFailInfo("Wrong Value", 127, _ulong, "")
x = ulong(_sbyte)
if (x != 127) apLogFailInfo("Wrong Value", 127, _ulong, "")
x = ulong(sbyte(127))
if (x != 127) apLogFailInfo("Wrong Value", 127, _ulong, "")

_sbyte = 0
_ulong = _sbyte
if (_ulong != 0) apLogFailInfo("Wrong Value", 0, _ulong, "")
_ulong = sbyte(0)
if (_ulong != 0) apLogFailInfo("Wrong Value", 0, _ulong, "")
x = ulong(_sbyte)
if (x != 0) apLogFailInfo("Wrong Value", 0, _ulong, "")
x = ulong(sbyte(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _ulong, "")

_sbyte = 101
_ulong = _sbyte
if (_ulong != 101) apLogFailInfo("Wrong Value", 101, _ulong, "")
_ulong = sbyte(101)
if (_ulong != 101) apLogFailInfo("Wrong Value", 101, _ulong, "")
x = ulong(_sbyte)
if (x != 101) apLogFailInfo("Wrong Value", 101, _ulong, "")
x = ulong(sbyte(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _ulong, "")



apInitScenario("ulong <-- byte")

_byte = 255
_ulong = _byte
if (_ulong != 255) apLogFailInfo("Wrong Value", 255, _ulong, "")
_ulong = byte(255)
if (_ulong != 255) apLogFailInfo("Wrong Value", 255, _ulong, "")
x = ulong(_byte)
if (x != 255) apLogFailInfo("Wrong Value", 255, _ulong, "")
x = ulong(byte(255))
if (x != 255) apLogFailInfo("Wrong Value", 255, _ulong, "")

_byte = 0
_ulong = _byte
if (_ulong != 0) apLogFailInfo("Wrong Value", 0, _ulong, "")
_ulong = byte(0)
if (_ulong != 0) apLogFailInfo("Wrong Value", 0, _ulong, "")
x = ulong(_byte)
if (x != 0) apLogFailInfo("Wrong Value", 0, _ulong, "")
x = ulong(byte(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _ulong, "")

_byte = 101
_ulong = _byte
if (_ulong != 101) apLogFailInfo("Wrong Value", 101, _ulong, "")
_ulong = byte(101)
if (_ulong != 101) apLogFailInfo("Wrong Value", 101, _ulong, "")
x = ulong(_byte)
if (x != 101) apLogFailInfo("Wrong Value", 101, _ulong, "")
x = ulong(byte(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _ulong, "")



apInitScenario("ulong <-- short")

_short = 32767
_ulong = _short
if (_ulong != 32767) apLogFailInfo("Wrong Value", 32767, _ulong, "")
_ulong = short(32767)
if (_ulong != 32767) apLogFailInfo("Wrong Value", 32767, _ulong, "")
x = ulong(_short)
if (x != 32767) apLogFailInfo("Wrong Value", 32767, _ulong, "")
x = ulong(short(32767))
if (x != 32767) apLogFailInfo("Wrong Value", 32767, _ulong, "")

_short = 0
_ulong = _short
if (_ulong != 0) apLogFailInfo("Wrong Value", 0, _ulong, "")
_ulong = short(0)
if (_ulong != 0) apLogFailInfo("Wrong Value", 0, _ulong, "")
x = ulong(_short)
if (x != 0) apLogFailInfo("Wrong Value", 0, _ulong, "")
x = ulong(short(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _ulong, "")

_short = 101
_ulong = _short
if (_ulong != 101) apLogFailInfo("Wrong Value", 101, _ulong, "")
_ulong = short(101)
if (_ulong != 101) apLogFailInfo("Wrong Value", 101, _ulong, "")
x = ulong(_short)
if (x != 101) apLogFailInfo("Wrong Value", 101, _ulong, "")
x = ulong(short(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _ulong, "")



apInitScenario("ulong <-- ushort")

_ushort = 65535
_ulong = _ushort
if (_ulong != 65535) apLogFailInfo("Wrong Value", 65535, _ulong, "")
_ulong = ushort(65535)
if (_ulong != 65535) apLogFailInfo("Wrong Value", 65535, _ulong, "")
x = ulong(_ushort)
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _ulong, "")
x = ulong(ushort(65535))
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _ulong, "")

_ushort = 0
_ulong = _ushort
if (_ulong != 0) apLogFailInfo("Wrong Value", 0, _ulong, "")
_ulong = ushort(0)
if (_ulong != 0) apLogFailInfo("Wrong Value", 0, _ulong, "")
x = ulong(_ushort)
if (x != 0) apLogFailInfo("Wrong Value", 0, _ulong, "")
x = ulong(ushort(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _ulong, "")

_ushort = 101
_ulong = _ushort
if (_ulong != 101) apLogFailInfo("Wrong Value", 101, _ulong, "")
_ulong = ushort(101)
if (_ulong != 101) apLogFailInfo("Wrong Value", 101, _ulong, "")
x = ulong(_ushort)
if (x != 101) apLogFailInfo("Wrong Value", 101, _ulong, "")
x = ulong(ushort(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _ulong, "")



apInitScenario("ulong <-- int")

_int = 2147483647
_ulong = _int
if (_ulong != 2147483647) apLogFailInfo("Wrong Value", 2147483647, _ulong, "")
_ulong = int(2147483647)
if (_ulong != 2147483647) apLogFailInfo("Wrong Value", 2147483647, _ulong, "")
x = ulong(_int)
if (x != 2147483647) apLogFailInfo("Wrong Value", 2147483647, _ulong, "")
x = ulong(int(2147483647))
if (x != 2147483647) apLogFailInfo("Wrong Value", 2147483647, _ulong, "")

_int = 0
_ulong = _int
if (_ulong != 0) apLogFailInfo("Wrong Value", 0, _ulong, "")
_ulong = int(0)
if (_ulong != 0) apLogFailInfo("Wrong Value", 0, _ulong, "")
x = ulong(_int)
if (x != 0) apLogFailInfo("Wrong Value", 0, _ulong, "")
x = ulong(int(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _ulong, "")

_int = 101
_ulong = _int
if (_ulong != 101) apLogFailInfo("Wrong Value", 101, _ulong, "")
_ulong = int(101)
if (_ulong != 101) apLogFailInfo("Wrong Value", 101, _ulong, "")
x = ulong(_int)
if (x != 101) apLogFailInfo("Wrong Value", 101, _ulong, "")
x = ulong(int(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _ulong, "")



apInitScenario("ulong <-- uint")

_uint = uint.MaxValue 
_ulong = _uint
if (_ulong != uint.MaxValue ) apLogFailInfo("Wrong Value", uint.MaxValue , _ulong, "")
_ulong = uint(uint.MaxValue )
if (_ulong != uint.MaxValue ) apLogFailInfo("Wrong Value", uint.MaxValue , _ulong, "")
x = ulong(_uint)
if (x != uint.MaxValue ) apLogFailInfo("Wrong Value", uint.MaxValue , _ulong, "")
x = ulong(uint(uint.MaxValue ))
if (x != uint.MaxValue ) apLogFailInfo("Wrong Value", uint.MaxValue , _ulong, "")

_uint = 0
_ulong = _uint
if (_ulong != 0) apLogFailInfo("Wrong Value", 0, _ulong, "")
_ulong = uint(0)
if (_ulong != 0) apLogFailInfo("Wrong Value", 0, _ulong, "")
x = ulong(_uint)
if (x != 0) apLogFailInfo("Wrong Value", 0, _ulong, "")
x = ulong(uint(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _ulong, "")

_uint = 101
_ulong = _uint
if (_ulong != 101) apLogFailInfo("Wrong Value", 101, _ulong, "")
_ulong = uint(101)
if (_ulong != 101) apLogFailInfo("Wrong Value", 101, _ulong, "")
x = ulong(_uint)
if (x != 101) apLogFailInfo("Wrong Value", 101, _ulong, "")
x = ulong(uint(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _ulong, "")



apInitScenario("ulong <-- long")

_long = long.MaxValue
_ulong = _long
_ulong = long(long.MaxValue)
x = ulong(_long)
if (x != long.MaxValue) apLogFailInfo("Wrong Value", long.MaxValue, _ulong, "")
x = ulong(long(long.MaxValue))
if (x != long.MaxValue) apLogFailInfo("Wrong Value", long.MaxValue, _ulong, "")

_long = 0
_ulong = _long
if (_ulong != 0) apLogFailInfo("Wrong Value", 0, _ulong, "")
_ulong = long(0)
if (_ulong != 0) apLogFailInfo("Wrong Value", 0, _ulong, "")
x = ulong(_long)
if (x != 0) apLogFailInfo("Wrong Value", 0, _ulong, "")
x = ulong(long(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _ulong, "")

_long = 101
_ulong = _long
if (_ulong != 101) apLogFailInfo("Wrong Value", 101, _ulong, "")
_ulong = long(101)
if (_ulong != 101) apLogFailInfo("Wrong Value", 101, _ulong, "")
x = ulong(_long)
if (x != 101) apLogFailInfo("Wrong Value", 101, _ulong, "")
x = ulong(long(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _ulong, "")



apInitScenario("ulong <-- ulong")

_ulong = ulong.MaxValue
_ulong = _ulong
if (_ulong != ulong.MaxValue) apLogFailInfo("Wrong Value", ulong.MaxValue, _ulong, "")
_ulong = ulong(ulong.MaxValue)
if (_ulong != ulong.MaxValue) apLogFailInfo("Wrong Value", ulong.MaxValue, _ulong, "")
x = ulong(_ulong)
if (x != ulong.MaxValue) apLogFailInfo("Wrong Value", ulong.MaxValue, _ulong, "")
x = ulong(ulong(ulong.MaxValue))
if (x != ulong.MaxValue) apLogFailInfo("Wrong Value", ulong.MaxValue, _ulong, "")

_ulong = 0
_ulong = _ulong
if (_ulong != 0) apLogFailInfo("Wrong Value", 0, _ulong, "")
_ulong = ulong(0)
if (_ulong != 0) apLogFailInfo("Wrong Value", 0, _ulong, "")
x = ulong(_ulong)
if (x != 0) apLogFailInfo("Wrong Value", 0, _ulong, "")
x = ulong(ulong(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _ulong, "")

_ulong = 101
_ulong = _ulong
if (_ulong != 101) apLogFailInfo("Wrong Value", 101, _ulong, "")
_ulong = ulong(101)
if (_ulong != 101) apLogFailInfo("Wrong Value", 101, _ulong, "")
x = ulong(_ulong)
if (x != 101) apLogFailInfo("Wrong Value", 101, _ulong, "")
x = ulong(ulong(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _ulong, "")



apInitScenario("ulong <-- char")

_char = 65535
_ulong = _char
if (_ulong != 65535) apLogFailInfo("Wrong Value", 65535, _ulong, "")
_ulong = char(65535)
if (_ulong != 65535) apLogFailInfo("Wrong Value", 65535, _ulong, "")
x = ulong(_char)
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _ulong, "")
x = ulong(char(65535))
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _ulong, "")

_char = 0
_ulong = _char
if (_ulong != 0) apLogFailInfo("Wrong Value", 0, _ulong, "")
_ulong = char(0)
if (_ulong != 0) apLogFailInfo("Wrong Value", 0, _ulong, "")
x = ulong(_char)
if (x != 0) apLogFailInfo("Wrong Value", 0, _ulong, "")
x = ulong(char(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _ulong, "")

_char = 101
_ulong = _char
if (_ulong != 101) apLogFailInfo("Wrong Value", 101, _ulong, "")
_ulong = char(101)
if (_ulong != 101) apLogFailInfo("Wrong Value", 101, _ulong, "")
x = ulong(_char)
if (x != 101) apLogFailInfo("Wrong Value", 101, _ulong, "")
x = ulong(char(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _ulong, "")



apInitScenario("ulong <-- decimal")

_decimal = ulong.MaxValue 
_ulong = _decimal
if (_ulong != ulong.MaxValue ) apLogFailInfo("1 Wrong Value", ulong.MaxValue , _ulong, "")
_ulong = decimal(ulong.MaxValue )
if (_ulong != ulong.MaxValue ) apLogFailInfo("2 Wrong Value", ulong.MaxValue , _ulong, "")
x = ulong(_decimal)
x = ulong(decimal(ulong.MaxValue ))
if (x != ulong.MaxValue ) apLogFailInfo("4 Wrong Value", ulong.MaxValue , _ulong, "")

_decimal = 0
_ulong = _decimal
if (_ulong != 0) apLogFailInfo("Wrong Value", 0, _ulong, "")
_ulong = decimal(0)
if (_ulong != 0) apLogFailInfo("Wrong Value", 0, _ulong, "")
x = ulong(_decimal)
if (x != 0) apLogFailInfo("Wrong Value", 0, _ulong, "")
x = ulong(decimal(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _ulong, "")

_decimal = 101
_ulong = _decimal
if (_ulong != 101) apLogFailInfo("Wrong Value", 101, _ulong, "")
_ulong = decimal(101)
if (_ulong != 101) apLogFailInfo("Wrong Value", 101, _ulong, "")
x = ulong(_decimal)
if (x != 101) apLogFailInfo("Wrong Value", 101, _ulong, "")
x = ulong(decimal(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _ulong, "")



apInitScenario("char <-- sbyte")
/*  PGMTODOPGM
_sbyte = 127
_char = _sbyte
if (_char != 127) apLogFailInfo("Wrong Value", 127, _char, "")
_char = sbyte(127)
if (_char != 127) apLogFailInfo("Wrong Value", 127, _char, "")
x = char(_sbyte)
if (x != 127) apLogFailInfo("Wrong Value", 127, _char, "")
x = char(sbyte(127))
if (x != 127) apLogFailInfo("Wrong Value", 127, _char, "")

_sbyte = 0
_char = _sbyte
if (_char != 0) apLogFailInfo("Wrong Value", 0, _char, "")
_char = sbyte(0)
if (_char != 0) apLogFailInfo("Wrong Value", 0, _char, "")
x = char(_sbyte)
if (x != 0) apLogFailInfo("Wrong Value", 0, _char, "")
x = char(sbyte(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _char, "")

_sbyte = 101
_char = _sbyte
if (_char != 101) apLogFailInfo("Wrong Value", 101, _char, "")
_char = sbyte(101)
if (_char != 101) apLogFailInfo("Wrong Value", 101, _char, "")
x = char(_sbyte)
if (x != 101) apLogFailInfo("Wrong Value", 101, _char, "")
x = char(sbyte(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _char, "")
*/


apInitScenario("char <-- byte")

_byte = 255
_char = _byte
if (_char != 255) apLogFailInfo("Wrong Value", 255, _char, "")
_char = byte(255)
if (_char != 255) apLogFailInfo("Wrong Value", 255, _char, "")
x = char(_byte)
if (x != 255) apLogFailInfo("Wrong Value", 255, _char, "")
x = char(byte(255))
if (x != 255) apLogFailInfo("Wrong Value", 255, _char, "")

_byte = 0
_char = _byte
if (_char != 0) apLogFailInfo("Wrong Value", 0, _char, "")
_char = byte(0)
if (_char != 0) apLogFailInfo("Wrong Value", 0, _char, "")
x = char(_byte)
if (x != 0) apLogFailInfo("Wrong Value", 0, _char, "")
x = char(byte(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _char, "")

_byte = 101
_char = _byte
if (_char != 101) apLogFailInfo("Wrong Value", 101, _char, "")
_char = byte(101)
if (_char != 101) apLogFailInfo("Wrong Value", 101, _char, "")
x = char(_byte)
if (x != 101) apLogFailInfo("Wrong Value", 101, _char, "")
x = char(byte(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _char, "")



apInitScenario("char <-- short")
/*  PGMTODOPGM
_short = 32767
_char = _short
if (_char != 32767) apLogFailInfo("Wrong Value", 32767, _char, "")
_char = short(32767)
if (_char != 32767) apLogFailInfo("Wrong Value", 32767, _char, "")
x = char(_short)
if (x != 32767) apLogFailInfo("Wrong Value", 32767, _char, "")
x = char(short(32767))
if (x != 32767) apLogFailInfo("Wrong Value", 32767, _char, "")

_short = 0
_char = _short
if (_char != 0) apLogFailInfo("Wrong Value", 0, _char, "")
_char = short(0)
if (_char != 0) apLogFailInfo("Wrong Value", 0, _char, "")
x = char(_short)
if (x != 0) apLogFailInfo("Wrong Value", 0, _char, "")
x = char(short(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _char, "")

_short = 101
_char = _short
if (_char != 101) apLogFailInfo("Wrong Value", 101, _char, "")
_char = short(101)
if (_char != 101) apLogFailInfo("Wrong Value", 101, _char, "")
x = char(_short)
if (x != 101) apLogFailInfo("Wrong Value", 101, _char, "")
x = char(short(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _char, "")
*/


apInitScenario("char <-- ushort")

_ushort = 65535
_char = _ushort
if (_char != 65535) apLogFailInfo("Wrong Value", 65535, _char, "")
_char = ushort(65535)
if (_char != 65535) apLogFailInfo("Wrong Value", 65535, _char, "")
x = char(_ushort)
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _char, "")
x = char(ushort(65535))
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _char, "")

_ushort = 0
_char = _ushort
if (_char != 0) apLogFailInfo("Wrong Value", 0, _char, "")
_char = ushort(0)
if (_char != 0) apLogFailInfo("Wrong Value", 0, _char, "")
x = char(_ushort)
if (x != 0) apLogFailInfo("Wrong Value", 0, _char, "")
x = char(ushort(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _char, "")

_ushort = 101
_char = _ushort
if (_char != 101) apLogFailInfo("Wrong Value", 101, _char, "")
_char = ushort(101)
if (_char != 101) apLogFailInfo("Wrong Value", 101, _char, "")
x = char(_ushort)
if (x != 101) apLogFailInfo("Wrong Value", 101, _char, "")
x = char(ushort(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _char, "")



apInitScenario("char <-- int")

_int = 65535
_char = _int
if (_char != 65535) apLogFailInfo("Wrong Value", 65535, _char, "")
_char = int(65535)
if (_char != 65535) apLogFailInfo("Wrong Value", 65535, _char, "")
x = char(_int)
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _char, "")
x = char(int(65535))
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _char, "")

_int = 0
_char = _int
if (_char != 0) apLogFailInfo("Wrong Value", 0, _char, "")
_char = int(0)
if (_char != 0) apLogFailInfo("Wrong Value", 0, _char, "")
x = char(_int)
if (x != 0) apLogFailInfo("Wrong Value", 0, _char, "")
x = char(int(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _char, "")

_int = 101
_char = _int
if (_char != 101) apLogFailInfo("Wrong Value", 101, _char, "")
_char = int(101)
if (_char != 101) apLogFailInfo("Wrong Value", 101, _char, "")
x = char(_int)
if (x != 101) apLogFailInfo("Wrong Value", 101, _char, "")
x = char(int(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _char, "")



apInitScenario("char <-- uint")

_uint = 65535
_char = _uint
if (_char != 65535) apLogFailInfo("Wrong Value", 65535, _char, "")
_char = uint(65535)
if (_char != 65535) apLogFailInfo("Wrong Value", 65535, _char, "")
x = char(_uint)
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _char, "")
x = char(uint(65535))
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _char, "")

_uint = 0
_char = _uint
if (_char != 0) apLogFailInfo("Wrong Value", 0, _char, "")
_char = uint(0)
if (_char != 0) apLogFailInfo("Wrong Value", 0, _char, "")
x = char(_uint)
if (x != 0) apLogFailInfo("Wrong Value", 0, _char, "")
x = char(uint(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _char, "")

_uint = 101
_char = _uint
if (_char != 101) apLogFailInfo("Wrong Value", 101, _char, "")
_char = uint(101)
if (_char != 101) apLogFailInfo("Wrong Value", 101, _char, "")
x = char(_uint)
if (x != 101) apLogFailInfo("Wrong Value", 101, _char, "")
x = char(uint(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _char, "")



apInitScenario("char <-- long")

_long = 65535
_char = _long
if (_char != 65535) apLogFailInfo("Wrong Value", 65535, _char, "")
_char = long(65535)
if (_char != 65535) apLogFailInfo("Wrong Value", 65535, _char, "")
x = char(_long)
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _char, "")
x = char(long(65535))
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _char, "")

_long = 0
_char = _long
if (_char != 0) apLogFailInfo("Wrong Value", 0, _char, "")
_char = long(0)
if (_char != 0) apLogFailInfo("Wrong Value", 0, _char, "")
x = char(_long)
if (x != 0) apLogFailInfo("Wrong Value", 0, _char, "")
x = char(long(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _char, "")

_long = 101
_char = _long
if (_char != 101) apLogFailInfo("Wrong Value", 101, _char, "")
_char = long(101)
if (_char != 101) apLogFailInfo("Wrong Value", 101, _char, "")
x = char(_long)
if (x != 101) apLogFailInfo("Wrong Value", 101, _char, "")
x = char(long(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _char, "")



apInitScenario("char <-- ulong")

_ulong = 65535
_char = _ulong
if (_char != 65535) apLogFailInfo("Wrong Value", 65535, _char, "")
_char = ulong(65535)
if (_char != 65535) apLogFailInfo("Wrong Value", 65535, _char, "")
x = char(_ulong)
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _char, "")
x = char(ulong(65535))
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _char, "")

_ulong = 0
_char = _ulong
if (_char != 0) apLogFailInfo("Wrong Value", 0, _char, "")
_char = ulong(0)
if (_char != 0) apLogFailInfo("Wrong Value", 0, _char, "")
x = char(_ulong)
if (x != 0) apLogFailInfo("Wrong Value", 0, _char, "")
x = char(ulong(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _char, "")

_ulong = 101
_char = _ulong
if (_char != 101) apLogFailInfo("Wrong Value", 101, _char, "")
_char = ulong(101)
if (_char != 101) apLogFailInfo("Wrong Value", 101, _char, "")
x = char(_ulong)
if (x != 101) apLogFailInfo("Wrong Value", 101, _char, "")
x = char(ulong(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _char, "")



apInitScenario("char <-- char")

_char = 65535
_char = _char
if (_char != 65535) apLogFailInfo("Wrong Value", 65535, _char, "")
_char = char(65535)
if (_char != 65535) apLogFailInfo("Wrong Value", 65535, _char, "")
x = char(_char)
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _char, "")
x = char(char(65535))
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _char, "")

_char = 0
_char = _char
if (_char != 0) apLogFailInfo("Wrong Value", 0, _char, "")
_char = char(0)
if (_char != 0) apLogFailInfo("Wrong Value", 0, _char, "")
x = char(_char)
if (x != 0) apLogFailInfo("Wrong Value", 0, _char, "")
x = char(char(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _char, "")

_char = 101
_char = _char
if (_char != 101) apLogFailInfo("Wrong Value", 101, _char, "")
_char = char(101)
if (_char != 101) apLogFailInfo("Wrong Value", 101, _char, "")
x = char(_char)
if (x != 101) apLogFailInfo("Wrong Value", 101, _char, "")
x = char(char(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _char, "")



apInitScenario("char <-- decimal")

_decimal = 65535
_char = _decimal
if (_char != 65535) apLogFailInfo("Wrong Value", 65535, _char, "")
_char = decimal(65535)
if (_char != 65535) apLogFailInfo("Wrong Value", 65535, _char, "")
x = char(_decimal)
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _char, "")
x = char(decimal(65535))
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _char, "")

_decimal = 0
_char = _decimal
if (_char != 0) apLogFailInfo("Wrong Value", 0, _char, "")
_char = decimal(0)
if (_char != 0) apLogFailInfo("Wrong Value", 0, _char, "")
x = char(_decimal)
if (x != 0) apLogFailInfo("Wrong Value", 0, _char, "")
x = char(decimal(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _char, "")

_decimal = 101
_char = _decimal
if (_char != 101) apLogFailInfo("Wrong Value", 101, _char, "")
_char = decimal(101)
if (_char != 101) apLogFailInfo("Wrong Value", 101, _char, "")
x = char(_decimal)
if (x != 101) apLogFailInfo("Wrong Value", 101, _char, "")
x = char(decimal(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _char, "")



apInitScenario("decimal <-- sbyte")

_sbyte = 127
_decimal = _sbyte
if (_decimal != 127) apLogFailInfo("Wrong Value", 127, _decimal, "")
_decimal = sbyte(127)
if (_decimal != 127) apLogFailInfo("Wrong Value", 127, _decimal, "")
x = decimal(_sbyte)
if (x != 127) apLogFailInfo("Wrong Value", 127, _decimal, "")
x = decimal(sbyte(127))
if (x != 127) apLogFailInfo("Wrong Value", 127, _decimal, "")

_sbyte = -128
_decimal = _sbyte
if (_decimal != -128) apLogFailInfo("Wrong Value", -128, _decimal, "")
_decimal = sbyte(-128)
if (_decimal != -128) apLogFailInfo("Wrong Value", -128, _decimal, "")
x = decimal(_sbyte)
if (x != -128) apLogFailInfo("Wrong Value", -128, _decimal, "")
x = decimal(sbyte(-128))
if (x != -128) apLogFailInfo("Wrong Value", -128, _decimal, "")

_sbyte = 101
_decimal = _sbyte
if (_decimal != 101) apLogFailInfo("Wrong Value", 101, _decimal, "")
_decimal = sbyte(101)
if (_decimal != 101) apLogFailInfo("Wrong Value", 101, _decimal, "")
x = decimal(_sbyte)
if (x != 101) apLogFailInfo("Wrong Value", 101, _decimal, "")
x = decimal(sbyte(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _decimal, "")



apInitScenario("decimal <-- byte")

_byte = 255
_decimal = _byte
if (_decimal != 255) apLogFailInfo("Wrong Value", 255, _decimal, "")
_decimal = byte(255)
if (_decimal != 255) apLogFailInfo("Wrong Value", 255, _decimal, "")
x = decimal(_byte)
if (x != 255) apLogFailInfo("Wrong Value", 255, _decimal, "")
x = decimal(byte(255))
if (x != 255) apLogFailInfo("Wrong Value", 255, _decimal, "")

_byte = 0
_decimal = _byte
if (_decimal != 0) apLogFailInfo("Wrong Value", 0, _decimal, "")
_decimal = byte(0)
if (_decimal != 0) apLogFailInfo("Wrong Value", 0, _decimal, "")
x = decimal(_byte)
if (x != 0) apLogFailInfo("Wrong Value", 0, _decimal, "")
x = decimal(byte(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _decimal, "")

_byte = 101
_decimal = _byte
if (_decimal != 101) apLogFailInfo("Wrong Value", 101, _decimal, "")
_decimal = byte(101)
if (_decimal != 101) apLogFailInfo("Wrong Value", 101, _decimal, "")
x = decimal(_byte)
if (x != 101) apLogFailInfo("Wrong Value", 101, _decimal, "")
x = decimal(byte(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _decimal, "")



apInitScenario("decimal <-- short")

_short = 32767
_decimal = _short
if (_decimal != 32767) apLogFailInfo("Wrong Value", 32767, _decimal, "")
_decimal = short(32767)
if (_decimal != 32767) apLogFailInfo("Wrong Value", 32767, _decimal, "")
x = decimal(_short)
if (x != 32767) apLogFailInfo("Wrong Value", 32767, _decimal, "")
x = decimal(short(32767))
if (x != 32767) apLogFailInfo("Wrong Value", 32767, _decimal, "")

_short = -32768
_decimal = _short
if (_decimal != -32768) apLogFailInfo("Wrong Value", -32768, _decimal, "")
_decimal = short(-32768)
if (_decimal != -32768) apLogFailInfo("Wrong Value", -32768, _decimal, "")
x = decimal(_short)
if (x != -32768) apLogFailInfo("Wrong Value", -32768, _decimal, "")
x = decimal(short(-32768))
if (x != -32768) apLogFailInfo("Wrong Value", -32768, _decimal, "")

_short = 101
_decimal = _short
if (_decimal != 101) apLogFailInfo("Wrong Value", 101, _decimal, "")
_decimal = short(101)
if (_decimal != 101) apLogFailInfo("Wrong Value", 101, _decimal, "")
x = decimal(_short)
if (x != 101) apLogFailInfo("Wrong Value", 101, _decimal, "")
x = decimal(short(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _decimal, "")



apInitScenario("decimal <-- ushort")

_ushort = 65535
_decimal = _ushort
if (_decimal != 65535) apLogFailInfo("Wrong Value", 65535, _decimal, "")
_decimal = ushort(65535)
if (_decimal != 65535) apLogFailInfo("Wrong Value", 65535, _decimal, "")
x = decimal(_ushort)
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _decimal, "")
x = decimal(ushort(65535))
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _decimal, "")

_ushort = 0
_decimal = _ushort
if (_decimal != 0) apLogFailInfo("Wrong Value", 0, _decimal, "")
_decimal = ushort(0)
if (_decimal != 0) apLogFailInfo("Wrong Value", 0, _decimal, "")
x = decimal(_ushort)
if (x != 0) apLogFailInfo("Wrong Value", 0, _decimal, "")
x = decimal(ushort(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _decimal, "")

_ushort = 101
_decimal = _ushort
if (_decimal != 101) apLogFailInfo("Wrong Value", 101, _decimal, "")
_decimal = ushort(101)
if (_decimal != 101) apLogFailInfo("Wrong Value", 101, _decimal, "")
x = decimal(_ushort)
if (x != 101) apLogFailInfo("Wrong Value", 101, _decimal, "")
x = decimal(ushort(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _decimal, "")



apInitScenario("decimal <-- int")

_int = 2147483647
_decimal = _int
if (_decimal != 2147483647) apLogFailInfo("Wrong Value", 2147483647, _decimal, "")
_decimal = int(2147483647)
if (_decimal != 2147483647) apLogFailInfo("Wrong Value", 2147483647, _decimal, "")
x = decimal(_int)
if (x != 2147483647) apLogFailInfo("Wrong Value", 2147483647, _decimal, "")
x = decimal(int(2147483647))
if (x != 2147483647) apLogFailInfo("Wrong Value", 2147483647, _decimal, "")

_int = -2147483648
_decimal = _int
if (_decimal != -2147483648) apLogFailInfo("Wrong Value", -2147483648, _decimal, "")
_decimal = int(-2147483648)
if (_decimal != -2147483648) apLogFailInfo("Wrong Value", -2147483648, _decimal, "")
x = decimal(_int)
if (x != -2147483648) apLogFailInfo("Wrong Value", -2147483648, _decimal, "")
x = decimal(int(-2147483648))
if (x != -2147483648) apLogFailInfo("Wrong Value", -2147483648, _decimal, "")

_int = 101
_decimal = _int
if (_decimal != 101) apLogFailInfo("Wrong Value", 101, _decimal, "")
_decimal = int(101)
if (_decimal != 101) apLogFailInfo("Wrong Value", 101, _decimal, "")
x = decimal(_int)
if (x != 101) apLogFailInfo("Wrong Value", 101, _decimal, "")
x = decimal(int(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _decimal, "")



apInitScenario("decimal <-- uint")

_uint = uint.MaxValue 
_decimal = _uint
if (_decimal != uint.MaxValue ) apLogFailInfo("Wrong Value", uint.MaxValue , _decimal, "")
_decimal = uint(uint.MaxValue )
if (_decimal != uint.MaxValue ) apLogFailInfo("Wrong Value", uint.MaxValue , _decimal, "")
x = decimal(_uint)
if (x != uint.MaxValue ) apLogFailInfo("Wrong Value", uint.MaxValue , _decimal, "")
x = decimal(uint(uint.MaxValue ))
if (x != uint.MaxValue ) apLogFailInfo("Wrong Value", uint.MaxValue , _decimal, "")

_uint = 0
_decimal = _uint
if (_decimal != 0) apLogFailInfo("Wrong Value", 0, _decimal, "")
_decimal = uint(0)
if (_decimal != 0) apLogFailInfo("Wrong Value", 0, _decimal, "")
x = decimal(_uint)
if (x != 0) apLogFailInfo("Wrong Value", 0, _decimal, "")
x = decimal(uint(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _decimal, "")

_uint = 101
_decimal = _uint
if (_decimal != 101) apLogFailInfo("Wrong Value", 101, _decimal, "")
_decimal = uint(101)
if (_decimal != 101) apLogFailInfo("Wrong Value", 101, _decimal, "")
x = decimal(_uint)
if (x != 101) apLogFailInfo("Wrong Value", 101, _decimal, "")
x = decimal(uint(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _decimal, "")



apInitScenario("decimal <-- long")
/* TODOPGMTODO
_long = long.MaxValue 
_decimal = _long
if (_decimal != long.MaxValue ) apLogFailInfo("Wrong Value", long.MaxValue , _decimal, "")
_decimal = long(long.MaxValue )
if (_decimal != long.MaxValue ) apLogFailInfo("Wrong Value", long.MaxValue , _decimal, "")
x = decimal(_long)
if (x != long.MaxValue ) apLogFailInfo("Wrong Value", long.MaxValue , _decimal, "")
x = decimal(long(long.MaxValue ))
if (x != long.MaxValue ) apLogFailInfo("Wrong Value", long.MaxValue , _decimal, "")

_long = -9223372036854775808
_decimal = _long
if (_decimal != -9223372036854775808) apLogFailInfo("Wrong Value", -9223372036854775808, _decimal, "")
_decimal = long(-9223372036854775808)
if (_decimal != -9223372036854775808) apLogFailInfo("Wrong Value", -9223372036854775808, _decimal, "")
x = decimal(_long)
if (x != -9223372036854775808) apLogFailInfo("Wrong Value", -9223372036854775808, _decimal, "")
x = decimal(long(-9223372036854775808))
if (x != -9223372036854775808) apLogFailInfo("Wrong Value", -9223372036854775808, _decimal, "")

_long = 101
_decimal = _long
if (_decimal != 101) apLogFailInfo("Wrong Value", 101, _decimal, "")
_decimal = long(101)
if (_decimal != 101) apLogFailInfo("Wrong Value", 101, _decimal, "")
x = decimal(_long)
if (x != 101) apLogFailInfo("Wrong Value", 101, _decimal, "")
x = decimal(long(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _decimal, "")
*/


apInitScenario("decimal <-- ulong")

_ulong = ulong.MaxValue 
_decimal = _ulong
if (_decimal != ulong.MaxValue ) apLogFailInfo("Wrong Value", ulong.MaxValue , _decimal, "")
_decimal = ulong(ulong.MaxValue )
if (_decimal != ulong.MaxValue ) apLogFailInfo("Wrong Value", ulong.MaxValue , _decimal, "")
x = decimal(_ulong)
if (x != ulong.MaxValue ) apLogFailInfo("Wrong Value", ulong.MaxValue , _decimal, "")
x = decimal(ulong(ulong.MaxValue ))
if (x != ulong.MaxValue ) apLogFailInfo("Wrong Value", ulong.MaxValue , _decimal, "")

_ulong = 0
_decimal = _ulong
if (_decimal != 0) apLogFailInfo("Wrong Value", 0, _decimal, "")
_decimal = ulong(0)
if (_decimal != 0) apLogFailInfo("Wrong Value", 0, _decimal, "")
x = decimal(_ulong)
if (x != 0) apLogFailInfo("Wrong Value", 0, _decimal, "")
x = decimal(ulong(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _decimal, "")

_ulong = 101
_decimal = _ulong
if (_decimal != 101) apLogFailInfo("Wrong Value", 101, _decimal, "")
_decimal = ulong(101)
if (_decimal != 101) apLogFailInfo("Wrong Value", 101, _decimal, "")
x = decimal(_ulong)
if (x != 101) apLogFailInfo("Wrong Value", 101, _decimal, "")
x = decimal(ulong(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _decimal, "")



apInitScenario("decimal <-- char")

_char = 65535
_decimal = _char
if (_decimal != 65535) apLogFailInfo("Wrong Value", 65535, _decimal, "")
_decimal = char(65535)
if (_decimal != 65535) apLogFailInfo("Wrong Value", 65535, _decimal, "")
x = decimal(_char)
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _decimal, "")
x = decimal(char(65535))
if (x != 65535) apLogFailInfo("Wrong Value", 65535, _decimal, "")

_char = 0
_decimal = _char
if (_decimal != 0) apLogFailInfo("Wrong Value", 0, _decimal, "")
_decimal = char(0)
if (_decimal != 0) apLogFailInfo("Wrong Value", 0, _decimal, "")
x = decimal(_char)
if (x != 0) apLogFailInfo("Wrong Value", 0, _decimal, "")
x = decimal(char(0))
if (x != 0) apLogFailInfo("Wrong Value", 0, _decimal, "")

_char = 101
_decimal = _char
if (_decimal != 101) apLogFailInfo("Wrong Value", 101, _decimal, "")
_decimal = char(101)
if (_decimal != 101) apLogFailInfo("Wrong Value", 101, _decimal, "")
x = decimal(_char)
if (x != 101) apLogFailInfo("Wrong Value", 101, _decimal, "")
x = decimal(char(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _decimal, "")



apInitScenario("decimal <-- decimal")

_decimal = decimal.MaxValue
_decimal = _decimal
if (_decimal != decimal.MaxValue) apLogFailInfo("Wrong Value", decimal.MaxValue, _decimal, "")
_decimal = decimal(decimal.MaxValue)
if (_decimal != decimal.MaxValue) apLogFailInfo("Wrong Value", decimal.MaxValue, _decimal, "")
x = decimal(_decimal)
if (x != decimal.MaxValue) apLogFailInfo("Wrong Value", decimal.MaxValue, _decimal, "")
x = decimal(decimal(decimal.MaxValue))
if (x != decimal.MaxValue) apLogFailInfo("Wrong Value", decimal.MaxValue, _decimal, "")

_decimal = -590295810358705651712
_decimal = _decimal
if (_decimal != -590295810358705651712) apLogFailInfo("Wrong Value", -590295810358705651712, _decimal, "")
_decimal = decimal(-590295810358705651712)
if (_decimal != -590295810358705651712) apLogFailInfo("Wrong Value", -590295810358705651712, _decimal, "")
x = decimal(_decimal)
if (x != -590295810358705651712) apLogFailInfo("Wrong Value", -590295810358705651712, _decimal, "")
x = decimal(decimal(-590295810358705651712))
if (x != -590295810358705651712) apLogFailInfo("Wrong Value", -590295810358705651712, _decimal, "")

_decimal = 101
_decimal = _decimal
if (_decimal != 101) apLogFailInfo("Wrong Value", 101, _decimal, "")
_decimal = decimal(101)
if (_decimal != 101) apLogFailInfo("Wrong Value", 101, _decimal, "")
x = decimal(_decimal)
if (x != 101) apLogFailInfo("Wrong Value", 101, _decimal, "")
x = decimal(decimal(101))
if (x != 101) apLogFailInfo("Wrong Value", 101, _decimal, "")






  apEndTest();

}


explicit01();


if(lFailCount >= 0) System.Environment.ExitCode = lFailCount;
else System.Environment.ExitCode = 1;

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
