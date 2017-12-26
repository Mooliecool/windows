// ==++==
//
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
//
// ==--==
namespace System.Globalization {
    using System;


    ////////////////////////////////////////////////////////////////////////////
    //
    //  Notes about UmAlQuraCalendar
    //
    ////////////////////////////////////////////////////////////////////////////
     /*
     **  Calendar support range:
     **      Calendar    Minimum     Maximum
     **      ==========  ==========  ==========
     **      Gregorian   1900/04/30   2029/05/13
     **      UmAlQura    1318/01/01   1450/12/29
     */

    [Serializable]
    public class UmAlQuraCalendar : Calendar {

        internal const int MinCalendarYear = 1318;
        internal const int MaxCalendarYear = 1450;

        internal struct DateMapping
        {
            internal DateMapping(int MonthsLengthFlags, int GYear, int GMonth, int GDay) 
            {
                HijriMonthsLengthFlags = MonthsLengthFlags;
                GregorianDate = new DateTime(GYear, GMonth, GDay);
            }
            internal int HijriMonthsLengthFlags;
            internal DateTime GregorianDate;
        }

//These data is taken from Tables/Excel/UmAlQura.xls please make sure that the two places are in sync        
        static readonly DateMapping [] HijriYearInfo = {
/*                    DaysPerM     GY      GM     GD     D1   D2   D3   D4   D5   D6   D7   D8   D9   D10  D11  D12    
1318*/new DateMapping (0x02EA,    1900,     4,    30),/* 0    1    0    1    0    1    1    1    0    1    0    0    4/30/1900
1319*/new DateMapping (0x06E9,    1901,     4,    19),/* 1    0    0    1    0    1    1    1    0    1    1    0    4/19/1901
1320*/new DateMapping (0x0ED2,    1902,     4,     9),/* 0    1    0    0    1    0    1    1    0    1    1    1    4/9/1902
1321*/new DateMapping (0x0EA4,    1903,     3,    30),/* 0    0    1    0    0    1    0    1    0    1    1    1    3/30/1903
1322*/new DateMapping (0x0D4A,    1904,     3,    18),/* 0    1    0    1    0    0    1    0    1    0    1    1    3/18/1904
1323*/new DateMapping (0x0A96,    1905,     3,     7),/* 0    1    1    0    1    0    0    1    0    1    0    1    3/7/1905
1324*/new DateMapping (0x0536,    1906,     2,    24),/* 0    1    1    0    1    1    0    0    1    0    1    0    2/24/1906
1325*/new DateMapping (0x0AB5,    1907,     2,    13),/* 1    0    1    0    1    1    0    1    0    1    0    1    2/13/1907
1326*/new DateMapping (0x0DAA,    1908,     2,     3),/* 0    1    0    1    0    1    0    1    1    0    1    1    2/3/1908
1327*/new DateMapping (0x0BA4,    1909,     1,    23),/* 0    0    1    0    0    1    0    1    1    1    0    1    1/23/1909
1328*/new DateMapping (0x0B49,    1910,     1,    12),/* 1    0    0    1    0    0    1    0    1    1    0    1    1/12/1910
1329*/new DateMapping (0x0A93,    1911,     1,     1),/* 1    1    0    0    1    0    0    1    0    1    0    1    1/1/1911
1330*/new DateMapping (0x052B,    1911,    12,    21),/* 1    1    0    1    0    1    0    0    1    0    1    0    12/21/1911
1331*/new DateMapping (0x0A57,    1912,    12,     9),/* 1    1    1    0    1    0    1    0    0    1    0    1    12/9/1912
1332*/new DateMapping (0x04B6,    1913,    11,    29),/* 0    1    1    0    1    1    0    1    0    0    1    0    11/29/1913
1333*/new DateMapping (0x0AB5,    1914,    11,    18),/* 1    0    1    0    1    1    0    1    0    1    0    1    11/18/1914
1334*/new DateMapping (0x05AA,    1915,    11,     8),/* 0    1    0    1    0    1    0    1    1    0    1    0    11/8/1915
1335*/new DateMapping (0x0D55,    1916,    10,    27),/* 1    0    1    0    1    0    1    0    1    0    1    1    10/27/1916
1336*/new DateMapping (0x0D2A,    1917,    10,    17),/* 0    1    0    1    0    1    0    0    1    0    1    1    10/17/1917
1337*/new DateMapping (0x0A56,    1918,    10,     6),/* 0    1    1    0    1    0    1    0    0    1    0    1    10/6/1918
1338*/new DateMapping (0x04AE,    1919,     9,    25),/* 0    1    1    1    0    1    0    1    0    0    1    0    9/25/1919
1339*/new DateMapping (0x095D,    1920,     9,    13),/* 1    0    1    1    1    0    1    0    1    0    0    1    9/13/1920
1340*/new DateMapping (0x02EC,    1921,     9,     3),/* 0    0    1    1    0    1    1    1    0    1    0    0    9/3/1921
1341*/new DateMapping (0x06D5,    1922,     8,    23),/* 1    0    1    0    1    0    1    1    0    1    1    0    8/23/1922
1342*/new DateMapping (0x06AA,    1923,     8,    13),/* 0    1    0    1    0    1    0    1    0    1    1    0    8/13/1923
1343*/new DateMapping (0x0555,    1924,     8,     1),/* 1    0    1    0    1    0    1    0    1    0    1    0    8/1/1924
1344*/new DateMapping (0x04AB,    1925,     7,    21),/* 1    1    0    1    0    1    0    1    0    0    1    0    7/21/1925
1345*/new DateMapping (0x095B,    1926,     7,    10),/* 1    1    0    1    1    0    1    0    1    0    0    1    7/10/1926
1346*/new DateMapping (0x02BA,    1927,     6,    30),/* 0    1    0    1    1    1    0    1    0    1    0    0    6/30/1927
1347*/new DateMapping (0x0575,    1928,     6,    18),/* 1    0    1    0    1    1    1    0    1    0    1    0    6/18/1928
1348*/new DateMapping (0x0BB2,    1929,     6,     8),/* 0    1    0    0    1    1    0    1    1    1    0    1    6/8/1929
1349*/new DateMapping (0x0764,    1930,     5,    29),/* 0    0    1    0    0    1    1    0    1    1    1    0    5/29/1930
1350*/new DateMapping (0x0749,    1931,     5,    18),/* 1    0    0    1    0    0    1    0    1    1    1    0    5/18/1931
1351*/new DateMapping (0x0655,    1932,     5,     6),/* 1    0    1    0    1    0    1    0    0    1    1    0    5/6/1932
1352*/new DateMapping (0x02AB,    1933,     4,    25),/* 1    1    0    1    0    1    0    1    0    1    0    0    4/25/1933
1353*/new DateMapping (0x055B,    1934,     4,    14),/* 1    1    0    1    1    0    1    0    1    0    1    0    4/14/1934
1354*/new DateMapping (0x0ADA,    1935,     4,     4),/* 0    1    0    1    1    0    1    1    0    1    0    1    4/4/1935
1355*/new DateMapping (0x06D4,    1936,     3,    24),/* 0    0    1    0    1    0    1    1    0    1    1    0    3/24/1936
1356*/new DateMapping (0x0EC9,    1937,     3,    13),/* 1    0    0    1    0    0    1    1    0    1    1    1    3/13/1937
1357*/new DateMapping (0x0D92,    1938,     3,     3),/* 0    1    0    0    1    0    0    1    1    0    1    1    3/3/1938
1358*/new DateMapping (0x0D25,    1939,     2,    20),/* 1    0    1    0    0    1    0    0    1    0    1    1    2/20/1939
1359*/new DateMapping (0x0A4D,    1940,     2,     9),/* 1    0    1    1    0    0    1    0    0    1    0    1    2/9/1940
1360*/new DateMapping (0x02AD,    1941,     1,    28),/* 1    0    1    1    0    1    0    1    0    1    0    0    1/28/1941
1361*/new DateMapping (0x056D,    1942,     1,    17),/* 1    0    1    1    0    1    1    0    1    0    1    0    1/17/1942
1362*/new DateMapping (0x0B6A,    1943,     1,     7),/* 0    1    0    1    0    1    1    0    1    1    0    1    1/7/1943
1363*/new DateMapping (0x0B52,    1943,    12,    28),/* 0    1    0    0    1    0    1    0    1    1    0    1    12/28/1943
1364*/new DateMapping (0x0AA5,    1944,    12,    16),/* 1    0    1    0    0    1    0    1    0    1    0    1    12/16/1944
1365*/new DateMapping (0x0A4B,    1945,    12,     5),/* 1    1    0    1    0    0    1    0    0    1    0    1    12/5/1945
1366*/new DateMapping (0x0497,    1946,    11,    24),/* 1    1    1    0    1    0    0    1    0    0    1    0    11/24/1946
1367*/new DateMapping (0x0937,    1947,    11,    13),/* 1    1    1    0    1    1    0    0    1    0    0    1    11/13/1947
1368*/new DateMapping (0x02B6,    1948,    11,     2),/* 0    1    1    0    1    1    0    1    0    1    0    0    11/2/1948
1369*/new DateMapping (0x0575,    1949,    10,    22),/* 1    0    1    0    1    1    1    0    1    0    1    0    10/22/1949
1370*/new DateMapping (0x0D6A,    1950,    10,    12),/* 0    1    0    1    0    1    1    0    1    0    1    1    10/12/1950
1371*/new DateMapping (0x0D52,    1951,    10,     2),/* 0    1    0    0    1    0    1    0    1    0    1    1    10/2/1951
1372*/new DateMapping (0x0A96,    1952,     9,    20),/* 0    1    1    0    1    0    0    1    0    1    0    1    9/20/1952
1373*/new DateMapping (0x092D,    1953,     9,     9),/* 1    0    1    1    0    1    0    0    1    0    0    1    9/9/1953
1374*/new DateMapping (0x025D,    1954,     8,    29),/* 1    0    1    1    1    0    1    0    0    1    0    0    8/29/1954
1375*/new DateMapping (0x04DD,    1955,     8,    18),/* 1    0    1    1    1    0    1    1    0    0    1    0    8/18/1955
1376*/new DateMapping (0x0ADA,    1956,     8,     7),/* 0    1    0    1    1    0    1    1    0    1    0    1    8/7/1956
1377*/new DateMapping (0x05D4,    1957,     7,    28),/* 0    0    1    0    1    0    1    1    1    0    1    0    7/28/1957
1378*/new DateMapping (0x0DA9,    1958,     7,    17),/* 1    0    0    1    0    1    0    1    1    0    1    1    7/17/1958
1379*/new DateMapping (0x0D52,    1959,     7,     7),/* 0    1    0    0    1    0    1    0    1    0    1    1    7/7/1959
1380*/new DateMapping (0x0AAA,    1960,     6,    25),/* 0    1    0    1    0    1    0    1    0    1    0    1    6/25/1960
1381*/new DateMapping (0x04D6,    1961,     6,    14),/* 0    1    1    0    1    0    1    1    0    0    1    0    6/14/1961
1382*/new DateMapping (0x09B6,    1962,     6,     3),/* 0    1    1    0    1    1    0    1    1    0    0    1    6/3/1962
1383*/new DateMapping (0x0374,    1963,     5,    24),/* 0    0    1    0    1    1    1    0    1    1    0    0    5/24/1963
1384*/new DateMapping (0x0769,    1964,     5,    12),/* 1    0    0    1    0    1    1    0    1    1    1    0    5/12/1964
1385*/new DateMapping (0x0752,    1965,     5,     2),/* 0    1    0    0    1    0    1    0    1    1    1    0    5/2/1965
1386*/new DateMapping (0x06A5,    1966,     4,    21),/* 1    0    1    0    0    1    0    1    0    1    1    0    4/21/1966
1387*/new DateMapping (0x054B,    1967,     4,    10),/* 1    1    0    1    0    0    1    0    1    0    1    0    4/10/1967
1388*/new DateMapping (0x0AAB,    1968,     3,    29),/* 1    1    0    1    0    1    0    1    0    1    0    1    3/29/1968
1389*/new DateMapping (0x055A,    1969,     3,    19),/* 0    1    0    1    1    0    1    0    1    0    1    0    3/19/1969
1390*/new DateMapping (0x0AD5,    1970,     3,     8),/* 1    0    1    0    1    0    1    1    0    1    0    1    3/8/1970
1391*/new DateMapping (0x0DD2,    1971,     2,    26),/* 0    1    0    0    1    0    1    1    1    0    1    1    2/26/1971
1392*/new DateMapping (0x0DA4,    1972,     2,    16),/* 0    0    1    0    0    1    0    1    1    0    1    1    2/16/1972
1393*/new DateMapping (0x0D49,    1973,     2,     4),/* 1    0    0    1    0    0    1    0    1    0    1    1    2/4/1973
1394*/new DateMapping (0x0A95,    1974,     1,    24),/* 1    0    1    0    1    0    0    1    0    1    0    1    1/24/1974
1395*/new DateMapping (0x052D,    1975,     1,    13),/* 1    0    1    1    0    1    0    0    1    0    1    0    1/13/1975
1396*/new DateMapping (0x0A5D,    1976,     1,     2),/* 1    0    1    1    1    0    1    0    0    1    0    1    1/2/1976
1397*/new DateMapping (0x055A,    1976,    12,    22),/* 0    1    0    1    1    0    1    0    1    0    1    0    12/22/1976
1398*/new DateMapping (0x0AD5,    1977,    12,    11),/* 1    0    1    0    1    0    1    1    0    1    0    1    12/11/1977
1399*/new DateMapping (0x06AA,    1978,    12,     1),/* 0    1    0    1    0    1    0    1    0    1    1    0    12/1/1978
1400*/new DateMapping (0x0695,    1979,    11,    20),/* 1    0    1    0    1    0    0    1    0    1    1    0    11/20/1979
1401*/new DateMapping (0x052B,    1980,    11,     8),/* 1    1    0    1    0    1    0    0    1    0    1    0    11/8/1980
1402*/new DateMapping (0x0A57,    1981,    10,    28),/* 1    1    1    0    1    0    1    0    0    1    0    1    10/28/1981
1403*/new DateMapping (0x04AE,    1982,    10,    18),/* 0    1    1    1    0    1    0    1    0    0    1    0    10/18/1982
1404*/new DateMapping (0x0976,    1983,    10,     7),/* 0    1    1    0    1    1    1    0    1    0    0    1    10/7/1983
1405*/new DateMapping (0x056C,    1984,     9,    26),/* 0    0    1    1    0    1    1    0    1    0    1    0    9/26/1984
1406*/new DateMapping (0x0B55,    1985,     9,    15),/* 1    0    1    0    1    0    1    0    1    1    0    1    9/15/1985
1407*/new DateMapping (0x0AAA,    1986,     9,     5),/* 0    1    0    1    0    1    0    1    0    1    0    1    9/5/1986
1408*/new DateMapping (0x0A55,    1987,     8,    25),/* 1    0    1    0    1    0    1    0    0    1    0    1    8/25/1987
1409*/new DateMapping (0x04AD,    1988,     8,    13),/* 1    0    1    1    0    1    0    1    0    0    1    0    8/13/1988
1410*/new DateMapping (0x095D,    1989,     8,     2),/* 1    0    1    1    1    0    1    0    1    0    0    1    8/2/1989
1411*/new DateMapping (0x02DA,    1990,     7,    23),/* 0    1    0    1    1    0    1    1    0    1    0    0    7/23/1990
1412*/new DateMapping (0x05D9,    1991,     7,    12),/* 1    0    0    1    1    0    1    1    1    0    1    0    7/12/1991
1413*/new DateMapping (0x0DB2,    1992,     7,     1),/* 0    1    0    0    1    1    0    1    1    0    1    1    7/1/1992
1414*/new DateMapping (0x0BA4,    1993,     6,    21),/* 0    0    1    0    0    1    0    1    1    1    0    1    6/21/1993
1415*/new DateMapping (0x0B4A,    1994,     6,    10),/* 0    1    0    1    0    0    1    0    1    1    0    1    6/10/1994
1416*/new DateMapping (0x0A55,    1995,     5,    30),/* 1    0    1    0    1    0    1    0    0    1    0    1    5/30/1995
1417*/new DateMapping (0x02B5,    1996,     5,    18),/* 1    0    1    0    1    1    0    1    0    1    0    0    5/18/1996
1418*/new DateMapping (0x0575,    1997,     5,     7),/* 1    0    1    0    1    1    1    0    1    0    1    0    5/7/1997
1419*/new DateMapping (0x0B6A,    1998,     4,    27),/* 0    1    0    1    0    1    1    0    1    1    0    1    4/27/1998
1420*/new DateMapping (0x0BD2,    1999,     4,    17),/* 0    1    0    0    1    0    1    1    1    1    0    1    4/17/1999
1421*/new DateMapping (0x0BC4,    2000,     4,     6),/* 0    0    1    0    0    0    1    1    1    1    0    1    4/6/2000
1422*/new DateMapping (0x0B89,    2001,     3,    26),/* 1    0    0    1    0    0    0    1    1    1    0    1    3/26/2001
1423*/new DateMapping (0x0A95,    2002,     3,    15),/* 1    0    1    0    1    0    0    1    0    1    0    1    3/15/2002
1424*/new DateMapping (0x052D,    2003,     3,     4),/* 1    0    1    1    0    1    0    0    1    0    1    0    3/4/2003
1425*/new DateMapping (0x05AD,    2004,     2,    21),/* 1    0    1    1    0    1    0    1    1    0    1    0    2/21/2004
1426*/new DateMapping (0x0B6A,    2005,     2,    10),/* 0    1    0    1    0    1    1    0    1    1    0    1    2/10/2005
1427*/new DateMapping (0x06D4,    2006,     1,    31),/* 0    0    1    0    1    0    1    1    0    1    1    0    1/31/2006
1428*/new DateMapping (0x0DC9,    2007,     1,    20),/* 1    0    0    1    0    0    1    1    1    0    1    1    1/20/2007
1429*/new DateMapping (0x0D92,    2008,     1,    10),/* 0    1    0    0    1    0    0    1    1    0    1    1    1/10/2008
1430*/new DateMapping (0x0AA6,    2008,    12,    29),/* 0    1    1    0    0    1    0    1    0    1    0    1    12/29/2008
1431*/new DateMapping (0x0956,    2009,    12,    18),/* 0    1    1    0    1    0    1    0    1    0    0    1    12/18/2009
1432*/new DateMapping (0x02AE,    2010,    12,     7),/* 0    1    1    1    0    1    0    1    0    1    0    0    12/7/2010
1433*/new DateMapping (0x056D,    2011,    11,    26),/* 1    0    1    1    0    1    1    0    1    0    1    0    11/26/2011
1434*/new DateMapping (0x036A,    2012,    11,    15),/* 0    1    0    1    0    1    1    0    1    1    0    0    11/15/2012
1435*/new DateMapping (0x0B55,    2013,    11,     4),/* 1    0    1    0    1    0    1    0    1    1    0    1    11/4/2013
1436*/new DateMapping (0x0AAA,    2014,    10,    25),/* 0    1    0    1    0    1    0    1    0    1    0    1    10/25/2014
1437*/new DateMapping (0x094D,    2015,    10,    14),/* 1    0    1    1    0    0    1    0    1    0    0    1    10/14/2015
1438*/new DateMapping (0x049D,    2016,    10,     2),/* 1    0    1    1    1    0    0    1    0    0    1    0    10/2/2016
1439*/new DateMapping (0x095D,    2017,     9,    21),/* 1    0    1    1    1    0    1    0    1    0    0    1    9/21/2017
1440*/new DateMapping (0x02BA,    2018,     9,    11),/* 0    1    0    1    1    1    0    1    0    1    0    0    9/11/2018
1441*/new DateMapping (0x05B5,    2019,     8,    31),/* 1    0    1    0    1    1    0    1    1    0    1    0    8/31/2019
1442*/new DateMapping (0x05AA,    2020,     8,    20),/* 0    1    0    1    0    1    0    1    1    0    1    0    8/20/2020
1443*/new DateMapping (0x0D55,    2021,     8,     9),/* 1    0    1    0    1    0    1    0    1    0    1    1    8/9/2021
1444*/new DateMapping (0x0A9A,    2022,     7,    30),/* 0    1    0    1    1    0    0    1    0    1    0    1    7/30/2022
1445*/new DateMapping (0x092E,    2023,     7,    19),/* 0    1    1    1    0    1    0    0    1    0    0    1    7/19/2023
1446*/new DateMapping (0x026E,    2024,     7,     7),/* 0    1    1    1    0    1    1    0    0    1    0    0    7/7/2024
1447*/new DateMapping (0x055D,    2025,     6,    26),/* 1    0    1    1    1    0    1    0    1    0    1    0    6/26/2025
1448*/new DateMapping (0x0ADA,    2026,     6,    16),/* 0    1    0    1    1    0    1    1    0    1    0    1    6/16/2026
1449*/new DateMapping (0x06D4,    2027,     6,     6),/* 0    0    1    0    1    0    1    1    0    1    1    0    6/6/2027
1450*/new DateMapping (0x06A5,    2028,     5,    25),/* 1    0    1    0    0    1    0    1    0    1    1    0    5/25/2028
1451*/new DateMapping (   0,      2029,     5,    14),/* 0    0    0    0    0    0    0    0    0    0    0    0    5/14/2029
*/    };

        public const int UmAlQuraEra = 1;

        internal const int DateCycle = 30;
        internal const int DatePartYear = 0;
        internal const int DatePartDayOfYear = 1;
        internal const int DatePartMonth = 2;
        internal const int DatePartDay = 3;

        internal static short[] gmonth ={31,31,28,31,30,31,30,31,31,30,31,30,31,31};/* makes it circular m[0]=m[12] & m[13]=m[1] */

        //internal static Calendar m_defaultInstance;


        // This is the minimal Gregorian date that we support in the UmAlQuraCalendar.
        internal static DateTime minDate = new DateTime(1900, 4, 30);
        internal static DateTime maxDate = new DateTime((new DateTime(2029, 5, 13, 23, 59, 59, 999)).Ticks + 9999);

        /*=================================GetDefaultInstance==========================
        **Action: Internal method to provide a default intance of UmAlQuraCalendar.  Used by NLS+ implementation
        **       and other calendars.
        **Returns:
        **Arguments:
        **Exceptions:
        ============================================================================*/
        /*
        internal static Calendar GetDefaultInstance() {
            if (m_defaultInstance == null) {
                m_defaultInstance = new UmAlQuraCalendar();
            }
            return (m_defaultInstance);
        }
        */



        public override DateTime MinSupportedDateTime
        {
            get
            {
                return (minDate);
            }
        }


        public override DateTime MaxSupportedDateTime
        {
            get
            {
                return (maxDate);
            }
        }


        // Return the type of the UmAlQura calendar.
        //


        public override CalendarAlgorithmType AlgorithmType {
            get {
                return CalendarAlgorithmType.LunarCalendar;
            }
        }

        // Construct an instance of UmAlQura calendar.

        public UmAlQuraCalendar() {
        }

        internal override int BaseCalendarID {
            get {
                return (CAL_HIJRI);
            }
        }

        internal override int ID {
            get {
                return (CAL_UMALQURA);
            }
        }
        /*==========================ConvertHijriToGregorian==========================
        ** Purpose: convert Hdate(year,month,day) to Gdate(year,month,day)                               
        ** Arguments:                                                                                                                
        ** Input/Ouput: Hijrah  date: year:yh, month:mh, day:dh                                                  
        ** Output: Gregorian date: year:yg, month:mg, day:dg , day of week:dayweek                   
        **       and returns flag found:1 not found:0                                                                      
        =========================ConvertHijriToGregorian============================*/
        void ConvertHijriToGregorian(int HijriYear, int HijriMonth, int HijriDay, ref int yg, ref int mg, ref int dg)
        {
            BCLDebug.Assert( (HijriYear >= MinCalendarYear) && (HijriYear <= MaxCalendarYear), "Hijri year is out of range.");
            BCLDebug.Assert( HijriMonth >= 1, "Hijri month is out of range.");
            BCLDebug.Assert( HijriDay >= 1, "Hijri day is out of range.");
            int index, b, nDays = HijriDay-1;
            DateTime dt;
			

            index = HijriYear - MinCalendarYear;
            dt = HijriYearInfo[index].GregorianDate;


            b = HijriYearInfo[index].HijriMonthsLengthFlags;

            for(int m = 1; m < HijriMonth; m++)
            {
                nDays = nDays+ 29 + (b&1);   /* Add the months lengths before mh */
                b = b >> 1;
            }

            dt = dt.AddDays(nDays);
            yg = dt.Year;
            mg = dt.Month;
            dg = dt.Day;
        }

        /*=================================GetAbsoluteDateUmAlQura==========================
        **Action: Gets the Absolute date for the given UmAlQura date.  The absolute date means
        **       the number of days from January 1st, 1 A.D.
        **Returns:
        **Arguments:
        **Exceptions:
        ============================================================================*/
        long GetAbsoluteDateUmAlQura(int year, int month, int day) {
            //Caller should check the validaty of year, month and day.

            int yg=0,mg=0,dg=0;
            ConvertHijriToGregorian(year, month, day, ref yg, ref mg, ref dg);
            return GregorianCalendar.GetAbsoluteDate(yg,mg,dg);
        }

        internal void CheckTicksRange(long ticks)  {
            if (ticks < minDate.Ticks || ticks > maxDate.Ticks)  {
                throw new ArgumentOutOfRangeException(
                            "time",
                            String.Format(
                                CultureInfo.InvariantCulture,
                                Environment.GetResourceString("ArgumentOutOfRange_CalendarRange"),
                                minDate,
                                maxDate));
            }
        }

        internal void CheckEraRange(int era) {
            if (era != CurrentEra && era != UmAlQuraEra) {
                throw new ArgumentOutOfRangeException("era", Environment.GetResourceString("ArgumentOutOfRange_InvalidEraValue"));
            }
        }

        internal void CheckYearRange(int year, int era)  {
            CheckEraRange(era);
            if (year < MinCalendarYear || year > MaxCalendarYear)  {
                throw new ArgumentOutOfRangeException(
                            "year",
                            String.Format(
                                CultureInfo.CurrentCulture,
                                Environment.GetResourceString("ArgumentOutOfRange_Range"),
                                MinCalendarYear,
                                MaxCalendarYear));
            }
        }

        internal void CheckYearMonthRange(int year, int month, int era)  {
            CheckYearRange(year, era);
            if (month < 1 || month > 12)  {
                throw new ArgumentOutOfRangeException("month", Environment.GetResourceString("ArgumentOutOfRange_Month"));
            }
        }

        /*========================ConvertGregorianToHijri============================
        ** Purpose: convert DateTime to Hdate(year,month,day)                                                   
        ** Arguments:                                                                                                                
        ** Input: DateTime                                                                                                         
        ** Output: Hijrah  date: year:yh, month:mh, day:dh                                                          
        ============================================================================*/
        void ConvertGregorianToHijri(DateTime time, ref int HijriYear, ref int HijriMonth, ref int HijriDay)
        {

            int index, b, DaysPerThisMonth;
            double nDays;
            TimeSpan ts;
            int yh1=0, mh1=0, dh1=0;

            BCLDebug.Assert((time.Ticks >= minDate.Ticks) && (time.Ticks <= maxDate.Ticks), "Gregorian date is out of range.");

            // Find the index where we should start our search by quessing the Hijri year that we will be in HijriYearInfo.
            // A Hijri year is 354 or 355 days.  Use 355 days so that we will search from a lower index.
            
            index = (int)((time.Ticks - minDate.Ticks) / Calendar.TicksPerDay) / 355;
            do 
            {
            } while (time.CompareTo(HijriYearInfo[++index].GregorianDate)>0); //while greater

            if (time.CompareTo(HijriYearInfo[index].GregorianDate) != 0)
            {
                index--;
            }

            ts = time.Subtract(HijriYearInfo[index].GregorianDate);
            yh1 = index + MinCalendarYear;
            
            mh1 = 1;
            dh1 = 1;
            nDays = ts.TotalDays;
            b = HijriYearInfo[index].HijriMonthsLengthFlags;
            DaysPerThisMonth = 29 + (b&1);

            while (nDays >= DaysPerThisMonth)
            {
                nDays -= DaysPerThisMonth;
                b = b >> 1;
                DaysPerThisMonth = 29 + (b&1);
                mh1++;
            }
            dh1 += (int)nDays;

            HijriDay = dh1;
            HijriMonth = mh1;
            HijriYear = yh1;
        }

        /*=================================GetDatePart==========================
        **Action: Returns a given date part of this <i>DateTime</i>. This method is used
        **       to compute the year, day-of-year, month, or day part.
        **Returns:
        **Arguments:
        **Exceptions:  ArgumentException if part is incorrect.
        **Notes:
        **      First, we get the absolute date (the number of days from January 1st, 1 A.C) for the given ticks.
        **      Use the formula (((AbsoluteDate - 226894) * 33) / (33 * 365 + 8)) + 1, we can a rough value for the UmAlQura year.
        **      In order to get the exact UmAlQura year, we compare the exact absolute date for UmAlQuraYear and (UmAlQuraYear + 1).
        **      From here, we can get the correct UmAlQura year.
        ============================================================================*/

        internal virtual int GetDatePart(DateTime time, int part) {
            int UmAlQuraYear=0;              // UmAlQura year
            int UmAlQuraMonth=0;             // UmAlQura month
            int UmAlQuraDay=0;               // UmAlQura day
            long ticks = time.Ticks;
            CheckTicksRange(ticks);

            ConvertGregorianToHijri(time, ref UmAlQuraYear, ref UmAlQuraMonth, ref UmAlQuraDay);

            if (part == DatePartYear)
                return (UmAlQuraYear);

            if (part == DatePartMonth)
                return (UmAlQuraMonth);

            if (part == DatePartDay)
                return (UmAlQuraDay);

            if (part == DatePartDayOfYear)
                return (int)(GetAbsoluteDateUmAlQura(UmAlQuraYear, UmAlQuraMonth, UmAlQuraDay) - GetAbsoluteDateUmAlQura(UmAlQuraYear, 1, 1) + 1);

            // Incorrect part value.
            throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_DateTimeParsing"));
        }

        // Returns the DateTime resulting from adding the given number of
        // months to the specified DateTime. The result is computed by incrementing
        // (or decrementing) the year and month parts of the specified DateTime by
        // value months, and, if required, adjusting the day part of the
        // resulting date downwards to the last day of the resulting month in the
        // resulting year. The time-of-day part of the result is the same as the
        // time-of-day part of the specified DateTime.
        //
        // In more precise terms, considering the specified DateTime to be of the
        // form y / m / d + t, where y is the
        // year, m is the month, d is the day, and t is the
        // time-of-day, the result is y1 / m1 / d1 + t,
        // where y1 and m1 are computed by adding value months
        // to y and m, and d1 is the largest value less than
        // or equal to d that denotes a valid day in month m1 of year
        // y1.
        //


        public override DateTime AddMonths(DateTime time, int months)  {
            if (months < -120000 || months > 120000)  {
                throw new ArgumentOutOfRangeException(
                            "months",
                            String.Format(
                                CultureInfo.CurrentCulture,
                                Environment.GetResourceString("ArgumentOutOfRange_Range"),
                                -120000,
                                120000));
            }
            // Get the date in UmAlQura calendar.
            int y = GetDatePart(time, DatePartYear);
            int m = GetDatePart(time, DatePartMonth);
            int d = GetDatePart(time, DatePartDay);
            int i = m - 1 + months;

            if (i >= 0)  {
                m = i % 12 + 1;
                y = y + i / 12;
            }  else  {
                m = 12 + (i + 1) % 12;
                y = y + (i - 11) / 12;
            }

            if (d>29)
            {
                int days = GetDaysInMonth(y, m);
                if (d > days) {
                    d = days;
                }
            }
	     CheckYearRange(y, UmAlQuraEra);			
            DateTime dt = new DateTime(GetAbsoluteDateUmAlQura(y, m, d) * TicksPerDay + time.Ticks % TicksPerDay);
            Calendar.CheckAddResult(dt.Ticks, MinSupportedDateTime, MaxSupportedDateTime);
            return (dt);
        }

        // Returns the DateTime resulting from adding the given number of
        // years to the specified DateTime. The result is computed by incrementing
        // (or decrementing) the year part of the specified DateTime by value
        // years. If the month and day of the specified DateTime is 2/29, and if the
        // resulting year is not a leap year, the month and day of the resulting
        // DateTime becomes 2/28. Otherwise, the month, day, and time-of-day
        // parts of the result are the same as those of the specified DateTime.
        //


        public override DateTime AddYears(DateTime time, int years) {
            return (AddMonths(time, years * 12));
        }

        // Returns the day-of-month part of the specified DateTime. The returned
        // value is an integer between 1 and 31.
        //


        public override int GetDayOfMonth(DateTime time) {
            return (GetDatePart(time, DatePartDay));
        }

        // Returns the day-of-week part of the specified DateTime. The returned value
        // is an integer between 0 and 6, where 0 indicates Sunday, 1 indicates
        // Monday, 2 indicates Tuesday, 3 indicates Wednesday, 4 indicates
        // Thursday, 5 indicates Friday, and 6 indicates Saturday.
        //


        public override DayOfWeek GetDayOfWeek(DateTime time) {
            return ((DayOfWeek)((int)(time.Ticks / TicksPerDay + 1) % 7));
        }

        // Returns the day-of-year part of the specified DateTime. The returned value
        // is an integer between 1 and 354 or 355.
        //


        public override int GetDayOfYear(DateTime time) {
            return (GetDatePart(time, DatePartDayOfYear));
        }

        /*
        internal bool CouldBeLeapYear(int year)
        {
            return ((((year * 11) + 14) % 30) < 11);
        }
        */
        
        // Returns the number of days in the month given by the year and
        // month arguments.
        //


        public override int GetDaysInMonth(int year, int month, int era) {
            CheckYearMonthRange(year, month, era);

            if ((HijriYearInfo[year-MinCalendarYear].HijriMonthsLengthFlags & (1<<month-1))==0)
                return 29;
            else
                return 30;
        }

        internal int RealGetDaysInYear(int year)
        {
            int days = 0, b;

            BCLDebug.Assert( (year >= MinCalendarYear) && (year <= MaxCalendarYear), "Hijri year is out of range.");

            b = HijriYearInfo[year-MinCalendarYear].HijriMonthsLengthFlags;

            for(int m = 1; m <= 12; m++)
            {
                days = days+ 29 + (b&1);   /* Add the months lengths before mh */
                b = b >> 1;
            }
            BCLDebug.Assert((days == 354)||(days == 355), "Hijri year has to be 354 or 355 days.");
            return days;
        }

        // Returns the number of days in the year given by the year argument for the current era.
        //


        public override int GetDaysInYear(int year, int era)
        {
            CheckYearRange(year, era);
            return (RealGetDaysInYear(year));
        }

        // Returns the era for the specified DateTime value.


        public override int GetEra(DateTime time) {
            CheckTicksRange(time.Ticks);
            return (UmAlQuraEra);
        }



        public override int[] Eras {
            get {
                return (new int[] {UmAlQuraEra});
            }
        }

        // Returns the month part of the specified DateTime. The returned value is an
        // integer between 1 and 12.
        //


        public override int GetMonth(DateTime time) {
            return (GetDatePart(time, DatePartMonth));
        }

        // Returns the number of months in the specified year and era.


        public override int GetMonthsInYear(int year, int era) {
            CheckYearRange(year, era);
            return (12);
        }

        // Returns the year part of the specified DateTime. The returned value is an
        // integer between MinCalendarYear and MaxCalendarYear.
        //


        public override int GetYear(DateTime time) {
            return (GetDatePart(time, DatePartYear));
        }

        // Checks whether a given day in the specified era is a leap day. This method returns true if
        // the date is a leap day, or false if not.
        //


        public override bool IsLeapDay(int year, int month, int day, int era) {
            if (day>=1 && day <=29)
            {
                CheckYearMonthRange(year, month, era);
                return (false);
            }

            // The year/month/era value checking is done in GetDaysInMonth().
            int daysInMonth = GetDaysInMonth(year, month, era);
            if (day < 1 || day > daysInMonth) {
                throw new ArgumentOutOfRangeException(
                            "day",
                            String.Format(
                                CultureInfo.CurrentCulture,
                                Environment.GetResourceString("ArgumentOutOfRange_Day"),
                                daysInMonth,
                                month));
            }
            return (false);
        }

        // Returns  the leap month in a calendar year of the specified era. This method returns 0
        // if this calendar does not have leap month, or this year is not a leap year.
        //


        public override int GetLeapMonth(int year, int era)
        {
            CheckYearRange(year, era);
            return (0);
        }

        // Checks whether a given month in the specified era is a leap month. This method returns true if
        // month is a leap month, or false if not.
        //


        public override bool IsLeapMonth(int year, int month, int era) {
            CheckYearMonthRange(year, month, era);
            return (false);
        }

        // Checks whether a given year in the specified era is a leap year. This method returns true if
        // year is a leap year, or false if not.
        //


        public override bool IsLeapYear(int year, int era)
        {
            CheckYearRange(year, era);
            if (RealGetDaysInYear(year) == 355)
                return true;
            else
                return false;
        }

        // Returns the date and time converted to a DateTime value.  Throws an exception if the n-tuple is invalid.
        //


        public override DateTime ToDateTime(int year, int month, int day, int hour, int minute, int second, int millisecond, int era) {
            if (day >= 1 && day <= 29)
            {
                CheckYearMonthRange(year, month, era);
                goto DayInRang;
            }

            // The year/month/era value checking is done in GetDaysInMonth().
             int daysInMonth = GetDaysInMonth(year, month, era);

             if (day < 1 || day > daysInMonth) {
                 BCLDebug.Log("year = " + year + ", month = " + month + ", day = " + day);
                 throw new ArgumentOutOfRangeException(
                            "day",
                            String.Format(
                                CultureInfo.CurrentCulture,
                                Environment.GetResourceString("ArgumentOutOfRange_Day"),
                                daysInMonth,
                                month));
             }
DayInRang:
            long lDate = GetAbsoluteDateUmAlQura(year, month, day);

            if (lDate >= 0) {
                return (new DateTime(lDate * GregorianCalendar.TicksPerDay + TimeToTicks(hour, minute, second, millisecond)));
            } else {
                throw new ArgumentOutOfRangeException(null, Environment.GetResourceString("ArgumentOutOfRange_BadYearMonthDay"));
            }
        }

        private const int DEFAULT_TWO_DIGIT_YEAR_MAX = 1451;



        public override int TwoDigitYearMax {
            get {
                if (twoDigitYearMax == -1) {
                    twoDigitYearMax = GetSystemTwoDigitYearSetting(ID, DEFAULT_TWO_DIGIT_YEAR_MAX);
                }
                return (twoDigitYearMax);
            }

            set {
                VerifyWritable();
                if (value != 99 && (value < MinCalendarYear || value > MaxCalendarYear)) {
                    throw new ArgumentOutOfRangeException(
                                "value",
                                String.Format(
                                    CultureInfo.CurrentCulture,
                                    Environment.GetResourceString("ArgumentOutOfRange_Range"),
                                    MinCalendarYear,
                                    MaxCalendarYear));
                }
                // We allow year 99 to be set so that one can make ToFourDigitYearMax a no-op by setting TwoDigitYearMax to 99.
                twoDigitYearMax = value;
            }
        }



        public override int ToFourDigitYear(int year) {
            if (year < 100) {
                return (base.ToFourDigitYear(year));
            }

            if ((year < MinCalendarYear) || (year > MaxCalendarYear)) {
                throw new ArgumentOutOfRangeException(
                            "year",
                            String.Format(
                                    CultureInfo.CurrentCulture,
                                    Environment.GetResourceString("ArgumentOutOfRange_Range"),
                                    MinCalendarYear,
                                    MaxCalendarYear));
            }
            return (year);
        }
    }
}

