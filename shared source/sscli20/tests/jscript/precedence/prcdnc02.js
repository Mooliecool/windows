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


var iTestID = 53126;

function prcdnc02() {
    apInitTest("prcdnc02");

    // tokenizer output scenario #1
    apInitScenario("|| and ||");
    var expected = 3;
    var result = (3 || -1 || 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #2
    apInitScenario("|| and &&");
    expected = 3;
    result = (3 || -1 && 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #3
    apInitScenario("|| and |");
    expected = 3;
    result = (3 || -1 | 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #4
    apInitScenario("|| and ^");
    expected = 3;
    result = (3 || -1 ^ 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #5
    apInitScenario("|| and &");
    expected = 3;
    result = (3 || -1 & 0);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #6
    apInitScenario("|| and ==");
    expected = 3;
    result = (3 || -1 == 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #7
    apInitScenario("|| and !=");
    expected = 3;
    result = (3 || -1 != 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #8
    apInitScenario("|| and <");
    expected = 3;
    result = (3 || -1 < 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #9
    apInitScenario("|| and <=");
    expected = 3;
    result = (3 || -1 <= 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #10
    apInitScenario("|| and >");
    expected = 3;
    result = (3 || -1 > 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #11
    apInitScenario("|| and >=");
    expected = 3;
    result = (3 || -1 >= 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #12
    apInitScenario("|| and <<");
    expected = 3;
    result = (3 || -1 << 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #13
    apInitScenario("|| and >>");
    expected = 3;
    result = (3 || -1 >> 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #14
    apInitScenario("|| and >>>");
    expected = 3;
    result = (3 || -1 >>> 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #15
    apInitScenario("|| and +");
    expected = 3;
    result = (3 || -1 + 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #16
    apInitScenario("|| and -");
    expected = 3;
    result = (3 || -1 - 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #17
    apInitScenario("|| and *");
    expected = 3;
    result = (3 || -1 * 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #18
    apInitScenario("|| and /");
    expected = 3;
    result = (3 || -1 / 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #19
    apInitScenario("|| and %");
    expected = 3;
    result = (3 || -1 % 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #20
    apInitScenario("&& and ||");
    expected = -1;
    result = (0 && -1 || -1);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #21
    apInitScenario("&& and &&");
    expected = 2;
    result = (3 && -1 && 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #22
    apInitScenario("&& and |");
    expected = -1;
    result = (3 && -1 | 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #23
    apInitScenario("&& and ^");
    expected = -3;
    result = (3 && -1 ^ 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #24
    apInitScenario("&& and &");
    expected = 0;
    result = (3 && 0 & 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #25
    apInitScenario("&& and ==");
    expected = false;
    result = (3 && -1 == 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #26
    apInitScenario("&& and !=");
    expected = true;
    result = (3 && -1 != 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #27
    apInitScenario("&& and <");
    expected = true;
    result = (3 && -1 < 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #28
    apInitScenario("&& and <=");
    expected = true;
    result = (3 && -1 <= 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #29
    apInitScenario("&& and >");
    expected = false;
    result = (3 && -1 > 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #30
    apInitScenario("&& and >=");
    expected = false;
    result = (3 && -1 >= 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #31
    apInitScenario("&& and <<");
    expected = -4;
    result = (3 && -1 << 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #32
    apInitScenario("&& and >>");
    expected = -1;
    result = (3 && -1 >> 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #33
    apInitScenario("&& and >>>");
    expected = 1073741823;
    result = (3 && -1 >>> 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #34
    apInitScenario("&& and +");
    expected = 1;
    result = (3 && -1 + 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #35
    apInitScenario("&& and -");
    expected = -3;
    result = (3 && -1 - 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #36
    apInitScenario("&& and *");
    expected = -2;
    result = (3 && -1 * 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #37
    apInitScenario("&& and /");
    expected = -.5;
    result = (3 && -1 / 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #38
    apInitScenario("&& and %");
    expected = 2;
    result = (3 && 5 % 3);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #39
    apInitScenario("| and ||");
    expected = -1;
    result = (3 | -1 || 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #40
    apInitScenario("| and &&");
    expected = 2;
    result = (3 | -1 && 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #41
    apInitScenario("| and |");
    expected = -1;
    result = (3 | -1 | 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #42
    apInitScenario("| and ^");
    expected = -1;
    result = (3 | -1 ^ 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #43
    apInitScenario("| and &");
    expected = 3;
    result = (3 | -1 & 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #44
    apInitScenario("| and ==");
    expected = 3;
    result = (3 | -1 == 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #45
    apInitScenario("| and !=");
    expected = 3;
    result = (3 | -1 != 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #46
    apInitScenario("| and <");
    expected = 3;
    result = (3 | -1 < 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #47
    apInitScenario("| and <=");
    expected = 3;
    result = (3 | -1 <= 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #48
    apInitScenario("| and >");
    expected = 3;
    result = (3 | -1 > 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #49
    apInitScenario("| and >=");
    expected = 3;
    result = (3 | -1 >= 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #50
    apInitScenario("| and <<");
    expected = -1;
    result = (3 | -1 << 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #51
    apInitScenario("| and >>");
    expected = -1;
    result = (3 | -1 >> 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #52
    apInitScenario("| and >>>");
    expected = 1073741823;
    result = (3 | -1 >>> 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #53
    apInitScenario("| and +");
    expected = 3;
    result = (3 | -1 + 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #54
    apInitScenario("| and -");
    expected = -1;
    result = (3 | -1 - 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #55
    apInitScenario("| and *");
    expected = -1;
    result = (3 | -1 * 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #56
    apInitScenario("| and /");
    expected = 3;
    result = (3 | -1 / 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #57
    apInitScenario("| and %");
    expected = -1;
    result = (3 | -1 % 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #58
    apInitScenario("^ and ||");
    expected = -4;
    result = (3 ^ -1 || 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #59
    apInitScenario("^ and &&");
    expected = 2;
    result = (3 ^ -1 && 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #60
    apInitScenario("^ and |");
    expected = -2;
    result = (3 ^ -1 | 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #61
    apInitScenario("^ and ^");
    expected = -2;
    result = (3 ^ -1 ^ 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #62
    apInitScenario("^ and &");
    expected = 1;
    result = (3 ^ -1 & 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"VBE 24775");
    }

    // tokenizer output scenario #63
    apInitScenario("^ and ==");
    expected = 3;
    result = (3 ^ -1 == 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #64
    apInitScenario("^ and !=");
    expected = 2;
    result = (3 ^ -1 != 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #65
    apInitScenario("^ and <");
    expected = 2;
    result = (3 ^ -1 < 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #66
    apInitScenario("^ and <=");
    expected = 2;
    result = (3 ^ -1 <= 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #67
    apInitScenario("^ and >");
    expected = 3;
    result = (3 ^ -1 > 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #68
    apInitScenario("^ and >=");
    expected = 3;
    result = (3 ^ -1 >= 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #69
    apInitScenario("^ and <<");
    expected = -1;
    result = (3 ^ -1 << 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #70
    apInitScenario("^ and >>");
    expected = -4;
    result = (3 ^ -1 >> 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #71
    apInitScenario("^ and >>>");
    expected = 1073741820;
    result = (3 ^ -1 >>> 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #72
    apInitScenario("^ and +");
    expected = 2;
    result = (3 ^ -1 + 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #73
    apInitScenario("^ and -");
    expected = -2;
    result = (3 ^ -1 - 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #74
    apInitScenario("^ and *");
    expected = -3;
    result = (3 ^ -1 * 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #75
    apInitScenario("^ and /");
    expected = 3;
    result = (3 ^ -1 / 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #76
    apInitScenario("^ and %");
    expected = -4;
    result = (3 ^ -1 % 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #77
    apInitScenario("& and ||");
    expected = 3;
    result = (3 & -1 || 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #78
    apInitScenario("& and &&");
    expected = 2;
    result = (3 & -1 && 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #79
    apInitScenario("& and |");
    expected = 3;
    result = (3 & -1 | 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #80
    apInitScenario("& and ^");
    expected = 1;
    result = (3 & -1 ^ 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #81
    apInitScenario("& and &");
    expected = 2;
    result = (3 & -1 & 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #82
    apInitScenario("& and ==");
    expected = 0;
    result = (3 & -1 == 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #83
    apInitScenario("& and !=");
    expected = 1;
    result = (3 & -1 != 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #84
    apInitScenario("& and <");
    expected = 1;
    result = (3 & -1 < 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #85
    apInitScenario("& and <=");
    expected = 1;
    result = (3 & -1 <= 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #86
    apInitScenario("& and >");
    expected = 0;
    result = (3 & -1 > 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #87
    apInitScenario("& and >=");
    expected = 0;
    result = (3 & -1 >= 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #88
    apInitScenario("& and <<");
    expected = 0;
    result = (3 & -1 << 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #89
    apInitScenario("& and >>");
    expected = 3;
    result = (3 & -1 >> 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #90
    apInitScenario("& and >>>");
    expected = 3;
    result = (3 & -1 >>> 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #91
    apInitScenario("& and +");
    expected = 1;
    result = (3 & -1 + 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #92
    apInitScenario("& and -");
    expected = 1;
    result = (3 & -1 - 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #93
    apInitScenario("& and *");
    expected = 2;
    result = (3 & -1 * 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #94
    apInitScenario("& and /");
    expected = 0;
    result = (3 & -1 / 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #95
    apInitScenario("& and %");
    expected = 3;
    result = (3 & -1 % 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #96
    apInitScenario("== and ||");
    expected = 2;
    result = (3 == -1 || 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #97
    apInitScenario("== and &&");
    expected = false;
    result = (3 == -1 && 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #98
    apInitScenario("== and |");
    expected = 2;
    result = (3 == -1 | 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #99
    apInitScenario("== and ^");
    expected = 2;
    result = (3 == -1 ^ 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #100
    apInitScenario("== and &");
    expected = 0;
    result = (3 == -1 & 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #101
    apInitScenario("== and ==");
    expected = false;
    result = (3 == -1 == 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #102
    apInitScenario("== and !=");
    expected = true;
    result = (3 == -1 != 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #103
    apInitScenario("== and <");
    expected = false;
    result = (3 == -1 < 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #104
    apInitScenario("== and <=");
    expected = false;
    result = (3 == -1 <= 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #105
    apInitScenario("== and >");
    expected = false;
    result = (3 == -1 > 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #106
    apInitScenario("== and >=");
    expected = false;
    result = (3 == -1 >= 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #107
    apInitScenario("== and <<");
    expected = false;
    result = (3 == -1 << 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #108
    apInitScenario("== and >>");
    expected = false;
    result = (3 == -1 >> 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #109
    apInitScenario("== and >>>");
    expected = false;
    result = (3 == -1 >>> 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #110
    apInitScenario("== and +");
    expected = false;
    result = (3 == -1 + 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #111
    apInitScenario("== and -");
    expected = true;
    result = (-3 == -1 - 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #112
    apInitScenario("== and *");
    expected = false;
    result = (3 == -1 * 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #113
    apInitScenario("== and /");
    expected = false;
    result = (3 == -1 / 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #114
    apInitScenario("== and %");
    expected = false;
    result = (3 == -1 % 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #115
    apInitScenario("!= and ||");
    expected = true;
    result = (3 != -1 || 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #116
    apInitScenario("!= and &&");
    expected = 2;
    result = (3 != -1 && 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #117
    apInitScenario("!= and |");
    expected = 3;
    result = (3 != -1 | 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #118
    apInitScenario("!= and ^");
    expected = 3;
    result = (3 != -1 ^ 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #119
    apInitScenario("!= and &");
    expected = 0;
    result = (3 != -1 & 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #120
    apInitScenario("!= and ==");
    expected = false;
    result = (3 != -1 == 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #121
    apInitScenario("!= and !=");
    expected = true;
    result = (3 != -1 != 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #122
    apInitScenario("!= and <");
    expected = true;
    result = (3 != -1 < 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #123
    apInitScenario("!= and <=");
    expected = true;
    result = (3 != -1 <= 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #124
    apInitScenario("!= and >");
    expected = true;
    result = (3 != -1 > 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #125
    apInitScenario("!= and >=");
    expected = true;
    result = (3 != -1 >= 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #126
    apInitScenario("!= and <<");
    expected = true;
    result = (3 != -1 << 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #127
    apInitScenario("!= and >>");
    expected = true;
    result = (3 != -1 >> 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #128
    apInitScenario("!= and >>>");
    expected = true;
    result = (3 != -1 >>> 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #129
    apInitScenario("!= and +");
    expected = true;
    result = (3 != -1 + 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #130
    apInitScenario("!= and -");
    expected = false;
    result = (-3 != -1 - 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #131
    apInitScenario("!= and *");
    expected = true;
    result = (3 != -1 * 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #132
    apInitScenario("!= and /");
    expected = true;
    result = (3 != -1 / 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #133
    apInitScenario("!= and %");
    expected = true;
    result = (3 != -1 % 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #134
    apInitScenario("< and ||");
    expected = 2;
    result = (3 < -1 || 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #135
    apInitScenario("< and &&");
    expected = false;
    result = (3 < -1 && 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #136
    apInitScenario("< and |");
    expected = 2;
    result = (3 < -1 | 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #137
    apInitScenario("< and ^");
    expected = 2;
    result = (3 < -1 ^ 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #138
    apInitScenario("< and &");
    expected = 0;
    result = (3 < -1 & 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #139
    apInitScenario("< and ==");
    expected = false;
    result = (-3 < -1 == 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #140
    apInitScenario("< and !=");
    expected = true;
    result = (1 < 3 != 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #141
    apInitScenario("< and <");
    expected = true;
    result = (3 < -1 < 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #142
    apInitScenario("< and <=");
    expected = true;
    result = (3 < -1 <= 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #143
    apInitScenario("< and >");
    expected = false;
    result = (-3 < -1 > 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #144
    apInitScenario("< and >=");
    expected = false;
    result = (-3 < -1 >= 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #145
    apInitScenario("< and <<");
    expected = false;
    result = (3 < -1 << 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #146
    apInitScenario("< and >>");
    expected = false;
    result = (3 < -1 >> 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #147
    apInitScenario("< and >>>");
    expected = true;
    result = (3 < -1 >>> 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #148
    apInitScenario("< and +");
    expected = true;
    result = (-3 < -1 + 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #149
    apInitScenario("< and -");
    expected = false;
    result = (-3 < -1 - 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #150
    apInitScenario("< and *");
    expected = true;
    result = (-3 < -1 * 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #151
    apInitScenario("< and /");
    expected = true;
    result = (-3 < -1 / 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #152
    apInitScenario("< and %");
    expected = true;
    result = (-3 < -1 % 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #153
    apInitScenario("<= and ||");
    expected = 2;
    result = (3 <= -1 || 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #154
    apInitScenario("<= and &&");
    expected = false;
    result = (3 <= -1 && 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #155
    apInitScenario("<= and |");
    expected = 2;
    result = (3 <= -1 | 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #156
    apInitScenario("<= and ^");
    expected = 2;
    result = (3 <= -1 ^ 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #157
    apInitScenario("<= and &");
    expected = 0;
    result = (3 <= -1 & 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #158
    apInitScenario("<= and ==");
    expected = false;
    result = (-3 <= -1 == 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #159
    apInitScenario("<= and !=");
    expected = false;
    result = (-3 <= -1 != 1);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #160
    apInitScenario("<= and <");
    expected = true;
    result = (3 <= -1 < 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #161
    apInitScenario("<= and <=");
    expected = true;
    result = (3 <= -1 <= 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #162
    apInitScenario("<= and >");
    expected = false;
    result = (3 <= -1 > 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #163
    apInitScenario("<= and >=");
    expected = false;
    result = (3 <= -1 >= 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #164
    apInitScenario("<= and <<");
    expected = false;
    result = (3 <= -1 << 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #165
    apInitScenario("<= and >>");
    expected = false;
    result = (3 <= -1 >> 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #166
    apInitScenario("<= and >>>");
    expected = true;
    result = (3 <= -1 >>> 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #167
    apInitScenario("<= and +");
    expected = false;
    result = (3 <= -1 + 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #168
    apInitScenario("<= and -");
    expected = false;
    result = (3 <= -1 - 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #169
    apInitScenario("<= and *");
    expected = false;
    result = (3 <= -1 * 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #170
    apInitScenario("<= and /");
    expected = false;
    result = (3 <= -1 / 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #171
    apInitScenario("<= and %");
    expected = false;
    result = (3 <= -1 % 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #172
    apInitScenario("> and ||");
    expected = true;
    result = (3 > -1 || 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #173
    apInitScenario("> and &&");
    expected = 2;
    result = (3 > -1 && 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #174
    apInitScenario("> and |");
    expected = 3;
    result = (3 > -1 | 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #175
    apInitScenario("> and ^");
    expected = 3;
    result = (3 > -1 ^ 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #176
    apInitScenario("> and &");
    expected = 0;
    result = (3 > -1 & 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #177
    apInitScenario("> and ==");
    expected = false;
    result = (3 > -1 == 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #178
    apInitScenario("> and !=");
    expected = true;
    result = (3 > -1 != 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #179
    apInitScenario("> and <");
    expected = true;
    result = (3 > -1 < 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #180
    apInitScenario("> and <=");
    expected = true;
    result = (3 > -1 <= 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #181
    apInitScenario("> and >");
    expected = false;
    result = (3 > -1 > 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #182
    apInitScenario("> and >=");
    expected = false;
    result = (3 > -1 >= 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #183
    apInitScenario("> and <<");
    expected = true;
    result = (3 > -1 << 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #184
    apInitScenario("> and >>");
    expected = true;
    result = (3 > -1 >> 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #185
    apInitScenario("> and >>>");
    expected = false;
    result = (3 > -1 >>> 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #186
    apInitScenario("> and +");
    expected = true;
    result = (3 > -1 + 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #187
    apInitScenario("> and -");
    expected = true;
    result = (3 > -1 - 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #188
    apInitScenario("> and *");
    expected = true;
    result = (3 > -1 * 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #189
    apInitScenario("> and /");
    expected = true;
    result = (3 > -1 / 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #190
    apInitScenario("> and %");
    expected = true;
    result = (3 > -1 % 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    apEndTest();
}


prcdnc02();


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
