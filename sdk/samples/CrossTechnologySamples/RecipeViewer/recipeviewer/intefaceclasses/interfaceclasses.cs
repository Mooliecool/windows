// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
using System;
using System.Collections;
using System.Collections.Generic;
using System.Xml;
using System.IO;

namespace Microsoft.Samples.RecipeCatalog
{
    public enum RecipeAttachmentType
    {
        Unknown,
        Photo,
        Video
    }

    public interface IRecipeAttachment
    {

        RecipeAttachmentType Type
        {
            get;
        }

        String Name
        {
            get;
        }

        String Key
        {
            get;
        }

        Stream AttachmentStream
        {
            get;
        }
    }

    public interface IRecipeAttachmentCollection : IEnumerable<IRecipeAttachment>
    {
        int Length
        {
            get;
        }

        IRecipeAttachment this[String key]
        {
            get;
        }
    }

    public interface IRecipe
    {
        String Title
        {
            get;
        }

        int Rating
        {
            get;
        }

        String[] Keywords
        {
            get;
        }

        XmlDocument RecipeXml
        {
            get;
        }

        string Key
        {
            get;
        }

        IRecipeAttachmentCollection Attachments
        {
            get;
        }
    }

    public interface IRecipeCollection : IEnumerator<IRecipe>, IEnumerable
    {
        int Length
        {
            get;
        }

        IRecipe this[String key]
        {
            get;

        }

        string AddRecipe(XmlReader recipeDoc);
        IRecipeCollection Find(string[] keyword);
    }
}
