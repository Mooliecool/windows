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


var iTestID = 51828;

// -----------------------------------------------------------------------
// date0002.js
//

var HoursPerDay = 24;
var MinutesPerHour = 60;
var SecondsPerMinute = 60;
var msPerSecond = 1000;
var msPerMinute = msPerSecond * SecondsPerMinute;
var msPerHour = msPerMinute * MinutesPerHour;
var msPerDay = HoursPerDay * msPerHour;
var msPerYear = msPerDay * 365;

var inf = 1/0;
var nan = 0/0;
var max = 8640000000000000;
var min = -8640000000000000;

// is DST enabled on this system?
var iOffset;
var iValueOffset;
var strTemp;
var strDT;


function getSavings(){
	strTemp = new Date(2000,7,1).toString();
	if (strTemp.search(/DT/) != -1) { // Daylight time
		apWriteDebug ("DST enabled");
		iOffset = -1;
		iValueOffset = 3600000;
		strDT = "PDT";
	}
	else {
		apWriteDebug("DST disabled");
		iOffset = 0;
		iValueOffset = 0;
		strDT = "PST";
	}
}

// Verifies all properties set to NaN
function vDateNaN(Dt)
{
	var res;

	if(!(res=isNaN(Dt.getFullYear())))
		apLogFailInfo( "Incorrect year property.",NaN,res,"");
	if(!(res=isNaN(Dt.getMonth())))
		apLogFailInfo( "Incorrect month property.",NaN,res,"");
	if(!(res=isNaN(Dt.getDate())))
		apLogFailInfo( "Incorrect date property.",NaN,res,"");
	if(!(res=isNaN(Dt.getHours())))
		apLogFailInfo( "Incorrect hours property.",NaN,res,"");
	if(!(res=isNaN(Dt.getMinutes())))
		apLogFailInfo( "Incorrect minutes property.",NaN,res,"");
	if(!(res=isNaN(Dt.getSeconds())))
		apLogFailInfo( "Incorrect seconds property.",NaN,res,"");
	if(!(res=isNaN(Dt.getMilliseconds())))
		apLogFailInfo( "Incorrect ms property.",NaN,res,"");
}

// Verifies all properties using UTC get functions
function vUTCDateProp(Dt,Yr,Mt,Dy,Hr,Mn,Sc,Ms)
{
	var res;

	if((res=typeof(Dt)) != "object")
		apLogFailInfo("Construction failed.","object",res,"");
	if( (res=Dt.getUTCFullYear()) != Yr)
		apLogFailInfo("Incorrect year property.",Yr,res,"");
	if( (res=Dt.getUTCMonth()) != Mt)
		apLogFailInfo("Incorrect month property.",Mt,res,"");
	if( (res=Dt.getUTCDate()) != Dy)
		apLogFailInfo("Incorrect date property.",Dy,res,"");
	if( (res=Dt.getUTCHours()) != Hr)
		apLogFailInfo("Incorrect hours property.",Hr,res,"");
	if( (res=Dt.getUTCMinutes()) != Mn)
		apLogFailInfo("Incorrect minutes property.",Mn,res,"");
	if( (res=Dt.getUTCSeconds()) != Sc)
		apLogFailInfo("Incorrect seconds property.",Sc,res,"");
	if( (res=Dt.getUTCMilliseconds()) != Ms)
		apLogFailInfo("Incorrect ms property.",Ms,res,"");
}

// Verifies all properties using standard get functions
function vDateProp(Dt,Yr,Mt,Dy,Hr,Mn,Sc,Ms)
{
	var res;

	if((res=typeof(Dt)) != "object")
		apLogFailInfo("Construction failed.","object",res,"");
	if( (res=Dt.getFullYear()) != Yr)
		apLogFailInfo("Incorrect year property.",Yr,res,"");
	if( (res=Dt.getMonth()) != Mt)
		apLogFailInfo("Incorrect month property.",Mt,res,"");
	if( (res=Dt.getDate()) != Dy)
		apLogFailInfo("Incorrect date property.",Dy,res,"");
	if( (res=Dt.getHours()) != Hr)
		apLogFailInfo("Incorrect hours property.",Hr,res,"");
	if( (res=Dt.getMinutes()) != Mn)
		apLogFailInfo("Incorrect minutes property.",Mn,res,"");
	if( (res=Dt.getSeconds()) != Sc)
		apLogFailInfo("Incorrect seconds property.",Sc,res,"");
	if( (res=Dt.getMilliseconds()) != Ms)
		apLogFailInfo("Incorrect ms property.",Ms,res,"");
}

function verify(res,exp)
{
	if ( isNaN(exp) ) {
		if ( !isNaN(res) )
			apLogFailInfo("Wrong result.",exp,res,"");
	}
	else if(res != exp)
		apLogFailInfo("Wrong result.",exp,res,"");
}
	
function TestConstr(){

	apInitScenario("Date(null) constructor.");
	getSavings()

	var x = new Date();
	var y = new Date();
	if(y.valueOf()-x.valueOf() > 100)
		apLogFailInfo("Times are distant.","<100",y.valueOf()-x.valueOf(),"");
	if(x.valueOf == y.valueOf())
		apLogFailInfo("Times are equal.",x,y,"");

	apInitScenario("Date(milliseconds) constructor w/valid values.");
	vUTCDateProp(new Date(msPerHour*24*365),1971,0,1,0,0,0,0);

	apInitScenario("Date(milliseonds) constructor w/ inf == NaN.");
	vDateNaN(new Date(inf));

	apInitScenario("Date(milliseonds) constructor w/ NaN == NaN.");
	vDateNaN(new Date(nan));

	apInitScenario("Date(milliseconds) constructor w/ 0.");
	vUTCDateProp(new Date(0),1970,0,1,0,0,0,0);

	apInitScenario("Date(milliseconds) constructor w/max.");
	vUTCDateProp(new Date(max),275760,8,13,0,0,0,0);

	apInitScenario("Date(milliseconds) constructor w/min.");
	vUTCDateProp(new Date(min),-271821,3,20,0,0,0,0);

	apInitScenario("Date(milliseconds) constructor w/max+1.");
	vDateNaN(new Date(max+1));

	apInitScenario("Date(milliseconds) constructor w/min-1.");
	vDateNaN(new Date(min-1));

	apInitScenario("Date(milliseconds) constructor w/max-1.");
	vUTCDateProp(new Date(max-1),275760,8,12,23,59,59,999);

	apInitScenario("Date(milliseconds) constructor w/min+1.");
	vUTCDateProp(new Date(min+1),-271821,3,20,0,0,0,1);


	apInitScenario("Date(datestring) constructor w/valid values.")
	vUTCDateProp(new Date("12/31/97 11:59:59PM UTC"),1997,11,31,23,59,59,0);

	apInitScenario("Date(datestring) constructor w/invalid values.");
	vDateNaN(new Date("3PM"),1997,11,31,23,59,59,0);

	apInitScenario("Date(datestring) constructor utc values.");
	vUTCDateProp(new Date("12/31/97 11:59:59PM utc"),1997,11,31,23,59,59,0);

	apInitScenario("Date(datestring) constructor UTC-XXXX values.");
	vUTCDateProp(new Date("12/31/97 11:59:59PM UTC-0300"),1998,0,1,2,59,59,0);

	apInitScenario("Date(datestring) constructor UTC+XXXX values.");
	vUTCDateProp(new Date("12/31/97 11:59:59PM UTC+1000"),1997,11,31,13,59,59,0);

	apInitScenario("Date(datestring) constructor PST values.");
	vDateProp(new Date("12/31/97 11:59:59PM PST"),1997,11,31,23,59,59,0);

	apInitScenario("Date(datestring) constructor pst values.");
	vDateProp(new Date("12/31/97 11:59:59PM pst"),1997,11,31,23,59,59,0);

	apInitScenario("Date(datestring) constructor pdt values.");
	vDateProp(new Date("12/31/97 11:59:59AM pdt"),1997,11,31,10,59,59,0);

	apInitScenario("Date(datestring) constructor PDT values.");
	vDateProp(new Date("12/31/97 11:59:59AM PDT"),1997,11,31,10,59,59,0);

	apInitScenario("Date(datestring) constructor 24HR time.");
	vDateProp(new Date("1/31/1898 14:59:59AM PST"),1898,0,31,14,59,59,0);

	apInitScenario("Date(datestring) constructor BC values.");
	vDateProp(new Date("12/31/97 BC 11:59:59AM"),-96,11,31,11,59,59,0);

	apInitScenario("Date(datestring) constructor B.C. time.");
	vDateProp(new Date("1/31/1898 B.C. 2:59:59PM"),-1897,0,31,14,59,59,0);

	apInitScenario("Date(datestring) constructor AD values.");
	vDateProp(new Date("12/31/1997 AD 11:59:59AM"),1997,11,31,11,59,59,0);

	apInitScenario("Date(datestring) constructor A.D. time.");
	vDateProp(new Date("1/31/1898 A.D. 2:59:59PM"),1898,0,31,14,59,59,0);

	apInitScenario("Date(year,month,day) constructor w/valid values.")
	vDateProp(new Date(1997,11,31),1997,11,31,0,0,0,0);

	apInitScenario("Date(year,month,day) abbrev years -> 19XX.")
	vDateProp(new Date(70,0,1),1970,0,1,0,0,0,0);

	apInitScenario("Date(year,month,day) negative years(B.C)")
	vDateProp(new Date(-1000,0,31),-1000,0,31,0,0,0,0);

	apInitScenario("Date(year,month,day,hour) constructor w/valid values.")
	vDateProp(new Date(1997,11,31,23),1997,11,31,23,0,0,0);

	apInitScenario("Date(year,month,day,hour) 1 hour before 0ms.")
	vDateProp(new Date(1969,11,31,23),1969,11,31,23,0,0,0);
	
	apInitScenario("Date(year,month,day,hour) 1 hour after 0ms.")
	vDateProp(new Date(1970,0,1,1),1970,0,1,1,0,0,0);

	apInitScenario("Date(year,month,day,hour) 0 year")
	vDateProp(new Date(0,0,1,0),1900,0,1,0,0,0,0);

	apInitScenario("Date(year,month,day,hour,minutes) constructor w/valid values.")
	vDateProp(new Date(1997,11,31,23,59),1997,11,31,23,59,0,0);

	apInitScenario("Date(year,month,day,hour,minutes,seconds) constructor w/valid values.")
	vDateProp(new Date(1997,11,31,23,59,59),1997,11,31,23,59,59,0);

	apInitScenario("Date(year,month,day,hour,minutes,seconds) with NaN")
	vDateNaN(new Date(nan,nan,nan,nan,nan,nan));

	apInitScenario("Date(year,month,day,hour,minutes,seconds) with inf")
	vDateNaN(new Date(inf,inf,inf,inf,inf,inf));

	apInitScenario("Date(year,month,day,hour,minutes,seconds) with null")
	vDateProp(new Date(null,null,null,null,null,null),1899,11,31,0,0,0,0);

	apInitScenario("Date(year,month,day,hour,minutes,seconds) Invalid dates(Dec 34 1999 25:67:80)")
	vDateProp(new Date(1999,11,34,25,67,80),2000,0,4,2,8,20,0);

	apInitScenario("Date(year,month,day,hour,minutes,seconds,ms) constructor 1ms before midnight.")
	vDateProp(new Date(1997,11,31,23,59,59,999),1997,11,31,23,59,59,999);

	apInitScenario("Date(year,month,day,hour,minutes,seconds,ms) constructor 1ms on 2/29/97.")
	vDateProp(new Date(1997,1,29,0,0,0,1),1997,2,1,0,0,0,1);

	apInitScenario("Date(year,month,day,hour,minutes,seconds,ms) constructor 1ms on 2/29/96.")
	vDateProp(new Date(1996,1,29,0,0,0,1),1996,1,29,0,0,0,1);
}

function TestGetSet()
{
	var Dt = new Date(1900,0,1);
	var exp = 0;
	var res =0;
	
	apInitScenario("getTime()/setTime() - valid values.");
	exp = 19;
	Dt.setTime(exp);
	verify(Dt.getTime(),exp);

	apInitScenario("getDate()/setDate() - valid values.");
	exp = 31;
	Dt.setDate(exp);
	verify(Dt.getDate(),exp);

	apInitScenario("getUTCDate()/setUTCDate() - with 0->31.");
	exp = 31;
	Dt.setUTCDate(0);
	verify(Dt.getUTCDate(),exp);

	apInitScenario("getYear()/setYear() - with 5 -> 1905.");
	exp = 5;
	Dt.setYear(5);
	verify(Dt.getYear(), exp);

	apInitScenario("getFullYear()/setFullYear() - valid values.");
	exp = 2119;
	Dt.setFullYear(exp);
	verify(Dt.getFullYear(),exp);

	apInitScenario("getUTCFullYear()/setFullYear() - valid values.");
	exp = 100;
	Dt.setUTCFullYear(exp);
	verify(Dt.getUTCFullYear(),exp);

	apInitScenario("getMonth()/setMonth() - with 12 -> 0.");
	exp = 0;
	Dt.setMonth(12);
	verify(Dt.getMonth(),exp);

	apInitScenario("getUTCMonth()/setUTCMonth() -  with -1 ->11.");
	exp = 11;
	Dt.setUTCMonth(-1);
	verify(Dt.getUTCMonth(),exp);

	apInitScenario("getHours()/setHours() -  with 24 -> 0");
	exp = 0;
	Dt.setHours(24);
	verify(Dt.getHours(),exp);

	apInitScenario("getUTCHours()/setUTCHours() -  with -1 -> 23");
	exp = 23;
	Dt.setUTCHours(-1);
	verify(Dt.getUTCHours(),exp);

	apInitScenario("getMinutes()/setMinutes() -  valid values");
	exp = 30;
	Dt.setMinutes(exp);
	verify(Dt.getMinutes(),exp);

	apInitScenario("getUTCMinutes()/setUTCMinutes() -  with 120 -> 0");
	exp = 0;
	Dt.setUTCMinutes(120);
	verify(Dt.getUTCMinutes(),exp);

	apInitScenario("getSeconds()/setSeconds() -  with 59.999 -> 59");
	exp = 59;
	Dt.setSeconds(59.999);
	verify(Dt.getSeconds(),exp);

	apInitScenario("getUTCSeonds()/setUTCSeconds() -  with (-.0000001) -> 0");
	exp = 0;
	Dt.setUTCSeconds(-.0000001);
	verify(Dt.getUTCSeconds(),exp);

	apInitScenario("getMilliSeconds()/setMilliSeconds() -  with 999");
	exp = 999;
	Dt.setMilliseconds(exp);
	verify(Dt.getMilliseconds(),exp);

	apInitScenario("getUTCMilliSeonds()/setUTCMilliSeconds() -  with -1000 -> 0");
	exp = 0;
	Dt.setUTCMilliseconds(-1000);
	verify(Dt.getUTCMilliseconds(),exp);

	apInitScenario("All props set to NaN  == NaN")
	Dt.setTime(nan);
	Dt.setDate(nan);
	Dt.setYear(nan);
	Dt.setMonth(nan);
	Dt.setHours(nan);
	Dt.setMinutes(nan);
	Dt.setSeconds(nan);
	Dt.setMilliseconds(nan);
	vDateNaN(Dt);

	apInitScenario("One prop set to NaN  == NaN")
	Dt.setTime(0);
	Dt.setMilliseconds(nan);
	vDateNaN(Dt);

	apInitScenario("setTime(null) == setTime(0)");
	exp = 0;
	Dt.setTime(null);
	verify(Dt.getTime(),exp);

	apInitScenario("getDays() for Sun = 0");
	exp = 0;
	Dt.setTime(Date.parse("Jul 27 17:57:24 PDT 1997"));
	verify(Dt.getDay(),exp);

	apInitScenario("getDays() for Mon = 1");
	exp = 1;
	Dt.setTime(Date.parse("Jul 28 17:57:24 PDT 1997"));
	verify(Dt.getDay(),exp);

	apInitScenario("getDays() for Tues = 2");
	exp = 2;
	Dt.setTime(Date.parse("Jul 29 17:57:24 PDT 1997"));
	verify(Dt.getDay(),exp);

	apInitScenario("getDays() for Wed = 3");
	exp = 3;
	Dt.setTime(Date.parse("Jul 30 17:57:24 PDT 1997"));
	verify(Dt.getDay(),exp);

	apInitScenario("getDays() for Thurs = 4");
	exp = 4;
	Dt.setTime(Date.parse("Jul 31 17:57:24 PDT 1997"));
	verify(Dt.getDay(),exp);

	apInitScenario("getDays() for Fri = 5");
	exp = 5;
	Dt.setTime(Date.parse("Aug 1 17:57:24 PDT 1997"));
	verify(Dt.getDay(),exp);

	apInitScenario("getDays() for Sat = 6");
	exp = 6;
	Dt.setTime(Date.parse("Aug 2 17:57:24 PDT 1997"));
	verify(Dt.getDay(),exp);



	apInitScenario("getDay() vs getUTCDay: same day");
	Dt.setTime(Date.parse("Aug 1 0" + (iOffset + 1) + ":00:00 PDT 1997"));
	verify(Dt.getDay(),Dt.getUTCDay());

	apInitScenario("getDay() vs getUTCDay: different day");
	Dt.setTime(Date.parse("Aug 1 17:00:00 PDT 1997"));
	verify(Dt.getDay(),Dt.getUTCDay()-1);
}

function TestString()
{
	var Dt = new Date();
	var exp, res;
	
////  roundtrip all functions with AD values
	apInitScenario("toString() Roundtrip with AD(no tag added) dates");
	exp = Dt.toString();
	res	= (new Date(exp)).toString()
	verify(exp,res);

	apInitScenario("toLocaleString() Roundtrip AD(no tag added) dates");
	exp = Dt.toLocaleString();
	res	= (new Date(exp)).toLocaleString()
	verify(exp,res);

	apInitScenario("toUTCString() Roundtrip AD(no tag added) dates");
	exp = Dt.toUTCString();
	res	= (new Date(exp)).toUTCString()
	verify(exp,res);

	apInitScenario("toGMTString() Roundtrip AD(no tag added) dates");
	exp = Dt.toGMTString();
	res	= (new Date(exp)).toGMTString()
	verify(exp,res);

////  roundtrip all functions with BC values
	Dt.setFullYear(-100,0,1);
	apInitScenario("toString() Roundtrip with BC dates");
	exp = Dt.toString();
	res	= (new Date(exp)).toString()
	verify(exp,res);

	apInitScenario("toLocaleString() Roundtrip with BC dates");
	exp = Dt.toLocaleString();
	res	= (new Date(exp)).toLocaleString()
	verify(exp,res);

	apInitScenario("toUTCString() Roundtrip with BC dates");
	exp = Dt.toUTCString();
	res	= (new Date(exp)).toUTCString()
	verify(exp,res);

	apInitScenario("toGMTString() Roundtrip with BC dates");
	exp = Dt.toGMTString();
	res	= (new Date(exp)).toGMTString()
	verify(exp,res);

////  roundtrip all functions with B.C. values
	Dt.setFullYear("12/31/100 B.C. 11:59:59PM UTC");
	apInitScenario("toString() Roundtrip with B.C. dates");
	exp = Dt.toString();
	res	= (new Date(exp)).toString()
	verify(exp,res);

	apInitScenario("toLocaleString() Roundtrip with B.C. dates");
	exp = Dt.toLocaleString();
	res	= (new Date(exp)).toLocaleString()
	verify(exp,res);

	apInitScenario("toUTCString() Roundtrip with B.C. dates");
	exp = Dt.toUTCString();
	res	= (new Date(exp)).toUTCString()
	verify(exp,res);

	apInitScenario("toGMTString() Roundtrip with B.C. dates");
	exp = Dt.toGMTString();
	res	= (new Date(exp)).toGMTString()
	verify(exp,res);

}

function TestMisc(){
  @if(@_fast)
     var exp, res;
  @end

	apInitScenario("getTimezoneOffset() for NaN");
	res = isNaN((new Date(nan)).getTimezoneOffset());
	verify(true,res);

////  valueOf()
	apInitScenario("valueOf() for 0ms");
	exp = 0;
	res = (new Date(0)).valueOf();
	verify(exp,res);

	apInitScenario("valueOf() for 1/1/1971");
	exp = msPerYear;
	res = (new Date("1/1/1971 12:00:00AM UTC")).valueOf();
	verify(exp,res);

	apInitScenario("valueOf() for 1/1/1969");
	exp = -msPerYear;
	res = (new Date("1/1/1969 12:00:00AM UTC")).valueOf();
	verify(exp,res);

	apInitScenario("valueOf() for 1/1/1974(Verify Leap Year forwards)");
	exp = msPerYear*4 + msPerDay;
	res = (new Date("1/1/1974 12:00:00AM UTC")).valueOf();
	verify(exp,res);

	apInitScenario("valueOf() for 1/1/1966(Verify Leap Year backwards)");
	exp = -msPerYear*4 - msPerDay;
	res = (new Date("1/1/1966 12:00:00AM UTC")).valueOf();
	verify(exp,res);

////  Date.parse()
	apInitScenario("Date.parse roundtrip w/ zero values");
	exp = Date.parse("1/1/1970 00:00:00 UTC");
	res = Date.parse(new Date(exp));
	verify(res,exp);

	apInitScenario("Date.parse roundtrip w/positive values");
	exp = Date.parse("1/1/2000 12:00:00AM UTC");
	res = Date.parse(new Date(exp));
	verify(res,exp);

	apInitScenario("Date.parse roundtrip w/negative values");
	exp = Date.parse("1/1/1966 12:00:00AM UTC");
	res = Date.parse(new Date(exp));
	verify(res,exp);

	apInitScenario("Date.parse roundtrip at max");
	exp = Date.parse("8/13/275760 12:00:00AM UTC");
	res = Date.parse(new Date(exp));
	verify(res,exp);

	apInitScenario("Date.parse roundtrip at min");
	exp = Date.parse("3/20/-271821 12:00:00AM UTC");
	res = Date.parse(new Date(exp));
	verify(res,exp);

	apInitScenario("Date.parse roundtrip at max-1");
	exp = Date.parse("9/12/275760 23:59:59 UTC");
	res = Date.parse(new Date(exp));
	verify(res,exp);

	apInitScenario("Date.parse roundtrip at min+1");
	exp = Date.parse("3/20/-271821 12:00:01AM UTC");
	res = Date.parse(new Date(exp));
	verify(res,exp);

}

function TestArith()
{
	var Dt = new Date("12/31/97 11:59:59PM");
	var exp,res;

////  Test addition
	apInitScenario("DateObj + string == concat");
	exp ="Wed Dec 31 23:59:59 PST 1997 Plus this";
	res = Dt+" Plus this";
	verify(exp,res);
	
	apInitScenario("DateObj + null == concat");
	exp ="Wed Dec 31 23:59:59 PST 1997null";
	res = Dt+null;
	verify(exp,res);

	apInitScenario("DateObj + nan == concat");
	exp ="Wed Dec 31 23:59:59 PST 1997NaN";
	res = Dt+nan;
	verify(exp,res);

	apInitScenario("DateObj  - string== NaN");
	res = isNaN(Dt-"Minus this");
	verify(true,res);

	apInitScenario("DateObj  - 0 == ms - 0");
	exp = Dt.valueOf() - 0;
	res = Dt-0;
	verify(exp,res);

	apInitScenario("DateObj  - (-0) == ms - 0");
	exp = Dt.valueOf();
	res = Dt-(-0);
	verify(exp,res);

	apInitScenario("DateObj  - 10 == ms - 10");
	exp = Dt.valueOf() - 10;
	res = Dt-10;
	verify(exp,res);

	apInitScenario("DateObj  - (-)10 == ms + 10");
	exp = Dt.valueOf() + 10;
	res = Dt-(-10);
	verify(exp,res);

	apInitScenario("DateObj  * 0 == 0ms");
	exp = 0;
	res = Dt*0;
	verify(exp,res);

	apInitScenario("DateObj  * 10 == ms * 10");
	exp = Dt.valueOf() * 10;
	res = Dt*10;
	verify(exp,res);

	apInitScenario("DateObj  / 0 == inf");
	res = Dt/0;
	exp = inf;
	verify(exp,res);

	apInitScenario("DateObj  / 10 == ms / 10");
	exp = Dt.valueOf() / 10;
	res = Dt/10;
	verify(exp,res);
}

function date0002()
{
	apInitTest("date0002");
	if( new Date("12/25/2002 12:00:00 PST").getTimezoneOffset()/60 == 8) {
	TestConstr();
	TestGetSet();	
	TestString();
	TestMisc();
	TestArith();
	}
	apEndTest();
}


date0002();


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
