/**************************** Module Header ******************************\
Module Name:  <Person.cs>
Project:      <CSASPNETMVCDataView>
Copyright (c) Microsoft Corporation.

The file is the Model of person module.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/


using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.ComponentModel.DataAnnotations;

namespace CSASPNETMVCDataView.Models
{
    public class Person
    {
        [Required(ErrorMessage = "The ID is required.")]
        public int Id { get; set; }

        [Required(ErrorMessage = "The name is required.")]
        public string Name { get; set; }

        [Range(1, 200, ErrorMessage = "A number between 1 and 200.")]
        public int Age { get; set; }

        [RegularExpression(@"(^189\d{8}$)|(^13\d{9}$)|(^15\d{9}$)", 
            ErrorMessage = "Invalid phone number. 11 length and start with 13,15,189 prefix. e.g. 13800138000")]
        public string Phone { get; set; }

        [RegularExpression(@"^[\w-\.]+@([\w-]+\.)+[\w-]{2,4}$", 
            ErrorMessage = "Invalid email address.")]
        public string Email { get; set; }
    }
}
