/****************************** Module Header ******************************\
* Module Name:  MethodNamingRule.cs
* Project:	    CSCustomCodeAnalysisRule
* Copyright (c) Microsoft Corporation.
* 
* The class MethodNamingRule inherits the class Microsoft.FxCop.Sdk.BaseIntrospectionRule
* and override the method 
*     public ProblemCollection Check(Member member).
* 
* This rule is use to check whether a method name starts with a uppercase character. 
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
    sealed class MethodNamingRule : BaseIntrospectionRule
    {
        /// <summary>
        /// Define the rule name, resource file and resource assembly.
        /// </summary>
        public MethodNamingRule()
            : base(
                "MethodNamingRule",
                "CSCustomCodeAnalysisRule.Rules",
                typeof(MethodNamingRule).Assembly)
        {

        }

        /// <summary>
        /// Check the name of the member if it is a method.
        /// If the method is not a constructor or an accessor, its name should 
        /// start with a uppercase character.
        /// </summary>
        public override ProblemCollection Check(Member member)
        {         
            if (member is Method
                && !(member is InstanceInitializer)
                && !(member is StaticInitializer))
            {
                Method method = member as Method;
                if (!method.IsAccessor
                    && (method.Name.Name[0] < 'A' || method.Name.Name[0] > 'Z'))
                {
                    this.Problems.Add(new Problem(
                       this.GetNamedResolution(
                       "UppercaseMethod",
                       method.Name.Name,
                       method.DeclaringType.FullName)));
                }
            }
            return this.Problems;
        }


    }
}
