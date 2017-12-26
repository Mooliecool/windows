//------------------------------------------------------------------------------
// <copyright file="ImageListUtils.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>
//------------------------------------------------------------------------------

/*
 */
namespace System.Windows.Forms {
    using System;
    using System.ComponentModel;
    using System.Diagnostics;
    
    // Miscellaneous utilities
    static internal class ImageListUtils {

        public static PropertyDescriptor GetImageListProperty(PropertyDescriptor currentComponent, ref object instance) {
            if(instance is object[]) //multiple selection is not supported by this class
                return null;
            
            PropertyDescriptor imageListProp = null;
            object parentInstance = instance;
            
            RelatedImageListAttribute relILAttr = currentComponent.Attributes[typeof(RelatedImageListAttribute)] as RelatedImageListAttribute;
            if (relILAttr != null) 
            {
                string[] pathInfo = relILAttr.RelatedImageList.Split('.');
                for(int i=0;i<pathInfo.Length;i++) {
                    if(parentInstance == null) {
                        Debug.Fail("A property specified in the path is null or not yet instanciated at this time");
                        break;
                    }
                    PropertyDescriptor prop = TypeDescriptor.GetProperties(parentInstance)[pathInfo[i]];
                    if(prop == null) {
                        Debug.Fail("The path specified to the property is wrong");
                        break;
                    }
                    if(i==pathInfo.Length-1) {
                        // we're on the last one, look if that's our guy
                        if (typeof(ImageList).IsAssignableFrom(prop.PropertyType)) {
                            instance = parentInstance;
                            imageListProp = prop;
                            break;
                        }
                    } else {
                        parentInstance = prop.GetValue(parentInstance);                                    
                    }
                }
            }
    
            return imageListProp;
        }
    }
}

