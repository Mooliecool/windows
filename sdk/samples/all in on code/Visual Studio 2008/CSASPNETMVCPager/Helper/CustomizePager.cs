using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Text;
using System.Web.Mvc;

namespace CSASPNETMVCPager.Helper
{
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
        public Pager(ICollection<T> collection, int currentPageIndex, string requestBaseUrl)
        {
            int count = collection.Count;
            int remainder = count % pageSize;
            int quotient = count / pageSize;

            this.CurrentPageIndex = currentPageIndex;
            this.TotalPages = remainder == 0 ? quotient : quotient + 1;
            this.RequestBaseUrl = requestBaseUrl;
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
            int count = collection.Count;
            int remainder = count % pageSize;
            int quotient = count / pageSize;

            this.CurrentPageIndex = currentPageIndex;
            this.TotalPages = remainder == 0 ? quotient : quotient + 1;
            this.RequestBaseUrl = requestBaseUrl;
            this.ImageUrlForDown = imgUrlForDown;
            this.ImageUrlForUp = imgUrlForUp;
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
            int count = collection.Count;
            int remainder = count % pageSize;
            int quotient = count / pageSize;

            this.CurrentPageIndex = currentPageIndex;
            this.TotalPages = remainder == 0 ? quotient : quotient + 1;
            this.RequestBaseUrl = requestBaseUrl;
            this.ImageUrlForDown = imgUrlForDown;
            this.ImageUrlForUp = imgUrlForUp;
            this.PagesSize = this.PagesSize > pagesSize ? this.PagesSize : pagesSize;
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
        /// base url and id value construct a whole url, e.g. http://RequestBaseUrl/id
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


    /// <summary>
    /// Customize pager engineer
    /// </summary>
    public static class CustomizePager
    {
        /// <summary>
        /// Simply input 'Up' or 'Down' for paging
        /// </summary>
        /// <param name="currentPageIndex">current page index</param>
        /// <param name="totalPages">total pages</param>
        /// <param name="requestBaseUrl">base url and id value construct a whole url, eg:http://baseURL/id</param>   
        public static string CreatePager(int currentPageIndex, int totalPages, string requestBaseUrl)
        {
            Validate(currentPageIndex, totalPages);
            StringBuilder sb = new StringBuilder();
            if (currentPageIndex > 0)
            {
                sb.Append(string.Format("<a href=\"{0}\">Up</a>", requestBaseUrl + "/" + (currentPageIndex - 1).ToString()));

            }
            if (currentPageIndex != totalPages - 1)
            {
                sb.Append(string.Format("<a href=\"{0}\">Down</a>", requestBaseUrl + "/" + (currentPageIndex + 1).ToString()));
            }
            return sb.ToString();
        }

        /// <summary>
        /// Use the passed images for paging 
        /// </summary>
        /// <param name="currentPageIndex">current page index</param>
        /// <param name="totalPages">total pages</param>
        /// <param name="requestBaseUrl">base url and id value construct a whole url, eg:http://baseURL/id</param>   
        /// <param name="imageUrlForUp">image for previous page</param>
        /// <param name="imageUrlForDown">image for next page</param>
        /// <returns></returns>
        public static string CreatePager(int currentPageIndex, int totalPages, string requestBaseUrl, string imageUrlForUp, string imageUrlForDown)
        {

            Validate(currentPageIndex, totalPages);
            StringBuilder sb = new StringBuilder();
            if (currentPageIndex > 0)
            {
                sb.Append(string.Format("<a href=\"{0}\"><img src={1} style=\"boder:0px;border-color: #FFFFFF\"/></a>", requestBaseUrl + "/" + (currentPageIndex - 1).ToString(), imageUrlForUp));

            }

            if (currentPageIndex != totalPages - 1)
            {
                sb.Append(string.Format("<a href=\"{0}\"><img src={1} style=\"boder:0px;border-color: #FFFFFF\"/></a>", requestBaseUrl + "/" + (currentPageIndex + 1).ToString(), imageUrlForDown));

            }

            return sb.ToString();
        }

        /// <summary>
        /// Use arabic numerals and images for paging
        /// </summary>
        /// <param name="currentPageIndex">current page index</param>
        /// <param name="totalPages">total pages</param>
        /// <param name="requestBaseUrl">base url and id value construct a whole url, eg:http://baseURL/id</param>
        /// <param name="pagesSize">size of page numbers which are displayed </param>
        /// <param name="imageUrlForUp">image for previous page</param>
        /// <param name="imageUrlForDown">image for next page</param>
        /// <returns></returns>
        public static string CreatePager(int currentPageIndex, int totalPages, string requestBaseUrl, int pagesSize, string imageUrlForUp, string imageUrlForDown)
        {
            StringBuilder sb = new StringBuilder();
            Validate(currentPageIndex, totalPages, pagesSize);
            string href = "javascript:void(0)";

            // Determine whether there is previous page
            if (currentPageIndex / pagesSize != 0)
            {
                href = string.Format(requestBaseUrl + "/" + ((currentPageIndex / pagesSize) * pagesSize - 1).ToString());
            }

            sb.Append(string.Format("<a href=\"{0}\"><img src=\"{1}\" style=\"boder:0px;border-color: #FFFFFF\"/></a>", href, imageUrlForUp));
            sb.Append("&nbsp;&nbsp;");

            // The max size of page numbers which are displayed
            int pageIndexMaxIncrement = (currentPageIndex / pagesSize + 1) * pagesSize > totalPages ? totalPages - pagesSize * (currentPageIndex / pagesSize) : pagesSize;

            // Inital page number
            int pageIndex = pagesSize * (int)(currentPageIndex / pagesSize);
            for (int i = 0; i < pageIndexMaxIncrement; i++)
            {
                if (pageIndex == currentPageIndex)
                {
                    sb.Append(string.Format("<a href='{0}' style='{2}'>{1}</a>", "javascript:void(0)", (pageIndex + 1).ToString(), "text-decoration:none;"));

                }
                else
                {
                    sb.Append(string.Format("<a href='{0}'>{1}</a>", requestBaseUrl + "/" + pageIndex.ToString(), (pageIndex + 1).ToString()));

                }
                sb.Append("&nbsp;&nbsp;");
                pageIndex++;
            }
            href = "javascript:void(0)";

            // Determine whether there is next page
            if (pageIndex < totalPages - 1)
            {
                href = string.Format(requestBaseUrl + "/" + pageIndex.ToString());
            }
            sb.Append(string.Format("<a href=\"{0}\"><img src=\"{1}\" style=\"boder:0px;border-color: #FFFFFF\"/></a>", href, imageUrlForDown));

            return sb.ToString();
        }

        /// <summary>
        /// customize html helper method
        /// </summary>
        /// <param name="htmlHelper">HtmlHelper</param>
        /// <param name="pager">Pager</param>
        /// <returns></returns>
        public static string CreatePager<T>(this HtmlHelper htmlHelper, Pager<T> pager)
        {
            if (string.IsNullOrEmpty(pager.ImageUrlForDown.ToString()) || string.IsNullOrEmpty(pager.ImageUrlForUp.ToString()))
            {
                return CreatePager(pager.CurrentPageIndex, pager.TotalPages, pager.RequestBaseUrl);
            }
            else if (pager.PagesSize <= 1)
            {
                return CreatePager(pager.CurrentPageIndex, pager.TotalPages, pager.RequestBaseUrl, pager.ImageUrlForUp.ToString(), pager.ImageUrlForDown.ToString());
            }
            else
            {
                return CreatePager(pager.CurrentPageIndex, pager.TotalPages, pager.RequestBaseUrl, pager.PagesSize, pager.ImageUrlForUp.ToString(), pager.ImageUrlForDown.ToString());
            }
        }

        private static void Validate(int currentPageIndex, int totalPages)
        {
            if (currentPageIndex < 0)
            {
                throw new Exception("The current page indx must not be less than zero");
            }
            if (totalPages < 1)
            {
                throw new Exception("The total pages must not be less than 1");
            }
            if (currentPageIndex > totalPages - 1)
            {
                throw new Exception("The current page index can't be greater than total pages");
            }
        }

        private static void Validate(int currentPageIndex, int totalPages, int pagesSize)
        {
            Validate(currentPageIndex, totalPages);
            if (pagesSize < 0)
            {
                throw new Exception("The page size must be greater thatn zero");
            }
        }
    }
}
