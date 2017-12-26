=============================================================================
         ASP.NET APPLICATION :  CSASPNETMVCPager Project Overview 
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

  The project illustrates how to customize a html extension method for paging.
In this project we will define a class named Pager<T> to instialize some basic
properties for paing, such as the total pages,how many records will be 
displayed in a page and so on. Then a html helper method would be define in 
the next CustomizePager class. 


/////////////////////////////////////////////////////////////////////////////
Prerequisites:

ASP.NET MVC RTM and .NET Framework 3.5
You can download ASP.NET MVC RTM from the following link.
http://www.microsoft.com/downloads/details.aspx?displaylang=en&FamilyID=c9ba1fe1-3ba8-439a-9e21-def90a8615a9


/////////////////////////////////////////////////////////////////////////////
Implementation:

Step1: Create a Visual C# ASP.NET MVC application in Visual Studio 2008 and 
name it as CSASPNETMVCPager.

Step2: Add a new folder named Images and add two pictures for next page or 
previous page.

Step3: Add a new folder named Helper and add a new class file named CustomizePager.cs.

Step4: Create Pager<T> class which is used to contain basic information about 
the datasource  and CustomizePager class which is used to render the pager html 
code.
    public class Pager<T>
    {
        /// <summary>
        /// determine how many records displayed in one page
        /// </summary>
        public int pageSize = 6;
    
        /// <summary>
        /// instantiate a pager object
        /// </summary>
        /// <param name="collection">datasource which implement ICollection<T></param>
        /// <param name="currentPageIndex">Current page index</param>
        /// <param name="requestBaseUrl">Request base url</param>
        public Pager(ICollection<T> collection,int currentPageIndex,string requestBaseUrl)
        {
            //Initialize properties
        }
        
        /// <summary>
        /// instantiate a pager object
        /// </summary>
        /// <param name="collection">datasource which implement ICollection<T></param>
        /// <param name="currentPageIndex">Current page index</param>
        /// <param name="requestBaseUrl">Request base url</param>
        /// <param name="imgUrlForUp">image for previous page</param>
        /// <param name="imgUrlForDown">image for next page</param>
        public Pager(ICollection<T> collection, int currentPageIndex, string requestBaseUrl, string imgUrlForUp, string imgUrlForDown)
        {
            //Initialize properties
        }
        
        /// <summary>
        /// instantiate a pager object
        /// </summary>
        /// <param name="collection">datasource which implement ICollection<T></param>
        /// <param name="currentPageIndex">Current page index</param>
        /// <param name="requestBaseUrl">Request base url</param>
        /// <param name="imgUrlForUp">image for previous page</param>
        /// <param name="imgUrlForDown">image for next page</param>
        /// <param name="pagesSize">determine the size of page numbers displayed</param>
        public Pager(IList<T> collection, int currentPageIndex, string requestBaseUrl, string imgUrlForUp, string imgUrlForDown, int pagesSize)
        {
            //Initialize properties
        }
    
        /// <summary>
        /// current page index
        /// </summary>
        public int CurrentPageIndex { get; private set; }
        
        /// <summary>
        /// total pages
        /// </summary>
        public int TotalPages { get; private set; }
        
        /// <summary>
        /// base url and id value construct a whole url, eg:http://RequestBaseUrl/id
        /// </summary>
        public string RequestBaseUrl { get; private set; }
        
        /// <summary>
        /// image for previous page
        /// </summary>
        public string ImageUrlForUp { get; private set; }
        
        /// <summary>
        /// image for next page
        /// </summary>
        public string ImageUrlForDown { get; private set; }
        
        /// <summary>
        /// size of page numbers which are displayed
        /// </summary>
        public int PagesSize { get; private set; }
    }

Step5: Create a Employee class in the Model folder as the model of the demo
and a static class named EmployeeSet to generate demo data. For convenience 
we generate the demo data manually. You can create any data object which is 
inherited from ICollection<T> according to your requirement.

Step6: Modify the Index method in the Home Controller to prepare for paging.

    public ActionResult Index(int ? id)
    {
        int pageIndex = Convert.ToInt32(id);
        List<Employee> empList=EmployeeSet.Employees;
        int pagesSize = 5;
            
        Pager<Employee> pager = new Pager<Employee>(empList , pageIndex, Url.Content("~/Home/Index"), Url.Content("~/images/left.gif"), Url.Content("~/images/right.gif"), pagesSize);

        ViewData["pager"] =pager;
        return View(empList.Skip(pager.pageSize * pageIndex).Take(pager.pageSize));
    }


Step7: Modify the Index view in the Home folder which is in the Views folder
to render the employee information and output pager.

Step8: Build and run the ASP.NET project.


/////////////////////////////////////////////////////////////////////////////
Reference:

http://www.asp.net/mvc/fundamentals/


/////////////////////////////////////////////////////////////////////////////