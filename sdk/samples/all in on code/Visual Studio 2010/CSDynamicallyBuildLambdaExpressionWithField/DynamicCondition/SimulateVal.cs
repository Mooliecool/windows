/********************************** Module Header **********************************\
* Module Name:  SimulateVal.cs
* Project:      CSDynamicallyBuildLambdaExpressionWithField
* Copyright (c) Microsoft Corporation.
* 
* The SimulateVal.cs file defines variant function which is deal with DateTime and Boolean.
*
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
* EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
* MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***********************************************************************************/

public static class SimulateVal
{
    public static double Val(string expression)
    {
        if (expression == null)
        {
            return 0;
        }
        // Try the entire string, then progressively smaller
        // substrings to simulate the behavior of VB's 'Val',
        // which ignores trailing characters after a recognizable value:
        for (int size = expression.Length; size > 0; size--)
        {
            double testDouble;
            if (double.TryParse(expression.Substring(0, size), out testDouble))
                return testDouble;
        }

        // No value is recognized, so return 0:
        return 0;
    }

    public static double Val(object expression)
    {
        if (expression == null)
        {
            return 0;
        }

        double testDouble;
        if (double.TryParse(expression.ToString(), out testDouble))
        {
            return testDouble;
        }
        // CSharp's 'Val' function returns -1 for 'true':
        bool testBool;

        if (bool.TryParse(expression.ToString(), out testBool))
        {
            return testBool ? -1 : 0;
        }
        // CSharp's 'Val' function returns the day of the month for dates:
        System.DateTime testDate;
        if (System.DateTime.TryParse(expression.ToString(), out testDate))
        {
            return testDate.Day;
        }
        // No value is recognized, so return 0:
        return 0;

    }

    /// <summary>
    /// Convert char into string
    /// </summary>
    public static int Val(char expression)
    {
        int testInt;
        if (int.TryParse(expression.ToString(), out testInt))
        {
            return testInt;
        }
        else
        {
            return 0;
        }
    }
}