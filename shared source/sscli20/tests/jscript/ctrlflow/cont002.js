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


var iTestID = 51826;


function cont002() {

    apInitTest("cont002 ");


    //----------------------------------------------------------------------------
    apInitScenario("1. verify continue in massive test condition");

    var n=0;

    for (var i=0; i<6; i++) {
        if (false) n=0;
	    else if (false) n=1;
	    else if (false) n=2;
	    else if (false) n=3;
	    else if (false) n=4;
	    else if (false) n=5;
	    else if (false) n=6;
	    else if (false) n=7;
	    else if (false) n=8;
	    else if (false) n=9;
	    else if (false) n=10;
	    else if (false) n=11;
	    else if (false) n=12;
	    else if (false) n=13;
	    else if (false) n=14;
	    else if (false) n=15;
	    else if (false) n=16;
	    else if (false) n=17;
	    else if (false) n=18;
	    else if (false) n=19;
	    else if (false) n=20;
	    else if (false) n=21;
	    else if (false) n=22;
	    else if (false) n=23;
	    else if (false) n=24;
	    else if (false) n=25;
	    else if (false) n=26;
	    else if (false) n=27;
	    else if (false) n=28;
	    else if (false) n=29;
	    else if (false) n=30;
	    else if (false) n=31;
	    else if (false) n=32;
	    else if (false) n=33;
	    else if (false) n=34;
	    else if (false) n=35;
	    else if (false) n=36;
	    else if (false) n=37;
	    else if (false) n=38;
	    else if (false) n=39;
	    else if (false) n=40;
	    else if (false) n=41;
	    else if (false) n=42;
	    else if (false) n=43;
	    else if (false) n=44;
	    else if (false) n=45;
	    else if (false) n=46;
	    else if (false) n=47;
	    else if (false) n=48;
	    else if (false) n=49;
	    else if (false) n=50;
	    else if (false) n=51;
	    else if (false) n=52;
	    else if (false) n=53;
	    else if (false) n=54;
	    else if (false) n=55;
	    else if (false) n=56;
	    else if (false) n=57;
	    else if (false) n=58;
	    else if (false) n=59;
	    else if (false) n=60;
	    else if (false) n=61;
	    else if (false) n=62;
	    else if (false) n=63;
	    else if (false) n=64;
	    else if (false) n=65;
	    else if (false) n=66;
	    else if (false) n=67;
	    else if (false) n=68;
	    else if (false) n=69;
	    else if (false) n=70;
	    else if (false) n=71;
	    else if (false) n=72;
	    else if (false) n=73;
	    else if (false) n=74;
	    else if (false) n=75;
	    else if (false) n=76;
	    else if (false) n=77;
	    else if (false) n=78;
	    else if (false) n=79;
	    else if (false) n=80;
	    else if (false) n=81;
	    else if (false) n=82;
	    else if (false) n=83;
	    else if (false) n=84;
	    else if (false) n=85;
	    else if (false) n=86;
	    else if (false) n=87;
	    else if (false) n=88;
	    else if (false) n=89;
	    else if (false) n=90;
	    else if (false) n=91;
	    else if (false) n=92;
	    else if (false) n=93;
	    else if (false) n=94;
	    else if (false) n=95;
	    else if (false) n=96;
	    else if (false) n=97;
	    else if (false) n=98;
	    else if (false) n=99;
	    else if (false) n=100;
	    else if (false) n=101;
	    else if (false) n=102;
	    else if (false) n=103;
	    else if (false) n=104;
	    else if (false) n=105;
	    else if (false) n=106;
	    else if (false) n=107;
	    else if (false) n=108;
	    else if (false) n=109;
	    else if (false) n=110;
	    else if (false) n=111;
	    else if (false) n=112;
	    else if (false) n=113;
	    else if (false) n=114;
	    else if (false) n=115;
	    else if (false) n=116;
	    else if (false) n=117;
	    else if (false) n=118;
	    else if (false) n=119;
	    else if (false) n=120;
	    else if (false) n=121;
	    else if (false) n=122;
	    else if (false) n=123;
	    else if (false) n=124;
	    else if (false) n=125;
	    else if (false) n=126;
	    else if (false) n=127;
	    else if (false) n=128;
	    else if (false) n=129;
	    else if (false) n=130;
	    else if (false) n=131;
	    else if (false) n=132;
	    else if (false) n=133;
	    else if (false) n=134;
	    else if (false) n=135;
	    else if (false) n=136;
	    else if (false) n=137;
	    else if (false) n=138;
	    else if (false) n=139;
	    else if (false) n=140;
	    else if (false) n=141;
	    else if (false) n=142;
	    else if (false) n=143;
	    else if (false) n=144;
	    else if (false) n=145;
	    else if (false) n=146;
	    else if (false) n=147;
	    else if (false) n=148;
	    else if (false) n=149;
	    else if (false) n=150;
	    else if (false) n=151;
	    else if (false) n=152;
	    else if (false) n=153;
	    else if (false) n=154;
	    else if (false) n=155;
	    else if (false) n=156;
	    else if (false) n=157;
	    else if (false) n=158;
	    else if (false) n=159;
	    else if (false) n=160;
	    else if (false) n=161;
	    else if (false) n=162;
	    else if (false) n=163;
	    else if (false) n=164;
	    else if (false) n=165;
	    else if (false) n=166;
	    else if (false) n=167;
	    else if (false) n=168;
	    else if (false) n=169;
	    else if (false) n=170;
	    else if (false) n=171;
	    else if (false) n=172;
	    else if (false) n=173;
	    else if (false) n=174;
	    else if (false) n=175;
	    else if (false) n=176;
	    else if (false) n=177;
	    else if (false) n=178;
	    else if (false) n=179;
	    else if (false) n=180;
	    else if (false) n=181;
	    else if (false) n=182;
	    else if (false) n=183;
	    else if (false) n=184;
	    else if (false) n=185;
	    else if (false) n=186;
	    else if (false) n=187;
	    else if (false) n=188;
	    else if (false) n=189;
	    else if (false) n=190;
	    else if (false) n=191;
	    else if (false) n=192;
	    else if (false) n=193;
	    else if (false) n=194;
	    else if (false) n=195;
	    else if (false) n=196;
	    else if (false) n=197;
	    else if (false) n=198;
	    else if (false) n=199;
	    else if (false) n=200;
	    else if (false) n=201;
	    else if (false) n=202;
	    else if (false) n=203;
	    else if (false) n=204;
	    else if (false) n=205;
	    else if (false) n=206;
	    else if (false) n=207;
	    else if (false) n=208;
	    else if (false) n=209;
	    else if (false) n=210;
	    else if (false) n=211;
	    else if (false) n=212;
	    else if (false) n=213;
	    else if (false) n=214;
	    else if (false) n=215;
	    else if (false) n=216;
	    else if (false) n=217;
	    else if (false) n=218;
	    else if (false) n=219;
	    else if (false) n=220;
	    else if (false) n=221;
	    else if (false) n=222;
	    else if (false) n=223;
	    else if (false) n=224;
	    else if (false) n=225;
	    else if (false) n=226;
	    else if (false) n=227;
	    else if (false) n=228;
	    else if (false) n=229;
	    else if (false) n=230;
	    else if (false) n=231;
	    else if (false) n=232;
	    else if (false) n=233;
	    else if (false) n=234;
	    else if (false) n=235;
	    else if (false) n=236;
	    else if (false) n=237;
	    else if (false) n=238;
	    else if (false) n=239;
	    else if (false) n=240;
	    else if (false) n=241;
	    else if (false) n=242;
	    else if (false) n=243;
	    else if (false) n=244;
	    else if (false) n=245;
	    else if (false) n=246;
	    else if (false) n=247;
	    else if (false) n=248;
	    else if (false) n=249;
	    else if (false) n=250;
	    else if (false) n=251;
	    else if (false) n=252;
	    else if (false) n=253;
	    else if (false) n=254;
	    else if (false) n=255;
	    else if (false) n=256;
	    else if (false) n=257;
	    else if (false) n=258;
	    else if (false) n=259;
	    else if (false) n=260;
	    else if (false) n=261;
	    else if (false) n=262;
	    else if (false) n=263;
	    else if (false) n=264;
	    else if (false) n=265;
	    else if (false) n=266;
	    else if (false) n=267;
	    else if (false) n=268;
	    else if (false) n=269;
	    else if (false) n=270;
	    else if (false) n=271;
	    else if (false) n=272;
	    else if (false) n=273;
	    else if (false) n=274;
	    else if (false) n=275;
	    else if (false) n=276;
	    else if (false) n=277;
	    else if (false) n=278;
	    else if (false) n=279;
	    else if (false) n=280;
	    else if (false) n=281;
	    else if (false) n=282;
	    else if (false) n=283;
	    else if (false) n=284;
	    else if (false) n=285;
	    else if (false) n=286;
	    else if (false) n=287;
	    else if (false) n=288;
	    else if (false) n=289;
	    else if (false) n=290;
	    else if (false) n=291;
	    else if (false) n=292;
	    else if (false) n=293;
	    else if (false) n=294;
	    else if (false) n=295;
	    else if (false) n=296;
	    else if (false) n=297;
	    else if (false) n=298;
	    else if (false) n=299;
	    else if (false) n=300;
	    else if (false) n=301;
	    else if (false) n=302;
	    else if (false) n=303;
	    else if (false) n=304;
	    else if (false) n=305;
	    else if (false) n=306;
	    else if (false) n=307;
	    else if (false) n=308;
	    else if (false) n=309;
	    else if (false) n=310;
	    else if (false) n=311;
	    else if (false) n=312;
	    else if (false) n=313;
	    else if (false) n=314;
	    else if (false) n=315;
	    else if (false) n=316;
	    else if (false) n=317;
	    else if (false) n=318;
	    else if (false) n=319;
	    else if (false) n=320;
	    else if (false) n=321;
	    else if (false) n=322;
	    else if (false) n=323;
	    else if (false) n=324;
	    else if (false) n=325;
	    else if (false) n=326;
	    else if (false) n=327;
	    else if (false) n=328;
	    else if (false) n=329;
	    else if (false) n=330;
	    else if (false) n=331;
	    else if (false) n=332;
	    else if (false) n=333;
	    else if (false) n=334;
	    else if (false) n=335;
	    else if (false) n=336;
	    else if (false) n=337;
	    else if (false) n=338;
	    else if (false) n=339;
	    else if (false) n=340;
@cc_on
@if (!@_win16)
	    else if (false) n=341;
	    else if (false) n=342;
	    else if (false) n=343;
	    else if (false) n=344;
	    else if (false) n=345;
	    else if (false) n=346;
	    else if (false) n=347;
	    else if (false) n=348;
	    else if (false) n=349;
	    else if (false) n=350;
	    else if (false) n=351;
	    else if (false) n=352;
	    else if (false) n=353;
	    else if (false) n=354;
	    else if (false) n=355;
	    else if (false) n=356;
	    else if (false) n=357;
	    else if (false) n=358;
	    else if (false) n=359;
	    else if (false) n=360;
	    else if (false) n=361;
	    else if (false) n=362;
	    else if (false) n=363;
	    else if (false) n=364;
	    else if (false) n=365;
	    else if (false) n=366;
	    else if (false) n=367;
	    else if (false) n=368;
	    else if (false) n=369;
	    else if (false) n=370;
	    else if (false) n=371;
	    else if (false) n=372;
	    else if (false) n=373;
	    else if (false) n=374;
	    else if (false) n=375;
	    else if (false) n=376;
	    else if (false) n=377;
	    else if (false) n=378;
	    else if (false) n=379;
	    else if (false) n=380;
	    else if (false) n=381;
	    else if (false) n=382;
	    else if (false) n=383;
	    else if (false) n=384;
	    else if (false) n=385;
	    else if (false) n=386;
	    else if (false) n=387;
	    else if (false) n=388;
	    else if (false) n=389;
	    else if (false) n=390;
	    else if (false) n=391;
	    else if (false) n=392;
	    else if (false) n=393;
	    else if (false) n=394;
	    else if (false) n=395;
	    else if (false) n=396;
	    else if (false) n=397;
	    else if (false) n=398;
	    else if (false) n=399;
	    else if (false) n=400;
	    else if (false) n=401;
	    else if (false) n=402;
	    else if (false) n=403;
	    else if (false) n=404;
	    else if (false) n=405;
	    else if (false) n=406;
	    else if (false) n=407;
	    else if (false) n=408;
	    else if (false) n=409;
	    else if (false) n=410;
	    else if (false) n=411;
	    else if (false) n=412;
	    else if (false) n=413;
	    else if (false) n=414;
	    else if (false) n=415;
	    else if (false) n=416;
	    else if (false) n=417;
	    else if (false) n=418;
	    else if (false) n=419;
	    else if (false) n=420;
	    else if (false) n=421;
	    else if (false) n=422;
	    else if (false) n=423;
	    else if (false) n=424;
	    else if (false) n=425;
	    else if (false) n=426;
	    else if (false) n=427;
	    else if (false) n=428;
	    else if (false) n=429;
	    else if (false) n=430;
	    else if (false) n=431;
	    else if (false) n=432;
	    else if (false) n=433;
	    else if (false) n=434;
	    else if (false) n=435;
	    else if (false) n=436;
	    else if (false) n=437;
	    else if (false) n=438;
	    else if (false) n=439;
	    else if (false) n=440;
	    else if (false) n=441;
	    else if (false) n=442;
	    else if (false) n=443;
	    else if (false) n=444;
	    else if (false) n=445;
	    else if (false) n=446;
	    else if (false) n=447;
	    else if (false) n=448;
	    else if (false) n=449;
	    else if (false) n=450;
	    else if (false) n=451;
	    else if (false) n=452;
	    else if (false) n=453;
	    else if (false) n=454;
	    else if (false) n=455;
	    else if (false) n=456;
	    else if (false) n=457;
	    else if (false) n=458;
	    else if (false) n=459;
	    else if (false) n=460;
	    else if (false) n=461;
	    else if (false) n=462;
	    else if (false) n=463;
	    else if (false) n=464;
	    else if (false) n=465;
	    else if (false) n=466;
	    else if (false) n=467;
	    else if (false) n=468;
	    else if (false) n=469;
	    else if (false) n=470;
	    else if (false) n=471;
	    else if (false) n=472;
	    else if (false) n=473;
	    else if (false) n=474;
	    else if (false) n=475;
	    else if (false) n=476;
	    else if (false) n=477;
	    else if (false) n=478;
	    else if (false) n=479;
	    else if (false) n=480;
	    else if (false) n=481;
	    else if (false) n=482;
	    else if (false) n=483;
	    else if (false) n=484;
	    else if (false) n=485;
	    else if (false) n=486;
	    else if (false) n=487;
	    else if (false) n=488;
	    else if (false) n=489;
	    else if (false) n=490;
	    else if (false) n=491;
	    else if (false) n=492;
	    else if (false) n=493;
	    else if (false) n=494;
	    else if (false) n=495;
	    else if (false) n=496;
	    else if (false) n=497;
	    else if (false) n=498;
	    else if (false) n=499;
@end
	    else if (i>0) continue;
        else n++;
    }

    if ( n != 1 ) apLogFailInfo( "deep nesting failed ", 1, n,"");


    apEndTest();

}



cont002();


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
