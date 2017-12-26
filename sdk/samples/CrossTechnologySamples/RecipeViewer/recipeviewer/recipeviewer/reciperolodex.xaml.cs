// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Xml;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Shapes;
using System.Windows.Navigation;
using System.Windows.Media.Animation;
using Microsoft.Samples.RecipeCatalog;


namespace Microsoft.Samples.RecipeCatalog.RecipeViewer
{
    /// <summary>
    /// Interaction logic for RecipeRolodex.xaml
    /// </summary>
    public partial class RecipeRolodex : Page
    {
        private Recipes recipeList;
        private XmlDocument recipeDocument;
        private UInt32 currentIndex;
        private Storyboard pageTurnForwardStoryboard, pageTurnBackwardStoryboard;
        private NavigationWindow navWindow;

        void OnPageLoaded(object sender, EventArgs e)
        {
            //  Retrieve the Recipe list and current index from the app properties
            RecipeViewerApp app = (RecipeViewerApp)System.Windows.Application.Current;
            recipeList = (Recipes)app.Properties["RecipeList"];
            MoveToNewRecipe(currentIndex);

            // Get access to the two storyboards we'll need
            pageTurnForwardStoryboard = (Storyboard)recipeCardFile.Resources["recipeCardAnimationForward"];
            pageTurnBackwardStoryboard = (Storyboard)recipeCardFile.Resources["recipeCardAnimationBackward"];

            // Retrieve ref to parent main window
            navWindow = (NavigationWindow)Application.Current.MainWindow;
        }
  
        private void OnMenuPrevious(object sender, RoutedEventArgs e)
        {
            if (currentIndex > 0)
            {
                // Move to the new recipe and invoke the appropriate storyboard
                MoveToNewRecipe(currentIndex - 1);
                pageTurnBackwardStoryboard.Begin(RecipeCards);
            }
        }

        private void OnMenuNext(object sender, RoutedEventArgs e)
        {
            if (recipeList != null)
            {
                if (currentIndex < (recipeList.Count - 1))
                {
                    // Move to the new recipe and invoke the appropriate storyboard
                    MoveToNewRecipe(currentIndex + 1);
                    pageTurnForwardStoryboard.Begin(RecipeCards);
                }
            }
        }

        void OnMenuListPage(object sender, RoutedEventArgs e)
        {
            navWindow.Navigate(new Uri("RecipeListPage.xaml", UriKind.RelativeOrAbsolute));
        }

        private void MoveToNewRecipe(UInt32 index)
        {
            if (recipeList != null)
            {
                //Push the current recipe onto the journalling stack, so we can back/forward
                //using the NavigationWindow's buttons
                //RecipeCardJournalEntry backEntry = new RecipeCardJournalEntry((int)currentIndex, "foo");
                //navWindow.AddBackEntry(backEntry);
                currentIndex = index;

                RecipeViewerApp app = (RecipeViewerApp)System.Windows.Application.Current;
                RecipeListItem item = (RecipeListItem)recipeList[(int)currentIndex];
                recipeDocument = app.GetRecipe(item.Key);   
                currentRecipeCard.DataContext = recipeDocument;
                UpdateMenuState();
            }
        }

        // Handle enabling/disabling menu items based on index into recipe list
        private void UpdateMenuState()
        {
            if (currentIndex == recipeList.Count - 1)
                nextMenu.IsEnabled = false;
            else
                nextMenu.IsEnabled = true;

            if (currentIndex == 0)
                previousMenu.IsEnabled = false;
            else
                previousMenu.IsEnabled = true;
        }
    }

    
}