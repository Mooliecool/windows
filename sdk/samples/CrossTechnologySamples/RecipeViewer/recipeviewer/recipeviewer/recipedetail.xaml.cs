// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Media.Imaging;
using System.Xml;
using System.Collections;
using System.Windows.Navigation;

namespace Microsoft.Samples.RecipeCatalog.RecipeViewer
{
    /// <summary>
    /// Interaction logic for RecipeDetail.xaml
    /// </summary>
    public partial class RecipeDetail : Page
    {
        XmlDocument recipeDocument;
        ArrayList recipeAttachments;
        NavigationWindow navWindow;

        private void OnPageLoading(object sender, EventArgs e)
        {
            //  Retrieve the current recipe object from the app properties
            RecipeViewerApp app = (RecipeViewerApp)System.Windows.Application.Current;
            navWindow = (NavigationWindow)app.MainWindow;
            Recipes list = (Recipes)app.Properties["RecipeList"];
            String key = (String)app.Properties["CurrentRecipe"];
            RecipeListItem item = list.Find(key);
            if (item != null)
            {
                //  Retrieve the full recipe document
                recipeDocument = app.GetRecipe(item.Key);
                if (recipeDocument != null)
                {
                    RecipePanel.DataContext = recipeDocument;

                    //  Get the list of attachments
                    recipeAttachments = item.Attachments;

                    //  Get the first picture attachment,
                    //  and assign it to the image in the XAML page
                    foreach (RecipeAttachment attachment in recipeAttachments)
                    {
                        if (attachment.GetType() == typeof(BitmapAttachment))
                        {
                            BitmapSource bs = (BitmapSource)attachment.Content;
                            if (bs != null)
                                RecipeImage.Source = bs;
                            break;
                        }
                    }
                }
            }
        }

        private void OnMenuRolodexPage(object sender, EventArgs e)
        {
            navWindow.Navigate(new Uri("RecipeRolodex.xaml", UriKind.RelativeOrAbsolute));
        }

        private void OnMenuListPage(object sender, EventArgs e)
        {
            navWindow.Navigate(new Uri("RecipeListPage.xaml", UriKind.RelativeOrAbsolute));
        }

    }
}