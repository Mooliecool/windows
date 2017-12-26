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


var iTestID = 52696;


function verify( ob, sExp, bugNum) {
    if (bugNum == null) bugNum = "";

    var sAct = Math.floor(ob);

    if (sAct != sExp)
      if ((Math.abs((sAct - sExp) / sAct) > 5e-17))
        apLogFailInfo( m_scen+" failed", sExp, sAct,bugNum);
}

var m_scen = "";

function floor012() {

    apInitTest("floor012 ");

    //----------------------------------------------------------------------------
    apInitScenario("1. min pos");

    m_scen = "min pos";

    verify("2.2250738585072014e-308", 0, null);


    //----------------------------------------------------------------------------
    apInitScenario("2. min pos < n < max pos, up to full display precision");
    
    m_scen = "min pos < n < max pos, up to full display precision";

    verify(".6", 0, null);
    verify("2.", 2, null);
    verify(".44", 0, null);
    verify("7.2", 7, null);
    verify("14.", 14, null);
    verify(".786", 0, null);
    verify("7.03", 7, null);
    verify("86.6", 86, null);
    verify("135.", 135, null);
    verify(".1198", 0, null);
    verify("0.287", 0, null);
    verify("17.55", 17, null);
    verify("540.0", 540, null);
    verify("6076.", 6076, null);
    verify(".62153", 0, null);
    verify("2.6611", 2, null);
    verify("96.121", 96, null);
    verify("489.20", 489, null);
    verify("5499.2", 5499, null);
    verify("90415.", 90415, null);
    verify(".155358", 0, null);
    verify("2.02021", 2, null);
    verify("94.1046", 94, null);
    verify("564.804", 564, null);
    verify("7918.40", 7918, null);
    verify("68048.8", 68048, null);
    verify("213309.", 213309, null);
    verify(".1226468", 0, null);
    verify("9.544780", 9, null);
    verify("88.08627", 88, null);
    verify("772.8569", 772, null);
    verify("7405.900", 7405, null);
    verify("52981.86", 52981, null);
    verify("557085.8", 557085, null);
    verify("2446900.", 2446900, null);
    verify(".72709566", 0, null);
    verify("5.2120043", 5, null);
    verify("13.252459", 13, null);
    verify("241.43360", 241, null);
    verify("4458.9784", 4458, null);
    verify("80312.218", 80312, null);
    verify("696010.94", 696010, null);
    verify("9958380.8", 9958380, null);
    verify("78504937.", 78504937, null);
    verify(".143470961", 0, null);
    verify("1.23601932", 1, null);
    verify("39.0912734", 39, null);
    verify("463.555346", 463, null);
    verify("1312.74929", 1312, null);
    verify("98224.9972", 98224, null);
    verify("595098.763", 595098, null);
    verify("7061195.21", 7061195, null);
    verify("75903211.0", 75903211, null);
    verify("690839080.", 690839080, null);
    verify(".8807708514", 0, null);
    verify("4.088911435", 4, null);
    verify("75.27016363", 75, null);
    verify("561.0251120", 561, null);
    verify("5562.421819", 5562, null);
    verify("48013.26405", 48013, null);
    verify("318622.0868", 318622, null);
    verify("2255066.563", 2255066, null);
    verify("63141035.99", 63141035, null);
    verify("152643838.4", 152643838, null);
    verify("4132283254.", 4132283254, null);
    verify(".90337464930", 0, null);
    verify("2.5474295081", 2, null);
    verify("56.356969717", 56, null);
    verify("197.23794979", 197, null);
    verify("7226.6664991", 7226, null);
    verify("47583.278459", 47583, null);
    verify("155763.59386", 155763, null);
    verify("6237632.6977", 6237632, null);
    verify("45252125.874", 45252125, null);
    verify("475920249.94", 475920249, null);
    verify("5531669963.8", 5531669963, null);
    verify("71875996973.", 71875996973, null);
    verify("72860084283", 72860084283, null);
    verify(".148158349250", 0, null);
    verify("5.10823278330", 5, null);
    verify("86.1765834983", 86, null);
    verify("410.944670926", 410, null);
    verify("3101.12061200", 3101, null);
    verify("52944.3618810", 52944, null);
    verify("207709.254881", 207709, null);
    verify("4977900.91535", 4977900, null);
    verify("79846204.7014", 79846204, null);
    verify("792294719.622", 792294719, null);
    verify("1030065957.74", 1030065957, null);
    verify("22745604335.8", 22745604335, null);
    verify("978322384890.", 978322384890, null);
    verify("943471893780", 943471893780, null);
    verify(".2071476641772", 0, null);
    verify("5.197179852767", 5, null);
    verify("28.92491713587", 28, null);
    verify("213.3537603013", 213, null);
    verify("7105.498363543", 7105, null);
    verify("53209.09272198", 53209, null);
    verify("365883.2780820", 365883, null);
    verify("1824205.070693", 1824205, null);
    verify("83667863.76719", 83667863, null);
    verify("587065498.1415", 587065498, null);
    verify("1234297163.026", 1234297163, null);
    verify("69903937280.88", 69903937280, null);
    verify("507570635327.0", 507570635327, null);
    verify("7159818530489.", 7159818530489, null);
    verify("9415642957298", 9415642957298, null);
    verify(".68920778576504", 0, null);
    verify("2.5348921384159", 2, null);
    verify("78.064458756760", 78, null);
    verify("413.92148252314", 413, null);
    verify("3291.2624079456", 3291, null);
    verify("72462.406537462", 72462, null);
    verify("660928.11304554", 660928, null);
    verify("9090729.6320133", 9090729, null);
    verify("96018819.280453", 96018819, null);
    verify("496567687.42381", 496567687, null);
    verify("6216971651.0665", 6216971651, null);
    verify("38220781319.655", 38220781319, null);
    verify("253064691100.61", 253064691100, null);
    verify("9068799484665.9", 9068799484665, null);
    verify("35400069447487.", 35400069447487, null);
    verify("43823967881364", 43823967881364, null);
    verify(".727869124134605", 0, null);
    verify("7.89049944596633", 7, null);
    verify("38.8539718544432", 38, null);
    verify("386.440145639707", 386, null);
    verify("9941.26468948541", 9941, null);
    verify("15637.3266253974", 15637, null);
    verify("553929.976613362", 553929, null);
    verify("6322851.30065633", 6322851, null);
    verify("63872380.3346230", 63872380, null);
    verify("376480639.494894", 376480639, null);
    verify("1209785286.94289", 1209785286, null);
    verify("27835156131.2802", 27835156131, null);
    verify("981668226611.797", 981668226611, null);
    verify("6716339086274.29", 6716339086274, null);
    verify("31018751905191.5", 31018751905191, null);
    verify("690931313397259.", 690931313397259, null);
    verify("225264066871575", 225264066871575, null);


    //----------------------------------------------------------------------------
    apInitScenario("3. min pos < n < 1, e notation");

    m_scen = "min pos < n < 1, e notation";

    verify("3.2e-0", 3, null);
    verify("3.2e-1", 0, null);
    verify("3.2e-2", 0, null);
    verify("4.85e-1", 0, null);
    verify("4.85e-2", 0, null);
    verify("4.85e-3", 0, null);
    verify("8.498e-2", 0, null);
    verify("8.498e-3", 0, null);
    verify("8.498e-4", 0, null);
    verify("2.3424e-3", 0, null);
    verify("2.3424e-4", 0, null);
    verify("2.3424e-5", 0, null);
    verify("6.44322e-4", 0, null);
    verify("6.44322e-5", 0, null);
    verify("6.44322e-6", 0, null);
    verify("3.162709e-5", 0, null);
    verify("3.162709e-6", 0, null);
    verify("3.162709e-7", 0, null);
    verify("8.3030443e-6", 0, null);
    verify("8.3030443e-7", 0, null);
    verify("8.3030443e-8", 0, null);
    verify("2.86347466e-7", 0, null);
    verify("2.86347466e-8", 0, null);
    verify("2.86347466e-9", 0, null);
    verify("4.251479468e-8", 0, null);
    verify("4.251479468e-9", 0, null);
    verify("4.251479468e-10", 0, null);
    verify("1.7389808272e-9", 0, null);
    verify("1.7389808272e-10", 0, null);
    verify("1.7389808272e-11", 0, null);
    verify("5.95856989484e-10", 0, null);
    verify("5.95856989484e-11", 0, null);
    verify("5.95856989484e-12", 0, null);
    verify("5.312540051362e-11", 0, null);
    verify("5.312540051362e-12", 0, null);
    verify("5.312540051362e-13", 0, null);
    verify("3.7016780183062e-12", 0, null);
    verify("3.7016780183062e-13", 0, null);
    verify("3.7016780183062e-14", 0, null);
    verify("8.24431554355400e-13", 0, null);
    verify("8.24431554355400e-14", 0, null);
    verify("8.24431554355400e-15", 0, null);
    verify("6.24673773175102e-14", 0, null);
    verify("6.24673773175102e-15", 0, null);
    verify("6.24673773175102e-16", 0, null);
    verify("3.20960934796270e-15", 0, null);
    verify("3.20960934796270e-16", 0, null);
    verify("3.20960934796270e-17", 0, null);
    verify("6.70088883684615e-16", 0, null);
    verify("6.70088883684615e-17", 0, null);
    verify("6.70088883684615e-18", 0, null);
    verify("7.76359016862755e-17", 0, null);
    verify("7.76359016862755e-18", 0, null);
    verify("7.76359016862755e-19", 0, null);
    verify("5.88691141009287e-18", 0, null);
    verify("5.88691141009287e-19", 0, null);
    verify("5.88691141009287e-20", 0, null);
    verify("3.09492250471093e-19", 0, null);
    verify("3.09492250471093e-20", 0, null);
    verify("3.09492250471093e-21", 0, null);
    verify("8.19457916709552e-20", 0, null);
    verify("8.19457916709552e-21", 0, null);
    verify("8.19457916709552e-22", 0, null);
    verify("8.14085943218543e-21", 0, null);
    verify("8.14085943218543e-22", 0, null);
    verify("8.14085943218543e-23", 0, null);
    verify("9.76007205420988e-22", 0, null);
    verify("9.76007205420988e-23", 0, null);
    verify("9.76007205420988e-24", 0, null);
    verify("5.56585846982525e-23", 0, null);
    verify("5.56585846982525e-24", 0, null);
    verify("5.56585846982525e-25", 0, null);
    verify("3.82611861733991e-105", 0, null);
    verify("3.82611861733991e-106", 0, null);
    verify("3.82611861733991e-107", 0, null);
    verify("7.55119898189387e-303", 0, null);
    verify("7.55119898189387e-304", 0, null);
    verify("7.55119898189387e-305", 0, null);
    verify("9.33079308870154e-304", 0, null);
    verify("9.33079308870154e-305", 0, null);
    verify("9.33079308870154e-306", 0, null);
    verify("5.43393900702960e-305", 0, null);
    verify("5.43393900702960e-306", 0, null);
    verify("5.43393900702960e-307", 0, null);
    verify("7.40832645012184e-306", 0, null);
    verify("7.40832645012184e-307", 0, null);
    verify("7.40832645012184e-308", 0, null);


    //----------------------------------------------------------------------------
    apInitScenario("4. min pos < n < max pos, e notation");

    m_scen = "min pos < n < max pos, e notation";

    verify("8.8e0", 8, null);
    verify("8.8e1", 88, null);
    verify("8.8e2", 880, null);
    verify("9.29e1", 92, null);
    verify("9.29e2", 929, null);
    verify("9.29e3", 9290, null);
    verify("3.101e2", 310, null);
    verify("3.101e3", 3101, null);
    verify("3.101e4", 31010, null);
    verify("2.9432e3", 2943, null);
    verify("2.9432e4", 29432, null);
    verify("2.9432e5", 294320, null);
    verify("7.40524e4", 74052, null);
    verify("7.40524e5", 740524, null);
    verify("7.40524e6", 7405240, null);
    verify("8.210094e5", 821009, null);
    verify("8.210094e6", 8210094, null);
    verify("8.210094e7", 82100940, null);
    verify("7.6889116e6", 7688911, null);
    verify("7.6889116e7", 76889116, null);
    verify("7.6889116e8", 768891160, null);
    verify("1.37978822e7", 13797882, null);
    verify("1.37978822e8", 137978822, null);
    verify("1.37978822e9", 1379788220, null);
    verify("4.681924409e8", 468192440, null);
    verify("4.681924409e9", 4681924409, null);
    verify("4.681924409e10", 46819244090, null);
    verify("2.4377238908e9", 2437723890, null);
    verify("2.4377238908e10", 24377238908, null);
    verify("2.4377238908e11", 243772389080, null);
    verify("4.25294288348e10", 42529428834, null);
    verify("4.25294288348e11", 425294288348, null);
    verify("4.25294288348e12", 4252942883480, null);
    verify("1.113176106094e11", 111317610609, null);
    verify("1.113176106094e12", 1113176106094, null);
    verify("1.113176106094e13", 11131761060940, null);
    verify("5.0887522211581e12", 5088752221158, null);
    verify("5.0887522211581e13", 50887522211581, null);
    verify("5.0887522211581e14", 508875222115810, null);
    verify("1.78620560045955e13", 17862056004595, null);
    verify("1.78620560045955e14", 178620560045955, null);
    verify("1.78620560045955e15", 1786205600459550, null);
    verify("5.31710407920338e14", 531710407920338, null);
    verify("5.31710407920338e15", 5317104079203380, null);
    verify("5.31710407920338e16", 53171040792033800, null);
    verify("7.13740747001018e15", 7137407470010180, null);
    verify("7.13740747001018e16", 71374074700101800, null);
    verify("7.13740747001018e17", 713740747001017980, null);
    verify("5.88101283783150e16", 58810128378315000, null);
    verify("5.88101283783150e17", 588101283783149950, null);
    verify("5.88101283783150e18", 5881012837831499800, null);
    verify("8.54797277388807e17", 854797277388807040, null);
    verify("8.54797277388807e18", 8547972773888069600, null);
    verify("8.54797277388807e19", 85479727738880705000, null);
    verify("6.78722035300482e18", 6787220353004820500, null);
    verify("6.78722035300482e19", 67872203530048201000, null);
    verify("6.78722035300482e20", 6.7872203530048202e+020, null);
    verify("3.12354695000003e19", 31235469500000301000, null);
    verify("3.12354695000003e20", 3.1235469500000299e+020, null);
    verify("3.12354695000003e21", 3.1235469500000303e+021, null);
    verify("7.53586331797861e20", 7.5358633179786104e+020, null);
    verify("7.53586331797861e21", 7.5358633179786101e+021, null);
    verify("7.53586331797861e22", 7.5358633179786097e+022, null);
    verify("5.00032366740068e21", 5.0003236674006795e+021, null);
    verify("5.00032366740068e22", 5.0003236674006803e+022, null);
    verify("5.00032366740068e23", 5.0003236674006802e+023, null);
    verify("3.10807699355180e22", 3.1080769935518e+022, null);
    verify("3.10807699355180e23", 3.1080769935517997e+023, null);
    verify("3.10807699355180e24", 3.1080769935518001e+024, null);
    verify("2.14979750979343e23", 2.1497975097934299e+023, null);
    verify("2.14979750979343e24", 2.1497975097934299e+024, null);
    verify("2.14979750979343e25", 2.14979750979343e+025, null);
    verify("4.43269743388067e105", 4.4326974338806698e+105, null);
    verify("4.43269743388067e106", 4.4326974338806703e+106, null);
    verify("4.43269743388067e107", 4.4326974338806703e+107, null);
    verify("6.64540894198634e304", 6.6454089419863403e+304, null);
    verify("6.64540894198634e305", 6.6454089419863403e+305, null);
    verify("6.64540894198634e306", 6.6454089419863395e+306, null);
    verify("3.03593676523021e305", 3.03593676523021e+305, null);
    verify("3.03593676523021e306", 3.0359367652302102e+306, null);
    verify("3.03593676523021e307", 3.0359367652302101e+307, null);
    verify("3.94522426900222e306", 3.9452242690022199e+306, null);
    verify("3.94522426900222e307", 3.9452242690022201e+307, null);
    verify("3.94522426900222e308", 3.94522426900222e+308, null);
    verify("4.78390317175112e307", 4.7839031717511203e+307, null);
    verify("4.78390317175112e308", 4.78390317175112e+308, null);
    verify("4.78390317175112e309", 4.78390317175112e+309, null);


    //----------------------------------------------------------------------------
    apInitScenario("4. max pos");

    m_scen = "max pos";

    verify("1.7976931348623158e308",1.7976931348623158e308, null);

    
    //----------------------------------------------------------------------------
    apInitScenario("5. > max pos float (1.#INF)");
    
    m_scen = "> max pos float (1.#INF)";

    verify("1.797693134862315807e309",1.797693134862315807e309, null);

    
    //----------------------------------------------------------------------------
    apInitScenario("6. max neg");

    m_scen = "max neg";

    verify("-2.2250738585072012595e-308",-1, null);


    //----------------------------------------------------------------------------
    apInitScenario("7. max neg > n > min neg, up to full display precision");
    
    m_scen = "max neg > n > min neg, up to full display precision";

    verify("-.5", -1, null);
    verify("-5.", -5, null);
    verify("-.96", -1, null);
    verify("-3.1", -4, null);
    verify("-46.", -46, null);
    verify("-.479", -1, null);
    verify("-1.49", -2, null);
    verify("-95.5", -96, null);
    verify("-571.", -571, null);
    verify("-.4592", -1, null);
    verify("-0.569", -1, null);
    verify("-26.33", -27, null);
    verify("-543.6", -544, null);
    verify("-5967.", -5967, null);
    verify("-.90277", -1, null);
    verify("-7.9163", -8, null);
    verify("-41.279", -42, null);
    verify("-435.14", -436, null);
    verify("-8087.2", -8088, null);
    verify("-93731.", -93731, null);
    verify("-.905499", -1, null);
    verify("-2.40249", -3, null);
    verify("-49.7373", -50, null);
    verify("-616.218", -617, null);
    verify("-3380.45", -3381, null);
    verify("-38655.0", -38655, null);
    verify("-330284.", -330284, null);
    verify("-.3311229", -1, null);
    verify("-2.258134", -3, null);
    verify("-57.53541", -58, null);
    verify("-296.2066", -297, null);
    verify("-5262.788", -5263, null);
    verify("-69894.69", -69895, null);
    verify("-817561.2", -817562, null);
    verify("-1508910.", -1508910, null);
    verify("-.30719336", -1, null);
    verify("-3.6205174", -4, null);
    verify("-49.521465", -50, null);
    verify("-293.83437", -294, null);
    verify("-9940.2411", -9941, null);
    verify("-40721.260", -40722, null);
    verify("-588214.95", -588215, null);
    verify("-4061036.7", -4061037, null);
    verify("-14240519.", -14240519, null);
    verify("-.313171193", -1, null);
    verify("-4.31762894", -5, null);
    verify("-51.2635061", -52, null);
    verify("-678.616716", -679, null);
    verify("-3690.35822", -3691, null);
    verify("-77552.1661", -77553, null);
    verify("-307853.281", -307854, null);
    verify("-8827044.56", -8827045, null);
    verify("-65487957.7", -65487958, null);
    verify("-368544489.", -368544489, null);
    verify("-.1991785416", -1, null);
    verify("-2.272555041", -3, null);
    verify("-78.77000769", -79, null);
    verify("-848.5457325", -849, null);
    verify("-2501.275743", -2502, null);
    verify("-20022.56009", -20023, null);
    verify("-772824.9969", -772825, null);
    verify("-8111880.441", -8111881, null);
    verify("-32635057.88", -32635058, null);
    verify("-959661249.6", -959661250, null);
    verify("-3981052388.", -3981052388, null);
    verify("-9075243880", -9075243880, null);
    verify("-.60844235274", -1, null);
    verify("-3.0151020907", -4, null);
    verify("-91.410728930", -92, null);
    verify("-655.18392972", -656, null);
    verify("-6895.7597385", -6896, null);
    verify("-36894.158554", -36895, null);
    verify("-380261.89037", -380262, null);
    verify("-4585902.3424", -4585903, null);
    verify("-58454441.454", -58454442, null);
    verify("-949203226.66", -949203227, null);
    verify("-1401348344.2", -1401348345, null);
    verify("-76363606118.", -76363606118, null);
    verify("-95660430789", -95660430789, null);
    verify("-.586931356752", -1, null);
    verify("-1.62114054111", -2, null);
    verify("-31.0670443374", -32, null);
    verify("-772.618954449", -773, null);
    verify("-9226.13843442", -9227, null);
    verify("-32393.5294507", -32394, null);
    verify("-567241.586742", -567242, null);
    verify("-5885209.59360", -5885210, null);
    verify("-16405512.2467", -16405513, null);
    verify("-332285057.260", -332285058, null);
    verify("-7311868918.51", -7311868919, null);
    verify("-22516507490.4", -22516507491, null);
    verify("-512925511957.", -512925511957, null);
    verify("-604644063961", -604644063961, null);
    verify("-.3489196701652", -1, null);
    verify("-8.764807916087", -9, null);
    verify("-28.25601763587", -29, null);
    verify("-514.7378519873", -515, null);
    verify("-3593.085212360", -3594, null);
    verify("-40542.41949671", -40543, null);
    verify("-370979.9626897", -370980, null);
    verify("-7893828.344884", -7893829, null);
    verify("-18948367.87536", -18948368, null);
    verify("-645330477.8117", -645330478, null);
    verify("-6263973064.869", -6263973065, null);
    verify("-92229379100.37", -92229379101, null);
    verify("-283988953726.7", -283988953727, null);
    verify("-8883069582080.", -8883069582080, null);
    verify("-8969247128178", -8969247128178, null);
    verify("-.99747464946105", -1, null);
    verify("-7.8390010385020", -8, null);
    verify("-38.706642505934", -39, null);
    verify("-533.18973357646", -534, null);
    verify("-1197.1252686590", -1198, null);
    verify("-46082.538756131", -46083, null);
    verify("-799827.87110566", -799828, null);
    verify("-4709736.1173721", -4709737, null);
    verify("-77450602.685780", -77450603, null);
    verify("-550693548.34689", -550693549, null);
    verify("-6661523519.2974", -6661523520, null);
    verify("-58640312226.305", -58640312227, null);
    verify("-560690445612.22", -560690445613, null);
    verify("-1396666139436.1", -1396666139437, null);
    verify("-13595809502008.", -13595809502008, null);
    verify("-93482603370539", -93482603370539, null);
    verify("-.946037233674496", -1, null);
    verify("-5.48428455805845", -6, null);
    verify("-94.1054707644185", -95, null);
    verify("-113.409414264112", -114, null);
    verify("-2814.49360265583", -2815, null);
    verify("-90108.1357162047", -90109, null);
    verify("-863454.843502128", -863455, null);
    verify("-9795197.06271489", -9795198, null);
    verify("-13044202.5876058", -13044203, null);
    verify("-381640833.081739", -381640834, null);
    verify("-9870443883.43428", -9870443884, null);
    verify("-60422412387.5419", -60422412388, null);
    verify("-628182548242.912", -628182548243, null);
    verify("-8149733900124.40", -8149733900125, null);
    verify("-92432576712182.4", -92432576712183, null);
    verify("-710446389100490.", -710446389100490, null);
    verify("-990962943490046", -990962943490046, null);


    //----------------------------------------------------------------------------
    apInitScenario("8. max neg > n > -1, e notation");

    m_scen = "max neg > n > -1, e notation";

    verify("-5.0e-0", -5, null);
    verify("-5.0e-1", -1, null);
    verify("-5.0e-2", -1, null);
    verify("-1.44e-1", -1, null);
    verify("-1.44e-2", -1, null);
    verify("-1.44e-3", -1, null);
    verify("-1.927e-2", -1, null);
    verify("-1.927e-3", -1, null);
    verify("-1.927e-4", -1, null);
    verify("-5.4511e-3", -1, null);
    verify("-5.4511e-4", -1, null);
    verify("-5.4511e-5", -1, null);
    verify("-9.00017e-4", -1, null);
    verify("-9.00017e-5", -1, null);
    verify("-9.00017e-6", -1, null);
    verify("-4.837171e-5", -1, null);
    verify("-4.837171e-6", -1, null);
    verify("-4.837171e-7", -1, null);
    verify("-6.8920901e-6", -1, null);
    verify("-6.8920901e-7", -1, null);
    verify("-6.8920901e-8", -1, null);
    verify("-7.75575499e-7", -1, null);
    verify("-7.75575499e-8", -1, null);
    verify("-7.75575499e-9", -1, null);
    verify("-3.784053351e-8", -1, null);
    verify("-3.784053351e-9", -1, null);
    verify("-3.784053351e-10", -1, null);
    verify("-4.2510597333e-9", -1, null);
    verify("-4.2510597333e-10", -1, null);
    verify("-4.2510597333e-11", -1, null);
    verify("-1.27228923449e-10", -1, null);
    verify("-1.27228923449e-11", -1, null);
    verify("-1.27228923449e-12", -1, null);
    verify("-2.803018452148e-11", -1, null);
    verify("-2.803018452148e-12", -1, null);
    verify("-2.803018452148e-13", -1, null);
    verify("-2.7110671628691e-12", -1, null);
    verify("-2.7110671628691e-13", -1, null);
    verify("-2.7110671628691e-14", -1, null);
    verify("-9.56217783273567e-13", -1, null);
    verify("-9.56217783273567e-14", -1, null);
    verify("-9.56217783273567e-15", -1, null);
    verify("-6.99456620166961e-14", -1, null);
    verify("-6.99456620166961e-15", -1, null);
    verify("-6.99456620166961e-16", -1, null);
    verify("-3.40005567324267e-15", -1, null);
    verify("-3.40005567324267e-16", -1, null);
    verify("-3.40005567324267e-17", -1, null);
    verify("-6.17227311203627e-16", -1, null);
    verify("-6.17227311203627e-17", -1, null);
    verify("-6.17227311203627e-18", -1, null);
    verify("-9.36645803333569e-17", -1, null);
    verify("-9.36645803333569e-18", -1, null);
    verify("-9.36645803333569e-19", -1, null);
    verify("-7.26859340546239e-18", -1, null);
    verify("-7.26859340546239e-19", -1, null);
    verify("-7.26859340546239e-20", -1, null);
    verify("-7.62866563135477e-19", -1, null);
    verify("-7.62866563135477e-20", -1, null);
    verify("-7.62866563135477e-21", -1, null);
    verify("-2.43012881718031e-20", -1, null);
    verify("-2.43012881718031e-21", -1, null);
    verify("-2.43012881718031e-22", -1, null);
    verify("-7.97350417926250e-21", -1, null);
    verify("-7.97350417926250e-22", -1, null);
    verify("-7.97350417926250e-23", -1, null);
    verify("-2.37745678018969e-22", -1, null);
    verify("-2.37745678018969e-23", -1, null);
    verify("-2.37745678018969e-24", -1, null);
    verify("-7.41928308312151e-23", -1, null);
    verify("-7.41928308312151e-24", -1, null);
    verify("-7.41928308312151e-25", -1, null);
    verify("-7.27813770416071e-105", -1, null);
    verify("-7.27813770416071e-106", -1, null);
    verify("-7.27813770416071e-107", -1, null);
    verify("-3.31023416390304e-304", -1, null);
    verify("-3.31023416390304e-305", -1, null);
    verify("-3.31023416390304e-306", -1, null);
    verify("-1.30581979540712e-305", -1, null);
    verify("-1.30581979540712e-306", -1, null);
    verify("-1.30581979540712e-307", -1, null);
    verify("-2.64396529892862e-306", -1, null);
    verify("-2.64396529892862e-307", -1, null);
    verify("-2.64396529892862e-308", -1, null);


    //----------------------------------------------------------------------------
    apInitScenario("9. max neg > n > min neg, e notation");

    m_scen = "max neg > n > min neg, e notation";

    verify("-8.5e0", -9, null);
    verify("-8.5e1", -85, null);
    verify("-8.5e2", -850, null);
    verify("-1.52e1", -16, null);
    verify("-1.52e2", -152, null);
    verify("-1.52e3", -1520, null);
    verify("-5.363e2", -537, null);
    verify("-5.363e3", -5363, null);
    verify("-5.363e4", -53630, null);
    verify("-2.0302e3", -2031, null);
    verify("-2.0302e4", -20302, null);
    verify("-2.0302e5", -203020, null);
    verify("-4.06856e4", -40686, null);
    verify("-4.06856e5", -406856, null);
    verify("-4.06856e6", -4068560, null);
    verify("-3.526625e5", -352663, null);
    verify("-3.526625e6", -3526625, null);
    verify("-3.526625e7", -35266250, null);
    verify("-4.2131970e6", -4213197, null);
    verify("-4.2131970e7", -42131970, null);
    verify("-4.2131970e8", -421319700, null);
    verify("-9.39827625e7", -93982763, null);
    verify("-9.39827625e8", -939827625, null);
    verify("-9.39827625e9", -9398276250, null);
    verify("-5.296667600e8", -529666760, null);
    verify("-5.296667600e9", -5296667600, null);
    verify("-5.296667600e10", -52966676000, null);
    verify("-2.9894724429e9", -2989472443, null);
    verify("-2.9894724429e10", -29894724429, null);
    verify("-2.9894724429e11", -298947244290, null);
    verify("-9.31269515836e10", -93126951584, null);
    verify("-9.31269515836e11", -931269515836, null);
    verify("-9.31269515836e12", -9312695158360, null);
    verify("-9.785227422363e11", -978522742237, null);
    verify("-9.785227422363e12", -9785227422363, null);
    verify("-9.785227422363e13", -97852274223630, null);
    verify("-6.2804629881637e12", -6280462988164, null);
    verify("-6.2804629881637e13", -62804629881637, null);
    verify("-6.2804629881637e14", -628046298816370, null);
    verify("-1.97084335264933e13", -19708433526494, null);
    verify("-1.97084335264933e14", -197084335264933, null);
    verify("-1.97084335264933e15", -1970843352649330, null);
    verify("-9.87601801405715e14", -987601801405715, null);
    verify("-9.87601801405715e15", -9876018014057150, null);
    verify("-9.87601801405715e16", -98760180140571500, null);
    verify("-2.00941085362823e15", -2009410853628230, null);
    verify("-2.00941085362823e16", -20094108536282300, null);
    verify("-2.00941085362823e17", -200941085362823000, null);
    verify("-6.75649868607965e16", -67564986860796496, null);
    verify("-6.75649868607965e17", -675649868607965060, null);
    verify("-6.75649868607965e18", -6756498686079649800, null);
    verify("-7.43963320455925e17", -743963320455924990, null);
    verify("-7.43963320455925e18", -7439633204559250400, null);
    verify("-7.43963320455925e19", -74396332045592494000, null);
    verify("-5.37604621849243e18", -5376046218492430300, null);
    verify("-5.37604621849243e19", -53760462184924299000, null);
    verify("-5.37604621849243e20", -5.3760462184924303e+020, null);
    verify("-1.36075801208007e19", -13607580120800700000, null);
    verify("-1.36075801208007e20", -1.36075801208007e+020, null);
    verify("-1.36075801208007e21", -1.3607580120800701e+021, null);
    verify("-9.30987673607026e20", -9.3098767360702598e+020, null);
    verify("-9.30987673607026e21", -9.3098767360702598e+021, null);
    verify("-9.30987673607026e22", -9.3098767360702592e+022, null);
    verify("-5.75728671948621e21", -5.7572867194862101e+021, null);
    verify("-5.75728671948621e22", -5.7572867194862099e+022, null);
    verify("-5.75728671948621e23", -5.75728671948621e+023, null);
    verify("-5.59643799688426e22", -5.5964379968842603e+022, null);
    verify("-5.59643799688426e23", -5.5964379968842603e+023, null);
    verify("-5.59643799688426e24", -5.5964379968842596e+024, null);
    verify("-9.81083935669294e23", -9.8108393566929399e+023, null);
    verify("-9.81083935669294e24", -9.8108393566929402e+024, null);
    verify("-9.81083935669294e25", -9.8108393566929406e+025, null);
    verify("-3.14403816749344e105", -3.1440381674934399e+105, null);
    verify("-3.14403816749344e106", -3.1440381674934401e+106, null);
    verify("-3.14403816749344e107", -3.1440381674934399e+107, null);
    verify("-5.81127373217717e303", -5.8112737321771704e+303, null);
    verify("-5.81127373217717e304", -5.8112737321771704e+304, null);
    verify("-5.81127373217717e305", -5.8112737321771697e+305, null);
    verify("-5.86800472182360e304", -5.8680047218236002e+304, null);
    verify("-5.86800472182360e305", -5.8680047218235998e+305, null);
    verify("-5.86800472182360e306", -5.8680047218235998e+306, null);
    verify("-8.30817936376931e305", -8.3081793637693096e+305, null);
    verify("-8.30817936376931e306", -8.3081793637693106e+306, null);
    verify("-8.30817936376931e307", -8.3081793637693096e+307, null);
    verify("-8.20646232996096e306", -8.2064623299609596e+306, null);
    verify("-8.20646232996096e307", -8.2064623299609596e+307, null);
    verify("-8.20646232996096e308", -8.20646232996096e+309, null);
    
    //----------------------------------------------------------------------------
    apInitScenario("9. min neg");
    
    m_scen = "min neg";

    verify("-1.797693134862315807e308", -1.797693134862315807e308, null);


    //----------------------------------------------------------------------------
    apInitScenario("10. < min neg float (-1.#INF)");
    
    m_scen = "< min neg float (-1.#INF)";

    verify("-1.797693134862315807e309",-1.797693134862315807e309, null);


    //----------------------------------------------------------------------------
    apInitScenario("11. zero");
    
    m_scen = "zero";
    
    verify("0.0",0, null);


    apEndTest();

}



floor012();


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
