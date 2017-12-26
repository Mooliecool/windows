/****************************** Module Header ******************************\
* Module Name:  PropertyNamingRule.cs
* Project:	    CSCustomCodeAnalysisRule
* Copyright (c) Microsoft Corporation.
* 
* The class PropertyNamingRule inherits the class Microsoft.FxCop.Sdk.BaseIntrospectionRule
* and override the method 
*     public ProblemCollection Check(Member member).
* 
* This rule is use to check whether a property name starts with a uppercase character. 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using Microsoft.FxCop.Sdk;

namespace CSCustomCodeAnalysisRule
{
    sealed class PropertyNamingRule : BaseIntrospectionRule
    {
        /// <summary>
        /// Define the rule name, resource file and resource assembly.
        /// </summary>
        public PropertyNamingRule()
            : base(
                "PropertyNamingRule",
                "CSCustomCodeAnalysisRule.Rules",
                typeof(FieldNamingRule).Assembly)
        {

        }

        /// <summary>
        /// Check the name of the member if it is a method.
        /// The name of a property should start with a uppercase character.
        /// </summary>
        public override ProblemCollection Check(Member member)
        {         
            if (member is PropertyNode)
            {
                PropertyNode property = member as PropertyNode;

                if (property.Name.Name[0] < 'A' || property.Name.Name[0] > 'Z')
                {
                    this.Problems.Add(new Problem(
                       this.GetNamedResolution(
                       "UppercaseProperty", 
                       property.Name.Name,
                       property.DeclaringType.FullName)));
                }
            }

            return this.Problems;
        }


    }
}
