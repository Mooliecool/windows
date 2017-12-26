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


var iTestID = 53365;


function verify( ob, sExp, bugNum) {
    if (bugNum == null) bugNum = "";

    var sAct = Math.round(ob);

    if (sAct != sExp)
        apLogFailInfo( m_scen+" failed", sExp, sAct,bugNum);
}

var m_scen = "";

function round012() {

    apInitTest("round012 ");

    //----------------------------------------------------------------------------
    apInitScenario("1. min pos");

    m_scen = "min pos";

    verify("2.2250738585072014e-308", 0, null);


    //----------------------------------------------------------------------------
    apInitScenario("2. min pos < n < max pos, up to full display precision");
    
    m_scen = "min pos < n < max pos, up to full display precision";

    verify(".6", 1, null);
    verify("2.", 2, null);
    verify(".44", 0, null);
    verify("7.2", 7, null);
    verify("14.", 14, null);
    verify(".786", 1, null);
    verify("7.03", 7, null);
    verify("86.6", 87, null);
    verify("135.", 135, null);
    verify(".1198", 0, null);
    verify("0.287", 0, null);
    verify("17.55", 18, null);
    verify("540.0", 540, null);
    verify("6076.", 6076, null);
    verify(".62153", 1, null);
    verify("2.6611", 3, null);
    verify("96.121", 96, null);
    verify("489.20", 489, null);
    verify("5499.2", 5499, null);
    verify("90415.", 90415, null);
    verify(".155358", 0, null);
    verify("2.02021", 2, null);
    verify("94.1046", 94, null);
    verify("564.804", 565, null);
    verify("7918.40", 7918, null);
    verify("68048.8", 68049, null);
    verify("213309.", 213309, null);
    verify(".1226468", 0, null);
    verify("9.544780", 10, null);
    verify("88.08627", 88, null);
    verify("772.8569", 773, null);
    verify("7405.900", 7406, null);
    verify("52981.86", 52982, null);
    verify("557085.8", 557086, null);
    verify("2446900.", 2446900, null);
    verify(".72709566", 1, null);
    verify("5.2120043", 5, null);
    verify("13.252459", 13, null);
    verify("241.43360", 241, null);
    verify("4458.9784", 4459, null);
    verify("80312.218", 80312, null);
    verify("696010.94", 696011, null);
    verify("9958380.8", 9958381, null);
    verify("78504937.", 78504937, null);
    verify(".143470961", 0, null);
    verify("1.23601932", 1, null);
    verify("39.0912734", 39, null);
    verify("463.555346", 464, null);
    verify("1312.74929", 1313, null);
    verify("98224.9972", 98225, null);
    verify("595098.763", 595099, null);
    verify("7061195.21", 7061195, null);
    verify("75903211.0", 75903211, null);
    verify("690839080.", 690839080, null);
    verify(".8807708514", 1, null);
    verify("4.088911435", 4, null);
    verify("75.27016363", 75, null);
    verify("561.0251120", 561, null);
    verify("5562.421819", 5562, null);
    verify("48013.26405", 48013, null);
    verify("318622.0868", 318622, null);
    verify("2255066.563", 2255067, null);
    verify("63141035.99", 63141036, null);
    verify("152643838.4", 152643838, null);
    verify("4132283254.", 4132283254, null);
    verify(".90337464930", 1, null);
    verify("2.5474295081", 3, null);
    verify("56.356969717", 56, null);
    verify("197.23794979", 197, null);
    verify("7226.6664991", 7227, null);
    verify("47583.278459", 47583, null);
    verify("155763.59386", 155764, null);
    verify("6237632.6977", 6237633, null);
    verify("45252125.874", 45252126, null);
    verify("475920249.94", 475920250, null);
    verify("5531669963.8", 5531669964, null);
    verify("71875996973.", 71875996973, null);
    verify("72860084283", 72860084283, null);
    verify(".148158349250", 0, null);
    verify("5.10823278330", 5, null);
    verify("86.1765834983", 86, null);
    verify("410.944670926", 411, null);
    verify("3101.12061200", 3101, null);
    verify("52944.3618810", 52944, null);
    verify("207709.254881", 207709, null);
    verify("4977900.91535", 4977901, null);
    verify("79846204.7014", 79846205, null);
    verify("792294719.622", 792294720, null);
    verify("1030065957.74", 1030065958, null);
    verify("22745604335.8", 22745604336, null);
    verify("978322384890.", 978322384890, null);
    verify("943471893780", 943471893780, null);
    verify(".2071476641772", 0, null);
    verify("5.197179852767", 5, null);
    verify("28.92491713587", 29, null);
    verify("213.3537603013", 213, null);
    verify("7105.498363543", 7105, null);
    verify("53209.09272198", 53209, null);
    verify("365883.2780820", 365883, null);
    verify("1824205.070693", 1824205, null);
    verify("83667863.76719", 83667864, null);
    verify("587065498.1415", 587065498, null);
    verify("1234297163.026", 1234297163, null);
    verify("69903937280.88", 69903937281, null);
    verify("507570635327.0", 507570635327, null);
    verify("7159818530489.", 7159818530489, null);
    verify("9415642957298", 9415642957298, null);
    verify(".68920778576504", 1, null);
    verify("2.5348921384159", 3, null);
    verify("78.064458756760", 78, null);
    verify("413.92148252314", 414, null);
    verify("3291.2624079456", 3291, null);
    verify("72462.406537462", 72462, null);
    verify("660928.11304554", 660928, null);
    verify("9090729.6320133", 9090730, null);
    verify("96018819.280453", 96018819, null);
    verify("496567687.42381", 496567687, null);
    verify("6216971651.0665", 6216971651, null);
    verify("38220781319.655", 38220781320, null);
    verify("253064691100.61", 253064691101, null);
    verify("9068799484665.9", 9068799484666, null);
    verify("35400069447487.", 35400069447487, null);
    verify("43823967881364", 43823967881364, null);
    verify(".727869124134605", 1, null);
    verify("7.89049944596633", 8, null);
    verify("38.8539718544432", 39, null);
    verify("386.440145639707", 386, null);
    verify("9941.26468948541", 9941, null);
    verify("15637.3266253974", 15637, null);
    verify("553929.976613362", 553930, null);
    verify("6322851.30065633", 6322851, null);
    verify("63872380.3346230", 63872380, null);
    verify("376480639.494894", 376480639, null);
    verify("1209785286.94289", 1209785287, null);
    verify("27835156131.2802", 27835156131, null);
    verify("981668226611.797", 981668226612, null);
    verify("6716339086274.29", 6716339086274, null);
    verify("31018751905191.5", 31018751905192, null);
    verify("690931313397259.", 690931313397259, null);
    verify("225264066871575", 225264066871575, null);


    //----------------------------------------------------------------------------
    apInitScenario("3. min pos < n < 1, e notation");

    m_scen = "min pos < n < 1, e notation";

    verify("8.0e-0", 8, null);
    verify("8.0e-1", 1, null);
    verify("8.0e-2", 0, null);
    verify("6.40e-1", 1, null);
    verify("6.40e-2", 0, null);
    verify("6.40e-3", 0, null);
    verify("8.667e-2", 0, null);
    verify("8.667e-3", 0, null);
    verify("8.667e-4", 0, null);
    verify("3.0404e-3", 0, null);
    verify("3.0404e-4", 0, null);
    verify("3.0404e-5", 0, null);
    verify("9.69482e-4", 0, null);
    verify("9.69482e-5", 0, null);
    verify("9.69482e-6", 0, null);
    verify("2.638434e-5", 0, null);
    verify("2.638434e-6", 0, null);
    verify("2.638434e-7", 0, null);
    verify("4.7256510e-6", 0, null);
    verify("4.7256510e-7", 0, null);
    verify("4.7256510e-8", 0, null);
    verify("6.77611611e-7", 0, null);
    verify("6.77611611e-8", 0, null);
    verify("6.77611611e-9", 0, null);
    verify("3.474819856e-8", 0, null);
    verify("3.474819856e-9", 0, null);
    verify("3.474819856e-10", 0, null);
    verify("3.0371664621e-9", 0, null);
    verify("3.0371664621e-10", 0, null);
    verify("3.0371664621e-11", 0, null);
    verify("7.06342879866e-10", 0, null);
    verify("7.06342879866e-11", 0, null);
    verify("7.06342879866e-12", 0, null);
    verify("8.723993732395e-11", 0, null);
    verify("8.723993732395e-12", 0, null);
    verify("8.723993732395e-13", 0, null);
    verify("7.0575617301540e-12", 0, null);
    verify("7.0575617301540e-13", 0, null);
    verify("7.0575617301540e-14", 0, null);
    verify("8.27557583331216e-13", 0, null);
    verify("8.27557583331216e-14", 0, null);
    verify("8.27557583331216e-15", 0, null);
    verify("5.06693033864411e-14", 0, null);
    verify("5.06693033864411e-15", 0, null);
    verify("5.06693033864411e-16", 0, null);
    verify("5.69160741282644e-15", 0, null);
    verify("5.69160741282644e-16", 0, null);
    verify("5.69160741282644e-17", 0, null);
    verify("7.69183026651453e-16", 0, null);
    verify("7.69183026651453e-17", 0, null);
    verify("7.69183026651453e-18", 0, null);
    verify("4.47242979260183e-17", 0, null);
    verify("4.47242979260183e-18", 0, null);
    verify("4.47242979260183e-19", 0, null);
    verify("7.94034863858147e-18", 0, null);
    verify("7.94034863858147e-19", 0, null);
    verify("7.94034863858147e-20", 0, null);
    verify("9.37450335641405e-19", 0, null);
    verify("9.37450335641405e-20", 0, null);
    verify("9.37450335641405e-21", 0, null);
    verify("5.45589868294713e-20", 0, null);
    verify("5.45589868294713e-21", 0, null);
    verify("5.45589868294713e-22", 0, null);
    verify("9.69019120785362e-21", 0, null);
    verify("9.69019120785362e-22", 0, null);
    verify("9.69019120785362e-23", 0, null);
    verify("5.58225385330127e-22", 0, null);
    verify("5.58225385330127e-23", 0, null);
    verify("5.58225385330127e-24", 0, null);
    verify("3.01953106436364e-105", 0, null);
    verify("3.01953106436364e-106", 0, null);
    verify("3.01953106436364e-107", 0, null);
    verify("3.50135646186260e-305", 0, null);
    verify("3.50135646186260e-306", 0, null);
    verify("3.50135646186260e-307", 0, null);
    verify("7.85144575561003e-306", 0, null);
    verify("7.85144575561003e-307", 0, null);
    verify("7.85144575561003e-308", 0, null);


    //----------------------------------------------------------------------------
    apInitScenario("4. min pos < n < max pos, e notation");

    m_scen = "min pos < n < max pos, e notation";

    verify("8.8e0", 9, null);
    verify("8.8e1", 88, null);
    verify("8.8e2", 880, null);
    verify("9.29e1", 93, null);
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
    verify("7.6889116e6", 7688912, null);
    verify("7.6889116e7", 76889116, null);
    verify("7.6889116e8", 768891160, null);
    verify("1.37978822e7", 13797882, null);
    verify("1.37978822e8", 137978822, null);
    verify("1.37978822e9", 1379788220, null);
    verify("4.681924409e8", 468192441, null);
    verify("4.681924409e9", 4681924409, null);
    verify("4.681924409e10", 46819244090, null);
    verify("2.4377238908e9", 2437723891, null);
    verify("2.4377238908e10", 24377238908, null);
    verify("2.4377238908e11", 243772389080, null);
    verify("4.25294288348e10", 42529428835, null);
    verify("4.25294288348e11", 425294288348, null);
    verify("4.25294288348e12", 4252942883480, null);
    verify("1.113176106094e11", 111317610609, null);
    verify("1.113176106094e12", 1113176106094, null);
    verify("1.113176106094e13", 11131761060940, null);
    verify("5.0887522211581e12", 5088752221158, null);
    verify("5.0887522211581e13", 50887522211581, null);
    verify("5.0887522211581e14", 508875222115810, null);
    verify("1.78620560045955e13", 17862056004596, null);
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
    verify("4.43269743388067e105", 4.4326974338806698e+105, null);
    verify("4.43269743388067e106", 4.4326974338806703e+106, null);
    verify("4.43269743388067e107", 4.4326974338806703e+107, null);
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

    verify("-2.2250738585072012595e-308",0, null);


    //----------------------------------------------------------------------------
    apInitScenario("7. max neg > n > min neg, up to full display precision");
    
    m_scen = "max neg > n > min neg, up to full display precision";

    verify("-.5", 0, null);
    verify("-5.", -5, null);
    verify("-.96", -1, null);
    verify("-3.1", -3, null);
    verify("-46.", -46, null);
    verify("-.479", 0, null);
    verify("-1.49", -1, null);
    verify("-95.5", -95, null);
    verify("-571.", -571, null);
    verify("-.4592", 0, null);
    verify("-0.569", -1, null);
    verify("-26.33", -26, null);
    verify("-543.6", -544, null);
    verify("-5967.", -5967, null);
    verify("-.90277", -1, null);
    verify("-7.9163", -8, null);
    verify("-41.279", -41, null);
    verify("-435.14", -435, null);
    verify("-8087.2", -8087, null);
    verify("-93731.", -93731, null);
    verify("-.905499", -1, null);
    verify("-2.40249", -2, null);
    verify("-49.7373", -50, null);
    verify("-616.218", -616, null);
    verify("-3380.45", -3380, null);
    verify("-38655.0", -38655, null);
    verify("-330284.", -330284, null);
    verify("-.3311229", 0, null);
    verify("-2.258134", -2, null);
    verify("-57.53541", -58, null);
    verify("-296.2066", -296, null);
    verify("-5262.788", -5263, null);
    verify("-69894.69", -69895, null);
    verify("-817561.2", -817561, null);
    verify("-1508910.", -1508910, null);
    verify("-.30719336", 0, null);
    verify("-3.6205174", -4, null);
    verify("-49.521465", -50, null);
    verify("-293.83437", -294, null);
    verify("-9940.2411", -9940, null);
    verify("-40721.260", -40721, null);
    verify("-588214.95", -588215, null);
    verify("-4061036.7", -4061037, null);
    verify("-14240519.", -14240519, null);
    verify("-.313171193", 0, null);
    verify("-4.31762894", -4, null);
    verify("-51.2635061", -51, null);
    verify("-678.616716", -679, null);
    verify("-3690.35822", -3690, null);
    verify("-77552.1661", -77552, null);
    verify("-307853.281", -307853, null);
    verify("-8827044.56", -8827045, null);
    verify("-65487957.7", -65487958, null);
    verify("-368544489.", -368544489, null);
    verify("-.1991785416", 0, null);
    verify("-2.272555041", -2, null);
    verify("-78.77000769", -79, null);
    verify("-848.5457325", -849, null);
    verify("-2501.275743", -2501, null);
    verify("-20022.56009", -20023, null);
    verify("-772824.9969", -772825, null);
    verify("-8111880.441", -8111880, null);
    verify("-32635057.88", -32635058, null);
    verify("-959661249.6", -959661250, null);
    verify("-3981052388.", -3981052388, null);
    verify("-9075243880", -9075243880, null);
    verify("-.60844235274", -1, null);
    verify("-3.0151020907", -3, null);
    verify("-91.410728930", -91, null);
    verify("-655.18392972", -655, null);
    verify("-6895.7597385", -6896, null);
    verify("-36894.158554", -36894, null);
    verify("-380261.89037", -380262, null);
    verify("-4585902.3424", -4585902, null);
    verify("-58454441.454", -58454441, null);
    verify("-949203226.66", -949203227, null);
    verify("-1401348344.2", -1401348344, null);
    verify("-76363606118.", -76363606118, null);
    verify("-95660430789", -95660430789, null);
    verify("-.586931356752", -1, null);
    verify("-1.62114054111", -2, null);
    verify("-31.0670443374", -31, null);
    verify("-772.618954449", -773, null);
    verify("-9226.13843442", -9226, null);
    verify("-32393.5294507", -32394, null);
    verify("-567241.586742", -567242, null);
    verify("-5885209.59360", -5885210, null);
    verify("-16405512.2467", -16405512, null);
    verify("-332285057.260", -332285057, null);
    verify("-7311868918.51", -7311868919, null);
    verify("-22516507490.4", -22516507490, null);
    verify("-512925511957.", -512925511957, null);
    verify("-604644063961", -604644063961, null);
    verify("-.3489196701652", 0, null);
    verify("-8.764807916087", -9, null);
    verify("-28.25601763587", -28, null);
    verify("-514.7378519873", -515, null);
    verify("-3593.085212360", -3593, null);
    verify("-40542.41949671", -40542, null);
    verify("-370979.9626897", -370980, null);
    verify("-7893828.344884", -7893828, null);
    verify("-18948367.87536", -18948368, null);
    verify("-645330477.8117", -645330478, null);
    verify("-6263973064.869", -6263973065, null);
    verify("-92229379100.37", -92229379100, null);
    verify("-283988953726.7", -283988953727, null);
    verify("-8883069582080.", -8883069582080, null);
    verify("-8969247128178", -8969247128178, null);
    verify("-.99747464946105", -1, null);
    verify("-7.8390010385020", -8, null);
    verify("-38.706642505934", -39, null);
    verify("-533.18973357646", -533, null);
    verify("-1197.1252686590", -1197, null);
    verify("-46082.538756131", -46083, null);
    verify("-799827.87110566", -799828, null);
    verify("-4709736.1173721", -4709736, null);
    verify("-77450602.685780", -77450603, null);
    verify("-550693548.34689", -550693548, null);
    verify("-6661523519.2974", -6661523519, null);
    verify("-58640312226.305", -58640312226, null);
    verify("-560690445612.22", -560690445612, null);
    verify("-1396666139436.1", -1396666139436, null);
    verify("-13595809502008.", -13595809502008, null);
    verify("-93482603370539", -93482603370539, null);
    verify("-.946037233674496", -1, null);
    verify("-5.48428455805845", -5, null);
    verify("-94.1054707644185", -94, null);
    verify("-113.409414264112", -113, null);
    verify("-2814.49360265583", -2814, null);
    verify("-90108.1357162047", -90108, null);
    verify("-863454.843502128", -863455, null);
    verify("-9795197.06271489", -9795197, null);
    verify("-13044202.5876058", -13044203, null);
    verify("-381640833.081739", -381640833, null);
    verify("-9870443883.43428", -9870443883, null);
    verify("-60422412387.5419", -60422412388, null);
    verify("-628182548242.912", -628182548243, null);
    verify("-8149733900124.40", -8149733900124, null);
    verify("-92432576712182.4", -92432576712182, null);
    verify("-710446389100490.", -710446389100490, null);
    verify("-990962943490046", -990962943490046, null);


    //----------------------------------------------------------------------------
    apInitScenario("8. max neg > n > -1, e notation");

    m_scen = "max neg > n > -1, e notation";

    verify("-5.9e-0", -6, null);
    verify("-5.9e-1", -1, null);
    verify("-5.9e-2", 0, null);
    verify("-8.92e-1", -1, null);
    verify("-8.92e-2", 0, null);
    verify("-8.92e-3", 0, null);
    verify("-2.951e-2", 0, null);
    verify("-2.951e-3", 0, null);
    verify("-2.951e-4", 0, null);
    verify("-7.1573e-3", 0, null);
    verify("-7.1573e-4", 0, null);
    verify("-7.1573e-5", 0, null);
    verify("-7.83191e-4", 0, null);
    verify("-7.83191e-5", 0, null);
    verify("-7.83191e-6", 0, null);
    verify("-8.761734e-5", 0, null);
    verify("-8.761734e-6", 0, null);
    verify("-8.761734e-7", 0, null);
    verify("-5.6388918e-6", 0, null);
    verify("-5.6388918e-7", 0, null);
    verify("-5.6388918e-8", 0, null);
    verify("-5.51156439e-7", 0, null);
    verify("-5.51156439e-8", 0, null);
    verify("-5.51156439e-9", 0, null);
    verify("-3.640164456e-8", 0, null);
    verify("-3.640164456e-9", 0, null);
    verify("-3.640164456e-10", 0, null);
    verify("-1.4384034645e-9", 0, null);
    verify("-1.4384034645e-10", 0, null);
    verify("-1.4384034645e-11", 0, null);
    verify("-2.25583947958e-10", 0, null);
    verify("-2.25583947958e-11", 0, null);
    verify("-2.25583947958e-12", 0, null);
    verify("-2.011142560596e-11", 0, null);
    verify("-2.011142560596e-12", 0, null);
    verify("-2.011142560596e-13", 0, null);
    verify("-8.9001791229158e-12", 0, null);
    verify("-8.9001791229158e-13", 0, null);
    verify("-8.9001791229158e-14", 0, null);
    verify("-5.25024887140531e-13", 0, null);
    verify("-5.25024887140531e-14", 0, null);
    verify("-5.25024887140531e-15", 0, null);
    verify("-5.78935347372427e-14", 0, null);
    verify("-5.78935347372427e-15", 0, null);
    verify("-5.78935347372427e-16", 0, null);
    verify("-2.29358110607862e-15", 0, null);
    verify("-2.29358110607862e-16", 0, null);
    verify("-2.29358110607862e-17", 0, null);
    verify("-3.97922068769070e-16", 0, null);
    verify("-3.97922068769070e-17", 0, null);
    verify("-3.97922068769070e-18", 0, null);
    verify("-6.06392842726122e-17", 0, null);
    verify("-6.06392842726122e-18", 0, null);
    verify("-6.06392842726122e-19", 0, null);
    verify("-7.32488735425603e-18", 0, null);
    verify("-7.32488735425603e-19", 0, null);
    verify("-7.32488735425603e-20", 0, null);
    verify("-3.77369453157441e-19", 0, null);
    verify("-3.77369453157441e-20", 0, null);
    verify("-3.77369453157441e-21", 0, null);
    verify("-7.90712501118520e-20", 0, null);
    verify("-7.90712501118520e-21", 0, null);
    verify("-7.90712501118520e-22", 0, null);
    verify("-4.81083664920017e-21", 0, null);
    verify("-4.81083664920017e-22", 0, null);
    verify("-4.81083664920017e-23", 0, null);
    verify("-9.76622908870673e-22", 0, null);
    verify("-9.76622908870673e-23", 0, null);
    verify("-9.76622908870673e-24", 0, null);
    verify("-8.87038338701245e-105", 0, null);
    verify("-8.87038338701245e-106", 0, null);
    verify("-8.87038338701245e-107", 0, null);
    verify("-5.85334747871478e-305", 0, null);
    verify("-5.85334747871478e-306", 0, null);
    verify("-5.85334747871478e-307", 0, null);
    verify("-8.31942411940214e-306", 0, null);
    verify("-8.31942411940214e-307", 0, null);
    verify("-8.31942411940214e-308", 0, null);


    //----------------------------------------------------------------------------
    apInitScenario("9. max neg > n > min neg, e notation");

    m_scen = "max neg > n > min neg, e notation";

    verify("-3.2e0", -3, null);
    verify("-3.2e1", -32, null);
    verify("-3.2e2", -320, null);
    verify("-1.24e1", -12, null);
    verify("-1.24e2", -124, null);
    verify("-1.24e3", -1240, null);
    verify("-5.000e2", -500, null);
    verify("-5.000e3", -5000, null);
    verify("-5.000e4", -50000, null);
    verify("-9.4025e3", -9402, null);
    verify("-9.4025e4", -94025, null);
    verify("-9.4025e5", -940250, null);
    verify("-9.29718e4", -92972, null);
    verify("-9.29718e5", -929718, null);
    verify("-9.29718e6", -9297180, null);
    verify("-7.211709e5", -721171, null);
    verify("-7.211709e6", -7211709, null);
    verify("-7.211709e7", -72117090, null);
    verify("-3.6387110e6", -3638711, null);
    verify("-3.6387110e7", -36387110, null);
    verify("-3.6387110e8", -363871100, null);
    verify("-3.63890686e7", -36389069, null);
    verify("-3.63890686e8", -363890686, null);
    verify("-3.63890686e9", -3638906860, null);
    verify("-9.403881615e8", -940388161, null);
    verify("-9.403881615e9", -9403881615, null);
    verify("-9.403881615e10", -94038816150, null);
    verify("-4.6302738193e9", -4630273819, null);
    verify("-4.6302738193e10", -46302738193, null);
    verify("-4.6302738193e11", -463027381930, null);
    verify("-7.79660110253e10", -77966011025, null);
    verify("-7.79660110253e11", -779660110253, null);
    verify("-7.79660110253e12", -7796601102530, null);
    verify("-6.659646216312e11", -665964621631, null);
    verify("-6.659646216312e12", -6659646216312, null);
    verify("-6.659646216312e13", -66596462163120, null);
    verify("-5.8316948988823e12", -5831694898882, null);
    verify("-5.8316948988823e13", -58316948988823, null);
    verify("-5.8316948988823e14", -583169489888230, null);
    verify("-1.55282634020501e13", -15528263402050, null);
    verify("-1.55282634020501e14", -155282634020501, null);
    verify("-1.55282634020501e15", -1552826340205010, null);
    verify("-4.43824587100373e14", -443824587100373, null);
    verify("-4.43824587100373e15", -4438245871003730, null);
    verify("-4.43824587100373e16", -44382458710037296, null);
    verify("-9.32022561741136e15", -9320225617411360, null);
    verify("-9.32022561741136e16", -93202256174113600, null);
    verify("-9.32022561741136e17", -932022561741136000, null);
    verify("-4.22075855655506e16", -42207585565550600, null);
    verify("-4.22075855655506e17", -422075855655505980, null);
    verify("-4.22075855655506e18", -4220758556555060200, null);
    verify("-9.03120769102516e17", -903120769102515970, null);
    verify("-9.03120769102516e18", -9031207691025160200, null);
    verify("-9.03120769102516e19", -90312076910251606000, null);
    verify("-6.39783187882541e18", -6397831878825409500, null);
    verify("-6.39783187882541e19", -63978318788254097000, null);
    verify("-6.39783187882541e20", -6.3978318788254106e+020, null);
    verify("-6.61005335930233e19", -66100533593023300000, null);
    verify("-6.61005335930233e20", -6.6100533593023303e+020, null);
    verify("-6.61005335930233e21", -6.61005335930233e+021, null);
    verify("-3.28172428104275e20", -3.28172428104275e+020, null);
    verify("-3.28172428104275e21", -3.2817242810427501e+021, null);
    verify("-3.28172428104275e22", -3.2817242810427499e+022, null);
    verify("-2.83286411492686e21", -2.8328641149268598e+021, null);
    verify("-2.83286411492686e22", -2.83286411492686e+022, null);
    verify("-2.83286411492686e23", -2.8328641149268601e+023, null);
    verify("-6.16754253210713e22", -6.1675425321071298e+022, null);
    verify("-6.16754253210713e23", -6.1675425321071301e+023, null);
    verify("-6.16754253210713e24", -6.1675425321071301e+024, null);
    verify("-3.55036974585943e23", -3.5503697458594297e+023, null);
    verify("-3.55036974585943e24", -3.55036974585943e+024, null);
    verify("-3.55036974585943e25", -3.5503697458594299e+025, null);
    verify("-7.17170763763262e105", -7.1717076376326199e+105, null);
    verify("-7.17170763763262e106", -7.1717076376326203e+106, null);
    verify("-7.17170763763262e107", -7.1717076376326198e+107, null);
    verify("-5.98908565022927e305", -5.9890856502292702e+305, null);
    verify("-5.98908565022927e306", -5.9890856502292695e+306, null);
    verify("-5.98908565022927e307", -5.9890856502292698e+307, null);
    verify("-3.42098716329319e306", -3.4209871632931897e+306, null);
    verify("-3.42098716329319e307", -3.4209871632931902e+307, null);
    verify("-3.42098716329319e308", -3.42098716329319e+309, null);
    
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



round012();


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
