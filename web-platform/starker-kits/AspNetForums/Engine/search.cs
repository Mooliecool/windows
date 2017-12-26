using System;
using AspNetForums.Components;
using System.Web;

namespace AspNetForums {
    /// <summary>
    /// This class contains the method used to perform searches.  The Search Web control uses
    /// this class to perform its searching.
    /// </summary>
    public class Search {

        /// <summary>
        /// This method returns the result of a search.  It returns the exact number of records as determined
        /// by the RecsPerPage input parameter.
        /// </summary>
        /// <param name="ToSearch">Specifies what, specifically, to search.  Users can opt to search
        /// for posts (PostsSearch) or search for post authors (PostsBySearch).</param>
        /// <param name="SearchWhat">Instructs how to interpret the search query.  Values can be: SearchAllWords,
        /// SearchAnyWord, or SearchExactPhrase.</param>
        /// <param name="ForumToSearch">Indicates the ID of the Forum to search.  To search all forums,
        /// specify a value of -1 here.</param>
        /// <param name="SearchTerms">Specifies the user's typed-in search query.</param>
        /// <param name="Page">Indicates what page of the search results to display.</param>
        /// <param name="RecsPerPage">Determines how many records per page to display.</param>
        /// <returns>A PostCollection containing RecsPerPage Posts (perhaps less).</returns>
        /// <remarks>Page is indexed starting at 1.  That is, to get the first page of search results
        /// pass a value of 1 as the Page parameter.</remarks>
        public static PostCollection PerformSearch(ToSearchEnum ToSearch, SearchWhatEnum SearchWhat,
            int ForumToSearch, String SearchTerms, int Page, int RecsPerPage) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            // ensure that Page > 0 and RecsPerPage > 0
            if (RecsPerPage <= 0) RecsPerPage = 10;
            if (Page <= 0) Page = 1;

            return dp.GetSearchResults(ToSearch, SearchWhat, ForumToSearch, SearchTerms, Page, RecsPerPage, HttpContext.Current.User.Identity.Name);
        }

    }
}
