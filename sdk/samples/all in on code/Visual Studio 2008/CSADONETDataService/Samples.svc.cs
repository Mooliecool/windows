/****************************** Module Header ******************************\
* Module Name:	Samples.svc.cs
* Project:		CSADONETDataService
* Copyright (c) Microsoft Corporation.
* 
* Samples.svc demonstrates the ADO.NET Data Service for non-relational data
* source.  The non-relational data source is some in-memory objects which
* hold the All-In-One Code Framework sample projects information. The 
* non-relational entity class also implements the IUpdatable interface to let 
* the client insert new data.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#region Using directive
using System;
using System.Collections.Generic;
using System.Data.Services;
using System.Linq;
using System.ServiceModel.Web;
using System.Web;
using System.Data.Services.Common;
using System.Reflection;
using System.Collections;
#endregion


namespace CSADONETDataService
{
    public class Samples : DataService<SampleProjects>
    {
        // This method is called only once to initialize service-wide policies.
        public static void InitializeService(IDataServiceConfiguration config)
        {
            // Set rules to indicate which entity sets and service operations 
            // are visible, updatable, etc.
            config.UseVerboseErrors = true;
            config.SetEntitySetAccessRule("*", EntitySetRights.All);
        }
    }

    #region Non-relational data entity classes
    // Sample project entity class with DataServiceKey 'ProjectName'
    [DataServiceKey("ProjectName")]
    public class Project
    {
        public string ProjectName { get; set; }

        public string Owner { get; set; }

        public Category ProjectCategory { get; set; }
    }

    // Sample project category entity class with DataServiceKey 'CategoryName'
    [DataServiceKey("CategoryName")]
    public class Category
    {
        public string CategoryName { get; set; }
    }

    // Sample data entity class
    public class SampleProjects : IUpdatable
    {
        static List<Category> categories;
        static List<Project> projects;

        // Static constructor
        static SampleProjects()
        {
            // Initialize the sample project category list
            categories = new List<Category>()
            {
                new Category { CategoryName = "COM"},
                new Category { CategoryName = "Data Access"},
                new Category { CategoryName = "Office"},
                new Category { CategoryName = "IPC and RPC"},
                new Category { CategoryName = "WinForm"},
                new Category { CategoryName = "Hook"}
            };

            // Initialize the sample project list
            projects = new List<Project>()
            {
                new Project { ProjectName = "CSDllCOMServer", 
                    Owner = "Jialiang Ge", ProjectCategory = categories[0] },
                new Project { ProjectName = "VBDllCOMServer", 
                    Owner = "Jialiang Ge", ProjectCategory = categories[0] },
                new Project { ProjectName = "ATLDllCOMServer", 
                    Owner = "Jialiang Ge", ProjectCategory = categories[0] },
                new Project { ProjectName = "CSUseADONET", 
                    Owner = "Lingzhi Sun", ProjectCategory = categories[1] },
                new Project { ProjectName = "CppUseADONET", 
                    Owner = "Jialiang Ge", ProjectCategory = categories[1] },
                new Project { ProjectName = "CSLinqToObject", 
                    Owner = "Colbert Zhou", ProjectCategory = categories[1] },
                new Project { ProjectName = "CSLinqToSQL", 
                    Owner = "Rongchun Zhang", ProjectCategory = categories[1] },
                new Project { ProjectName = "CSOutlookUIDesigner", 
                    Owner = "Jie Wang", ProjectCategory = categories[2] },
                new Project { ProjectName = "CSOutlookRibbonXml", 
                    Owner = "Jie Wang", ProjectCategory = categories[2] },
                new Project { ProjectName = "CSAutomateExcel", 
                    Owner = "Jialiang Ge", ProjectCategory = categories[2] },
                new Project { ProjectName = "VBAutomateExcel", 
                    Owner = "Jialiang Ge", ProjectCategory = categories[2] },
                new Project { ProjectName = "CppFileMappingServer", 
                    Owner = "Hongye Sun", ProjectCategory = categories[3] },
                new Project { ProjectName = "CppFileMappingClient", 
                    Owner = "Hongye Sun", ProjectCategory = categories[3] },
                new Project { ProjectName = "CSReceiveWM_COPYDATA", 
                    Owner = "Riquel Dong", ProjectCategory = categories[3] },
                new Project { ProjectName = "CSSendWM_COPYDATA", 
                    Owner = "Riquel Dong", ProjectCategory = categories[3] },
                new Project { ProjectName = "CSWinFormGeneral", 
                    Owner = "Zhixin Ye", ProjectCategory = categories[4] },
                new Project { ProjectName = "CSWinFormDataBinding", 
                    Owner = "Zhixin Ye", ProjectCategory = categories[4] },
                new Project { ProjectName = "CSWindowsHook", 
                    Owner = "Rongchun Zhang", ProjectCategory = categories[5] }
            };
        }

        // Public property to get the sample projects information from the 
        // ADO.NET Data Service client side
        public IQueryable<Project> Projects
        {
            get { return projects.AsQueryable(); }
        }

        // Public property to get the sample projects categoryies information 
        // from the ADO.NET Data Service client side
        public IQueryable<Category> Categories
        {
            get { return categories.AsQueryable(); }
        }

        // Implement the IUpdatable methods to enable the insert function
        #region IUpdatable Members

        // Save the added object temporarily
        object tempObj = null;

        /// <summary>
        /// Adds the given value to the collection
        /// </summary>
        /// <param name="targetResource">target object which defines the 
        /// property</param>
        /// <param name="propertyName">name of the property whose value needs
        /// to be updated</param>
        /// <param name="resourceToBeAdded">value of the property which needs
        /// to be added</param>
        public void AddReferenceToCollection(object targetResource, string 
            propertyName, object resourceToBeAdded)
        {
            // Get the target object type
            Type t = targetResource.GetType();

            // Get the property to be updated
            PropertyInfo pi = t.GetProperty(propertyName);
            if (pi != null)
            {
                // Retrieve the collection property value
                IList collection = (IList)pi.GetValue(targetResource, null);

                // Add the resource into the collection
                collection.Add(resourceToBeAdded);
            }
        }

        /// <summary>
        /// Revert all the pending changes.
        /// </summary>
        public void ClearChanges()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Creates the resource of the given type and belonging to the 
        /// given container
        /// </summary>
        /// <param name="containerName">container name to which the resource 
        /// needs to be added</param>
        /// <param name="fullTypeName">full type name i.e. Namespace 
        /// qualified type name of the resource</param>
        /// <returns>object representing a resource of given type and 
        /// belonging to the given container</returns>
        public object CreateResource(string containerName, string 
            fullTypeName)
        {
            // Get the type of the resource
            Type t = Type.GetType(fullTypeName, true);

            // Create an instance of the resource type
            object resource = Activator.CreateInstance(t);
            
            // Return the resource object
            return resource;
        }

        /// <summary>
        /// Delete the given resource
        /// </summary>
        /// <param name="targetResource">resource that needs to be deleted
        /// </param>
        public void DeleteResource(object targetResource)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the resource of the given type that the query points to
        /// </summary>
        /// <param name="query">query pointing to a particular resource
        /// </param>
        /// <param name="fullTypeName">full type name i.e. Namespace 
        /// qualified type name of the resource</param>
        /// <returns>object representing a resource of given type and as 
        /// referenced by the query</returns>
        public object GetResource(IQueryable query, string fullTypeName)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the value of the given property on the target object
        /// </summary>
        /// <param name="targetResource">target object which defines the 
        /// property</param>
        /// <param name="propertyName">name of the property whose value needs
        /// to be updated</param>
        /// <returns>the value of the property for the given target resource
        /// </returns>
        public object GetValue(object targetResource, string propertyName)
        {
            // Get the target object type
            Type t = targetResource.GetType();

            // Get the property
            PropertyInfo pi = t.GetProperty(propertyName);
            if (pi != null)
            {
                // Return property value
                return pi.GetValue(targetResource, null);
            }
            else
            {
                return null;
            }
        }

        /// <summary>
        /// Removes the given value from the collection
        /// </summary>
        /// <param name="targetResource">target object which defines the 
        /// property</param>
        /// <param name="propertyName">name of the property whose value needs
        /// to be updated</param>
        /// <param name="resourceToBeRemoved">value of the property which 
        /// needs to be removed</param>
        public void RemoveReferenceFromCollection(object targetResource, 
            string propertyName, object resourceToBeRemoved)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Resets the value of the given resource to its default value
        /// </summary>
        /// <param name="resource">resource whose value needs to be reset
        /// </param>
        /// <returns>same resource with its value reset</returns>
        public object ResetResource(object resource)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns the actual instance of the resource represented by the 
        /// given resource object
        /// </summary>
        /// <param name="resource">object representing the resource whose 
        /// instance needs to be fetched</param>
        /// <returns>The actual instance of the resource represented by the
        /// given resource object</returns>
        public object ResolveResource(object resource)
        {
            return resource;
        }

        /// <summary>
        /// Saves all the pending changes made till now
        /// </summary>
        public void SaveChanges()
        {
            // Add the temp object into the local collection
            if (tempObj != null)
            {
                Type t = tempObj.GetType();
                if (t.Name == "Category")
                {
                    SampleProjects.categories.Add((Category)tempObj);
                }
                else if (t.Name == "Project")
                {
                    SampleProjects.projects.Add((Project)tempObj);
                }
            }
        }

        /// <summary>
        /// Sets the value of the given reference property on the target 
        /// object
        /// </summary>
        /// <param name="targetResource">target object which defines the 
        /// property</param>
        /// <param name="propertyName">name of the property whose value needs
        /// to be updated</param>
        /// <param name="propertyValue">value of the property</param>
        public void SetReference(object targetResource, string propertyName, 
            object propertyValue)
        {
            ((IUpdatable)this).SetValue(targetResource, propertyName, 
                propertyValue);
        }

        /// <summary>
        /// Sets the value of the given property on the target object
        /// </summary>
        /// <param name="targetResource">target object which defines the 
        /// property</param>
        /// <param name="propertyName">name of the property whose value needs
        /// to be updated</param>
        /// <param name="propertyValue">value of the property</param>
        public void SetValue(object targetResource, string propertyName, 
            object propertyValue)
        {
            // Get the resource object type
            Type t = targetResource.GetType();

            // Get the property to be updated
            PropertyInfo pi = t.GetProperty(propertyName);
            if (pi != null)
            {
                // Set the property value
                pi.SetValue(targetResource, propertyValue, null);
            }

            // Save the target object to temp added object
            tempObj = targetResource;
        }
        #endregion
    }
    #endregion
}
