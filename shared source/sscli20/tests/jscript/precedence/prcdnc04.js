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


var iTestID = 53128;

function prcdnc04() {

    apInitTest("prcdnc04");

    // tokenizer output scenario #1
    apInitScenario("|| and =");
    var expected = -1;
    var result = 2;
    result = -1 || 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #2
    apInitScenario("|| and +=");
    expected = 1;
    result = 2;
    result += -1 || 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #3
    apInitScenario("|| and -=");
    expected = 3;
    result = 2;
    result -= -1 || 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #4
    apInitScenario("|| and *=");
    expected = -2;
    result = 2;
    result *= -1 || 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #5
    apInitScenario("|| and /=");
    expected = -2;
    result = 2;
    result /= -1 || 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #6
    apInitScenario("|| and %=");
    expected = 0;
    result = 2;
    result %= -1 || 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #7
    apInitScenario("|| and <<=");
    expected = 4;
    result = 2;
    result <<= 1 || 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #8
    apInitScenario("|| and >>=");
    expected = 1;
    result = 2;
    result >>= 1 || 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #9
    apInitScenario("|| and >>>=");
    expected = 1;
    result = 2;
    result >>>= 1 || 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #10
    apInitScenario("|| and &=");
    expected = 0;
    result = 2;
    result &= 1 || 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #11
    apInitScenario("|| and ^=");
    expected = 3;
    result = 2;
    result ^= 1 || 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #12
    apInitScenario("|| and |=");
    expected = 3;
    result = 2;
    result |= 1 || 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #13
    apInitScenario("&& and =");
    expected = 3
    result = 2;
    result = 1 && 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #14
    apInitScenario("&& and +=");
    expected = 5;
    result = 2;
    result += 1 && 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #15
    apInitScenario("&& and -=");
    expected = -1;
    result = 2;
    result -= 1 && 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #16
    apInitScenario("&& and *=");
    expected = 6;
    result = 2;
    result *= 1 && 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #17
    apInitScenario("&& and /=");
    expected = 2/3;
    result = 2;
    result /= 1 && 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #18
    apInitScenario("&& and %=");
    expected = 2;
    result = 2;
    result %= 1 && 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #19
    apInitScenario("&& and <<=");
    expected = 16;
    result = 2;
    result <<= 1 && 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #20
    apInitScenario("&& and >>=");
    expected = 0;
    result = 2;
    result >>= 1 && 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #21
    apInitScenario("&& and >>>=");
    expected = 0;
    result = 2;
    result >>>= 1 && 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #22
    apInitScenario("&& and &=");
    expected = 2;
    result = 2;
    result &= 1 && 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #23
    apInitScenario("&& and ^=");
    expected = 1;
    result = 2;
    result ^= 1 && 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #24
    apInitScenario("&& and |=");
    expected = 3;
    result = 2;
    result |= 1 && 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #25
    apInitScenario("| and =");
    expected = 3;
    result = 2;
    result = 1 | 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #26
    apInitScenario("| and +=");
    expected = 5;
    result = 2;
    result += 1 | 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #27
    apInitScenario("| and -=");
    expected = -1;
    result = 2;
    result -= 1 | 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #28
    apInitScenario("| and *=");
    expected = 6;
    result = 2;
    result *= 1 | 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #29
    apInitScenario("| and /=");
    expected = 2/3;
    result = 2;
    result /= 1 | 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #30
    apInitScenario("| and %=");
    expected = 2;
    result = 2;
    result %= 1 | 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #31
    apInitScenario("| and <<=");
    expected = 16;
    result = 2;
    result <<= 1 | 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #32
    apInitScenario("| and >>=");
    expected = 0;
    result = 2;
    result >>= 1 | 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #33
    apInitScenario("| and >>>=");
    expected = 0;
    result = 2;
    result >>>= 1 | 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #34
    apInitScenario("| and &=");
    expected = 2;
    result = 2;
    result &= 1 | 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #35
    apInitScenario("| and ^=");
    expected = 1;
    result = 2;
    result ^= 1 | 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #36
    apInitScenario("| and |=");
    expected = 3;
    result = 2;
    result |= 1 | 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #37
    apInitScenario("^ and =");
    expected = 2;
    result = 2;
    result = 1 ^ 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #38
    apInitScenario("^ and +=");
    expected = 4;
    result = 2;
    result += 1 ^ 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #39
    apInitScenario("^ and -=");
    expected = 0;
    result = 2;
    result -= 1 ^ 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #40
    apInitScenario("^ and *=");
    expected = 4;
    result = 2;
    result *= 1 ^ 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #41
    apInitScenario("^ and /=");
    expected = 1;
    result = 2;
    result /= 1 ^ 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #42
    apInitScenario("^ and %=");
    expected = 0;
    result = 2;
    result %= 1 ^ 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #43
    apInitScenario("^ and <<=");
//    expected = 536870912;
    expected = 8;
    result = 2;
    result <<= 1 ^ 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #44
    apInitScenario("^ and >>=");
    expected = 0;
    result = 2;
    result >>= 1 ^ 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #45
    apInitScenario("^ and >>>=");
    expected = 0;
    result = 2;
    result >>>= 1 ^ 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #46
    apInitScenario("^ and &=");
    expected = 2;
    result = 2;
    result &= 1 ^ 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #47
    apInitScenario("^ and ^=");
    expected = 0;
    result = 2;
    result ^= 1 ^ 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #48
    apInitScenario("^ and |=");
    expected = 2;
    result = 2;
    result |= 1 ^ 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #49
    apInitScenario("& and =");
    expected = 1;
    result = 2;
    result = 1 & 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #50
    apInitScenario("& and +=");
    expected = 3;
    result = 2;
    result += 1 & 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #51
    apInitScenario("& and -=");
    expected = 1;
    result = 2;
    result -= 1 & 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #52
    apInitScenario("& and *=");
    expected = 2;
    result = 2;
    result *= 1 & 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #53
    apInitScenario("& and /=");
    expected = 1;
    result = 2;
    result /= 10 & 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #54
    apInitScenario("& and %=");
    expected = 2;
    result = 2;
    result %= 11 & 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #55
    apInitScenario("& and <<=");
    expected = 4;
    result = 2;
    result <<= 1 & 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #56
    apInitScenario("& and >>=");
    expected = 1;
    result = 2;
    result >>= 1 & 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #57
    apInitScenario("& and >>>=");
    expected = 1;
    result = 2;
    result >>>= 1 & 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #58
    apInitScenario("& and &=");
    expected = 0;
    result = 2;
    result &= 1 & 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #59
    apInitScenario("& and ^=");
    expected = 3;
    result = 2;
    result ^= 1 & 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #60
    apInitScenario("& and |=");
    expected = 3;
    result = 2;
    result |= 1 & 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #61
    apInitScenario("== and =");
    expected = false;
    result = 2;
    result = 1 == 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #62
    apInitScenario("== and +=");
    expected = 2;
    result = 2;
    result += 1 == 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #63
    apInitScenario("== and -=");
    expected = 2;
    result = 2;
    result -= 1 == 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #64
    apInitScenario("== and *=");
    expected = 0;
    result = 2;
    result *= 1 == 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #65
    apInitScenario("== and /=");
    expected = 2/false;
    result = 2;
    result /= 1 == -1;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #66
    apInitScenario("== and %=");
//    expected = 2 % false;
    result = 2;
    result %= 1 == -1;
    if (!isNaN(result)) {
        apLogFailInfo("wrong result","NaN",result,"");
    }

    // tokenizer output scenario #67
    apInitScenario("== and <<=");
    expected = 2;
    result = 2;
    result <<= 1 == 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #68
    apInitScenario("== and >>=");
    expected = 2;
    result = 2;
    result >>= 1 == 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #69
    apInitScenario("== and >>>=");
    expected = 2;
    result = 2;
    result >>>= 1 == 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #70
    apInitScenario("== and &=");
    expected = 0;
    result = 2;
    result &= 1 == 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #71
    apInitScenario("== and ^=");
    expected = 2;
    result = 2;
    result ^= 1 == 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #72
    apInitScenario("== and |=");
    expected = 2;
    result = 2;
    result |= 1 == 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #73
    apInitScenario("!= and =");
    expected = true;
    result = 2;
    result = 1 != 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #74
    apInitScenario("!= and +=");
    expected = 3;
    result = 2;
    result += 1 != 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #75
    apInitScenario("!= and -=");
    expected = 1;
    result = 2;
    result -= 1 != 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #76
    apInitScenario("!= and *=");
    expected = 2;
    result = 2;
    result *= 1 != 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #77
    apInitScenario("!= and /=");
    expected = 2;
    result = 2;
    result /= 1 != 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #78
    apInitScenario("!= and %=");
    expected = 0;
    result = 2;
    result %= 1 != 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #79
    apInitScenario("!= and <<=");
    expected = 4;
    result = 2;
    result <<= 1 != 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #80
    apInitScenario("!= and >>=");
    expected = 1;
    result = 2;
    result >>= 1 != 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #81
    apInitScenario("!= and >>>=");
    expected = 1;
    result = 2;
    result >>>= 1 != 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #82
    apInitScenario("!= and &=");
    expected = 0;
    result = 2;
    result &= 1 != 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #83
    apInitScenario("!= and ^=");
    expected = 3;
    result = 2;
    result ^= 1 != 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #84
    apInitScenario("!= and |=");
    expected = 3;
    result = 2;
    result |= 1 != 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #85
    apInitScenario("< and =");
    expected = true;
    result = 2;
    result = 1 < 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #86
    apInitScenario("< and +=");
    expected = 3;
    result = 2;
    result += 1 < 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #87
    apInitScenario("< and -=");
    expected = 1;
    result = 2;
    result -= 1 < 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #88
    apInitScenario("< and *=");
    expected = 2;
    result = 2;
    result *= 1 < 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #89
    apInitScenario("< and /=");
    expected = 2;
    result = 2;
    result /= 1 < 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #90
    apInitScenario("< and %=");
    expected = 0;
    result = 2;
    result %= 1 < 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #91
    apInitScenario("< and <<=");
    expected = 4;
    result = 2;
    result <<= 1 < 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #92
    apInitScenario("< and >>=");
    expected = 1;
    result = 2;
    result >>= 1 < 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #93
    apInitScenario("< and >>>=");
    expected = 1;
    result = 2;
    result >>>= 1 < 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #94
    apInitScenario("< and &=");
    expected = 0;
    result = 2;
    result &= 1 < 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #95
    apInitScenario("< and ^=");
    expected = 3;
    result = 2;
    result ^= 1 < 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #96
    apInitScenario("< and |=");
    expected = 3;
    result = 2;
    result |= 1 < 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #97
    apInitScenario("<= and =");
    expected = true;
    result = 2;
    result = 1 <= 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #98
    apInitScenario("<= and +=");
    expected = 3;
    result = 2;
    result += 1 <= 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #99
    apInitScenario("<= and -=");
    expected = 1;
    result = 2;
    result -= 1 <= 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #100
    apInitScenario("<= and *=");
    expected = 2;
    result = 2;
    result *= 1 <= 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #101
    apInitScenario("<= and /=");
    expected = 2;
    result = 2;
    result /= 1 <= 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #102
    apInitScenario("<= and %=");
    expected = 0;
    result = 2;
    result %= 1 <= 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #103
    apInitScenario("<= and <<=");
    expected = 4;
    result = 2;
    result <<= 1 <= 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #104
    apInitScenario("<= and >>=");
    expected = 1;
    result = 2;
    result >>= 1 <= 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #105
    apInitScenario("<= and >>>=");
    expected = 1;
    result = 2;
    result >>>= 1 <= 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #106
    apInitScenario("<= and &=");
    expected = 0;
    result = 2;
    result &= 1 <= 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #107
    apInitScenario("<= and ^=");
    expected = 3;
    result = 2;
    result ^= 1 <= 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #108
    apInitScenario("<= and |=");
    expected = 3;
    result = 2;
    result |= 1 <= 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #109
    apInitScenario("> and =");
    var expected = false;
    var result = 2;
    result = -1 > 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #110
    apInitScenario("> and +=");
    expected = 2;
    result = 2;
    result += -1 > 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #111
    apInitScenario("> and -=");
    expected = 2;
    result = 2;
    result -= -1 > 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #112
    apInitScenario("> and *=");
    expected = 0;
    result = 2;
    result *= -1 > 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #113
    apInitScenario("> and /=");
    expected = 1/0;
    result = 2;
    result /= -1 > 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #114
    apInitScenario("> and %=");
//    expected = 1/0;
    result = 2;
    result %= -1 > 3;
//    if (result != expected) {
    if (!(isNaN(result))) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    apEndTest();
}


prcdnc04();


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
