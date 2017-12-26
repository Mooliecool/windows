/***************************************************************************
    Copyright (c) Microsoft Corporation. All rights reserved.
    This code is licensed under the Visual Studio SDK license terms.

    THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
    ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
    IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
    PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.

***************************************************************************/

using System;
using System.Collections.Generic;
using System.Text;
using System.Collections.ObjectModel;
using Microsoft.VisualStudio.Modeling.Validation;

namespace Fabrikam.Dsl.TestValidation
{
    [ValidationState(ValidationState.Enabled)]
    public partial class Person
    {
        [ValidationMethod(ValidationCategories.Open |
                           ValidationCategories.Save |
                           ValidationCategories.Menu)]
        public void ValidateDates(ValidationContext context)
        {
            if (context == null)
                throw new global::System.ArgumentNullException("context");

            if (this.Birth > this.Death)
            {
                context.LogError("Birth must be before death","FamilyPersonDateError", this);
            }
        }

        [ValidationMethod(ValidationCategories.Open |
                    ValidationCategories.Save |
                    ValidationCategories.Menu)]
        public void ValidateParentBirth(ValidationContext context)
        {
            foreach (Person parent in this.Parent)
            {
                if (this.Birth <= parent.Birth)
                {
                    context.LogError("Birth must be after Parent's birth","FamilyParentBirthError", this, parent);
                }
            }
        }

        [ValidationMethod(ValidationCategories.Open |
            ValidationCategories.Save |
            ValidationCategories.Menu)]
        public void ValidateParentCount(ValidationContext context)
        {
            if (this.Parent.Count > 2)
            {
                context.LogError("A person can't have more than two parents","FamilyParentCountError", this);
            }
        }
    }

    [ValidationState(ValidationState.Enabled)]
    public partial class ParentRelation
    {

        [ValidationMethod(ValidationCategories.Open |
                            ValidationCategories.Save |
                            ValidationCategories.Menu)]
        public void ValidateParentDeath(ValidationContext context)
        {
            if (context == null)
                throw new global::System.ArgumentNullException("context");
            if (this.Parent.Death + 1 < this.Child.Birth)
            {
                context.LogError
                    ("Child must not be born later than a year after parent's death",
                    "FamilyParentRelationDeathDateError", this);
            }
        }

    }
}
