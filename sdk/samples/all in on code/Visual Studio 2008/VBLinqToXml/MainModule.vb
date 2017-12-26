'****************************** Module Header ******************************'
' Module Name:	MainModule.vb
' Project:		VBLinqToXml
' Copyright (c) Microsoft Corporation.
' 
' This example illustrates how to use Linq to XML in VB.NET to create XML 
' document from in-memory objects and SQL Server database. It also 
' demonstrates how to write Linq to XML queries in VB.NET.  It uses 
' Linq to SQL when querying data from SQL Server database. In this example, 
' you will see VB.NET XML literals methods to create, query and edit XML 
' document.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

' Import the default XML namespace
Imports <xmlns="http://cfx.codeplex.com">


Module MainModule

    Sub Main()

        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Create the in-memory objects, build the XML document based on these
        ' objects, and query the data inside the XML document.
        ' 

        ' The in-memory objects XML document path
        Dim path As String = "Projects.xml"

        ' Create the in-memory objects
        Dim categories As Category() = CreateObjects()

        ' Create the XML document based on the in-memory objects
        CreateObjectXml(categories, path)

        ' Query the data from the in-memory objects XML document
        QueryObjectXml(path)

        Console.WriteLine(vbNewLine)


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Create the XML document based on the data in SQL Server and query
        ' the data inside the XML document. (We use Linq to SQL to query the 
        ' database data, for detail about Linq to SQL examples, please see 
        ' CSLinqToSQL project in All-In-One Code Framework.)
        ' 

        ' The database XML document path
        path = "School.xml"

        ' Create the XML document based on the data in SQL Server
        CreateDatabaseXml(path)

        ' Query the data from the database XML document
        QueryDatabaseXml(path)

        Console.WriteLine(vbNewLine)


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Edit the Projects.xml XML document created by in-memory objects. 
        ' Insert. modify and delete certain XML elements.
        ' 

        ' The XML docuemnt path
        path = "Projects.xml"

        ' Edit and save the XML dcoument
        EditDatabaseXml(path)


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Query web resource XML document from a certain URL. 
        ' 

        ' The URL of certain web resource
        'Dim url As String = "WebResourceURL"

        ' Analyze the web resource XML document
        'AnalyzeWebResourceXml(url)

    End Sub

#Region "Data Array Fields"

    ' String array of the programme languages
    Dim programmeLanguages As String() = New String() _
    { _
        "C#", _
        "VB.NET", _
        "C++" _
    }

    ' String array of the All-In-One Code Framework contributors
    Dim ownerNames As String() = New String() _
    { _
        "Jialiang Ge", _
        "Colbert Zhou", _
        "Hongye Sun", _
        "Lingzhi Sun", _
        "Jie Wang", _
        "Riquel Dong", _
        "Rongchun Zhang", _
        "Zhixin Ye" _
    }

#End Region

#Region "In-Memory Object XML Methods"

    ''' <summary>
    ''' This method creates the XML document based on the in-memory objects
    ''' </summary>
    ''' <param name="categories">The array of in-memory objects</param>
    ''' <param name="path">The in-memory XML document path</param>
    Sub CreateObjectXml(ByVal categories As Category(), ByVal path As String)
        ' Build the XML document, each element is under the default XML 
        ' namespace ns: http://cfx.codeplex.com. We use new feature of VB.NET 
        ' 9.0, XML literals.
        Dim doc As XDocument = _
            <?xml version="1.0" encoding="utf-8" standalone="yes"?>
            <Categories>
                <%= From category In categories _
                    Select _
                    <Category name=<%= category.Name %>>
                        <%= From project In category.Projects _
                            Select _
                            <Project name=<%= project.ProjectName %>>
                                <Language><%= programmeLanguages(project.Language) %></Language>
                                <Description><%= project.Description %></Description>
                                <Owner><%= ownerNames(project.Owner) %></Owner>
                            </Project> _
                        %>
                    </Category> _
                %>
            </Categories>

        ' Save the XML document to file system
        doc.Save(path)
    End Sub


    ''' <summary>
    ''' This method queries the in-memory object XML document
    ''' </summary>
    ''' <param name="path">The in-memory XML document path</param>
    Sub QueryObjectXml(ByVal path As String)
        ' Load the root element of the in-memory object XML document
        Dim element As XElement = XElement.Load(path)

        ' Query all the projects whose owner is Jialiang Ge
        Dim projects = From p In element...<Project> _
                       Where p.<Owner>.Value = ownerNames(Owners.Jialiang) _
                       Select p.@name

        ' Display the query results
        Console.WriteLine("{0}'s projects:", ownerNames(Owners.Jialiang))
        Console.WriteLine()

        For Each p In projects
            Console.WriteLine(p)
        Next

        Console.WriteLine()

        ' Query the project name and owner in the IPC and RPC projects whose
        ' programme language is C++
        Dim owner = From c In element.<Category> _
                    From p In c.<Project> _
                    Where c.@name = "IPC and RPC" _
                    And p.<Language>.Value = _
                    programmeLanguages(ProgrammeLanguage.Cpp) _
                    Select New With _
                    { _
                        .ProjectName = p.@name, _
                        .Owner = p.<Owner>.Value _
                    }

        ' Display the query results
        Console.WriteLine("{0} examples in category IPC and RPC:", _
                          programmeLanguages(ProgrammeLanguage.Cpp))
        Console.WriteLine()

        For Each o In owner
            Console.WriteLine("Project {0} by {1}", o.ProjectName, o.Owner)
        Next
    End Sub


    ''' <summary>
    ''' This method creates the in-memory objects based on the 
    ''' All-In-One Code Framework examples information
    ''' </summary>
    ''' <returns>The array of in-memory objects</returns>
    Function CreateObjects() As Category()
        ' Build the array of in-memory objects based on the 
        ' All-In-One Code Framework examples information. Here we use the 
        ' new features of VB.NET 9.0, object initializers to build the objects.
        Dim categories As Category() = New Category() _
        { _
            New Category With _
            { _
                .Name = "COM", _
                .Projects = New Project() _
                { _
                    New Project With _
                    { _
                        .ProjectName = "CSDllCOMServer", _
                        .Language = ProgrammeLanguage.CS, _
                        .Description = "An in-process COM server in C#", _
                        .Owner = Owners.Jialiang _
                    }, _
                    New Project With _
                    { _
                        .ProjectName = "VBDllCOMServer", _
                        .Language = ProgrammeLanguage.VB, _
                        .Description = "An in-process COM server in " + _
                                        "VB.NET", _
                        .Owner = Owners.Jialiang _
                    }, _
                    New Project With _
                    { _
                        .ProjectName = "ATLDllCOMServer", _
                        .Language = ProgrammeLanguage.Cpp, _
                        .Description = "An in-process ATL COM Server", _
                        .Owner = Owners.Jialiang _
                    } _
                } _
            }, _
            New Category With _
            { _
                .Name = "Data Access", _
                .Projects = New Project() _
                { _
                    New Project With _
                    { _
                        .ProjectName = "CSUseADONET", _
                        .Language = ProgrammeLanguage.CS, _
                        .Description = "Use ADO.NET in a C# application", _
                        .Owner = Owners.LingzhiSun _
                    }, _
                    New Project With _
                    { _
                        .ProjectName = "CppUseADONET", _
                        .Language = ProgrammeLanguage.Cpp, _
                        .Description = "Use ADO.NET in a C++ application", _
                        .Owner = Owners.Jialiang _
                    }, _
                    New Project With _
                    { _
                        .ProjectName = "CSLinqToObject", _
                        .Language = ProgrammeLanguage.CS, _
                        .Description = "Use LINQ to Objects in C#", _
                        .Owner = Owners.Colbert _
                    }, _
                    New Project With _
                    { _
                        .ProjectName = "CSLinqToSQL", _
                        .Language = ProgrammeLanguage.CS, _
                        .Description = "Use LINQ to SQL in C#", _
                        .Owner = Owners.RongchunZhang _
                    } _
                } _
            }, _
            New Category With _
            { _
                .Name = "Office", _
                .Projects = New Project() _
                { _
                    New Project With _
                    { _
                        .ProjectName = "CSOutlookUIDesigner", _
                        .Language = ProgrammeLanguage.CS, _
                        .Description = "Customize Outlook UI using VSTO " + _
                                        "Designers", _
                        .Owner = Owners.midnightfrank _
                    }, _
                    New Project With _
                    { _
                        .ProjectName = "CSOutlookRibbonXml", _
                        .Language = ProgrammeLanguage.CS, _
                        .Description = "Customize Outlook UI using Ribbon " + _
                                        "XML", _
                        .Owner = Owners.midnightfrank _
                    }, _
                    New Project With _
                    { _
                        .ProjectName = "CSAutomateExcel", _
                        .Language = ProgrammeLanguage.CS, _
                        .Description = "Automate Excel in a C# application", _
                        .Owner = Owners.Colbert _
                    }, _
                    New Project With _
                    { _
                        .ProjectName = "VBAutomateExcel", _
                        .Language = ProgrammeLanguage.VB, _
                        .Description = "Automate Excel in a VB.NET " + _
                                            "application", _
                        .Owner = Owners.Jialiang _
                    } _
                } _
            }, _
            New Category With _
            { _
                .Name = "IPC and RPC", _
                .Projects = New Project() _
                { _
                    New Project With _
                    { _
                        .ProjectName = "CppFileMappingServer", _
                        .Language = ProgrammeLanguage.Cpp, _
                        .Description = "Create shared memory in C++", _
                        .Owner = Owners.hongyes _
                    }, _
                    New Project With _
                    { _
                        .ProjectName = "CppFileMappingClient", _
                        .Language = ProgrammeLanguage.Cpp, _
                        .Description = "Access shared memory in C++", _
                        .Owner = Owners.hongyes _
                    }, _
                    New Project With _
                    { _
                        .ProjectName = "CSReceiveWM_COPYDATA", _
                        .Language = ProgrammeLanguage.CS, _
                        .Description = "Receive WMCOPYDATA in C#", _
                        .Owner = Owners.Riquel _
                    }, _
                    New Project With _
                    { _
                        .ProjectName = "CSSendWM_COPYDATA", _
                        .Language = ProgrammeLanguage.CS, _
                        .Description = "Send WMCOPYDATA in C#", _
                        .Owner = Owners.Riquel _
                    } _
                } _
            }, _
            New Category With _
            { _
                .Name = "WinForm", _
                .Projects = New Project() _
                { _
                    New Project With _
                    { _
                        .ProjectName = "CSWinFormGeneral", _
                        .Language = ProgrammeLanguage.CS, _
                        .Description = "General scenarios in WinForm", _
                        .Owner = Owners.ZhiXin _
                    }, _
                    New Project With _
                    { _
                        .ProjectName = "CSWinFormDataBinding", _
                        .Language = ProgrammeLanguage.CS, _
                        .Description = "WinForm Data-binding in C#", _
                        .Owner = Owners.ZhiXin _
                    } _
                } _
            }, _
            New Category With _
            { _
                .Name = "Hook", _
                .Projects = New Project() _
                { _
                    New Project With _
                    { _
                        .ProjectName = "CSWindowsHook", _
                        .Language = ProgrammeLanguage.CS, _
                        .Description = "Windows Hook in a C# application", _
                        .Owner = Owners.RongchunZhang _
                    } _
                } _
            } _
        }

        ' Return the arrary of in-memory objects
        Return categories
    End Function

#End Region

#Region "Database XML Methods"

    ''' <summary>
    ''' This method creates the XML document based on the Person table in 
    ''' SQLServer2005DB database in All-In-One Code Framework
    ''' </summary>
    ''' <param name="path">The database XML document path</param>
    Sub CreateDatabaseXml(ByVal path As String)

        ' Create the Linq to SQL data context object
        ' For detail about Linq to SQL examples, please see the CSLinqToSQL
        ' project in All-In-One Code Framework
        Dim db As SchoolDataContext = New SchoolDataContext()

        ' Build the XML document, each element is under the default XML
        ' namespace ns: http://cfx.codeplex.com 
        Dim doc As XDocument = _
            <?xml version="1.0" encoding="utf-8" standalone="yes"?>
            <Person>
                <Employees>
                    <%= From employee In db.Persons _
                        Where employee.PersonCategory = 2 _
                        Select _
                        <Employee id=<%= employee.PersonID %>>
                            <Name><%= employee.FirstName & " " & employee.LastName %></Name>
                            <HireDate><%= employee.HireDate.ToString() %></HireDate>
                        </Employee> _
                    %>
                </Employees>
                <Students>
                    <%= From student In db.Persons _
                        Where student.PersonCategory = 1 _
                        Select _
                        <Student id=<%= student.PersonID %>>
                            <Name><%= student.FirstName & " " & student.LastName %></Name>
                            <EnrollmentDate><%= student.EnrollmentDate.Value.ToString() %></EnrollmentDate>
                        </Student> _
                    %>
                </Students>
            </Person>

        ' Save the XML document to the file system
        doc.Save(path)
    End Sub


    ''' <summary>
    ''' This method queries the database XML document
    ''' </summary>
    ''' <param name="path">The database XML document path</param>
    Sub QueryDatabaseXml(ByVal path As String)
        ' Load the root element of the database XML document 
        Dim doc = XDocument.Load(path)

        ' Query all the employees whose hire date is later than 2000/01/01
        Dim employees = From e In doc...<Employee> _
                        Where DateTime.Parse(e.<HireDate>.Value) > _
                        New DateTime(2000, 1, 1) _
                        Select e.<Name>.Value

        ' Display the query results
        Console.WriteLine("Employees whose hire date is later than " + _
                            "2000/01/01:")
        Console.WriteLine()

        For Each e In employees
            Console.WriteLine(e)
        Next
    End Sub

#End Region

#Region "Edit XML Method"

    ''' <summary>
    ''' This method edits an XML document in file system
    ''' </summary>
    ''' <param name="path">The XML document path</param>
    Sub EditDatabaseXml(ByVal path As String)
        Dim doc As XDocument = XDocument.Load(path)  ' Load the XML document

        Dim element As XElement = doc.Root  ' Retrieve the root element

        ' Insert new XML elements to the XML document

        Console.WriteLine("Insert new Category XML and LINQ to XML projects XML elements...")

        ' Build the objects
        Dim xmlCategory As New Category() With _
        { _
            .Name = "XML", _
            .Projects = New Project() _
            { _
                New Project With _
                { _
                    .ProjectName = "CSLinqToXml", _
                    .Language = ProgrammeLanguage.CS, _
                    .Description = "Use LINQ to XML in C#", _
                    .Owner = Owners.LingzhiSun _
                }, _
                New Project With _
                { _
                    .ProjectName = "VBLinqToXml", _
                    .Language = ProgrammeLanguage.VB, _
                    .Description = "Use LINQ to XML in VB.NET", _
                    .Owner = Owners.LingzhiSun _
                } _
            } _
        }

        ' Add the newly created objects as the last element of the root element
        element.Add( _
            <Category name=<%= xmlCategory.Name %>>
                <%= From project In xmlCategory.Projects _
                    Select _
                    <Project name=<%= project.ProjectName %>>
                        <Language><%= programmeLanguages(project.Language) %></Language>
                        <Description><%= project.Description %></Description>
                        <Owner><%= ownerNames(project.Owner) %></Owner>
                    </Project> _
                %>
            </Category>)

        ' Modify the value of certain XML element

        Console.WriteLine("Modify the prject CppUseADONET project's owner...")

        ' Retrieve the certin Category element
        Dim categoryElement As XElement = element.<Category>. _
        Where(Function(c As XElement) c.@name = "Data Access").Single()

        If Not categoryElement Is Nothing Then
            ' Retrieve the certain Project element
            Dim projectElement = categoryElement.<Project>. _
            Where(Function(p As XElement) p.@name = "CppUseADONET").Single()

            If Not projectElement Is Nothing Then
                ' Modify the Owner element value
                projectElement.<Owner>.Value = ownerNames(Owners.LingzhiSun)
            End If
        End If

        ' Delete certain XML element

        Console.WriteLine("Delete the Hook Category element and its descendants...")

        ' Retrieve the certain Category element
        categoryElement = element.<Category>. _
        Where(Function(c As XElement) c.@name = "Hook").Single()

        categoryElement.Remove()  ' Delete the element and all its descendants

        doc.Save(path)  ' Save the XML document
    End Sub

#End Region

#Region "Query Web Resource Method"

    ''' <summary>
    ''' This method query a web resource XML document
    ''' </summary>
    ''' <param name="url">The web resource XML document URL</param>
    Sub AnalyzeWebResourceXml(ByVal url As String)
        Dim doc As XDocument = XDocument.Load(url)

        ' Use the similar query method in QueryObjectXml and 
        ' QueryDatabaseXml to query the data from the web resource.  
        '...
    End Sub

#End Region

End Module


#Region "In-Memory Object Classes"

' The All-In-One Code Framework projects category class
Class Category
    ' The category name property
    Public Property Name() As String
        Get
            Return Me._name
        End Get
        Set(ByVal value As String)
            Me._name = value
        End Set
    End Property

    ' The category name field
    Private _name As String

    ' The array of All-In-One Code Framework projects objects property
    Public Property Projects() As Project()
        Get
            Return Me._projects
        End Get
        Set(ByVal value As Project())
            Me._projects = value
        End Set
    End Property

    ' The array of All-In-One Code Framework projects objects field
    Private _projects As Project()
End Class

' The All-In-One Code Framework project class
Class Project
    ' The project name property
    Public Property ProjectName() As String
        Get
            Return Me._projectName
        End Get
        Set(ByVal value As String)
            Me._projectName = value
        End Set
    End Property

    ' The project name field
    Private _projectName As String

    ' The project programme language property
    Public Property Language() As ProgrammeLanguage
        Get
            Return Me._language
        End Get
        Set(ByVal value As ProgrammeLanguage)
            Me._language = value
        End Set
    End Property

    ' The project programme language field
    Private _language As ProgrammeLanguage

    ' The project description property
    Public Property Description() As String
        Get
            Return Me._description
        End Get
        Set(ByVal value As String)
            Me._description = value
        End Set
    End Property

    ' The project description field
    Private _description As String

    ' The project owner property
    Public Property Owner() As Owners
        Get
            Return Me._owner
        End Get
        Set(ByVal value As Owners)
            Me._owner = value
        End Set
    End Property

    ' The project owner field
    Private _owner As Owners
End Class

#End Region


#Region "Data Enums"

' The enum of programme language with C#, VB.NET and C++
Enum ProgrammeLanguage
    CS
    VB
    Cpp
End Enum

' The enum of All-In-One Code Framework contributors's display names 
' on CodePlex
Enum Owners
    Jialiang
    Colbert
    hongyes
    LingzhiSun
    midnightfrank
    Riquel
    RongchunZhang
    ZhiXin
End Enum

#End Region