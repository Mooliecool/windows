/************************************* Module Header **************************************\
* Module Name:    Default.aspx.vb
* Project:        CSASPNETImageMap
* Copyright (c) Microsoft Corporation
*
* The project illustrates how to use ImageMap to create an introduction of the planets in 
* Solar System via VB.NET language. When the planet in the image is clicked, the brief 
* information of this planet will be displayed under the image and the iframe will navigate 
* to the corresponding page in WikiPedia.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
* EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
* MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\******************************************************************************************/

using System;
using System.Collections.Generic;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;


namespace CSASPNETImageMap
{
    public partial class _Default : System.Web.UI.Page
    {
        protected string[] arrPlanets = { "Mercury", "Venus", "Earth", "Mars", "Jupiter", "Saturn", "Uranus", "Neptune" };

        protected void Page_Load(object sender, EventArgs e)
        {

        }

        protected void imgMapSolarSystem_Click(object sender, ImageMapEventArgs e)
        {
            ifSelectResult.Attributes["src"] = "http://en.wikipedia.org/wiki/" + e.PostBackValue;

            switch (e.PostBackValue)
            {
                case "Sun":
                    //when user clicks on the Sun area
                    lbDirection.Text = "The Sun is the star at the centre of the Solar System.";

                    break;
                case "Mercury":
                    lbDirection.Text = "Mercury is the innermost and smallest planet in the Solar System.";

                    break;
                case "Venus":
                    lbDirection.Text = "Venus is the second-closest planet to the Sun.";

                    break;
                case "Earth":
                    lbDirection.Text = "Earth is the third planet from the Sun. It is also referred to as the World and the Blue Planet.";

                    break;
                case "Mars":
                    lbDirection.Text = "Mars is the fourth planet from the Sun in the Solar System.";

                    break;
                case "Jupiter":
                    lbDirection.Text = "Jupiter is the fifth planet from the Sun and the largest planet within the Solar System.";

                    break;
                case "Saturn":
                    lbDirection.Text = "Saturn is the sixth planet from the Sun and the second largest planet in the Solar System, after Jupiter.";

                    break;
                case "Uranus":
                    lbDirection.Text = "Uranus is the seventh planet from the Sun, and the third-largest and fourth most massive planet in the Solar System.";

                    break;
                case "Neptune":
                    lbDirection.Text = "Neptune is the eighth planet from the Sun in our Solar System.";

                    break;
                default:

                    break;
            }
        }
    }
}
