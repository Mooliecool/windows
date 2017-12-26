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


var iTestID = 74876;

/* -------------------------------------------------------------------------
  Test: 	DTUTC01
   
  
 
  Component:	JScript
 
  Major Area:	Date Object
 
  Test Area:	Changes in functionality from 5.1 to 5.5/7.0
 
  Keywords:	new Date object constructor UTC toDateString toTimeString
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:
		1.  Zero arguments

		2.  One argument (VT_DATE value)

		3.  Two arguments

		4.  Three arguments

		5.  Four arguments

		6.  Five arguments

		7.  Six arguments

		8.  Seven arguments

		9.  Eight arguments -- should throw an error in JS7 strict mode, otherwise ignored

		10. String arguments

		11. Array arguments

		12. Object arguments

		13-35.  Negative numbers
				13.  Year = -1
				14.  Year = -100000
				15.  Month = -1
				16.  Month = -11
				17.  Month = -12
				18.  Month = -1000
				19.  Day = -1
				20.  Day = -29
				21.  Day = -30
				22.  Day = -31
				23.  Day = -1000
				24.  Hour = -1
				25.  Hour = -23
				26.  Hour = -24
				27.  Minute = -1
				28.  Minute = -59
				29.  Minute = -60
				30.  Second = -1
				31.  Second = -59
				32.  Second = -60
				33.  Millisecond = -1
				34.  Millisecond = -999
				35.  Millisecond = -1000

		36-42.  Zero
				36.  Year = 0
				37.  Month = 0
				38.  Day = 0
				39.  Hour = 0
				40.  Minute = 0
				41.  Second = 0
				42.  Millisecond = 0

		43-66.  Numbers larger than required
				43.  Year = 100000
				44.  Month = 12
				45.  Month = 13
				46.  Month = 1000
				47.  Day = 29 on 30-day month
				48.  Day = 30 on 30-day month
				49.  Day = 30 on 31-day month
				50.  Day = 31 on 31-day month
				51.  Day = 31 on 30-day month
				52.  Day = 32
				53.  Day = 33
				54.  Day = 1000
				55.  Hour = 23
				56.  Hour = 24
				57.  Hour = 1000
				58.  Minute = 59
				59.  Minute = 60
				60.  Minute = 61
				61.  Second = 59
				62.  Second = 60
				63.  Second = 61
				64.  Millisecond = 999
				65.  Millisecond = 1000
				66.  Millisecond = 1001

		67-80.  Floating point values (should truncate to integer)
				67.  Year = positive floating point number
				68.  Year = negative floating point number
				69.  Month = positive floating point number
				70.  Month = negative floating point number
				71.  Day = positive floating point number
				72.  Day = negative floating point number
				73.  Hour = positive floating point number
				74.  Hour = negative floating point number
				75.  Minute = positive floating point number
				76.  Minute = negative floating point number
				77.  Second = positive floating point number
				78.  Second = negative floating point number
				79.  Millisecond = positive floating point number
				80.  Millisecond = negative floating point number

		81-101. February 29th placement
				81.  2/29/1996 (Feb 29)
				82.  2/29/1999 (Mar 1)
				83.  2/29/2000 (Feb 29)
				84.  2/29/2001 (Mar 1)
				85.  2/29/1900 (Mar 1)
				86.  1/60/2000 (Feb 29)
				87.  1/60/1999 (Mar 1)
				88.  2/29/1600 (Feb 29)
				89.  2/29/2400 (Feb 29)
				90.  2/29/2000 23:00:00:000 (Feb 29)
				91.  2/29/2000 24:00:00:000 (Mar 1)
				92.  2/29/2000 23:59:00:000 (Feb 29)
				93.  2/29/2000 23:60:00:000 (Mar 1)
				94.  2/29/2000 23:59:59:000 (Feb 29)
				95.  2/29/2000 23:59:60:000 (Mar 1)
				96.  2/29/2000 23:59:59:999 (Feb 29)
				97.  2/29/2000 23:59:59:1000 (Mar 1)
				98  2/29/2000 23:59:60:-0001 (Feb 29)
				99. 2/29/2000 23:60:-1:000 (Feb 29)
				100. 2/29/2000 24:-1:0:000 (Feb 29)
				101. 3/1/2000  -1:0:0:000 (Feb 29)

		102-103. Daylight Savings Time
				102. First Sunday of April, 2:00am -> 3:00am
				102. Last Sunday of October, 3:00am -> 2:00am

		104-119. Combinations of larger/smaller than expected month and day values
				104. Year = 1999, Month = 11, Day = 60 (should produce February 29, 2000)
				105. Year = 1999, Month = 15, Day = 31 (should produce May 1, 2000)
				106. Year = -1, Month = 23, Day = 32 (should produce Jan 1, 1 ad)
				107. Year = -1, Month = 12, Day = 367 (should produce Jan 1, 1 ad)
				108. Year = -1, Month = 23, Day = 31, time = 23:59:59:999 (Dec 31, 1 bc)
				109. Year = -1, Month = 23, Day = 31, time = 23:59:59:1000 (Jan 1, 1 ad)
				110. Year = -1, Month = 23, Day = 31, time = 23:59:50:-1 (Dec 31, 1 bc)
				111. Year = -1, Month = 23, Day = 31, time = 23:59:60:000 (Jan 1, 1 ad)
				112. Year = -1, Month = 23, Day = 31, time = 23:60:-1:000 (Dec 31, 1 bc)
				113. Year = -1, Month = 23, Day = 31, time = 23:60:0:000 (Jan 1, 1 ad)
				114. Year = -1, Month = 23, Day = 31, time = 24:-1:0:000 (Dec 31, 1 bc)
				115. Year = -1, Month = 23, Day = 31, time = 24:0:0:000 (Jan 1, 1 ad)
				116. Date.valueOf() == 8640000000000000
				117. Date.valueOf() == 8640000000000001 (NaN)
				118. Date.valueOf() == -8640000000000000
				119. Date.valueOf() == -8640000000000001 (NaN)


  Abstract:	 Date.UTC() -- All parameters after the first two are optional
			 (changed from first three parameters)
 ---------------------------------------------------------------------------
  Category:			Functionality
 
  Product:			JScript
 
  Related Files: 
 
  Notes:
 ---------------------------------------------------------------------------
  
 

 -------------------------------------------------------------------------*/



//////////
//
//  Helper functions


function verify(sAct, sExp, sMes){
	if (sAct != sExp)
        apLogFailInfo( "*** Scenario failed: "+sMes+" ", sExp, sAct, "");
}

function DateCompare (d1, d2) {
	if (Math.abs(d2 - d1) > 1000) {
		return false;
	}
	else {
		return true;
	}
}



//////////
//
// Global variables


var dDate1;



function dtutc01() {

	// Find out whether it's Daylight Savings Time or not
	//    Algorithm:  If current date/time is between 
	//                   First Sunday of April, 2:00am, and
	//                   Last Sunday of October, 3:00am
	
	var dCurrent;
	var dDSTStart;
	var dDSTEnd;
	
	dCurrent = new Date();
	dDSTStart = new Date();
	dDSTEnd = new Date();


	// Go to start of April, 2:00am
	dDSTStart.setMonth(3);
	dDSTStart.setDate(1);
	dDSTStart.setHours(2);
	while (dDSTStart.getDay() != 0) {
		dDSTStart.setDate(dDSTStart.getDate() + 1);
	}

	// Go to end of October, 3:00am
	dDSTEnd.setMonth(9);
	dDSTEnd.setDate(31);
	dDSTEnd.setHours(3);
	while (dDSTEnd.getDay() !=0) {
		dDSTEnd.setDate(dDSTEnd.getDate() - 1);
	}


	var iUTCOffset;
	var iUTCValueOffset;
	var strDT;
	if ((dCurrent.valueOf() > dDSTStart.valueOf()) && (dCurrent.valueOf() < dDSTEnd.valueOf())) {
		// is DST enabled on this system?
		var strTemp = new Date().toString();
		if (strTemp.search(/PST/) == -1) { // Daylight time
			iUTCOffset = 7;
			iUTCValueOffset = 3600000;
			strDT = "PDT";
		}
		else {
			apWriteDebug("DST disabled");
			iUTCOffset = 8;
			iUTCValueOffset = 0;
			strDT = "PST";
		}
	}
	else {
		apWriteDebug ("PST");
		iUTCOffset = 8;
		iUTCValueOffset = -3600000;
		strDT = "PST";
	}

	// is DST enabled on this system?
	strTemp = new Date(2000,7,1).toString();
	if (strTemp.search(/PST/) == -1) { // Daylight time
		iUTCOffset = 7;
		iUTCValueOffset = 3600000;
		strDT = "PDT";
	}
	else {
		iUTCOffset = 8;
		iUTCValueOffset = 0;
		strDT = "PST";
	}


    apInitTest("dtutc01: new Date() constructor -- all parameters after the first two are optional"); 
    
    if( new Date("12/25/2002 12:00:00 PST").getTimezoneOffset()/60 == 8) {


	apInitScenario("1 No arguments");
	dDate1 = new Date(Date.UTC());
	verify (true, DateCompare(Date.parse(Date.UTC()), dDate1.valueOf()), "1 Wrong date returned");	


	apInitScenario("2 One argument");
	dDate1 = new Date(Date.UTC(2000));
	verify (dDate1.getFullYear(), 1999, "2.1 Wrong date returned");
	verify (dDate1.getMonth(), 11, "2.2 Wrong date returned");
	verify (dDate1.getDate(), 31, "2.3 Wrong date returned");
	verify (dDate1.getHours(), 16, "2.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "2.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "2.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "2.7 Wrong date returned");
	verify (dDate1.valueOf(), 946684800000, "2.8 Wrong value returned");


	apInitScenario("3 Two arguments");
	dDate1 = new Date(Date.UTC(2000,0));
	verify (dDate1.getFullYear(), 1999, "3.1 Wrong date returned");
	verify (dDate1.getMonth(), 11, "3.2 Wrong date returned");
	verify (dDate1.getDate(), 31, "3.3 Wrong date returned");
	verify (dDate1.getHours(), 16, "3.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "3.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "3.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "3.7 Wrong date returned");
	verify (dDate1.valueOf(), 946684800000, "3.8 Wrong date value returned");


	apInitScenario("4 Three arguments");
	dDate1 = new Date(Date.UTC(2000,0,3));
	verify (dDate1.getFullYear(), 2000, "4.1 Wrong date returned");
	verify (dDate1.getMonth(), 0, "4.2 Wrong date returned");
	verify (dDate1.getDate(), 2, "4.3 Wrong date returned");
	verify (dDate1.getHours(), 16, "4.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "4.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "4.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "4.7 Wrong date returned");
	verify (dDate1.valueOf(), 946857600000, "4.8 Wrong date value returned");


	apInitScenario("5 Four arguments");
	dDate1 = new Date(Date.UTC(2000,0,3,16));
	verify (dDate1.getFullYear(), 2000, "5.1 Wrong date returned");
	verify (dDate1.getMonth(), 0, "5.2 Wrong date returned");
	verify (dDate1.getDate(), 3, "5.3 Wrong date returned");
	verify (dDate1.getHours(), 8, "5.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "5.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "5.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "5.7 Wrong date returned");
	verify (dDate1.valueOf(), 946915200000, "5.8 Wrong date value returned");


	apInitScenario("6 Five arguments");
	dDate1 = new Date(Date.UTC(2000,0,3,16,15));
	verify (dDate1.getFullYear(), 2000, "6.1 Wrong date returned");
	verify (dDate1.getMonth(), 0, "6.2 Wrong date returned");
	verify (dDate1.getDate(), 3, "6.3 Wrong date returned");
	verify (dDate1.getHours(), 8, "6.4 Wrong date returned");
	verify (dDate1.getMinutes(), 15, "6.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "6.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "6.7 Wrong date returned");
	verify (dDate1.valueOf(), 946916100000, "6.8 Wrong date value returned");


	apInitScenario("7 Six arguments");
	dDate1 = new Date(Date.UTC(2000,0,3,16,15,31));
	verify (dDate1.getFullYear(), 2000, "7.1 Wrong date returned");
	verify (dDate1.getMonth(), 0, "7.2 Wrong date returned");
	verify (dDate1.getDate(), 3, "7.3 Wrong date returned");
	verify (dDate1.getHours(), 8, "7.4 Wrong date returned");
	verify (dDate1.getMinutes(), 15, "7.5 Wrong date returned");
	verify (dDate1.getSeconds(), 31, "7.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "7.7 Wrong date returned");
	verify (dDate1.valueOf(), 946916131000, "7.8 Wrong date value returned");


	apInitScenario("8 Seven arguments");
	dDate1 = new Date(Date.UTC(2000,0,3,16,15,31,686));
	verify (dDate1.getFullYear(), 2000, "8.1 Wrong date returned");
	verify (dDate1.getMonth(), 0, "8.2 Wrong date returned");
	verify (dDate1.getDate(), 3, "8.3 Wrong date returned");
	verify (dDate1.getHours(), 8, "8.4 Wrong date returned");
	verify (dDate1.getMinutes(), 15, "8.5 Wrong date returned");
	verify (dDate1.getSeconds(), 31, "8.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 686, "8.7 Wrong date returned");
	verify (dDate1.valueOf(), 946916131686, "8.8 Wrong date value returned");


	apInitScenario("9 Eight arguments");
	dDate1 = new Date(Date.UTC(2000,0,3,16,15,31,686,40));
	verify (dDate1.getFullYear(), 2000, "9.1 Wrong date returned");
	verify (dDate1.getMonth(), 0, "9.2 Wrong date returned");
	verify (dDate1.getDate(), 3, "9.3 Wrong date returned");
	verify (dDate1.getHours(), 8, "9.4 Wrong date returned");
	verify (dDate1.getMinutes(), 15, "9.5 Wrong date returned");
	verify (dDate1.getSeconds(), 31, "9.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 686, "9.7 Wrong date returned");
	verify (dDate1.valueOf(), 946916131686, "9.8 Wrong date value returned");


	apInitScenario("10 String arguments"); // Date.UTC cannot take string arguments
	var str1;
	str1 = "1/3/2000 12:15:31 PM"
	dDate1 = new Date(Date.UTC(str1));
	verify ((dDate1.getFullYear()).toString(), "NaN", "10.1 Wrong date returned");
	verify ((dDate1.getMonth()).toString(), "NaN", "10.2 Wrong date returned");
	verify ((dDate1.getDate()).toString(), "NaN", "10.3 Wrong date returned");
	verify ((dDate1.getHours()).toString(), "NaN", "10.4 Wrong date returned");
	verify ((dDate1.getMinutes()).toString(), "NaN", "10.5 Wrong date returned");
	verify ((dDate1.getSeconds()).toString(), "NaN", "10.6 Wrong date returned");
	verify ((dDate1.getMilliseconds()).toString(), "NaN", "10.7 Wrong date returned");
	verify ((dDate1.valueOf()).toString(), "NaN", "10.8 Wrong date value returned");


	apInitScenario("11 Array arguments");
	var arr1 = new Array (2000,0,3,16,15,31,686);
	dDate1 = new Date(Date.UTC(arr1));
	verify ("NaN", dDate1, "11 Wrong value returned");


	apInitScenario("12 Object arguments");
	var obj1 = new Object();
	obj1.mem = 2000;
	dDate1 = new Date(Date.UTC(obj1));
	verify ("NaN", dDate1, "11 Wrong value returned");


	apInitScenario("13 Year = -1");
	dDate1 = new Date(Date.UTC(-1,0,1,8));
	verify (dDate1.getFullYear(), -1, "13.1 Wrong date returned");
	verify (dDate1.getMonth(), 0, "13.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "13.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "13.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "13.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "13.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "13.7 Wrong date returned");
	verify (dDate1.valueOf(), -62198726400000, "13.8 Wrong date value returned");
	verify (dDate1.toString(), "Fri Jan 1 00:00:00 PST 2 B.C.", "13.9 Wrong date string returned");


	apInitScenario("14 Year = -100000");
	dDate1 = new Date(Date.UTC(-100000,0,1,8));
	verify (dDate1.getFullYear(), -100000, "14.1 Wrong date returned");
	verify (dDate1.getMonth(), 0, "14.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "14.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "14.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "14.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "14.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "14.7 Wrong date returned");
	verify (dDate1.valueOf(), -3217862390400000, "14.8 Wrong date value returned");
	verify (dDate1.toString(), "Sat Jan 1 00:00:00 PST 100001 B.C.", "14.9 Wrong date string returned");


	apInitScenario("15 Month = -1");
	dDate1 = new Date(Date.UTC(2000,-1,1,8));
	verify (dDate1.getFullYear(), 1999, "15.1 Wrong date returned");
	verify (dDate1.getMonth(), 11, "15.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "15.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "15.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "15.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "15.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "15.7 Wrong date returned");
	verify (dDate1.valueOf(), 944035200000, "15.8 Wrong date value returned");
	verify (dDate1.toString(), "Wed Dec 1 00:00:00 PST 1999", "15.9 Wrong date string returned");


	apInitScenario("16 Month = -11");
	dDate1 = new Date(Date.UTC(2000,-11,1,8));
	verify (dDate1.getFullYear(), 1999, "16.1 Wrong date returned");
	verify (dDate1.getMonth(), 1, "16.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "16.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "16.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "16.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "16.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "16.7 Wrong date returned");
	verify (dDate1.valueOf(), 917856000000, "16.8 Wrong date value returned");
	verify (dDate1.toString(), "Mon Feb 1 00:00:00 PST 1999", "16.9 Wrong date string returned");


	apInitScenario("17 Month = -12");
	dDate1 = new Date(Date.UTC(2000,-12,1,8));
	verify (dDate1.getFullYear(), 1999, "17.1 Wrong date returned");
	verify (dDate1.getMonth(), 0, "17.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "17.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "17.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "17.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "17.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "17.7 Wrong date returned");
	verify (dDate1.valueOf(), 915177600000, "17.8 Wrong date value returned");
	verify (dDate1.toString(), "Fri Jan 1 00:00:00 PST 1999", "17.9 Wrong date string returned");


	apInitScenario("18 Month = -1000");
	dDate1 = new Date(Date.UTC(2000,-1000,1,iUTCOffset)); // took away the "-1"
	verify (dDate1.getFullYear(), 1916, "18.1 Wrong date returned");
	verify (dDate1.getMonth(), 8, "18.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "18.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "18.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "18.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "18.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "18.7 Wrong date returned");
	verify (dDate1.valueOf(), -1683043200000-iUTCValueOffset, "18.8 Wrong date value returned");
	verify (dDate1.toString(), "Fri Sep 1 00:00:00 " + strDT + " 1916", "18.9 Wrong date string returned");


	apInitScenario("19 Day = -1");
	dDate1 = new Date(Date.UTC(2000,0,-1,8));
	verify (dDate1.getFullYear(), 1999, "19.1 Wrong date returned");
	verify (dDate1.getMonth(), 11, "19.2 Wrong date returned");
	verify (dDate1.getDate(), 30, "19.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "19.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "19.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "19.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "19.7 Wrong date returned");
	verify (dDate1.valueOf(), 946540800000, "19.8 Wrong date value returned");
	verify (dDate1.toString(), "Thu Dec 30 00:00:00 PST 1999", "19.9 Wrong date string returned");


	apInitScenario("20 Day = -29");
	dDate1 = new Date(Date.UTC(2000,0,-29,8));
	verify (dDate1.getFullYear(), 1999, "20.1 Wrong date returned");
	verify (dDate1.getMonth(), 11, "20.2 Wrong date returned");
	verify (dDate1.getDate(), 2, "20.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "20.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "20.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "20.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "20.7 Wrong date returned");
	verify (dDate1.valueOf(), 944121600000, "20.8 Wrong date value returned");
	verify (dDate1.toString(), "Thu Dec 2 00:00:00 PST 1999", "20.9 Wrong date string returned");


	apInitScenario("21 Day = -30");
	dDate1 = new Date(Date.UTC(2000,0,-30,8));
	verify (dDate1.getFullYear(), 1999, "21.1 Wrong date returned");
	verify (dDate1.getMonth(), 11, "21.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "21.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "21.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "21.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "21.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "21.7 Wrong date returned");
	verify (dDate1.valueOf(), 944035200000, "21.8 Wrong date value returned");
	verify (dDate1.toString(), "Wed Dec 1 00:00:00 PST 1999", "21.9 Wrong date string returned");


	apInitScenario("22 Day = -31");
	dDate1 = new Date(Date.UTC(2000,0,-31,8));
	verify (dDate1.getFullYear(), 1999, "22.1 Wrong date returned");
	verify (dDate1.getMonth(), 10, "22.2 Wrong date returned");
	verify (dDate1.getDate(), 30, "22.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "22.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "22.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "22.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "22.7 Wrong date returned");
	verify (dDate1.valueOf(), 943948800000, "22.8 Wrong date value returned");
	verify (dDate1.toString(), "Tue Nov 30 00:00:00 PST 1999", "22.9 Wrong date string returned");


	apInitScenario("23 Day = -1000");
	dDate1 = new Date(Date.UTC(2000,0,-1000,8));
	verify (dDate1.getFullYear(), 1997, "23.1 Wrong date returned");
	verify (dDate1.getMonth(), 3, "23.2 Wrong date returned");
	verify (dDate1.getDate(), 5, "23.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "23.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "23.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "23.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "23.7 Wrong date returned");
	verify (dDate1.valueOf(), 860227200000, "23.8 Wrong date value returned");
	verify (dDate1.toString(), "Sat Apr 5 00:00:00 PST 1997", "23.9 Wrong date string returned");


	apInitScenario("24 Hour = -1");
	dDate1 = new Date(Date.UTC(2000,0,1,-1 + 8));
	verify (dDate1.getFullYear(), 1999, "24.1 Wrong date returned");
	verify (dDate1.getMonth(), 11, "24.2 Wrong date returned");
	verify (dDate1.getDate(), 31, "24.3 Wrong date returned");
	verify (dDate1.getHours(), 23, "24.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "24.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "24.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "24.7 Wrong date returned");
	verify (dDate1.valueOf(), 946710000000, "24.8 Wrong date value returned");
	verify (dDate1.toString(), "Fri Dec 31 23:00:00 PST 1999", "24.9 Wrong date string returned");


	apInitScenario("25 Hour = -23");
	dDate1 = new Date(Date.UTC(2000,0,1,-23 + 8));
	verify (dDate1.getFullYear(), 1999, "25.1 Wrong date returned");
	verify (dDate1.getMonth(), 11, "25.2 Wrong date returned");
	verify (dDate1.getDate(), 31, "25.3 Wrong date returned");
	verify (dDate1.getHours(), 1, "25.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "25.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "25.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "25.7 Wrong date returned");
	verify (dDate1.valueOf(), 946630800000, "25.8 Wrong date value returned");
	verify (dDate1.toString(), "Fri Dec 31 01:00:00 PST 1999", "25.9 Wrong date string returned");


	apInitScenario("26 Hour = -24");
	dDate1 = new Date(Date.UTC(2000,0,1,-24 + 8));
	verify (dDate1.getFullYear(), 1999, "26.1 Wrong date returned");
	verify (dDate1.getMonth(), 11, "26.2 Wrong date returned");
	verify (dDate1.getDate(), 31, "26.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "26.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "26.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "26.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "26.7 Wrong date returned");
	verify (dDate1.valueOf(), 946627200000, "26.8 Wrong date value returned");
	verify (dDate1.toString(), "Fri Dec 31 00:00:00 PST 1999", "26.9 Wrong date string returned");


	apInitScenario("27 Minute = -1");
	dDate1 = new Date(Date.UTC(2000,0,1,8,-1));
	verify (dDate1.getFullYear(), 1999, "27.1 Wrong date returned");
	verify (dDate1.getMonth(), 11, "27.2 Wrong date returned");
	verify (dDate1.getDate(), 31, "27.3 Wrong date returned");
	verify (dDate1.getHours(), 23, "27.4 Wrong date returned");
	verify (dDate1.getMinutes(), 59, "27.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "27.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "27.7 Wrong date returned");
	verify (dDate1.valueOf(), 946713540000, "27.8 Wrong date value returned");
	verify (dDate1.toString(), "Fri Dec 31 23:59:00 PST 1999", "27.9 Wrong date string returned");


	apInitScenario("28 Minute = -59");
	dDate1 = new Date(Date.UTC(2000,0,1,8,-59));
	verify (dDate1.getFullYear(), 1999, "28.1 Wrong date returned");
	verify (dDate1.getMonth(), 11, "28.2 Wrong date returned");
	verify (dDate1.getDate(), 31, "28.3 Wrong date returned");
	verify (dDate1.getHours(), 23, "28.4 Wrong date returned");
	verify (dDate1.getMinutes(), 1, "28.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "28.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "28.7 Wrong date returned");
	verify (dDate1.valueOf(), 946710060000, "28.8 Wrong date value returned");
	verify (dDate1.toString(), "Fri Dec 31 23:01:00 PST 1999", "28.9 Wrong date string returned");


	apInitScenario("29 Minute = -60");
	dDate1 = new Date(Date.UTC(2000,0,1,8,-60));
	verify (dDate1.getFullYear(), 1999, "29.1 Wrong date returned");
	verify (dDate1.getMonth(), 11, "29.2 Wrong date returned");
	verify (dDate1.getDate(), 31, "29.3 Wrong date returned");
	verify (dDate1.getHours(), 23, "29.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "29.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "29.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "29.7 Wrong date returned");
	verify (dDate1.valueOf(), 946710000000, "29.8 Wrong date value returned");
	verify (dDate1.toString(), "Fri Dec 31 23:00:00 PST 1999", "29.9 Wrong date string returned");


	apInitScenario("30 Second = -1");
	dDate1 = new Date(Date.UTC(2000,0,1,8,0,-1));
	verify (dDate1.getFullYear(), 1999, "30.1 Wrong date returned");
	verify (dDate1.getMonth(), 11, "30.2 Wrong date returned");
	verify (dDate1.getDate(), 31, "30.3 Wrong date returned");
	verify (dDate1.getHours(), 23, "30.4 Wrong date returned");
	verify (dDate1.getMinutes(), 59, "30.5 Wrong date returned");
	verify (dDate1.getSeconds(), 59, "30.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "30.7 Wrong date returned");
	verify (dDate1.valueOf(), 946713599000, "30.8 Wrong date value returned");
	verify (dDate1.toString(), "Fri Dec 31 23:59:59 PST 1999", "30.9 Wrong date string returned");


	apInitScenario("31 Second = -59");
	dDate1 = new Date(Date.UTC(2000,0,1,8,0,-59));
	verify (dDate1.getFullYear(), 1999, "31.1 Wrong date returned");
	verify (dDate1.getMonth(), 11, "31.2 Wrong date returned");
	verify (dDate1.getDate(), 31, "31.3 Wrong date returned");
	verify (dDate1.getHours(), 23, "31.4 Wrong date returned");
	verify (dDate1.getMinutes(), 59, "31.5 Wrong date returned");
	verify (dDate1.getSeconds(), 1, "31.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "31.7 Wrong date returned");
	verify (dDate1.valueOf(), 946713541000, "31.8 Wrong date value returned");
	verify (dDate1.toString(), "Fri Dec 31 23:59:01 PST 1999", "31.9 Wrong date string returned");


	apInitScenario("32 Second = -60");
	dDate1 = new Date(Date.UTC(2000,0,1,8,0,-60));
	verify (dDate1.getFullYear(), 1999, "32.1 Wrong date returned");
	verify (dDate1.getMonth(), 11, "32.2 Wrong date returned");
	verify (dDate1.getDate(), 31, "32.3 Wrong date returned");
	verify (dDate1.getHours(), 23, "32.4 Wrong date returned");
	verify (dDate1.getMinutes(), 59, "32.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "32.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "32.7 Wrong date returned");
	verify (dDate1.valueOf(), 946713540000, "32.8 Wrong date value returned");
	verify (dDate1.toString(), "Fri Dec 31 23:59:00 PST 1999", "32.9 Wrong date string returned");


	apInitScenario("33 Millisecond = -1");
	dDate1 = new Date(Date.UTC(2000,0,1,8,0,0,-1));
	verify (dDate1.getFullYear(), 1999, "33.1 Wrong date returned");
	verify (dDate1.getMonth(), 11, "33.2 Wrong date returned");
	verify (dDate1.getDate(), 31, "33.3 Wrong date returned");
	verify (dDate1.getHours(), 23, "33.4 Wrong date returned");
	verify (dDate1.getMinutes(), 59, "33.5 Wrong date returned");
	verify (dDate1.getSeconds(), 59, "33.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 999, "33.7 Wrong date returned");
	verify (dDate1.valueOf(), 946713599999, "33.8 Wrong date value returned");
	verify (dDate1.toString(), "Fri Dec 31 23:59:59 PST 1999", "33.9 Wrong date string returned");


	apInitScenario("34 Millisecond = -999");
	dDate1 = new Date(Date.UTC(2000,0,1,8,0,0,-999));
	verify (dDate1.getFullYear(), 1999, "34.1 Wrong date returned");
	verify (dDate1.getMonth(), 11, "34.2 Wrong date returned");
	verify (dDate1.getDate(), 31, "34.3 Wrong date returned");
	verify (dDate1.getHours(), 23, "34.4 Wrong date returned");
	verify (dDate1.getMinutes(), 59, "34.5 Wrong date returned");
	verify (dDate1.getSeconds(), 59, "34.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 1, "34.7 Wrong date returned");
	verify (dDate1.valueOf(), 946713599001, "34.8 Wrong date value returned");
	verify (dDate1.toString(), "Fri Dec 31 23:59:59 PST 1999", "34.9 Wrong date string returned");


	apInitScenario("35 Millisecond = -1000");
	dDate1 = new Date(Date.UTC(2000,0,1,8,0,0,-1000));
	verify (dDate1.getFullYear(), 1999, "35.1 Wrong date returned");
	verify (dDate1.getMonth(), 11, "35.2 Wrong date returned");
	verify (dDate1.getDate(), 31, "35.3 Wrong date returned");
	verify (dDate1.getHours(), 23, "35.4 Wrong date returned");
	verify (dDate1.getMinutes(), 59, "35.5 Wrong date returned");
	verify (dDate1.getSeconds(), 59, "35.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "35.7 Wrong date returned");
	verify (dDate1.valueOf(), 946713599000, "35.8 Wrong date value returned");
	verify (dDate1.toString(), "Fri Dec 31 23:59:59 PST 1999", "35.9 Wrong date string returned");


	apInitScenario("36 Year = 0");
	dDate1 = new Date(Date.UTC(0,0,1,8,0,0,0));
	verify (dDate1.getFullYear(), 1900, "36.1 Wrong date returned");
	verify (dDate1.getMonth(), 0, "36.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "36.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "36.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "36.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "36.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "36.7 Wrong date returned");
	verify (dDate1.valueOf(), -2208960000000, "36.8 Wrong date value returned");
	verify (dDate1.toString(), "Mon Jan 1 00:00:00 PST 1900", "36.9 Wrong date string returned");


	apInitScenario("37 Month = 0");
	dDate1 = new Date(Date.UTC(2000,0,1,8,0,0,0));
	verify (dDate1.getFullYear(), 2000, "37.1 Wrong date returned");
	verify (dDate1.getMonth(), 0, "37.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "37.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "37.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "37.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "37.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "37.7 Wrong date returned");
	verify (dDate1.valueOf(), 946713600000, "37.8 Wrong date value returned");
	verify (dDate1.toString(), "Sat Jan 1 00:00:00 PST 2000", "37.9 Wrong date string returned");


	apInitScenario("38 Day = 0");
	dDate1 = new Date(Date.UTC(2000,0,0,8,0,0,0));
	verify (dDate1.getFullYear(), 1999, "38.1 Wrong date returned");
	verify (dDate1.getMonth(), 11, "38.2 Wrong date returned");
	verify (dDate1.getDate(), 31, "38.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "38.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "38.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "38.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "38.7 Wrong date returned");
	verify (dDate1.valueOf(), 946627200000, "38.8 Wrong date value returned");
	verify (dDate1.toString(), "Fri Dec 31 00:00:00 PST 1999", "38.9 Wrong date string returned");


	apInitScenario("39 Hours = 0");
	dDate1 = new Date(Date.UTC(2000,0,1,8,0,0,0));
	verify (dDate1.getFullYear(), 2000, "39.1 Wrong date returned");
	verify (dDate1.getMonth(), 0, "39.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "39.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "39.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "39.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "39.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "39.7 Wrong date returned");
	verify (dDate1.valueOf(), 946713600000, "39.8 Wrong date value returned");
	verify (dDate1.toString(), "Sat Jan 1 00:00:00 PST 2000", "39.9 Wrong date string returned");


	apInitScenario("40 Minutes = 0");
	dDate1 = new Date(Date.UTC(2000,0,1,8,0,0,0));
	verify (dDate1.getFullYear(), 2000, "40.1 Wrong date returned");
	verify (dDate1.getMonth(), 0, "40.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "40.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "40.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "40.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "40.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "40.7 Wrong date returned");
	verify (dDate1.valueOf(), 946713600000, "40.8 Wrong date value returned");
	verify (dDate1.toString(), "Sat Jan 1 00:00:00 PST 2000", "40.9 Wrong date string returned");


	apInitScenario("41 Seconds = 0");
	dDate1 = new Date(Date.UTC(2000,0,1,8,0,0,0));
	verify (dDate1.getFullYear(), 2000, "41.1 Wrong date returned");
	verify (dDate1.getMonth(), 0, "41.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "41.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "41.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "41.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "41.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "41.7 Wrong date returned");
	verify (dDate1.valueOf(), 946713600000, "41.8 Wrong date value returned");
	verify (dDate1.toString(), "Sat Jan 1 00:00:00 PST 2000", "41.9 Wrong date string returned");


	apInitScenario("42 Milliseconds = 0");
	dDate1 = new Date(Date.UTC(2000,0,1,8,0,0,0));
	verify (dDate1.getFullYear(), 2000, "42.1 Wrong date returned");
	verify (dDate1.getMonth(), 0, "42.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "42.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "42.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "42.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "42.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "42.7 Wrong date returned");
	verify (dDate1.valueOf(), 946713600000, "42.8 Wrong date value returned");
	verify (dDate1.toString(), "Sat Jan 1 00:00:00 PST 2000", "42.9 Wrong date string returned");


	apInitScenario("43 Year = 100000");
	dDate1 = new Date(Date.UTC(100000,0,1,8,0,0,0));
	verify (dDate1.getFullYear(), 100000, "43.1 Wrong date returned");
	verify (dDate1.getMonth(), 0, "43.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "43.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "43.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "43.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "43.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "43.7 Wrong date returned");
	verify (dDate1.valueOf(), 3093528009600000, "43.8 Wrong date value returned");
	verify (dDate1.toString(), "Sat Jan 1 00:00:00 PST 100000", "43.9 Wrong date string returned");


	apInitScenario("44 Month = 12");
	dDate1 = new Date(Date.UTC(2000,12,1,8,0,0,0));
	verify (dDate1.getFullYear(), 2001, "44.1 Wrong date returned");
	verify (dDate1.getMonth(), 0, "44.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "44.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "44.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "44.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "44.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "44.7 Wrong date returned");
	verify (dDate1.valueOf(), 978336000000, "44.8 Wrong date value returned");
	verify (dDate1.toString(), "Mon Jan 1 00:00:00 PST 2001", "44.9 Wrong date string returned");


	apInitScenario("45 Month = 13");
	dDate1 = new Date(Date.UTC(2000,13,1,8,0,0,0));
	verify (dDate1.getFullYear(), 2001, "45.1 Wrong date returned");
	verify (dDate1.getMonth(), 1, "45.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "45.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "45.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "45.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "45.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "45.7 Wrong date returned");
	verify (dDate1.valueOf(), 981014400000, "45.8 Wrong date value returned");
	verify (dDate1.toString(), "Thu Feb 1 00:00:00 PST 2001", "45.9 Wrong date string returned");


	apInitScenario("46 Month = 1000");
	dDate1 = new Date(Date.UTC(2000,1000,1,iUTCOffset,0,0,0)); // took away the -1
	verify (dDate1.getFullYear(), 2083, "46.1 Wrong date returned");
	verify (dDate1.getMonth(), 4, "46.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "46.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "46.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "46.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "46.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "46.7 Wrong date returned");
	verify (dDate1.valueOf(), 3576384000000-iUTCValueOffset, "46.8 Wrong date value returned");
	verify (dDate1.toString(), "Sat May 1 00:00:00 " + strDT + " 2083", "46.9 Wrong date string returned");


	apInitScenario("47 Day = 29 on 30-day month");
	dDate1 = new Date(Date.UTC(2000,10,29,8,0,0,0));
	verify (dDate1.getFullYear(), 2000, "47.1 Wrong date returned");
	verify (dDate1.getMonth(), 10, "47.2 Wrong date returned");
	verify (dDate1.getDate(), 29, "47.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "47.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "47.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "47.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "47.7 Wrong date returned");
	verify (dDate1.valueOf(), 975484800000, "47.8 Wrong date value returned");
	verify (dDate1.toString(), "Wed Nov 29 00:00:00 PST 2000", "47.9 Wrong date string returned");


	apInitScenario("48 Day = 30 on 30-day month");
	dDate1 = new Date(Date.UTC(2000,10,30,8,0,0,0));
	verify (dDate1.getFullYear(), 2000, "48.1 Wrong date returned");
	verify (dDate1.getMonth(), 10, "48.2 Wrong date returned");
	verify (dDate1.getDate(), 30, "48.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "48.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "48.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "48.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "48.7 Wrong date returned");
	verify (dDate1.valueOf(), 975571200000, "48.8 Wrong date value returned");
	verify (dDate1.toString(), "Thu Nov 30 00:00:00 PST 2000", "48.9 Wrong date string returned");


	apInitScenario("49 Day = 30 on 31-day month");
	dDate1 = new Date(Date.UTC(2000,11,30,8,0,0,0));
	verify (dDate1.getFullYear(), 2000, "49.1 Wrong date returned");
	verify (dDate1.getMonth(), 11, "49.2 Wrong date returned");
	verify (dDate1.getDate(), 30, "49.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "49.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "49.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "49.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "49.7 Wrong date returned");
	verify (dDate1.valueOf(), 978163200000, "49.8 Wrong date value returned");
	verify (dDate1.toString(), "Sat Dec 30 00:00:00 PST 2000", "49.9 Wrong date string returned");


	apInitScenario("50 Day = 31 on 31-day month");
	dDate1 = new Date(Date.UTC(2000,11,31,8,0,0,0));
	verify (dDate1.getFullYear(), 2000, "50.1 Wrong date returned");
	verify (dDate1.getMonth(), 11, "50.2 Wrong date returned");
	verify (dDate1.getDate(), 31, "50.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "50.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "50.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "50.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "50.7 Wrong date returned");
	verify (dDate1.valueOf(), 978249600000, "50.8 Wrong date value returned");
	verify (dDate1.toString(), "Sun Dec 31 00:00:00 PST 2000", "50.9 Wrong date string returned");


	apInitScenario("51 Day = 31 on 30-day month");
	dDate1 = new Date(Date.UTC(2000,10,31,8,0,0,0));
	verify (dDate1.getFullYear(), 2000, "51.1 Wrong date returned");
	verify (dDate1.getMonth(), 11, "51.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "51.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "51.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "51.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "51.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "51.7 Wrong date returned");
	verify (dDate1.valueOf(), 975657600000, "51.8 Wrong date value returned");
	verify (dDate1.toString(), "Fri Dec 1 00:00:00 PST 2000", "51.9 Wrong date string returned");


	apInitScenario("52 Day = 32");
	dDate1 = new Date(Date.UTC(2000,10,32,8,0,0,0));
	verify (dDate1.getFullYear(), 2000, "52.1 Wrong date returned");
	verify (dDate1.getMonth(), 11, "52.2 Wrong date returned");
	verify (dDate1.getDate(), 2, "52.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "52.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "52.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "52.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "52.7 Wrong date returned");
	verify (dDate1.valueOf(), 975744000000, "52.8 Wrong date value returned");
	verify (dDate1.toString(), "Sat Dec 2 00:00:00 PST 2000", "52.9 Wrong date string returned");


	apInitScenario("53 Day = 33");
	dDate1 = new Date(Date.UTC(2000,10,33,8,0,0,0));
	verify (dDate1.getFullYear(), 2000, "53.1 Wrong date returned");
	verify (dDate1.getMonth(), 11, "53.2 Wrong date returned");
	verify (dDate1.getDate(), 3, "53.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "53.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "53.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "53.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "53.7 Wrong date returned");
	verify (dDate1.valueOf(), 975830400000, "53.8 Wrong date value returned");
	verify (dDate1.toString(), "Sun Dec 3 00:00:00 PST 2000", "53.9 Wrong date string returned");


	apInitScenario("54 Day = 1000");
	dDate1 = new Date(Date.UTC(2000,10,1000,iUTCOffset,0,0,0)); // took away the -1
	verify (dDate1.getFullYear(), 2003, "54.1 Wrong date returned");
	verify (dDate1.getMonth(), 6, "54.2 Wrong date returned");
	verify (dDate1.getDate(), 28, "54.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "54.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "54.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "54.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "54.7 Wrong date returned");
	verify (dDate1.valueOf(), 1059379200000-iUTCValueOffset, "54.8 Wrong date value returned");
	verify (dDate1.toString(), "Mon Jul 28 00:00:00 " + strDT + " 2003", "54.9 Wrong date string returned");


	apInitScenario("55 Hours = 23");
	dDate1 = new Date(Date.UTC(2000,10,1,23 + 8,0,0,0));
	verify (dDate1.getFullYear(), 2000, "55.1 Wrong date returned");
	verify (dDate1.getMonth(), 10, "55.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "55.3 Wrong date returned");
	verify (dDate1.getHours(), 23, "55.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "55.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "55.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "55.7 Wrong date returned");
	verify (dDate1.valueOf(), 973148400000, "55.8 Wrong date value returned");
	verify (dDate1.toString(), "Wed Nov 1 23:00:00 PST 2000", "55.9 Wrong date string returned");


	apInitScenario("56 Hours = 24");
	dDate1 = new Date(Date.UTC(2000,10,1,24 + 8,0,0,0));
	verify (dDate1.getFullYear(), 2000, "56.1 Wrong date returned");
	verify (dDate1.getMonth(), 10, "56.2 Wrong date returned");
	verify (dDate1.getDate(), 2, "56.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "56.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "56.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "56.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "56.7 Wrong date returned");
	verify (dDate1.valueOf(), 973152000000, "56.8 Wrong date value returned");
	verify (dDate1.toString(), "Thu Nov 2 00:00:00 PST 2000", "56.9 Wrong date string returned");


	apInitScenario("57 Hours = 1000");
	dDate1 = new Date(Date.UTC(2000,10,1,1000 + 8,0,0,0));
	verify (dDate1.getFullYear(), 2000, "57.1 Wrong date returned");
	verify (dDate1.getMonth(), 11, "57.2 Wrong date returned");
	verify (dDate1.getDate(), 12, "57.3 Wrong date returned");
	verify (dDate1.getHours(), 16, "57.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "57.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "57.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "57.7 Wrong date returned");
	verify (dDate1.valueOf(), 976665600000, "57.8 Wrong date value returned");
	verify (dDate1.toString(), "Tue Dec 12 16:00:00 PST 2000", "57.9 Wrong date string returned");


	apInitScenario("58 Minutes = 59");
	dDate1 = new Date(Date.UTC(2000,10,1,8,59,0,0));
	verify (dDate1.getFullYear(), 2000, "58.1 Wrong date returned");
	verify (dDate1.getMonth(), 10, "58.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "58.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "58.4 Wrong date returned");
	verify (dDate1.getMinutes(), 59, "58.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "58.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "58.7 Wrong date returned");
	verify (dDate1.valueOf(), 973069140000, "58.8 Wrong date value returned");
	verify (dDate1.toString(), "Wed Nov 1 00:59:00 PST 2000", "58.9 Wrong date string returned");


	apInitScenario("59 Minutes = 60");
	dDate1 = new Date(Date.UTC(2000,10,1,8,60,0,0));
	verify (dDate1.getFullYear(), 2000, "59.1 Wrong date returned");
	verify (dDate1.getMonth(), 10, "59.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "59.3 Wrong date returned");
	verify (dDate1.getHours(), 1, "59.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "59.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "59.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "59.7 Wrong date returned");
	verify (dDate1.valueOf(), 973069200000, "59.8 Wrong date value returned");
	verify (dDate1.toString(), "Wed Nov 1 01:00:00 PST 2000", "59.9 Wrong date string returned");


	apInitScenario("60 Minutes = 61");
	dDate1 = new Date(Date.UTC(2000,10,1,8,61,0,0));
	verify (dDate1.getFullYear(), 2000, "60.1 Wrong date returned");
	verify (dDate1.getMonth(), 10, "60.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "60.3 Wrong date returned");
	verify (dDate1.getHours(), 1, "60.4 Wrong date returned");
	verify (dDate1.getMinutes(), 1, "60.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "60.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "60.7 Wrong date returned");
	verify (dDate1.valueOf(), 973069260000, "60.8 Wrong date value returned");
	verify (dDate1.toString(), "Wed Nov 1 01:01:00 PST 2000", "60.9 Wrong date string returned");


	apInitScenario("61 Seconds = 59");
	dDate1 = new Date(Date.UTC(2000,10,1,8,0,59,0));
	verify (dDate1.getFullYear(), 2000, "61.1 Wrong date returned");
	verify (dDate1.getMonth(), 10, "61.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "61.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "61.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "61.5 Wrong date returned");
	verify (dDate1.getSeconds(), 59, "61.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "61.7 Wrong date returned");
	verify (dDate1.valueOf(), 973065659000, "61.8 Wrong date value returned");
	verify (dDate1.toString(), "Wed Nov 1 00:00:59 PST 2000", "61.9 Wrong date string returned");


	apInitScenario("62 Seconds = 60");
	dDate1 = new Date(Date.UTC(2000,10,1,8,0,60,0));
	verify (dDate1.getFullYear(), 2000, "62.1 Wrong date returned");
	verify (dDate1.getMonth(), 10, "62.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "62.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "62.4 Wrong date returned");
	verify (dDate1.getMinutes(), 1, "62.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "62.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "62.7 Wrong date returned");
	verify (dDate1.valueOf(), 973065660000, "62.8 Wrong date value returned");
	verify (dDate1.toString(), "Wed Nov 1 00:01:00 PST 2000", "62.9 Wrong date string returned");


	apInitScenario("63 Seconds = 61");
	dDate1 = new Date(Date.UTC(2000,10,1,8,0,61,0));
	verify (dDate1.getFullYear(), 2000, "63.1 Wrong date returned");
	verify (dDate1.getMonth(), 10, "63.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "63.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "63.4 Wrong date returned");
	verify (dDate1.getMinutes(), 1, "63.5 Wrong date returned");
	verify (dDate1.getSeconds(), 1, "63.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "63.7 Wrong date returned");
	verify (dDate1.valueOf(), 973065661000, "63.8 Wrong date value returned");
	verify (dDate1.toString(), "Wed Nov 1 00:01:01 PST 2000", "63.9 Wrong date string returned");


	apInitScenario("64 Milliseconds = 999");
	dDate1 = new Date(Date.UTC(2000,10,1,8,0,0,999));
	verify (dDate1.getFullYear(), 2000, "64.1 Wrong date returned");
	verify (dDate1.getMonth(), 10, "64.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "64.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "64.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "64.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "64.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 999, "64.7 Wrong date returned");
	verify (dDate1.valueOf(), 973065600999, "64.8 Wrong date value returned");
	verify (dDate1.toString(), "Wed Nov 1 00:00:00 PST 2000", "64.9 Wrong date string returned");


	apInitScenario("65 Milliseconds = 1000");
	dDate1 = new Date(Date.UTC(2000,10,1,8,0,0,1000));
	verify (dDate1.getFullYear(), 2000, "65.1 Wrong date returned");
	verify (dDate1.getMonth(), 10, "65.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "65.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "65.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "65.5 Wrong date returned");
	verify (dDate1.getSeconds(), 1, "65.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "65.7 Wrong date returned");
	verify (dDate1.valueOf(), 973065601000, "65.8 Wrong date value returned");
	verify (dDate1.toString(), "Wed Nov 1 00:00:01 PST 2000", "65.9 Wrong date string returned");


	apInitScenario("66 Milliseconds = 1001");
	dDate1 = new Date(Date.UTC(2000,10,1,8,0,0,1001));
	verify (dDate1.getFullYear(), 2000, "66.1 Wrong date returned");
	verify (dDate1.getMonth(), 10, "66.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "66.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "66.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "66.5 Wrong date returned");
	verify (dDate1.getSeconds(), 1, "66.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 1, "66.7 Wrong date returned");
	verify (dDate1.valueOf(), 973065601001, "66.8 Wrong date value returned");
	verify (dDate1.toString(), "Wed Nov 1 00:00:01 PST 2000", "66.9 Wrong date string returned");


	apInitScenario("67 Year = positive floating point number");
	dDate1 = new Date(Date.UTC(2000.14159,10,1,8,0,0,0));
	verify (dDate1.getFullYear(), 2000, "67.1 Wrong date returned");
	verify (dDate1.getMonth(), 10, "67.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "67.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "67.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "67.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "67.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "67.7 Wrong date returned");
	verify (dDate1.valueOf(), 973065600000, "67.8 Wrong date value returned");
	verify (dDate1.toString(), "Wed Nov 1 00:00:00 PST 2000", "67.9 Wrong date string returned");


	apInitScenario("68 Year = negative floating point number");
	dDate1 = new Date(Date.UTC(-2000.14159,10,1,8,0,0,0));
	verify (dDate1.getFullYear(), -2000, "68.1 Wrong date returned");
	verify (dDate1.getMonth(), 10, "68.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "68.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "68.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "68.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "68.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "68.7 Wrong date returned");
	verify (dDate1.valueOf(), -125254742400000, "68.8 Wrong date value returned");
	verify (dDate1.toString(), "Wed Nov 1 00:00:00 PST 2001 B.C.", "68.9 Wrong date string returned");


	apInitScenario("69 Month = positive floating point number");
	dDate1 = new Date(Date.UTC(2000,3.14159,1,8,0,0,0));
	verify (dDate1.getFullYear(), 2000, "69.1 Wrong date returned");
	verify (dDate1.getMonth(), 3, "69.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "69.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "69.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "69.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "69.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "69.7 Wrong date returned");
	verify (dDate1.valueOf(), 954576000000, "69.8 Wrong date value returned");
	verify (dDate1.toString(), "Sat Apr 1 00:00:00 PST 2000", "69.9 Wrong date string returned");


	apInitScenario("70 Month = negative floating point number");
	dDate1 = new Date(Date.UTC(2000,-3.14159,1,iUTCOffset,0,0,0)); // took away the -1
	verify (dDate1.getFullYear(), 1999, "70.1 Wrong date returned");
	verify (dDate1.getMonth(), 9, "70.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "70.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "70.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "70.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "70.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "70.7 Wrong date returned");
	verify (dDate1.valueOf(), 938764800000-iUTCValueOffset, "70.8 Wrong date value returned");
	verify (dDate1.toString(), "Fri Oct 1 00:00:00 " + strDT + " 1999", "70.9 Wrong date string returned");


	apInitScenario("71 Day = positive floating point number");
	dDate1 = new Date(Date.UTC(2000,10,3.14159,8,0,0,0));
	verify (dDate1.getFullYear(), 2000, "71.1 Wrong date returned");
	verify (dDate1.getMonth(), 10, "71.2 Wrong date returned");
	verify (dDate1.getDate(), 3, "71.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "71.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "71.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "71.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "71.7 Wrong date returned");
	verify (dDate1.valueOf(), 973238400000, "71.8 Wrong date value returned");
	verify (dDate1.toString(), "Fri Nov 3 00:00:00 PST 2000", "71.9 Wrong date string returned");


	apInitScenario("72 Day = negative floating point number");
	dDate1 = new Date(Date.UTC(2000,10,-3.14159,iUTCOffset,0,0,0)); // took away the -1
	verify (dDate1.getFullYear(), 2000, "721.1 Wrong date returned");
	verify (dDate1.getMonth(), 9, "72.2 Wrong date returned");
	verify (dDate1.getDate(), 28, "72.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "72.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "72.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "72.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "72.7 Wrong date returned");
	verify (dDate1.valueOf(), 972720000000-iUTCValueOffset, "72.8 Wrong date value returned");
	verify (dDate1.toString(), "Sat Oct 28 00:00:00 " + strDT + " 2000", "72.9 Wrong date string returned");


	apInitScenario("73 Hours = positive floating point number");
	dDate1 = new Date(Date.UTC(2000,10,1,3.14159 + 8,0,0,0));
	verify (dDate1.getFullYear(), 2000, "73.1 Wrong date returned");
	verify (dDate1.getMonth(), 10, "73.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "73.3 Wrong date returned");
	verify (dDate1.getHours(), 3, "73.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "73.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "73.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "73.7 Wrong date returned");
	verify (dDate1.valueOf(), 973076400000, "73.8 Wrong date value returned");
	verify (dDate1.toString(), "Wed Nov 1 03:00:00 PST 2000", "73.9 Wrong date string returned");


	apInitScenario("74 Hours = negative floating point number");
	dDate1 = new Date(Date.UTC(2000,10,2,-27.14159 + 8,0,0,0));
	verify (dDate1.getFullYear(), 2000, "74.1 Wrong date returned");
	verify (dDate1.getMonth(), 9, "74.2 Wrong date returned");
	verify (dDate1.getDate(), 31, "74.3 Wrong date returned");
	verify (dDate1.getHours(), 21, "74.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "74.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "74.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "74.7 Wrong date returned");
	verify (dDate1.valueOf(), 973054800000, "74.8 Wrong date value returned");
	verify (dDate1.toString(), "Tue Oct 31 21:00:00 PST 2000", "74.9 Wrong date string returned");


	apInitScenario("75 Minutes = positive floating point number");
	dDate1 = new Date(Date.UTC(2000,10,1,8,3.14159,0,0));
	verify (dDate1.getFullYear(), 2000, "75.1 Wrong date returned");
	verify (dDate1.getMonth(), 10, "75.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "75.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "75.4 Wrong date returned");
	verify (dDate1.getMinutes(), 3, "75.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "75.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "75.7 Wrong date returned");
	verify (dDate1.valueOf(), 973065780000, "75.8 Wrong date value returned");
	verify (dDate1.toString(), "Wed Nov 1 00:03:00 PST 2000", "75.9 Wrong date string returned");


	apInitScenario("76 Minutes = negative floating point number");
	dDate1 = new Date(Date.UTC(2000,10,1,8,-3.14159,0,0));
	verify (dDate1.getFullYear(), 2000, "76.1 Wrong date returned");
	verify (dDate1.getMonth(), 9, "76.2 Wrong date returned");
	verify (dDate1.getDate(), 31, "76.3 Wrong date returned");
	verify (dDate1.getHours(), 23, "76.4 Wrong date returned");
	verify (dDate1.getMinutes(), 57, "76.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "76.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "76.7 Wrong date returned");
	verify (dDate1.valueOf(), 973065420000, "76.8 Wrong date value returned");
	verify (dDate1.toString(), "Tue Oct 31 23:57:00 PST 2000", "76.9 Wrong date string returned");


	apInitScenario("77 Seconds = positive floating point number");
	dDate1 = new Date(Date.UTC(2000,10,1,8,0,3.14159,0));
	verify (dDate1.getFullYear(), 2000, "77.1 Wrong date returned");
	verify (dDate1.getMonth(), 10, "77.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "77.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "77.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "77.5 Wrong date returned");
	verify (dDate1.getSeconds(), 3, "77.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "77.7 Wrong date returned");
	verify (dDate1.valueOf(), 973065603000, "77.8 Wrong date value returned");
	verify (dDate1.toString(), "Wed Nov 1 00:00:03 PST 2000", "77.9 Wrong date string returned");


	apInitScenario("78 Seconds = negative floating point number");
	dDate1 = new Date(Date.UTC(2000,10,1,8,0,-3.14159,0));
	verify (dDate1.getFullYear(), 2000, "78.1 Wrong date returned");
	verify (dDate1.getMonth(), 9, "78.2 Wrong date returned");
	verify (dDate1.getDate(), 31, "78.3 Wrong date returned");
	verify (dDate1.getHours(), 23, "78.4 Wrong date returned");
	verify (dDate1.getMinutes(), 59, "78.5 Wrong date returned");
	verify (dDate1.getSeconds(), 57, "78.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "78.7 Wrong date returned");
	verify (dDate1.valueOf(), 973065597000, "78.8 Wrong date value returned");
	verify (dDate1.toString(), "Tue Oct 31 23:59:57 PST 2000", "78.9 Wrong date string returned");


	apInitScenario("79 Milliseconds = positive floating point number");
	dDate1 = new Date(Date.UTC(2000,10,1,8,0,0,3.14159));
	verify (dDate1.getFullYear(), 2000, "79.1 Wrong date returned");
	verify (dDate1.getMonth(), 10, "79.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "79.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "79.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "79.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "79.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 3, "79.7 Wrong date returned");
	verify (dDate1.valueOf(), 973065600003, "79.8 Wrong date value returned");
	verify (dDate1.toString(), "Wed Nov 1 00:00:00 PST 2000", "79.9 Wrong date string returned");


	apInitScenario("80 Milliseconds = negative floating point number");
	dDate1 = new Date(Date.UTC(2000,10,1,8,0,0,-3));
	verify (dDate1.getFullYear(), 2000, "80.1 Wrong date returned");
	verify (dDate1.getMonth(), 9, "80.2 Wrong date returned");
	verify (dDate1.getDate(), 31, "80.3 Wrong date returned");
	verify (dDate1.getHours(), 23, "80.4 Wrong date returned");
	verify (dDate1.getMinutes(), 59, "80.5 Wrong date returned");
	verify (dDate1.getSeconds(), 59, "80.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 997, "80.7 Wrong date returned");
	verify (dDate1.valueOf(), 973065599997, "80.8 Wrong date value returned");
	verify (dDate1.toString(), "Tue Oct 31 23:59:59 PST 2000", "80.9 Wrong date string returned");


	apInitScenario("81 2/29/1996 (Feb 29)");
	dDate1 = new Date(Date.UTC(1996,1,29,8,0,0,0));
	verify (dDate1.getFullYear(), 1996, "81.1 Wrong date returned");
	verify (dDate1.getMonth(), 1, "81.2 Wrong date returned");
	verify (dDate1.getDate(), 29, "81.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "81.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "81.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "81.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "81.7 Wrong date returned");
	verify (dDate1.valueOf(), 825580800000, "81.8 Wrong date value returned");
	verify (dDate1.toString(), "Thu Feb 29 00:00:00 PST 1996", "81.9 Wrong date string returned");


	apInitScenario("82 2/29/1999 (Mar 1)");
	dDate1 = new Date(Date.UTC(1999,1,29,8,0,0,0));
	verify (dDate1.getFullYear(), 1999, "82.1 Wrong date returned");
	verify (dDate1.getMonth(), 2, "82.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "82.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "82.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "82.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "82.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "82.7 Wrong date returned");
	verify (dDate1.valueOf(), 920275200000, "82.8 Wrong date value returned");
	verify (dDate1.toString(), "Mon Mar 1 00:00:00 PST 1999", "82.9 Wrong date string returned");


	apInitScenario("83 2/29/2000 (Feb 29)");
	dDate1 = new Date(Date.UTC(2000,1,29,8,0,0,0));
	verify (dDate1.getFullYear(), 2000, "83.1 Wrong date returned");
	verify (dDate1.getMonth(), 1, "83.2 Wrong date returned");
	verify (dDate1.getDate(), 29, "83.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "83.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "83.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "83.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "83.7 Wrong date returned");
	verify (dDate1.valueOf(), 951811200000, "83.8 Wrong date value returned");
	verify (dDate1.toString(), "Tue Feb 29 00:00:00 PST 2000", "83.9 Wrong date string returned");


	apInitScenario("84 2/29/2001 (Mar 1)");
	dDate1 = new Date(Date.UTC(2001,1,29,8,0,0,0));
	verify (dDate1.getFullYear(), 2001, "84.1 Wrong date returned");
	verify (dDate1.getMonth(), 2, "84.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "84.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "84.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "84.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "84.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "84.7 Wrong date returned");
	verify (dDate1.valueOf(), 983433600000, "84.8 Wrong date value returned");
	verify (dDate1.toString(), "Thu Mar 1 00:00:00 PST 2001", "84.9 Wrong date string returned");


	apInitScenario("85 2/29/1900 (Mar 1)");
	dDate1 = new Date(Date.UTC(1900,1,29,8,0,0,0));
	verify (dDate1.getFullYear(), 1900, "85.1 Wrong date returned");
	verify (dDate1.getMonth(), 2, "85.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "85.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "85.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "85.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "85.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "85.7 Wrong date returned");
	verify (dDate1.valueOf(), -2203862400000, "85.8 Wrong date value returned");
	verify (dDate1.toString(), "Thu Mar 1 00:00:00 PST 1900", "85.9 Wrong date string returned");


	apInitScenario("86 1/60/2000 (Feb 29)");
	dDate1 = new Date(Date.UTC(2000,0,60,8,0,0,0));
	verify (dDate1.getFullYear(), 2000, "86.1 Wrong date returned");
	verify (dDate1.getMonth(), 1, "86.2 Wrong date returned");
	verify (dDate1.getDate(), 29, "86.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "86.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "86.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "86.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "86.7 Wrong date returned");
	verify (dDate1.valueOf(), 951811200000, "86.8 Wrong date value returned");
	verify (dDate1.toString(), "Tue Feb 29 00:00:00 PST 2000", "86.9 Wrong date string returned");


	apInitScenario("87 1/60/1999 (Mar 1)");
	dDate1 = new Date(Date.UTC(1999,0,60,8,0,0,0));
	verify (dDate1.getFullYear(), 1999, "87.1 Wrong date returned");
	verify (dDate1.getMonth(), 2, "87.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "87.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "87.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "87.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "87.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "87.7 Wrong date returned");
	verify (dDate1.valueOf(), 920275200000, "87.8 Wrong date value returned");
	verify (dDate1.toString(), "Mon Mar 1 00:00:00 PST 1999", "87.9 Wrong date string returned");


	apInitScenario("88 2/29/1600 (Feb 29)");
	dDate1 = new Date(Date.UTC(1600,1,29,8,0,0,0));
	verify (dDate1.getFullYear(), 1600, "88.1 Wrong date returned");
	verify (dDate1.getMonth(), 1, "88.2 Wrong date returned");
	verify (dDate1.getDate(), 29, "88.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "88.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "88.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "88.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "88.7 Wrong date returned");
	verify (dDate1.valueOf(), -11670969600000, "88.8 Wrong date value returned");
	verify (dDate1.toString(), "Tue Feb 29 00:00:00 PST 1600", "88.9 Wrong date string returned");


	apInitScenario("89 2/29/2400 (Feb 29)");
	dDate1 = new Date(Date.UTC(2400,1,29,8,0,0,0));
	verify (dDate1.getFullYear(), 2400, "89.1 Wrong date returned");
	verify (dDate1.getMonth(), 1, "89.2 Wrong date returned");
	verify (dDate1.getDate(), 29, "89.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "89.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "89.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "89.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "89.7 Wrong date returned");
	verify (dDate1.valueOf(), 13574592000000, "89.8 Wrong date value returned");
	verify (dDate1.toString(), "Tue Feb 29 00:00:00 PST 2400", "89.9 Wrong date string returned");


	apInitScenario("90 2/29/2000 23:00:00:000 (Feb 29)");
	dDate1 = new Date(Date.UTC(2000,1,29,23 + 8,0,0,0));
	verify (dDate1.getFullYear(), 2000, "90.1 Wrong date returned");
	verify (dDate1.getMonth(), 1, "90.2 Wrong date returned");
	verify (dDate1.getDate(), 29, "90.3 Wrong date returned");
	verify (dDate1.getHours(), 23, "90.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "90.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "90.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "90.7 Wrong date returned");
	verify (dDate1.valueOf(), 951894000000, "90.8 Wrong date value returned");
	verify (dDate1.toString(), "Tue Feb 29 23:00:00 PST 2000", "90.9 Wrong date string returned");


	apInitScenario("91 2/29/2000 24:00:00:000 (Mar 1)");
	dDate1 = new Date(Date.UTC(2000,1,29,24 + 8,0,0,0));
	verify (dDate1.getFullYear(), 2000, "91.1 Wrong date returned");
	verify (dDate1.getMonth(), 2, "91.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "91.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "91.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "91.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "91.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "91.7 Wrong date returned");
	verify (dDate1.valueOf(), 951897600000, "91.8 Wrong date value returned");
	verify (dDate1.toString(), "Wed Mar 1 00:00:00 PST 2000", "91.9 Wrong date string returned");


	apInitScenario("92 2/29/2000 23:59:00:000 (Feb 29)");
	dDate1 = new Date(Date.UTC(2000,1,29,23 + 8,59,0,0));
	verify (dDate1.getFullYear(), 2000, "92.1 Wrong date returned");
	verify (dDate1.getMonth(), 1, "92.2 Wrong date returned");
	verify (dDate1.getDate(), 29, "92.3 Wrong date returned");
	verify (dDate1.getHours(), 23, "92.4 Wrong date returned");
	verify (dDate1.getMinutes(), 59, "92.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "92.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "92.7 Wrong date returned");
	verify (dDate1.valueOf(), 951897540000, "92.8 Wrong date value returned");
	verify (dDate1.toString(), "Tue Feb 29 23:59:00 PST 2000", "92.9 Wrong date string returned");


	apInitScenario("93 2/29/2000 23:60:00:000 (Mar 1)");
	dDate1 = new Date(Date.UTC(2000,1,29,23 + 8,60,0,0));
	verify (dDate1.getFullYear(), 2000, "93.1 Wrong date returned");
	verify (dDate1.getMonth(), 2, "93.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "93.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "93.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "93.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "93.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "93.7 Wrong date returned");
	verify (dDate1.valueOf(), 951897600000, "93.8 Wrong date value returned");
	verify (dDate1.toString(), "Wed Mar 1 00:00:00 PST 2000", "93.9 Wrong date string returned");


	apInitScenario("94 2/29/2000 23:59:59:000 (Feb 29)");
	dDate1 = new Date(Date.UTC(2000,1,29,23 + 8,59,59,0));
	verify (dDate1.getFullYear(), 2000, "94.1 Wrong date returned");
	verify (dDate1.getMonth(), 1, "94.2 Wrong date returned");
	verify (dDate1.getDate(), 29, "94.3 Wrong date returned");
	verify (dDate1.getHours(), 23, "94.4 Wrong date returned");
	verify (dDate1.getMinutes(), 59, "94.5 Wrong date returned");
	verify (dDate1.getSeconds(), 59, "94.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "94.7 Wrong date returned");
	verify (dDate1.valueOf(), 951897599000, "94.8 Wrong date value returned");
	verify (dDate1.toString(), "Tue Feb 29 23:59:59 PST 2000", "94.9 Wrong date string returned");


	apInitScenario("95 2/29/2000 23:59:60:000 (Mar 1)");
	dDate1 = new Date(Date.UTC(2000,1,29,23 + 8,59,60,0));
	verify (dDate1.getFullYear(), 2000, "95.1 Wrong date returned");
	verify (dDate1.getMonth(), 2, "95.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "95.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "95.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "95.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "95.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "95.7 Wrong date returned");
	verify (dDate1.valueOf(), 951897600000, "95.8 Wrong date value returned");
	verify (dDate1.toString(), "Wed Mar 1 00:00:00 PST 2000", "95.9 Wrong date string returned");


	apInitScenario("96 2/29/2000 23:59:59:999 (Feb 29)");
	dDate1 = new Date(Date.UTC(2000,1,29,23 + 8,59,59,999));
	verify (dDate1.getFullYear(), 2000, "96.1 Wrong date returned");
	verify (dDate1.getMonth(), 1, "96.2 Wrong date returned");
	verify (dDate1.getDate(), 29, "96.3 Wrong date returned");
	verify (dDate1.getHours(), 23, "96.4 Wrong date returned");
	verify (dDate1.getMinutes(), 59, "96.5 Wrong date returned");
	verify (dDate1.getSeconds(), 59, "96.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 999, "96.7 Wrong date returned");
	verify (dDate1.valueOf(), 951897599999, "96.8 Wrong date value returned");
	verify (dDate1.toString(), "Tue Feb 29 23:59:59 PST 2000", "96.9 Wrong date string returned");


	apInitScenario("97 2/29/2000 23:59:59:1000 (Mar 1)");
	dDate1 = new Date(Date.UTC(2000,1,29,23 + 8,59,59,1000));
	verify (dDate1.getFullYear(), 2000, "97.1 Wrong date returned");
	verify (dDate1.getMonth(), 2, "97.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "97.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "97.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "97.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "97.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "97.7 Wrong date returned");
	verify (dDate1.valueOf(), 951897600000, "96.8 Wrong date value returned");
	verify (dDate1.toString(), "Wed Mar 1 00:00:00 PST 2000", "97.9 Wrong date string returned");


	apInitScenario("98 2/29/2000 23:59:60:-1 (Feb29)");
	dDate1 = new Date(Date.UTC(2000,1,29,23 + 8,59,60,-1));
	verify (dDate1.getFullYear(), 2000, "98.1 Wrong date returned");
	verify (dDate1.getMonth(), 1, "98.2 Wrong date returned");
	verify (dDate1.getDate(), 29, "98.3 Wrong date returned");
	verify (dDate1.getHours(), 23, "98.4 Wrong date returned");
	verify (dDate1.getMinutes(), 59, "98.5 Wrong date returned");
	verify (dDate1.getSeconds(), 59, "98.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 999, "98.7 Wrong date returned");
	verify (dDate1.valueOf(), 951897599999, "98.8 Wrong date value returned");
	verify (dDate1.toString(), "Tue Feb 29 23:59:59 PST 2000", "98.9 Wrong date string returned");


	apInitScenario("99 2/29/2000 23:60:-1:000 (Feb29)");
	dDate1 = new Date(Date.UTC(2000,1,29,23 + 8,60,-1,0));
	verify (dDate1.getFullYear(), 2000, "99.1 Wrong date returned");
	verify (dDate1.getMonth(), 1, "99.2 Wrong date returned");
	verify (dDate1.getDate(), 29, "99.3 Wrong date returned");
	verify (dDate1.getHours(), 23, "99.4 Wrong date returned");
	verify (dDate1.getMinutes(), 59, "99.5 Wrong date returned");
	verify (dDate1.getSeconds(), 59, "99.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "99.7 Wrong date returned");
	verify (dDate1.valueOf(), 951897599000, "99.8 Wrong date value returned");
	verify (dDate1.toString(), "Tue Feb 29 23:59:59 PST 2000", "99.9 Wrong date string returned");


	apInitScenario("100 2/29/2000 24:-1:00:000 (Feb29)");
	dDate1 = new Date(Date.UTC(2000,1,29,24 + 8,-1,0,0));
	verify (dDate1.getFullYear(), 2000, "100.1 Wrong date returned");
	verify (dDate1.getMonth(), 1, "100.2 Wrong date returned");
	verify (dDate1.getDate(), 29, "100.3 Wrong date returned");
	verify (dDate1.getHours(), 23, "100.4 Wrong date returned");
	verify (dDate1.getMinutes(), 59, "100.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "100.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "100.7 Wrong date returned");
	verify (dDate1.valueOf(), 951897540000, "100.8 Wrong date value returned");
	verify (dDate1.toString(), "Tue Feb 29 23:59:00 PST 2000", "100.9 Wrong date string returned");


	apInitScenario("101 3/1/2000 -1:0:00:000 (Feb28)");
	dDate1 = new Date(Date.UTC(2000,2,1,-1 + 8,0,0,0));
	verify (dDate1.getFullYear(), 2000, "101.1 Wrong date returned");
	verify (dDate1.getMonth(), 1, "101.2 Wrong date returned");
	verify (dDate1.getDate(), 29, "101.3 Wrong date returned");
	verify (dDate1.getHours(), 23, "101.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "101.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "101.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "101.7 Wrong date returned");
	verify (dDate1.valueOf(), 951894000000, "101.8 Wrong date value returned");
	verify (dDate1.toString(), "Tue Feb 29 23:00:00 PST 2000", "101.9 Wrong date string returned");


	apInitScenario("102 Daylight Savings Time, First Sunday of April, 2:00am -> 3:00am");
	dDate1 = new Date(Date.UTC(2000,3,2,4 + iUTCOffset,0,0,0)); // hours used to be 2 +, then 3 +, now 4 +
	verify (dDate1.getFullYear(), 2000, "102.1 Wrong date returned");
	verify (dDate1.getMonth(), 3, "102.2 Wrong date returned");
	verify (dDate1.getDate(), 2, "102.3 Wrong date returned");
	verify (dDate1.getHours(), 4, "102.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "102.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "102.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "102.7 Wrong date returned");
	verify (dDate1.valueOf(), 954676800000-iUTCValueOffset, "102.8 Wrong date value returned");
	verify (dDate1.toString(), "Sun Apr 2 04:00:00 " + strDT + " 2000", "102.9 Wrong date string returned");


	apInitScenario("103 Daylight Savings Time, Last Sunday of October, 4:00am -> 4:00am");
	dDate1 = new Date(Date.UTC(2000,9,29,4 + 8,0,0,0));	//this is outside the DST range
	verify (dDate1.getFullYear(), 2000, "103.1 Wrong date returned");
	verify (dDate1.getMonth(), 9, "103.2 Wrong date returned");
	verify (dDate1.getDate(), 29, "103.3 Wrong date returned");
	verify (dDate1.getHours(), 4, "103.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "103.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "103.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "103.7 Wrong date returned");
	verify (dDate1.valueOf(), 972820800000-0, "103.8 Wrong date value returned");	//this is outside the DST range
	verify (dDate1.toString(), "Sun Oct 29 04:00:00 PST 2000", "103.9 Wrong date string returned");


	apInitScenario("104 Year = 1999, Month = 12, Day = 91 (should produce February 29, 2000)");
	dDate1 = new Date(Date.UTC(1999,11,91,8,0,0,0));
	verify (dDate1.getFullYear(), 2000, "104.1 Wrong date returned");
	verify (dDate1.getMonth(), 1, "104.2 Wrong date returned");
	verify (dDate1.getDate(), 29, "104.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "104.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "104.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "104.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "104.7 Wrong date returned");
	verify (dDate1.valueOf(), 951811200000, "104.8 Wrong date value returned");
	verify (dDate1.toString(), "Tue Feb 29 00:00:00 PST 2000", "104.9 Wrong date string returned");


	apInitScenario("105 Year = 1999, Month = 15, Day = 31 (should produce May 1, 2000)");
	dDate1 = new Date(Date.UTC(1999,15,31,iUTCOffset,0,0,0)); // took away -1
	verify (dDate1.getFullYear(), 2000, "105.1 Wrong date returned");
	verify (dDate1.getMonth(), 4, "105.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "105.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "105.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "105.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "105.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "105.7 Wrong date returned");
	verify (dDate1.valueOf(), 957168000000-iUTCValueOffset, "105.8 Wrong date value returned");
	verify (dDate1.toString(), "Mon May 1 00:00:00 " + strDT + " 2000", "105.9 Wrong date string returned");


	apInitScenario("106 Year = -1, Month = 23, Day = 32 (should produce Jan 1, 1 ad)");
	dDate1 = new Date(Date.UTC(-1,23,32,8,0,0,0));
	verify (dDate1.getFullYear(), 1, "106.1 Wrong date returned");
	verify (dDate1.getMonth(), 0, "106.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "106.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "106.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "106.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "106.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "106.7 Wrong date returned");
	verify (dDate1.valueOf(), -62135568000000, "106.8 Wrong date value returned");
	verify (dDate1.toString(), "Mon Jan 1 00:00:00 PST 1", "106.9 Wrong date string returned");


	apInitScenario("107 Year = -1, Month = 12, Day = 367 (should produce Jan 1, 1 ad)");
	dDate1 = new Date(Date.UTC(-1,12,367,8,0,0,0));
	verify (dDate1.getFullYear(), 1, "107.1 Wrong date returned");
	verify (dDate1.getMonth(), 0, "107.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "107.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "107.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "107.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "107.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "107.7 Wrong date returned");
	verify (dDate1.valueOf(), -62135568000000, "107.8 Wrong date value returned");
	verify (dDate1.toString(), "Mon Jan 1 00:00:00 PST 1", "107.9 Wrong date string returned");


	apInitScenario("108 Year = -1, Month = 23, Day = 31, time = 23:59:59:999 (Dec 31, 1 bc)");
	dDate1 = new Date(Date.UTC(-1,23,31,23 + 8,59,59,999));
	verify (dDate1.getFullYear(), 0, "108.1 Wrong date returned");
	verify (dDate1.getMonth(), 11, "108.2 Wrong date returned");
	verify (dDate1.getDate(), 31, "108.3 Wrong date returned");
	verify (dDate1.getHours(), 23, "108.4 Wrong date returned");
	verify (dDate1.getMinutes(), 59, "108.5 Wrong date returned");
	verify (dDate1.getSeconds(), 59, "108.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 999, "108.7 Wrong date returned");
	verify (dDate1.valueOf(), -62135568000001, "108.8 Wrong date value returned");
	verify (dDate1.toString(), "Sun Dec 31 23:59:59 PST 1 B.C.", "108.9 Wrong date string returned");


	apInitScenario("109 Year = -1, Month = 23, Day = 31, time = 23:59:59:1000 (Jan 1, 1 ad)");
	dDate1 = new Date(Date.UTC(-1,23,31,23 + 8,59,59,1000));
	verify (dDate1.getFullYear(), 1, "109.1 Wrong date returned");
	verify (dDate1.getMonth(), 0, "109.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "109.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "109.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "109.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "109.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "109.7 Wrong date returned");
	verify (dDate1.valueOf(), -62135568000000, "109.8 Wrong date value returned");
	verify (dDate1.toString(), "Mon Jan 1 00:00:00 PST 1", "109.9 Wrong date string returned");


	apInitScenario("110 Year = -1, Month = 23, Day = 31, time = 23:59:60:-1 (Dec 31, 1 bc)");
	dDate1 = new Date(Date.UTC(-1,23,31,23 + 8,59,60,-1));
	verify (dDate1.getFullYear(), 0, "110.1 Wrong date returned");
	verify (dDate1.getMonth(), 11, "110.2 Wrong date returned");
	verify (dDate1.getDate(), 31, "110.3 Wrong date returned");
	verify (dDate1.getHours(), 23, "110.4 Wrong date returned");
	verify (dDate1.getMinutes(), 59, "110.5 Wrong date returned");
	verify (dDate1.getSeconds(), 59, "110.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 999, "110.7 Wrong date returned");
	verify (dDate1.valueOf(), -62135568000001, "110.8 Wrong date value returned");
	verify (dDate1.toString(), "Sun Dec 31 23:59:59 PST 1 B.C.", "110.9 Wrong date string returned");


	apInitScenario("111 Year = -1, Month = 23, Day = 31, time = 23:59:60:000 (Jan 1, 1 ad)");
	dDate1 = new Date(Date.UTC(-1,23,31,23 + 8,59,60,0));
	verify (dDate1.getFullYear(), 1, "111.1 Wrong date returned");
	verify (dDate1.getMonth(), 0, "111.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "111.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "111.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "111.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "111.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "111.7 Wrong date returned");
	verify (dDate1.valueOf(), -62135568000000, "111.8 Wrong date value returned");
	verify (dDate1.toString(), "Mon Jan 1 00:00:00 PST 1", "111.9 Wrong date string returned");


	apInitScenario("112 Year = -1, Month = 23, Day = 31, time = 23:60:-1:000 (Dec 31, 1 bc)");
	dDate1 = new Date(Date.UTC(-1,23,31,23 + 8,60,-1,0));
	verify (dDate1.getFullYear(), 0, "112.1 Wrong date returned");
	verify (dDate1.getMonth(), 11, "112.2 Wrong date returned");
	verify (dDate1.getDate(), 31, "112.3 Wrong date returned");
	verify (dDate1.getHours(), 23, "112.4 Wrong date returned");
	verify (dDate1.getMinutes(), 59, "112.5 Wrong date returned");
	verify (dDate1.getSeconds(), 59, "112.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "112.7 Wrong date returned");
	verify (dDate1.valueOf(), -62135568001000, "112.8 Wrong date value returned");
	verify (dDate1.toString(), "Sun Dec 31 23:59:59 PST 1 B.C.", "112.9 Wrong date string returned");


	apInitScenario("113 Year = -1, Month = 23, Day = 31, time = 23:60:0:000 (Jan 1, 1 ad)");
	dDate1 = new Date(Date.UTC(-1,23,31,23 + 8,60,0,0));
	verify (dDate1.getFullYear(), 1, "113.1 Wrong date returned");
	verify (dDate1.getMonth(), 0, "113.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "113.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "113.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "113.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "113.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "113.7 Wrong date returned");
	verify (dDate1.valueOf(), -62135568000000, "113.8 Wrong date value returned");
	verify (dDate1.toString(), "Mon Jan 1 00:00:00 PST 1", "113.9 Wrong date string returned");


	apInitScenario("114 Year = -1, Month = 23, Day = 31, time = 24:-1:0:000 (Dec 31, 1 bc)");
	dDate1 = new Date(Date.UTC(-1,23,31,24 + 8,-1,0,0));
	verify (dDate1.getFullYear(), 0, "114.1 Wrong date returned");
	verify (dDate1.getMonth(), 11, "114.2 Wrong date returned");
	verify (dDate1.getDate(), 31, "114.3 Wrong date returned");
	verify (dDate1.getHours(), 23, "114.4 Wrong date returned");
	verify (dDate1.getMinutes(), 59, "114.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "114.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "114.7 Wrong date returned");
	verify (dDate1.valueOf(), -62135568060000, "114.8 Wrong date value returned");
	verify (dDate1.toString(), "Sun Dec 31 23:59:00 PST 1 B.C.", "114.9 Wrong date string returned");


	apInitScenario("115 Year = -1, Month = 23, Day = 31, time = 24:0:0:000 (Jan 1, 1 ad)");
	dDate1 = new Date(Date.UTC(-1,23,31,24 + 8,0,0,0));
	verify (dDate1.getFullYear(), 1, "115.1 Wrong date returned");
	verify (dDate1.getMonth(), 0, "115.2 Wrong date returned");
	verify (dDate1.getDate(), 1, "115.3 Wrong date returned");
	verify (dDate1.getHours(), 0, "115.4 Wrong date returned");
	verify (dDate1.getMinutes(), 0, "115.5 Wrong date returned");
	verify (dDate1.getSeconds(), 0, "115.6 Wrong date returned");
	verify (dDate1.getMilliseconds(), 0, "115.7 Wrong date returned");
	verify (dDate1.valueOf(), -62135568000000, "115.8 Wrong date value returned");
	verify (dDate1.toString(), "Mon Jan 1 00:00:00 PST 1", "115.9 Wrong date string returned");


	apInitScenario("117 Date.valueOf() == 8640000000000001 (NaN)");
	var dDate2;	
	dDate2 = new Date(8640000000000001);
	dDate1 = new Date(Date.UTC(dDate2.getFullYear(),
					           dDate2.getMonth(),
							   dDate2.getDate(),
							   dDate2.getHours() + iUTCOffset,
							   dDate2.getMinutes(),
							   dDate2.getSeconds(),
							   dDate2.getMilliseconds()));
	verify ((dDate1.getFullYear()).toString(), "NaN", "117.1 Wrong date returned");
	verify ((dDate1.getMonth()).toString(), "NaN", "117.2 Wrong date returned");
	verify ((dDate1.getDate()).toString(), "NaN", "117.3 Wrong date returned");
	verify ((dDate1.getHours()).toString(), "NaN", "117.4 Wrong date returned");
	verify ((dDate1.getMinutes()).toString(), "NaN", "117.5 Wrong date returned");
	verify ((dDate1.getSeconds()).toString(), "NaN", "117.6 Wrong date returned");
	verify ((dDate1.getMilliseconds()).toString(), "NaN", "117.7 Wrong date returned");
	verify ((dDate1.valueOf()).toString(), "NaN", "117.8 Wrong date value returned");
	verify (dDate1.toString(), "NaN", "117.9 Wrong date string returned");


	apInitScenario("119 Date.valueOf() == -8640000000000001 (NaN)");
	dDate2 = new Date(-8640000000000001);
	dDate1 = new Date(Date.UTC(dDate2.getFullYear(),
					           dDate2.getMonth(),
							   dDate2.getDate(),
							   dDate2.getHours() + iUTCOffset,
							   dDate2.getMinutes(),
							   dDate2.getSeconds(),
							   dDate2.getMilliseconds()));
	verify ((dDate1.getFullYear()).toString(), "NaN", "119.1 Wrong date returned");
	verify ((dDate1.getMonth()).toString(), "NaN", "119.2 Wrong date returned");
	verify ((dDate1.getDate()).toString(), "NaN", "119.3 Wrong date returned");
	verify ((dDate1.getHours()).toString(), "NaN", "119.4 Wrong date returned");
	verify ((dDate1.getMinutes()).toString(), "NaN", "119.5 Wrong date returned");
	verify ((dDate1.getSeconds()).toString(), "NaN", "119.6 Wrong date returned");
	verify ((dDate1.getMilliseconds()).toString(), "NaN", "119.7 Wrong date returned");
	verify ((dDate1.valueOf()).toString(), "NaN", "119.8 Wrong date value returned");
	verify (dDate1.toString(), "NaN", "119.9 Wrong date string returned");

        }
	apEndTest();
}

dtutc01();


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
