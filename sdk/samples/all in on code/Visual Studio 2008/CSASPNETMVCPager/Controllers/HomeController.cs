using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Mvc;
using CSASPNETMVCPager.Models;
using CSASPNETMVCPager.Helper;
namespace CSASPNETMVCPager.Controllers
{
    [HandleError]
    public class HomeController : Controller
    {
        /// <summary>
        /// Used to display paging
        /// </summary>
        /// <param name="id">current page index</param>
        /// <returns>actionresult</returns>
        public ActionResult Index(int? id)
        {
            int pageIndex = Convert.ToInt32(id);
            List<Employee> empList = EmployeeSet.Employees;
            int pagesSize = 5;

            Pager<Employee> pager = new Pager<Employee>(empList, pageIndex, Url.Content("~/Home/Index"), Url.Content("~/images/left.gif"), Url.Content("~/images/right.gif"), pagesSize);

            ViewData["pager"] = pager;
            return View(empList.Skip(pager.pageSize * pageIndex).Take(pager.pageSize));
        }

        public ActionResult About()
        {
            return View();
        }
    }
}
