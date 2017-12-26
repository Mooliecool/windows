// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Xml;
using System.Windows;
using System.Windows.Input;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Navigation;
using Microsoft.Samples.RecipeCatalog;
namespace Microsoft.Samples.RecipeCatalog.RecipeViewer
{
    /// <summary>
    /// Interaction logic for RecipeList.xaml
    /// </summary>

    public partial class RecipeListPage : Page
    {
        NavigationWindow navWindow;

        void OnPageLoaded(object sender, EventArgs e)
        {
            RecipeListBox.ItemsSource = (Recipes)Application.Current.Properties["RecipeList"];
            navWindow = (NavigationWindow)Application.Current.MainWindow;
        }

        void OnRecipeListNavigate(object sender, RequestNavigateEventArgs e)
        {
            RecipeViewerApp app = (RecipeViewerApp)System.Windows.Application.Current;
            RecipeListItem item = (RecipeListItem)((Hyperlink)sender).Tag;
            app.Properties["CurrentRecipe"] = item.Key;
        }

        void OnMenuRolodexPage(object sender, RoutedEventArgs e)
        {
            navWindow.Navigate(new Uri("RecipeRolodex.xaml", UriKind.RelativeOrAbsolute));
        }
    }
}