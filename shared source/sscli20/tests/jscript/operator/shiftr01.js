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


var iTestID = 230592;

///////////////////////////////////////////////////////////////////////////////////
//



//        this tescase tests the >> operator
	




              


function shiftr01() {
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

apInitScenario("sbyte >> byte")
_byte = 6
_sbyte = 110
_tempsbyte = 1
if ((_sbyte >> _byte) !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte >> _byte, "")

_byte = 243
_sbyte = 29
_tempsbyte = 3
if ((_sbyte >> _byte) !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte >> _byte, "")



apInitScenario("sbyte >> short")
_short = 2
_sbyte = 4
_tempsbyte = 1
if ((_sbyte >> _short) !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte >> _short, "")

_short = -6
_sbyte = 38
_tempsbyte = 0
if ((_sbyte >> -_short) !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte >> -_short, "")

_short = 10483
_sbyte = 19
_tempsbyte = 2
if ((_sbyte >> _short) !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte >> _short, "")



apInitScenario("sbyte >> ushort")
_ushort = 5
_sbyte = 37
_tempsbyte = 1
if ((_sbyte >> _ushort) !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte >> _ushort, "")

_ushort = 24343
_sbyte = 55
_tempsbyte = 0
if ((_sbyte >> _ushort) !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte >> _ushort, "")



apInitScenario("sbyte >> int")
_int = 7
_sbyte = 18
_tempsbyte = 0
if ((_sbyte >> _int) !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte >> _int, "")

_int = -5
_sbyte = 17
_tempsbyte = 0
if ((_sbyte >> -_int) !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte >> -_int, "")

_int = 1918913811
_sbyte = 68
_tempsbyte = 8
if ((_sbyte >> _int) !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte >> _int, "")



apInitScenario("sbyte >> uint")
_uint = 5
_sbyte = 76
_tempsbyte = 2
if ((_sbyte >> _uint) !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte >> _uint, "")

_uint = 321466832
_sbyte = 77
_tempsbyte = 77
if ((_sbyte >> _uint) !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte >> _uint, "")



apInitScenario("sbyte >> long")
_long = 6
_sbyte = 66
_tempsbyte = 1
if ((_sbyte >> _long) !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte >> _long, "")

_long = -7
_sbyte = 47
_tempsbyte = 0
if ((_sbyte >> -_long) !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte >> -_long, "")

_long = 309234737763139000
_sbyte = 93
_tempsbyte = 93
if ((_sbyte >> _long) !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte >> _long, "")



apInitScenario("sbyte >> ulong")
_ulong = 1
_sbyte = 100
_tempsbyte = 50
if ((_sbyte >> _ulong) !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte >> _ulong, "")

_ulong = 13981649092356400000
_sbyte = 8
_tempsbyte = 8
if ((_sbyte >> _ulong) !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte >> _ulong, "")



apInitScenario("sbyte >> char")
_char = 1
_sbyte = 121
_tempsbyte = 60
if ((_sbyte >> _char) !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte >> _char, "")

_char = 44020
_sbyte = 108
_tempsbyte = 6
if ((_sbyte >> _char) !== _tempsbyte) apLogFailInfo("Wrong Value", _tempsbyte, _sbyte >> _char, "")



apInitScenario("short >> sbyte")
_sbyte = 3
_short = 2595
_tempshort = 324
if ((_short >> _sbyte) !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short >> _sbyte, "")

_sbyte = -4
_short = 16773
_tempshort = 1048
if ((_short >> -_sbyte) !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short >> -_sbyte, "")

_sbyte = 97
_short = 20012
_tempshort = 10006
if ((_short >> _sbyte) !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short >> _sbyte, "")



apInitScenario("short >> byte")
_byte = 6
_short = 10807
_tempshort = 168
if ((_short >> _byte) !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short >> _byte, "")

_byte = 143
_short = 16744
_tempshort = 0
if ((_short >> _byte) !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short >> _byte, "")



apInitScenario("short >> ushort")
_ushort = 10
_short = 31677
_tempshort = 30
if ((_short >> _ushort) !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short >> _ushort, "")

_ushort = 10512
_short = 10555
_tempshort = 10555
if ((_short >> _ushort) !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short >> _ushort, "")



apInitScenario("short >> int")
_int = 7
_short = 14744
_tempshort = 115
if ((_short >> _int) !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short >> _int, "")

_int = -12
_short = 7792
_tempshort = 1
if ((_short >> -_int) !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short >> -_int, "")

_int = 907229180
_short = 16838
_tempshort = 4
if ((_short >> _int) !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short >> _int, "")



apInitScenario("short >> uint")
_uint = 1
_short = 17661
_tempshort = 8830
if ((_short >> _uint) !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short >> _uint, "")

_uint = 1044023914
_short = 30673
_tempshort = 29
if ((_short >> _uint) !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short >> _uint, "")



apInitScenario("short >> long")
_long = 1
_short = 20392
_tempshort = 10196
if ((_short >> _long) !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short >> _long, "")

_long = -15
_short = 13604
_tempshort = 0
if ((_short >> -_long) !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short >> -_long, "")

_long = 9013338704234390000
_short = 8942
_tempshort = 8942
if ((_short >> _long) !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short >> _long, "")



apInitScenario("short >> ulong")
_ulong = 6
_short = 13281
_tempshort = 207
if ((_short >> _ulong) !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short >> _ulong, "")

_ulong = 13032091565763800000
_short = 23504
_tempshort = 23504
if ((_short >> _ulong) !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short >> _ulong, "")



apInitScenario("short >> char")
_char = 2
_short = 9874
_tempshort = 2468
if ((_short >> _char) !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short >> _char, "")

_char = 31093
_short = 20505
_tempshort = 640
if ((_short >> _char) !== _tempshort) apLogFailInfo("Wrong Value", _tempshort, _short >> _char, "")



apInitScenario("int >> sbyte")
_sbyte = 10
_int = 1752017113
_tempint = 1710954
if ((_int >> _sbyte) !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int >> _sbyte, "")

_sbyte = -25
_int = 1721647504
_tempint = 51
if ((_int >> -_sbyte) !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int >> -_sbyte, "")

_sbyte = 0
_int = 1065107601
_tempint = 1065107601
if ((_int >> _sbyte) !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int >> _sbyte, "")



apInitScenario("int >> byte")
_byte = 4
_int = 1804760895
_tempint = 112797555
if ((_int >> _byte) !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int >> _byte, "")

_byte = 217
_int = 1559652255
_tempint = 46
if ((_int >> _byte) !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int >> _byte, "")



apInitScenario("int >> short")
_short = 23
_int = 1001945775
_tempint = 119
if ((_int >> _short) !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int >> _short, "")

_short = -14
_int = 281242207
_tempint = 17165
if ((_int >> -_short) !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int >> -_short, "")

_short = 7581
_int = 1234118039
_tempint = 2
if ((_int >> _short) !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int >> _short, "")



apInitScenario("int >> ushort")
_ushort = 31
_int = 278531726
_tempint = 0
if ((_int >> _ushort) !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int >> _ushort, "")

_ushort = 15288
_int = 3370786
_tempint = 0
if ((_int >> _ushort) !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int >> _ushort, "")



apInitScenario("int >> uint")
_uint = 29
_int = 724132687
_tempint = 1
if ((_int >> _uint) !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int >> _uint, "")

_uint = 177709776
_int = 816691038
_tempint = 12461
if ((_int >> _uint) !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int >> _uint, "")



apInitScenario("int >> long")
_long = 30
_int = 528005064
_tempint = 0
if ((_int >> _long) !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int >> _long, "")

_long = -16
_int = 2052448478
_tempint = 31317
if ((_int >> -_long) !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int >> -_long, "")

_long = 557837121532702000
_int = 1704652872
_tempint = 26010
if ((_int >> _long) !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int >> _long, "")



apInitScenario("int >> ulong")
_ulong = 19
_int = 1013075105
_tempint = 1932
if ((_int >> _ulong) !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int >> _ulong, "")

_ulong = 7817574904588080000
_int = 1317433782
_tempint = 1317433782
if ((_int >> _ulong) !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int >> _ulong, "")



apInitScenario("int >> char")
_char = 0
_int = 1242434162
_tempint = 1242434162
if ((_int >> _char) !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int >> _char, "")

_char = 22302
_int = 1842234262
_tempint = 1
if ((_int >> _char) !== _tempint) apLogFailInfo("Wrong Value", _tempint, _int >> _char, "")



apInitScenario("uint >> sbyte")
_sbyte = 0
_uint = 3679156605
_tempuint = 3679156605
if ((_uint >> _sbyte) !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint >> _sbyte, "")

_sbyte = -4
_uint = 1055748084
_tempuint = 65984255
if ((_uint >> -_sbyte) !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint >> -_sbyte, "")

_sbyte = 55
_uint = 2293431691
_tempuint = 4294967057
if ((_uint >> _sbyte) !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint >> _sbyte, "")



apInitScenario("uint >> byte")
_byte = 22
_uint = 3079476133
_tempuint = 4294967006
if ((_uint >> _byte) !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint >> _byte, "")

_byte = 179
_uint = 3627304751
_tempuint = 4294966022
if ((_uint >> _byte) !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint >> _byte, "")



apInitScenario("uint >> short")
_short = 30
_uint = 4076829137
_tempuint = 4294967295
if ((_uint >> _short) !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint >> _short, "")

_short = -24
_uint = 1002021102
_tempuint = 59
if ((_uint >> -_short) !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint >> -_short, "")

_short = 8910
_uint = 1059355326
_tempuint = 64657
if ((_uint >> _short) !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint >> _short, "")



apInitScenario("uint >> ushort")
_ushort = 9
_uint = 4218819993
_tempuint = 4294818570
if ((_uint >> _ushort) !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint >> _ushort, "")

_ushort = 50755
_uint = 1797928174
_tempuint = 224741021
if ((_uint >> _ushort) !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint >> _ushort, "")



apInitScenario("uint >> int")
_int = 26
_uint = 3249024843
_tempuint = 4294967280
if ((_uint >> _int) !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint >> _int, "")

_int = -31
_uint = 766728374
_tempuint = 0
if ((_uint >> -_int) !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint >> -_int, "")

_int = 1674879319
_uint = 231650766
_tempuint = 27
if ((_uint >> _int) !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint >> _int, "")



apInitScenario("uint >> long")
_long = 3
_uint = 1153631400
_tempuint = 144203925
if ((_uint >> _long) !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint >> _long, "")

_long = -1
_uint = 3658143419
_tempuint = 3976555357
if ((_uint >> -_long) !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint >> -_long, "")

_long = 986500550367482000
_uint = 2803812215
_tempuint = 2803812215
if ((_uint >> _long) !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint >> _long, "")



apInitScenario("uint >> ulong")
_ulong = 7
_uint = 606558228
_tempuint = 4738736
if ((_uint >> _ulong) !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint >> _ulong, "")

_ulong = 2045098715311720000
_uint = 1617776412
_tempuint = 1617776412
if ((_uint >> _ulong) !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint >> _ulong, "")



apInitScenario("uint >> char")
_char = 27
_uint = 2956618283
_tempuint = 4294967286
if ((_uint >> _char) !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint >> _char, "")

_char = 59344
_uint = 2179245381
_tempuint = 4294935012
if ((_uint >> _char) !== _tempuint) apLogFailInfo("Wrong Value", _tempuint, _uint >> _char, "")



apInitScenario("long >> sbyte")
_sbyte = 43
_long = 2295798932908320000
_templong = 261002
if ((_long >> _sbyte) !== _templong) apLogFailInfo("Wrong Value", _templong, _long >> _sbyte, "")

_sbyte = -34
_long = 6098399099785290000
_templong = 354973546
if ((_long >> -_sbyte) !== _templong) apLogFailInfo("Wrong Value", _templong, _long >> -_sbyte, "")

_sbyte = 0
_long = 1026285249060680000
_templong = 1026285249060680000
if ((_long >> _sbyte) !== _templong) apLogFailInfo("Wrong Value", _templong, _long >> _sbyte, "")



apInitScenario("long >> byte")
_byte = 1
_long = 3540829868638280000
_templong = 1770414934319140000
if ((_long >> _byte) !== _templong) apLogFailInfo("Wrong Value", _templong, _long >> _byte, "")

_byte = 51
_long = 1633839211227980000
_templong = 725
if ((_long >> _byte) !== _templong) apLogFailInfo("Wrong Value", _templong, _long >> _byte, "")



apInitScenario("long >> short")
_short = 20
_long = 384950617475334000
_templong = 367117516970
if ((_long >> _short) !== _templong) apLogFailInfo("Wrong Value", _templong, _long >> _short, "")

_short = -51
_long = 8501154018472780000
_templong = 3775
if ((_long >> -_short) !== _templong) apLogFailInfo("Wrong Value", _templong, _long >> -_short, "")

_short = 722
_long = 2419879610434600000
_templong = 9231108133066
if ((_long >> _short) !== _templong) apLogFailInfo("Wrong Value", _templong, _long >> _short, "")



apInitScenario("long >> ushort")
_ushort = 55
_long = 9005716730713300000
_templong = 249
if ((_long >> _ushort) !== _templong) apLogFailInfo("Wrong Value", _templong, _long >> _ushort, "")

_ushort = 30532
_long = 3483969406465420000
_templong = 217748087904088750
if ((_long >> _ushort) !== _templong) apLogFailInfo("Wrong Value", _templong, _long >> _ushort, "")



apInitScenario("long >> int")
_int = 27
_long = 4004401473116240000
_templong = 29835115917
if ((_long >> _int) !== _templong) apLogFailInfo("Wrong Value", _templong, _long >> _int, "")

_int = -43
_long = 7840149171102000000
_templong = 891321
if ((_long >> -_int) !== _templong) apLogFailInfo("Wrong Value", _templong, _long >> -_int, "")

_int = 1583964817
_long = 2755029455024400000
_templong = 21019206657595
if ((_long >> _int) !== _templong) apLogFailInfo("Wrong Value", _templong, _long >> _int, "")



apInitScenario("long >> uint")
_uint = 45
_long = 5750858716922400000
_templong = 163449
if ((_long >> _uint) !== _templong) apLogFailInfo("Wrong Value", _templong, _long >> _uint, "")

_uint = 525859452
_long = 8257182331004960000
_templong = 7
if ((_long >> _uint) !== _templong) apLogFailInfo("Wrong Value", _templong, _long >> _uint, "")



apInitScenario("long >> ulong")
_ulong = 25
_long = 5534649912817220000
_templong = 164945421004
if ((_long >> _ulong) !== _templong) apLogFailInfo("Wrong Value", _templong, _long >> _ulong, "")

_ulong = 1415568685380810000
_long = 2463333546935130000
_templong = 37587486983263
if ((_long >> _ulong) !== _templong) apLogFailInfo("Wrong Value", _templong, _long >> _ulong, "")



apInitScenario("long >> char")
_char = 47
_long = 5413069258445690000
_templong = 38462
if ((_long >> _char) !== _templong) apLogFailInfo("Wrong Value", _templong, _long >> _char, "")

_char = 13917
_long = 3034970548720160000
_templong = 5653073170
if ((_long >> _char) !== _templong) apLogFailInfo("Wrong Value", _templong, _long >> _char, "")



apInitScenario("ulong >> sbyte")
_sbyte = 11
_ulong = 12334533308281700000
_tempulong = 18443759595796745047
if ((_ulong >> _sbyte) !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong >> _sbyte, "")

_sbyte = -1
_ulong = 13377087597114400000
_tempulong = 15911915835411975808
if ((_ulong >> -_sbyte) !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong >> -_sbyte, "")

_sbyte = 8
_ulong = 15968139422654000000
_tempulong = 18437062024291365867
if ((_ulong >> _sbyte) !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong >> _sbyte, "")



apInitScenario("ulong >> byte")
_byte = 33
_ulong = 1203099865784290000
_tempulong = 140059258
if ((_ulong >> _byte) !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong >> _byte, "")

_byte = 217
_ulong = 5767822600798920000
_tempulong = 171894508624
if ((_ulong >> _byte) !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong >> _byte, "")



apInitScenario("ulong >> short")
_short = 7
_ulong = 15652083568152100000
_tempulong = 18424910788509884025
if ((_ulong >> _short) !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong >> _short, "")

_short = -32
_ulong = 16053246836773800000
_tempulong = 18446744073152272113
if ((_ulong >> -_short) !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong >> -_short, "")

_short = 7291
_ulong = 9736186403476990000
_tempulong = 18446744073709551600
if ((_ulong >> _short) !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong >> _short, "")



apInitScenario("ulong >> ushort")
_ushort = 5
_ulong = 4489651645857980000
_tempulong = 140301613933061875
if ((_ulong >> _ushort) !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong >> _ushort, "")

_ushort = 37662
_ulong = 12803558151133500000
_tempulong = 18446744068453925173
if ((_ulong >> _ushort) !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong >> _ushort, "")



apInitScenario("ulong >> int")
_int = 59
_ulong = 13014742672428700000
_tempulong = 18446744073709551606
if ((_ulong >> _int) !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong >> _int, "")

_int = -25
_ulong = 7481018131790680000
_tempulong = 222951714151
if ((_ulong >> -_int) !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong >> -_int, "")

_int = 561483971
_ulong = 5618710931169290000
_tempulong = 702338866396161250
if ((_ulong >> _int) !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong >> _int, "")



apInitScenario("ulong >> uint")
_uint = 49
_ulong = 13372979684232200000
_tempulong = 18446744073709542603
if ((_ulong >> _uint) !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong >> _uint, "")

_uint = 4144636127
_ulong = 9906050435967540000
_tempulong = 18446744069732481222
if ((_ulong >> _uint) !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong >> _uint, "")



apInitScenario("ulong >> long")
_long = 11
_ulong = 8027540995200450000
_tempulong = 3919697751562719
if ((_ulong >> _long) !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong >> _long, "")

_long = -3
_ulong = 9911920977659060000
_tempulong = 17379891186703240164
if ((_ulong >> -_long) !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong >> -_long, "")

_long = 7398208838707930000
_ulong = 14706254528440600000
_tempulong = 18446686998368589870
if ((_ulong >> _long) !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong >> _long, "")



apInitScenario("ulong >> char")
_char = 22
_ulong = 12907867324660600000
_tempulong = 18446742753138427289
if ((_ulong >> _char) !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong >> _char, "")

_char = 23946
_ulong = 14915780161566600000
_tempulong = 18443295866764099514
if ((_ulong >> _char) !== _tempulong) apLogFailInfo("Wrong Value", _tempulong, _ulong >> _char, "")



apInitScenario("char >> short")
_short = 2
_char = 32616
_tempchar = 8154
if ((_char >> _short) !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char >> _short, "")

_short = -0
_char = 33965
_tempchar = 33965
if ((_char >> -_short) !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char >> -_short, "")

_short = 13285
_char = 30720
_tempchar = 960
if ((_char >> _short) !== _tempchar) apLogFailInfo("Wrong Value", _tempchar, _char >> _short, "")




  apEndTest();
}


shiftr01();


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
