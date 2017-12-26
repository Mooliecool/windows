================================================================================
       WINDOWS FORMS APPLICATION : CSWinFormBindToNestedProp Project Overview
       
                       BindToNestedProperty Sample
                        
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The sample demonstrates how to bind a DataGridView column to a nested property 
in the data source.
   

/////////////////////////////////////////////////////////////////////////////
Code Logic:


1. Derive a class from the PropertyDescriptor class to create a 
   PropertyDescriptor for a sub-property.

2. Derive a class from the CustomTypeDescriptor class to add extra 
   PropertyDescriptor to the original PropertyDescriptorCollection of the 
   type, using the derived PropertyDescriptor class.

3. Derive a class from the TypeDescriptionProvider class to use the derived 
   CustomTypeDescriptor class.

4. Add a TypeDescriptionProviderAttribute on the type that contains complex 
   type properties.


/////////////////////////////////////////////////////////////////////////////
References:

ICustomTypeDescriptor, Part 2
http://msdn.microsoft.com/en-us/magazine/cc163804.aspx


/////////////////////////////////////////////////////////////////////////////
