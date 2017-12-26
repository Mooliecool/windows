=============================================================================
                     CSASPNETBingMaps Project Overview
=============================================================================

Use:

 This project illustrates how to embed Bing Maps in an ASP.NET page as well 
 as how to display the map according to several options and how to find a 
 location via an input.

/////////////////////////////////////////////////////////////////////////////
Demo the Sample.

Step1: Browse the Default.aspx from the sample and you can find a map located
at the left part and several inputs at the right part in the page.

Step2: You can move the map via dragging the mouse in the Map area as well as
zoom the map by using the the mouse wheel.

Step3: If you want to find a city, New York for example, in the map, you can 
input the city name in the TextBox after "Location" and click Submit button.

Step4: If you want to show a map according to your own habits, you can change 
the options in Show a Map area. For an instance, you can change the zoom level
and set both Latitude and Longitude in the accordingly TextBox.

/////////////////////////////////////////////////////////////////////////////
Code Logical:

Step1: Create a C# ASP.NET Empty Web Application in Visual Studio 2010.

Step2: Add a Default ASP.NET page into the application.

Step3: Add a table to the page with two cells in a row. The left cell is for
Bing Maps and the right cell is for more options and a search block.

Step4: Add a Panel named pnlBingMap in the left cell. Also, it doesn't matter
if you are using a div whose id is pnlBingMap as the panel will be rendered 
as a div when the page runs anyway.

Step5: Create an option and a search block in the right cell. You can follow 
the sample or even copy the code to finish this HTML coding work.

Step6: Add Bing Maps JavaScript API link to the page. The number 6.3 in the 
querystring stands for the API verion. It may be changed when you are tesing 
this sample. For a latest version, please refer to link: 
http://www.microsoft.com/maps/developers/.

<script type="text/javascript" 
	    src="http://ecn.dev.virtualearth.net/mapcontrol/mapcontrol.ashx?v=6.3" />

Step7: Create LoadMap funcion and call it when the page loads.

	function LoadMap() {
		map = new VEMap('pnlBingMap');

		var LA = new VELatLong(34.0540, -118.2370);
		map.LoadMap(LA, 12, style, false, VEMapMode.Mode2D, true, 1);
	}

NOTE: VEMap is a class defined in the Bing Map API. It achieves almost all
what we need to operate the map like loading a map, changing map options or 
adding new shapes and pushpins to the map.

Step8: Create the FindLoc() function and bind it to the click event of the
Submit button in the search block.

    function FindLoc() {
        var loc = document.getElementById("txtLocation").value;
        try {
            map.Find(null, loc);
        } catch (e) {
            alert(e.message);
        }
    }
		
Step9: Create the SetMap() function and make the Submit button in the Show a 
Map block link to it.

	function SetMap() {
		var lat = document.getElementById("txtLatitude").value;
		var lng = document.getElementById("txtLongitude").value;

		if (lng == "" | lat == "") {
			alert("You need to input both Latitude and Longitude first.");
			return;
		}

		var ddlzoom = document.getElementById("ddlZoomLevel");
		var zoom = ddlzoom.options[ddlzoom.selectedIndex].value;

		map.SetCenter(new VELatLong(lat, lng));
		map.SetMapStyle(style);
		map.SetZoomLevel(zoom);
	}

/////////////////////////////////////////////////////////////////////////////
References:

Bing Maps
# Bing Maps Platform - AJAX Map Control Interactive SDK
http://www.microsoft.com/maps/isdk/ajax/

Bing Maps
# Developers Getting Start
http://www.microsoft.com/maps/developers/

MSDN:
# VEMap Class
http://msdn.microsoft.com/en-us/library/bb429586.aspx

/////////////////////////////////////////////////////////////////////////////