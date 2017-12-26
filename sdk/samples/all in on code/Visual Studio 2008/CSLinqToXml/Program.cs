/****************************** Module Header ******************************\
* Module Name:  Program.cs
* Project:      CSLinqToXml
* Copyright (c) Microsoft Corporation.
* 
* This example illustrates how to use Linq to XML in C# to create XML document
* from in-memory objects and SQL Server database. It also demonstrates how to
* write Linq to XML queries in C#.  It uses Linq to SQL when querying data 
* from SQL Server database. In this example, you will see the basic Linq to
* XML methods to create XML document and the axis methods to query and edit 
* XML document. 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using CSLinqToXml;
#endregion


class Program
{
    // The default XML namespace
    private static XNamespace ns = "http://cfx.codeplex.com";

    static void Main(string[] args)
    {
        /////////////////////////////////////////////////////////////////////
        // Create the in-memory objects, build the XML document based on 
        // these objects, and query the data inside the XML document.
        // 

        // The in-memory objects XML document path
        string path = "Projects.xml";

        // Create the in-memory objects
        Category[] categories = CreateObjects();

        // Create the XML document based on the in-memory objects
        CreateObjectXml(categories, path);

        // Query the data from the in-memory objects XML document 
        QueryObjectXml(path);

        Console.WriteLine("\r\n");


        /////////////////////////////////////////////////////////////////////
        // Create the XML document based on the data in SQL Server and query 
        // the data inside the XML document. (We use Linq to SQL to query 
        // the database data, for detail about Linq to SQL examples, please 
        // see CSLinqToSQL project in All-In-One Code Framework.)
        //

        // The database XML document path
        path = "School.xml";

        // Create the XML document based on the data in SQL Server
        CreateDatabaseXml(path);

        // Query the data from the database XML document
        QueryDatabaseXml(path);

        Console.WriteLine("\r\n");


        /////////////////////////////////////////////////////////////////////
        // Edit the Projects.xml XML document created by in-memory objects. 
        // Insert. modify and delete certain XML elements.
        // 

        // The XML docuemnt path
        path = "Projects.xml";

        // Edit and save the XML dcoument
        EditDatabaseXml(path);


        /////////////////////////////////////////////////////////////////////
        // Query web resource XML document from a certain URL. 
        // 

        // The URL of certain web resource
        //string url = "WebResourceURL";

        // Analyze the web resource XML document
        //AnalyzeWebResourceXml(url);
    }

    #region Data Array Fields
    // String array of the programme languages
    private static string[] programmeLanguages = new string[3] 
    { 
        "C#", 
        "VB.NET", 
        "C++" 
    };


    // String array of the All-In-One Code Framework contributors
    private static string[] owners = new string[8] 
    {
        "Jialiang Ge", "Colbert Zhou", "Hongye Sun", "Lingzhi Sun", "Jie Wang",
        "Riquel Dong", "Rongchun Zhang", "Zhixin Ye"
    };
    #endregion

    #region In-Memory Object XML Methods
    /// <summary>
    /// This method creates the XML document based on the in-memory objects
    /// </summary>
    /// <param name="categories">The array of in-memory objects</param>
    /// <param name="path">The in-memory XML document path</param>
    private static void CreateObjectXml(Category[] categories, string path)
    {
        // Build the XML document, each element is under the default XML
        // namespace ns: http://cfx.codeplex.com
        XDocument doc = new XDocument(
            // XML declaration
            new XDeclaration("1.0", "uft-8", "yes"),
            // The root element
            new XElement(ns + "Categories",
                // Embeded Linq query to build the child XML elements
                from c in categories
                select new XElement(ns + "Category",
                    // Create the element's attribute
                    new XAttribute("name", c.Name),
                    from p in c.Projects
                    select new XElement(ns + "Project",
                        // Create the element's attribute
                        new XAttribute("name", p.ProjectName),
                        new XElement(ns + "Language", 
                            programmeLanguages[(int)p.Language]),
                        new XElement(ns + "Description", p.Description),
                        new XElement(ns + "Owner", owners[(int)p.Owner])
                    )
                )
            )
        );

        // Save the XML document to file system
        doc.Save(path);
    }


    /// <summary>
    /// This method queries the in-memory object XML document
    /// </summary>
    /// <param name="path">The in-memory XML document path</param>
    private static void QueryObjectXml(string path)
    {
        // Load the root element of the in-memory object XML document
        XElement element = XElement.Load(path);

        // Query all the projects whose owner is Jialiang Ge
        var projects = from p in element.Descendants(ns + "Project")
                       where p.Element(ns + "Owner").Value == 
                       owners[(int)Owners.Jialiang]
                       select p.Attribute("name").Value;

        // Display the query results
        Console.WriteLine("{0}'s projects:\r\n", owners[(int)Owners.Jialiang]);

        foreach (var p in projects)
        {
            Console.WriteLine(p);
        }
        Console.WriteLine("\r\n");


        // Query the project name and owner in the IPC and RPC projects whose
        // programme language is C++
        var owner = from c in element.Elements(ns + "Category")
                    from p in c.Elements(ns + "Project")
                    where c.Attribute("name").Value == "IPC and RPC"
                    && p.Element(ns + "Language").Value == 
                    programmeLanguages[(int)ProgrammeLanguage.Cpp]
                    select new
                    {
                        ProjectName = p.Attribute("name").Value,
                        Owner = p.Element(ns + "Owner").Value
                    };

        // Display the query results
        Console.WriteLine("{0} examples in category IPC and RPC:\r\n",
            programmeLanguages[(int)ProgrammeLanguage.Cpp]);

        foreach (var o in owner)
        {
            Console.WriteLine("Project {0} by {1}", o.ProjectName, o.Owner);
        }
    }


    /// <summary>
    /// This method creates the in-memory objects based on the 
    /// All-In-One Code Framework examples information
    /// </summary>
    /// <returns>The array of in-memory objects</returns>
    private static Category[] CreateObjects()
    {
        // Build the array of in-memory objects based on the 
        // All-In-One Code Framework examples information. Here we use the 
        // new features of C# 3.0, object initializers and collection 
        // initializers to build the objects.
        Category[] categories = new Category[]
        {
            new Category
            {
                Name = "COM", 
                Projects = new Project[]
                {
                    new Project
                    {
                        ProjectName = "CSDllCOMServer", 
                        Language = ProgrammeLanguage.CS,        
                        Description = "An in-process COM server in C#",        
                        Owner = Owners.Jialiang       
                    },                          
                    new Project
                    {
                        ProjectName = "VBDllCOMServer",
                        Language = ProgrammeLanguage.VB,
                        Description = "An in-process COM server in VB.NET",
                        Owner = Owners.Jialiang
                    },
                    new Project
                    {
                        ProjectName = "ATLDllCOMServer",
                        Language = ProgrammeLanguage.Cpp,
                        Description = "An in-process ATL COM Server",
                        Owner = Owners.Jialiang
                    }
                }
            },
            new Category
            {
                Name = "Data Access",
                Projects = new Project[]
                {
                    new Project
                    {
                        ProjectName = "CSUseADONET",
                        Language = ProgrammeLanguage.CS,
                        Description = "Use ADO.NET in a C# application",
                        Owner = Owners.LingzhiSun
                    },
                    new Project
                    {
                        ProjectName = "CppUseADONET",
                        Language = ProgrammeLanguage.Cpp,
                        Description = "Use ADO.NET in a C++ application",
                        Owner = Owners.Jialiang
                    },
                    new Project
                    {
                        ProjectName = "CSLinqToObject",
                        Language = ProgrammeLanguage.CS,
                        Description = "Use LINQ to Objects in C#",
                        Owner = Owners.Colbert
                    },
                    new Project
                    {
                        ProjectName = "CSLinqToSQL",
                        Language = ProgrammeLanguage.CS,
                        Description = "Use LINQ to SQL in C#",
                        Owner = Owners.RongchunZhang
                    }
                }
            },
            new Category
            {
                Name = "Office",
                Projects = new Project[]
                {
                    new Project
                    {
                        ProjectName = "CSOutlookUIDesigner",
                        Language = ProgrammeLanguage.CS,
                        Description = "Customize Outlook UI using VSTO" + 
                                        " Designers",
                        Owner = Owners.midnightfrank
                    },
                    new Project
                    {
                        ProjectName = "CSOutlookRibbonXml",
                        Language = ProgrammeLanguage.CS,
                        Description = "Customize Outlook UI using Ribbon " + 
                                        "XML",
                        Owner = Owners.midnightfrank
                    },
                    new Project
                    {
                        ProjectName = "CSAutomateExcel",
                        Language = ProgrammeLanguage.CS,
                        Description = "Automate Excel in a C# application",
                        Owner = Owners.Colbert
                    },
                    new Project
                    {
                        ProjectName = "VBAutomateExcel",
                        Language = ProgrammeLanguage.VB,
                        Description = "Automate Excel in a VB.NET " + 
                                        "application",
                        Owner = Owners.Jialiang
                    }
                }
            },
            new Category
            {
                Name = "IPC and RPC",
                Projects = new Project[]
                {
                    new Project
                    {
                        ProjectName = "CppFileMappingServer",
                        Language = ProgrammeLanguage.Cpp,
                        Description = "Create shared memory in C++",
                        Owner = Owners.hongyes
                    },
                    new Project
                    {
                        ProjectName = "CppFileMappingClient",
                        Language = ProgrammeLanguage.Cpp,
                        Description = "Access shared memory in C++",
                        Owner = Owners.hongyes
                    },
                    new Project
                    {
                        ProjectName = "CSReceiveWM_COPYDATA",
                        Language = ProgrammeLanguage.CS,
                        Description = "Receive WMCOPYDATA in C#",
                        Owner = Owners.Riquel
                    },
                    new Project
                    {
                        ProjectName = "CSSendWM_COPYDATA",
                        Language = ProgrammeLanguage.CS,
                        Description = "Send WMCOPYDATA in C#",
                        Owner = Owners.Riquel
                    }
                }
            },
            new Category
            {
                Name = "WinForm",
                Projects = new Project[]
                {
                    new Project
                    {
                        ProjectName = "CSWinFormGeneral",
                        Language = ProgrammeLanguage.CS,
                        Description = "General scenarios in WinForm",
                        Owner = Owners.ZhiXin
                    },
                    new Project
                    {
                        ProjectName = "CSWinFormDataBinding",
                        Language = ProgrammeLanguage.CS,
                        Description = "WinForm Data-binding in C#",
                        Owner = Owners.ZhiXin
                    }
                }
            },
            new Category
            {
                Name = "Hook",
                Projects = new Project[]
                {
                    new Project
                    {
                        ProjectName = "CSWindowsHook",
                        Language = ProgrammeLanguage.CS,
                        Description = "Windows Hook in a C# application",
                        Owner = Owners.RongchunZhang
                    }
                }
            }
        };

        // Return the arrary of in-memory objects
        return categories;
    }
    #endregion

    #region Database XML Methods
    /// <summary>
    /// This method creates the XML document based on the Person table in 
    /// SQLServer2005DB database in All-In-One Code Framework
    /// </summary>
    /// <param name="path">The database XML document path</param>
    private static void CreateDatabaseXml(string path)
    {
        // Create the Linq to SQL data context object
        // For detail about Linq to SQL examples, please see the CSLinqToSQL
        // project in All-In-One Code Framework
        SchoolDataContext db = new SchoolDataContext();

        // Build the XML document, each element is under the default XML
        // namespace ns: http://cfx.codeplex.com 
        XDocument doc = new XDocument(
            // XML declaration
            new XDeclaration("1.0", "utf-8", "yes"),
            // The root element
            new XElement(ns + "Person",
                // The Employees elements
                new XElement(ns + "Employees",
                    // Embeded Linq query to build the child XML elements
                    from e in db.Persons
                    where e.PersonCategory == 2
                    select new XElement(ns + "Employee",
                        // Create the element's attribute 
                        new XAttribute("id", e.PersonID),
                        new XElement(ns + "Name", e.FirstName + " " + 
                            e.LastName),
                        new XElement(ns + "HireDate", 
                            e.HireDate.Value.ToString())
                        )
                ),
                // The Students elements
                new XElement(ns + "Students",
                    // Embeded Linq query to build the child XML elements
                    from s in db.Persons
                    where s.PersonCategory == 1
                    select new XElement(ns + "Student",
                        // Create the element's attribute
                        new XAttribute("id", s.PersonID),
                        new XElement(ns + "Name", s.FirstName + " " + 
                            s.LastName),
                        new XElement(ns + "EnrollmentDate", 
                            s.EnrollmentDate.Value.ToString())
                        )
                )
            )
        );

        // Save the XML document to the file system
        doc.Save(path);
    }


    /// <summary>
    /// This method queries the database XML document
    /// </summary>
    /// <param name="path">The database XML document path</param>
    private static void QueryDatabaseXml(string path)
    {
        // Load the root element of the database XML document 
        XDocument doc = XDocument.Load(path);

        // Query all the employees whose hire date is later than 2000/01/01
        var employees = from e in doc.Descendants(ns + "Employee")
                        where DateTime.Parse(e.Element(ns + "HireDate").Value)
                        > new DateTime(2000, 1, 1)
                        select e.Element(ns + "Name").Value;

        // Display the query results
        Console.WriteLine("Employees whose hire date is later than " + 
            "2000/01/01:\r\n");

        foreach (var e in employees)
        {
            Console.WriteLine(e);
        }
    }
    #endregion

    #region Edit XML Method
    /// <summary>
    /// This method edits an XML document in file system
    /// </summary>
    /// <param name="path">The XML document path</param>
    private static void EditDatabaseXml(string path)
    {
        // Load the XML document
        XDocument doc = XDocument.Load(path);

        // Retrieve the root element
        XElement element = doc.Root;


        // Insert new XML elements to the XML document

        Console.WriteLine("Insert new Category XML and LINQ to XML " +
                            "projects XML elements...");

        // Build the objects
        Category xmlCategory = new Category()
        {
            Name = "XML",
            Projects = new Project[]
            {
                new Project
                {
                    ProjectName = "CSLinqToXml",
                    Language = ProgrammeLanguage.CS,
                    Description = "Use LINQ to XML in C#",
                    Owner = Owners.LingzhiSun
                },
                new Project
                {
                    ProjectName = "VBLinqToXml",
                    Language = ProgrammeLanguage.VB,
                    Description = "Use LINQ to XML in VB.NET",
                    Owner = Owners.LingzhiSun
                }
            }
        };

        // Add the newly created objects as the last element of the root 
        // element
        element.Add(
            new XElement(ns + "Category",
                new XAttribute("name", xmlCategory.Name),
                from p in xmlCategory.Projects
                select new XElement(ns + "Project",
                    new XAttribute("name", p.ProjectName),
                    new XElement(ns + "Language", 
                        programmeLanguages[(int)p.Language]),
                    new XElement(ns + "Description", p.Description),
                    new XElement(ns + "Owner", owners[(int)p.Owner])
                )
            )
        );


        // Modify the value of certain XML element

        Console.WriteLine("Modify the prject CppUseADONET project's" +
                            " owner...");

        // Retrieve the certin Category element
        XElement categoryElement = element.Elements(ns + "Category").
            Where(c => c.Attribute("name").Value == "Data Access").Single();

        if (categoryElement != null)
        {
            // Retrieve the certain Project element
            var projectElement = categoryElement.Elements(ns + "Project").
                Where(p => p.Attribute("name").Value == "CppUseADONET").Single();

            if (projectElement != null)
            {
                // Modify the Owner element value
                projectElement.Element(ns + "Owner").Value = 
                    owners[(int)Owners.LingzhiSun];
            }
        }


        // Delete certain XML element

        Console.WriteLine("Delete the Hook Category element and its descendants...");

        // Retrieve the certain Category element
        categoryElement = element.Elements(ns + "Category").
            Where(c => c.Attribute("name").Value == "Hook").Single();

        // Delete the element and all its descendants
        categoryElement.Remove();

        // Save the XML document
        doc.Save(path);
    }
    #endregion

    #region Query Web Resource Method
    /// <summary>
    /// This method query a web resource XML document
    /// </summary>
    /// <param name="url">The web resource XML document URL</param>
    private static void AnalyzeWebResourceXml(string url)
    {
        XDocument doc = XDocument.Load(url);

        // Use the similar query method in QueryObjectXml and 
        // QueryDatabaseXml to query the data from the web resource.  
        // ...
    }
    #endregion
}

#region In-Memory Object Classes
// The All-In-One Code Framework projects category class
public class Category
{
    // The category name
    public string Name { get; set; }

    // The array of All-In-One Code Framework projects objects
    public Project[] Projects { get; set; }
}

// The All-In-One Code Framework project class
public class Project
{
    // The project name
    public string ProjectName { get; set; }

    // The project programme language
    public ProgrammeLanguage Language { get; set; }

    // The project description
    public string Description { get; set; }

    // The project owner
    public Owners Owner { get; set; }
}
#endregion

#region Data Enums
// The enum of programme language with C#, VB.NET and C++
public enum ProgrammeLanguage
{
    CS,
    VB,
    Cpp
}

// The enum of All-In-One Code Framework contributors's display names on CodePlex
public enum Owners
{
    Jialiang, Colbert, hongyes, LingzhiSun, midnightfrank, Riquel, 
    RongchunZhang, ZhiXin
}
#endregion