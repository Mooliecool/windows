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


var iTestID = 53130;

function prcdnc06() {

    apInitTest("prcdnc06");
    // continuation of prcdnc04

    // tokenizer output scenario #115
    apInitScenario("> and <<=");
    var expected = 2; //add var for expected and result. -qiongou.
    var result = 2;
    result <<= -1 > 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #116
    apInitScenario("> and >>=");
    expected = 2;
    result = 2;
    result >>= -1 > 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #117
    apInitScenario("> and >>>=");
    expected = 2;
    result = 2;
    result >>>= -1 > 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #118
    apInitScenario("> and &=");
    expected = 0;
    result = 2;
    result &= -1 > 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #119
    apInitScenario("> and ^=");
    expected = 2;
    result = 2;
    result ^= -1 > 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #120
    apInitScenario("> and |=");
    expected = 2;
    result = 2;
    result |= -1 > 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #121
    apInitScenario(">= and =");
    expected = false;
    result = 2;
    result = -1 >= 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #122
    apInitScenario(">= and +=");
    expected = 2;
    result = 2;
    result += -1 >= 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #123
    apInitScenario(">= and -=");
    expected = 2;
    result = 2;
    result -= -1 >= 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #124
    apInitScenario(">= and *=");
    expected = 0;
    result = 2;
    result *= -1 >= 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #125
    apInitScenario(">= and /=");
    expected = 1/0;
    result = 2;
    result /= -1 >= 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #126
    apInitScenario(">= and %=");
    result = 2;
    result %= -1 >= 3;
    if (!isNaN(result)) {
        apLogFailInfo("wrong result","NaN",result,"");
    }

    // tokenizer output scenario #127
    apInitScenario(">= and <<=");
    expected = 2;
    result = 2;
    result <<= -1 >= 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #128
    apInitScenario(">= and >>=");
    expected = 2;
    result = 2;
    result >>= -1 >= 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #129
    apInitScenario(">= and >>>=");
    expected = 2;
    result = 2;
    result >>>= -1 >= 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #130
    apInitScenario(">= and &=");
    expected = 0;
    result = 2;
    result &= -1 >= 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #131
    apInitScenario(">= and ^=");
    expected = 2;
    result = 2;
    result ^= -1 >= 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #132
    apInitScenario(">= and |=");
    expected = 2;
    result = 2;
    result |= -1 >= 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #133
    apInitScenario("<< and =");
    expected = -8;
    result = 2;
    result = -1 << 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #134
    apInitScenario("<< and +=");
    expected = -6;
    result = 2;
    result += -1 << 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #135
    apInitScenario("<< and -=");
    expected = 10;
    result = 2;
    result -= -1 << 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #136
    apInitScenario("<< and *=");
    expected = -16;
    result = 2;
    result *= -1 << 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #137
    apInitScenario("<< and /=");
    expected = -.25;
    result = 2;
    result /= -1 << 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #138
    apInitScenario("<< and %=");
    expected = 2;
    result = 2;
    result %= -1 << 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #139
    apInitScenario("<< and <<=");
    expected = 33554432;
    result = 2;
    result <<= -1 << 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #140
    apInitScenario("<< and >>=");
    expected = 0;
    result = 2;
    result >>= -1 << 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #141
    apInitScenario("<< and >>>=");
    expected = 0;
    result = 2;
    result >>>= -1 << 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #142
    apInitScenario("<< and &=");
    expected = 0;
    result = 2;
    result &= -1 << 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #143
    apInitScenario("<< and ^=");
    expected = -6;
    result = 2;
    result ^= -1 << 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #144
    apInitScenario("<< and |=");
    expected = -6;
    result = 2;
    result |= -1 << 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #145
    apInitScenario(">> and =");
    expected = -1;
    result = 2;
    result = -1 >> 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #146
    apInitScenario(">> and +=");
    expected = 1;
    result = 2;
    result += -1 >> 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #147
    apInitScenario(">> and -=");
    expected = 3;
    result = 2;
    result -= -1 >> 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #148
    apInitScenario(">> and *=");
    expected = -2;
    result = 2;
    result *= -1 >> 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #149
    apInitScenario(">> and /=");
    expected = -2;
    result = 2;
    result /= -1 >> 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #150
    apInitScenario(">> and %=");
    expected = 0;
    result = 2;
    result %= -1 >> 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #151
    apInitScenario(">> and <<=");
    expected = 0;
    result = 2;
    result <<= -1 >> 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #152
    apInitScenario(">> and >>=");
    expected = 0;
    result = 2;
    result >>= -1 >> 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #153
    apInitScenario(">> and >>>=");
    expected = 0;
    result = 2;
    result >>>= -1 >> 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #154
    apInitScenario(">> and &=");
    expected = 2;
    result = 2;
    result &= -1 >> 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #155
    apInitScenario(">> and ^=");
    expected = -3;
    result = 2;
    result ^= -1 >> 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #156
    apInitScenario(">> and |=");
    expected = -1;
    result = 2;
    result |= -1 >> 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #157
    apInitScenario(">>> and =");
    expected = 536870911;
    result = 2;
    result = -1 >>> 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #158
    apInitScenario(">>> and +=");
    expected = 536870913;
    result = 2;
    result += -1 >>> 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #159
    apInitScenario(">>> and -=");
    expected = -536870909;
    result = 2;
    result -= -1 >>> 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #160
    apInitScenario(">>> and *=");
    expected = 536870911*2;
    result = 2;
    result *= -1 >>> 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #161
    apInitScenario(">>> and /=");
    expected = 2/536870911;
    result = 2;
    result /= -1 >>> 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #162
    apInitScenario(">>> and %=");
    expected = 2;
    result = 2;
    result %= -1 >>> 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #163
    apInitScenario(">>> and <<=");
    expected = 0;
    result = 2;
    result <<= -1 >>> 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #164
    apInitScenario(">>> and >>=");
    expected = 0;
    result = 2;
    result >>= -1 >>> 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #165
    apInitScenario(">>> and >>>=");
    expected = 0;
    result = 2;
    result >>>= -1 >>> 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #166
    apInitScenario(">>> and &=");
    expected = 2;
    result = 2;
    result &= -1 >>> 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #167
    apInitScenario(">>> and ^=");
    expected = 536870909;
    result = 2;
    result ^= -1 >>> 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #168
    apInitScenario(">>> and |=");
    expected = 536870911;
    result = 2;
    result |= -1 >>> 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #169
    apInitScenario("+ and =");
    expected = 2;
    result = 2;
    result = -1 + 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #170
    apInitScenario("+ and +=");
    expected = 4;
    result = 2;
    result += -1 + 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #171
    apInitScenario("+ and -=");
    expected = 0;
    result = 2;
    result -= -1 + 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #172
    apInitScenario("+ and *=");
    expected = 4;
    result = 2;
    result *= -1 + 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #173
    apInitScenario("+ and /=");
    expected = 1;
    result = 2;
    result /= -1 + 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #174
    apInitScenario("+ and %=");
    expected = 0;
    result = 2;
    result %= -1 + 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #175
    apInitScenario("+ and <<=");
    expected = 8;
    result = 2;
    result <<= -1 + 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #176
    apInitScenario("+ and >>=");
    expected = 0;
    result = 2;
    result >>= -1 + 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #177
    apInitScenario("+ and >>>=");
    expected = 0;
    result = 2;
    result >>>= -1 + 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #178
    apInitScenario("+ and &=");
    expected = 2;
    result = 2;
    result &= -1 + 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #179
    apInitScenario("+ and ^=");
    expected = 0;
    result = 2;
    result ^= -1 + 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #180
    apInitScenario("+ and |=");
    expected = 2;
    result = 2;
    result |= -1 + 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #181
    apInitScenario("- and =");
    expected = -4;
    result = 2;
    result = -1 - 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #182
    apInitScenario("- and +=");
    expected = -2;
    result = 2;
    result += -1 - 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #183
    apInitScenario("- and -=");
    expected = 6;
    result = 2;
    result -= -1 - 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #184
    apInitScenario("- and *=");
    expected = -8;
    result = 2;
    result *= -1 - 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #185
    apInitScenario("- and /=");
    expected = -.5;
    result = 2;
    result /= -1 - 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #186
    apInitScenario("- and %=");
    expected = 2;
    result = 2;
    result %= -1 - 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #187
    apInitScenario("- and <<=");
    expected = 536870912;
    result = 2;
    result <<= -1 - 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #188
    apInitScenario("- and >>=");
    expected = 0;
    result = 2;
    result >>= -1 - 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #189
    apInitScenario("- and >>>=");
    expected = 0;
    result = 2;
    result >>>= -1 - 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #190
    apInitScenario("- and &=");
    expected = 0;
    result = 2;
    result &= -1 - 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #191
    apInitScenario("- and ^=");
    expected = -2;
    result = 2;
    result ^= -1 - 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #192
    apInitScenario("- and |=");
    expected = -2;
    result = 2;
    result |= -1 - 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #193
    apInitScenario("* and =");
    expected = -3;
    result = 2;
    result = -1 * 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #194
    apInitScenario("* and +=");
    expected = -1;
    result = 2;
    result += -1 * 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #195
    apInitScenario("* and -=");
    expected = 5;
    result = 2;
    result -= -1 * 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #196
    apInitScenario("* and *=");
    expected = -6;
    result = 2;
    result *= -1 * 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #197
    apInitScenario("* and /=");
    expected = -2/3;
    result = 2;
    result /= -1 * 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #198
    apInitScenario("* and %=");
    expected = 2;
    result = 2;
    result %= -1 * 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #199
    apInitScenario("* and <<=");
    expected = 1073741824;
    result = 2;
    result <<= -1 * 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #200
    apInitScenario("* and >>=");
    expected = 0;
    result = 2;
    result >>= -1 * 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #201
    apInitScenario("* and >>>=");
    expected = 0;
    result = 2;
    result >>>= -1 * 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #202
    apInitScenario("* and &=");
    expected = 0;
    result = 2;
    result &= -1 * 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #203
    apInitScenario("* and ^=");
    expected = -1;
    result = 2;
    result ^= -1 * 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #204
    apInitScenario("* and |=");
    expected = -1;
    result = 2;
    result |= -1 * 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #205
    apInitScenario("/ and =");
    expected = (-(1/3));
    result = 2;
    result = -1 / 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #206
    apInitScenario("/ and +=");
    expected = 5/3;
    result = 2;
    result += -1 / 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #207
    apInitScenario("/ and -=");
    expected = 7/3;
    result = 2;
    result -= -1 / 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #208
    apInitScenario("/ and *=");
    expected = -2/3;
    result = 2;
    result *= -1 / 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #209
    apInitScenario("/ and /=");
    expected = -6;
    result = 2;
    result /= -1 / 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #210
    apInitScenario("/ and %=");
    expected = 2 % (-1/3);
    result = 2;
    result %= -1 / 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #211
    apInitScenario("/ and <<=");
    expected = 2;
    result = 2;
    result <<= -1 / 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #212
    apInitScenario("/ and >>=");
    expected = 2;
    result = 2;
    result >>= -1 / 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #213
    apInitScenario("/ and >>>=");
    expected = 2;
    result = 2;
    result >>>= -1 / 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #214
    apInitScenario("/ and &=");
    expected = 0;
    result = 2;
    result &= -1 / 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #215
    apInitScenario("/ and ^=");
    expected = 2;
    result = 2;
    result ^= -1 / 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #216
    apInitScenario("/ and |=");
    expected = 2;
    result = 2;
    result |= -1 / 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #217
    apInitScenario("% and =");
    expected = -1;
    result = 2;
    result = -1 % 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #218
    apInitScenario("% and +=");
    expected = 1;
    result = 2;
    result += -1 % 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #219
    apInitScenario("% and -=");
    expected = 3;
    result = 2;
    result -= -1 % 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #220
    apInitScenario("% and *=");
    expected = -2;
    result = 2;
    result *= -1 % 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #221
    apInitScenario("% and /=");
    expected = -2;
    result = 2;
    result /= -1 % 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #222
    apInitScenario("% and %=");
    expected = 0;
    result = 2;
    result %= -1 % 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #223
    apInitScenario("% and <<=");
    expected = 0;
    result = 2;
    result <<= -1 % 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #224
    apInitScenario("% and >>=");
    expected = 0;
    result = 2;
    result >>= -1 % 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #225
    apInitScenario("% and >>>=");
    expected = 0;
    result = 2;
    result >>>= -1 % 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #226
    apInitScenario("% and &=");
    expected = 2;
    result = 2;
    result &= -1 % 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #227
    apInitScenario("% and ^=");
    expected = -3;
    result = 2;
    result ^= -1 % 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #228
    apInitScenario("% and |=");
    expected = -1;
    result = 2;
    result |= -1 % 3;
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }


    apEndTest();
}


prcdnc06();


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
