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


var iTestID = 53127;

function prcdnc03() {
    apInitTest("prcdnc03");

    // tokenizer output scenario #191
    apInitScenario(">= and ||");
    var expected = true;
    var result = (3 >= -1 || 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #192
    apInitScenario(">= and &&");
    expected = 2;
    result = (3 >= -1 && 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #193
    apInitScenario(">= and |");
    expected = 3;
    result = (3 >= -1 | 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #194
    apInitScenario(">= and ^");
    expected = 3;
    result = (3 >= -1 ^ 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #195
    apInitScenario(">= and &");
    expected = 0
    result = (3 >= -1 & 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #196
    apInitScenario(">= and ==");
    expected = false;
    result = (3 >= -1 == 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #197
    apInitScenario(">= and !=");
    expected = true;
    result = (-3 >= -1 != 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #198
    apInitScenario(">= and <");
    expected = true;
    result = (3 >= -1 < 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #199
    apInitScenario(">= and <=");
    expected = true;
    result = (3 >= -1 <= 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #200
    apInitScenario(">= and >");
    expected = false
    result = (3 >= -1 > 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #201
    apInitScenario(">= and >=");
    expected = false
    result = (3 >= -1 >= 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #202
    apInitScenario(">= and <<");
    expected = true;
    result = (3 >= -1 << 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #203
    apInitScenario(">= and >>");
    expected = true;
    result = (3 >= -1 >> 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #204
    apInitScenario(">= and >>>");
    expected = false;
    result = (3 >= -1 >>> 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #205
    apInitScenario(">= and +");
    expected = true;
    result = (3 >= -1 + 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #206
    apInitScenario(">= and -");
    expected = true;
    result = (3 >= -1 - 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #207
    apInitScenario(">= and *");
    expected = true;
    result = (3 >= -1 * 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #208
    apInitScenario(">= and /");
    expected = true;
    result = (3 >= -1 / 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #209
    apInitScenario(">= and %");
    expected = true;
    result = (3 >= -1 % 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #210
    apInitScenario("<< and ||");
    expected = -2147483648;
    result = (3 << -1 || 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #211
    apInitScenario("<< and &&");
    expected = 2;
    result = (3 << -1 && 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #212
    apInitScenario("<< and |");
    expected = -2147483646;
    result = (3 << -1 | 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #213
    apInitScenario("<< and ^");
    expected = -2147483646;
    result = (3 << -1 ^ 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #214
    apInitScenario("<< and &");
    expected = 0;
    result = (3 << -1 & 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #215
    apInitScenario("<< and ==");
    expected = false
    result = (3 << -1 == 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #216
    apInitScenario("<< and !=");
    expected = true;
    result = (3 << -1 != 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #217
    apInitScenario("<< and <");
    expected = true;
    result = (3 << -1 < 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #218
    apInitScenario("<< and <=");
    expected = true;
    result = (3 << -1 <= 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #219
    apInitScenario("<< and >");
    expected = false
    result = (3 << -1 > 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #220
    apInitScenario("<< and >=");
    expected = false
    result = (3 << -1 >= 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #221
    apInitScenario("<< and <<");
    expected = 0;
    result = (3 << -1 << 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #222
    apInitScenario("<< and >>");
    expected = -536870912;
    result = (3 << -1 >> 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #223
    apInitScenario("<< and >>>");
    expected = 536870912;
    result = (3 << -1 >>> 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #224
    apInitScenario("<< and +");
    expected = 6;
    result = (3 << -1 + 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #225
    apInitScenario("<< and -");
    expected = 1610612736;
    result = (3 << -1 - 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #226
    apInitScenario("<< and *");
    expected = -1073741824;
    result = (3 << -1 * 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #227
    apInitScenario("<< and /");
    expected = 3;
    result = (3 << -1 / 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #228
    apInitScenario("<< and %");
    expected = -2147483648;
    result = (3 << -1 % 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #229
    apInitScenario(">> and ||");
    expected = 2;
    result = (3 >> -1 || 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #230
    apInitScenario(">> and &&");
    expected = 0
    result = (3 >> -1 && 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #231
    apInitScenario(">> and |");
    expected = 2;
    result = (3 >> -1 | 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #232
    apInitScenario(">> and ^");
    expected = 2;
    result = (3 >> -1 ^ 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #233
    apInitScenario(">> and &");
    expected = 0;
    result = (3 >> -1 & 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #234
    apInitScenario(">> and ==");
    expected = false
    result = (3 >> -1 == 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #235
    apInitScenario(">> and !=");
    expected = true;
    result = (3 >> -1 != 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #236
    apInitScenario(">> and <");
    expected = true;
    result = (3 >> -1 < 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #237
    apInitScenario(">> and <=");
    expected = true;
    result = (3 >> -1 <= 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #238
    apInitScenario(">> and >");
    expected = false
    result = (3 >> -1 > 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #239
    apInitScenario(">> and >=");
    expected = false
    result = (3 >> -1 >= 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #240
    apInitScenario(">> and <<");
    expected = 0;
    result = (3 >> -1 << 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #241
    apInitScenario(">> and >>");
    expected = 0;
    result = (3 >> -1 >> 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #242
    apInitScenario(">> and >>>");
    expected = 0;
    result = (3 >> -1 >>> 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #243
    apInitScenario(">> and +");
    expected = 1;
    result = (3 >> -1 + 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #244
    apInitScenario(">> and -");
    expected = 0;
    result = (3 >> -1 - 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #245
    apInitScenario(">> and *");
    expected = 0;
    result = (3 >> -1 * 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #246
    apInitScenario(">> and /");
    expected = 3;
    result = (3 >> -1 / 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #247
    apInitScenario(">> and %");
    expected = 0;
    result = (3 >> -1 % 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #248
    apInitScenario(">>> and ||");
    expected = 2;
    result = (3 >>> -1 || 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #249
    apInitScenario(">>> and &&");
    expected = 0
    result = (3 >>> -1 && 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #250
    apInitScenario(">>> and |");
    expected = 2;
    result = (3 >>> -1 | 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #251
    apInitScenario(">>> and ^");
    expected = 2;
    result = (3 >>> -1 ^ 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #252
    apInitScenario(">>> and &");
    expected = 0;
    result = (3 >>> -1 & 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #253
    apInitScenario(">>> and ==");
    expected = false
    result = (3 >>> -1 == 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #254
    apInitScenario(">>> and !=");
    expected = true;
    result = (3 >>> -1 != 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #255
    apInitScenario(">>> and <");
    expected = true;
    result = (3 >>> -1 < 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #256
    apInitScenario(">>> and <=");
    expected = true;
    result = (3 >>> -1 <= 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #257
    apInitScenario(">>> and >");
    expected = false
    result = (3 >>> -1 > 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #258
    apInitScenario(">>> and >=");
    expected = false
    result = (3 >>> -1 >= 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #259
    apInitScenario(">>> and <<");
    expected = 0
    result = (3 >>> -1 << 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #260
    apInitScenario(">>> and >>");
    expected = 0
    result = (3 >>> -1 >> 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #261
    apInitScenario(">>> and >>>");
    expected = 0;
    result = (3 >>> -1 >>> 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #262
    apInitScenario(">>> and +");
    expected = 1;
    result = (3 >>> -1 + 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #263
    apInitScenario(">>> and -");
    expected = 0;
    result = (3 >>> -1 - 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #264
    apInitScenario(">>> and *");
    expected = 0;
    result = (3 >>> -1 * 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #265
    apInitScenario(">>> and /");
    expected = 3;
    result = (3 >>> -1 / 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #266
    apInitScenario(">>> and %");
    expected = 0;
    result = (3 >>> -1 % 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #267
    apInitScenario("+ and ||");
    expected = 2;
    result = (3 + -1 || 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #268
    apInitScenario("+ and &&");
    expected = 2;
    result = (3 + -1 && 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #269
    apInitScenario("+ and |");
    expected = 2;
    result = (3 + -1 | 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #270
    apInitScenario("+ and ^");
    expected = 0;
    result = (3 + -1 ^ 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #271
    apInitScenario("+ and &");
    expected = 2;
    result = (3 + -1 & 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #272
    apInitScenario("+ and ==");
    expected = true;
    result = (3 + -1 == 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #273
    apInitScenario("+ and !=");
    expected = false;
    result = (3 + -1 != 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #274
    apInitScenario("+ and <");
    expected = false
    result = (3 + -1 < 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #275
    apInitScenario("+ and <=");
    expected = true;
    result = (3 + -1 <= 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #276
    apInitScenario("+ and >");
    expected = false
    result = (3 + -1 > 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #277
    apInitScenario("+ and >=");
    expected = true;
    result = (3 + -1 >= 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #278
    apInitScenario("+ and <<");
    expected = 8;
    result = (3 + -1 << 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #279
    apInitScenario("+ and >>");
    expected = 0;
    result = (3 + -1 >> 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #280
    apInitScenario("+ and >>>");
    expected = 0;
    result = (3 + -1 >>> 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #281
    apInitScenario("+ and +");
    expected = 4;
    result = (3 + -1 + 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #282
    apInitScenario("+ and -");
    expected = 0;
    result = (3 + -1 - 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #283
    apInitScenario("+ and *");
    expected = 1;
    result = (3 + -1 * 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #284
    apInitScenario("+ and /");
    expected = 2.5;
    result = (3 + -1 / 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #285
    apInitScenario("+ and %");
    expected = 2;
    result = (3 + -1 % 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #286
    apInitScenario("- and ||");
    expected = 4;
    result = (3 - -1 || 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #287
    apInitScenario("- and &&");
    expected = 2;
    result = (3 - -1 && 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #288
    apInitScenario("- and |");
    expected = 6;
    result = (3 - -1 | 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #289
    apInitScenario("- and ^");
    expected = 6;
    result = (3 - -1 ^ 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #290
    apInitScenario("- and &");
    expected = 0;
    result = (3 - -1 & 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #291
    apInitScenario("- and ==");
    expected = false
    result = (3 - -1 == 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #292
    apInitScenario("- and !=");
    expected = true;
    result = (3 - -1 != 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #293
    apInitScenario("- and <");
    expected = false
    result = (3 - -1 < 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #294
    apInitScenario("- and <=");
    expected = false
    result = (3 - -1 <= 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #295
    apInitScenario("- and >");
    expected = true;
    result = (3 - -1 > 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #296
    apInitScenario("- and >=");
    expected = true;
    result = (3 - -1 >= 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #297
    apInitScenario("- and <<");
    expected = 16;
    result = (3 - -1 << 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #298
    apInitScenario("- and >>");
    expected = 1;
    result = (3 - -1 >> 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #299
    apInitScenario("- and >>>");
    expected = 1;
    result = (3 - -1 >>> 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #300
    apInitScenario("- and +");
    expected = 6;
    result = (3 - -1 + 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #301
    apInitScenario("- and -");
    expected = 2;
    result = (3 - -1 - 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #302
    apInitScenario("- and *");
    expected = 5;
    result = (3 - -1 * 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #303
    apInitScenario("- and /");
    expected = 3.5;
    result = (3 - -1 / 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #304
    apInitScenario("- and %");
    expected = 4;
    result = (3 - -1 % 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #305
    apInitScenario("* and ||");
    expected = -3;
    result = (3 * -1 || 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #306
    apInitScenario("* and &&");
    expected = 2;
    result = (3 * -1 && 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #307
    apInitScenario("* and |");
    expected = -1;
    result = (3 * -1 | 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #308
    apInitScenario("* and ^");
    expected = -1;
    result = (3 * -1 ^ 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #309
    apInitScenario("* and &");
    expected = 0;
    result = (3 * -1 & 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #310
    apInitScenario("* and ==");
    expected = false
    result = (3 * -1 == 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #311
    apInitScenario("* and !=");
    expected = true;
    result = (3 * -1 != 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #312
    apInitScenario("* and <");
    expected = true;
    result = (3 * -1 < 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #313
    apInitScenario("* and <=");
    expected = true;
    result = (3 * -1 <= 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #314
    apInitScenario("* and >");
    expected = false;
    result = (3 * -1 > 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #315
    apInitScenario("* and >=");
    expected = false
    result = (3 * -1 >= 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #316
    apInitScenario("* and <<");
    expected = -12;
    result = (3 * -1 << 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #317
    apInitScenario("* and >>");
    expected = -1;
    result = (3 * -1 >> 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #318
    apInitScenario("* and >>>");
    expected = 1073741823;
    result = (3 * -1 >>> 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #319
    apInitScenario("* and +");
    expected = -1;
    result = (3 * -1 + 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #320
    apInitScenario("* and -");
    expected = -5;
    result = (3 * -1 - 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #321
    apInitScenario("* and *");
    expected = -6;
    result = (3 * -1 * 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #322
    apInitScenario("* and /");
    expected = -1.5;
    result = (3 * -1 / 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #323
    apInitScenario("* and %");
    expected = -1;
    result = (3 * -1 % 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #324
    apInitScenario("/ and ||");
    expected = -3;
    result = (3 / -1 || 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #325
    apInitScenario("/ and &&");
    expected = 2;
    result = (3 / -1 && 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #326
    apInitScenario("/ and |");
    expected = -1;
    result = (3 / -1 | 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #327
    apInitScenario("/ and ^");
    expected = -1;
    result = (3 / -1 ^ 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #328
    apInitScenario("/ and &");
    expected = 0;
    result = (3 / -1 & 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #329
    apInitScenario("/ and ==");
    expected = false
    result = (3 / -1 == 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #330
    apInitScenario("/ and !=");
    expected = true;
    result = (3 / -1 != 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #331
    apInitScenario("/ and <");
    expected = true;
    result = (3 / -1 < 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #332
    apInitScenario("/ and <=");
    expected = true;
    result = (3 / -1 <= 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #333
    apInitScenario("/ and >");
    expected = false
    result = (3 / -1 > 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #334
    apInitScenario("/ and >=");
    expected = false
    result = (3 / -1 >= 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #335
    apInitScenario("/ and <<");
    expected = -12;
    result = (3 / -1 << 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #336
    apInitScenario("/ and >>");
    expected = -1;
    result = (3 / -1 >> 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #337
    apInitScenario("/ and >>>");
    expected = 1073741823;
    result = (3 / -1 >>> 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #338
    apInitScenario("/ and +");
    expected = -1;
    result = (3 / -1 + 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #339
    apInitScenario("/ and -");
    expected = -5;
    result = (3 / -1 - 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #340
    apInitScenario("/ and *");
    expected = -6;
    result = (3 / -1 * 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #341
    apInitScenario("/ and /");
    expected = -1.5;
    result = (3 / -1 / 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #342
    apInitScenario("/ and %");
    expected = -1;
    result = (3 / -1 % 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #343
    apInitScenario("% and ||");
    expected = 2;
    result = (3 % -1 || 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #344
    apInitScenario("% and &&");
    expected = 0
    result = (3 % -1 && 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #345
    apInitScenario("% and |");
    expected = 2;
    result = (3 % -1 | 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #346
    apInitScenario("% and ^");
    expected = 2;
    result = (3 % -1 ^ 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #347
    apInitScenario("% and &");
    expected = 0;
    result = (3 % -1 & 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #348
    apInitScenario("% and ==");
    expected = false
    result = (3 % -1 == 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #349
    apInitScenario("% and !=");
    expected = true;
    result = (3 % -1 != 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #350
    apInitScenario("% and <");
    expected = true;
    result = (3 % -1 < 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #351
    apInitScenario("% and <=");
    expected = true;
    result = (3 % -1 <= 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #352
    apInitScenario("% and >");
    expected = false
    result = (3 % -1 > 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #353
    apInitScenario("% and >=");
    expected = false
    result = (3 % -1 >= 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #354
    apInitScenario("% and <<");
    expected = 0;
    result = (3 % -1 << 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #355
    apInitScenario("% and >>");
    expected = 0;
    result = (3 % -1 >> 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #356
    apInitScenario("% and >>>");
    expected = 0;
    result = (3 % -1 >>> 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #357
    apInitScenario("% and +");
    expected = 2;
    result = (3 % -1 + 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #358
    apInitScenario("% and -");
    expected = -2;
    result = (3 % -1 - 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #359
    apInitScenario("% and *");
    expected = 0;
    result = (3 % -1 * 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #360
    apInitScenario("% and /");
    expected = 0;
    result = (3 % -1 / 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }

    // tokenizer output scenario #361
    apInitScenario("% and %");
    expected = 0;
    result = (3 % -1 % 2);
    if (result != expected) {
        apLogFailInfo("wrong result",expected,result,"");
    }


    apEndTest();
}


prcdnc03();


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
