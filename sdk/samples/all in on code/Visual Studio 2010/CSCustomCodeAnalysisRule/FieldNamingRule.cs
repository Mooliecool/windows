/****************************** Module Header ******************************\
* Module Name:  FieldNamingRule.cs
* Project:	    CSCustomCodeAnalysisRule
* Copyright (c) Microsoft Corporation.
* 
* The class FieldNamingRule inherits the class Microsoft.FxCop.Sdk.BaseIntrospectionRule
* and override the method 
*     public ProblemCollection Check(Member member).
* 
* This rule is use to check whether a field name starts with a lowercase character. 
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
    sealed class FieldNamingRule : BaseIntrospectionRule
    {
        /// <summary>
        /// Define the rule name, resource file and resource assembly.
        /// </summary>
        public FieldNamingRule()
            : base(
                "FieldNamingRule",
                "CSCustomCodeAnalysisRule.Rules",
                typeof(FieldNamingRule).Assembly)
        {

        }

        /// <summary>
        /// Check the name of the member if it is a field.
        /// If the field is not an event or an static menber, its name should 
        /// start with a lowercase character.
        /// </summary>
        public override ProblemCollection Check(Member member)
        {
            if (member is Field)
            {
                Field field = member as Field;

                if (!(field.Type is DelegateNode)
                    && !field.IsStatic)
                {
                    if (field.Name.Name[0] < 'a' || field.Name.Name[0] > 'z')
                    {
                        this.Problems.Add(new Problem(
                           this.GetNamedResolution(
                           "LowercaseField", 
                           field.Name.Name, 
                           field.DeclaringType.FullName)));
                    }
                }

            }

            return this.Problems;
        }

    }
}
