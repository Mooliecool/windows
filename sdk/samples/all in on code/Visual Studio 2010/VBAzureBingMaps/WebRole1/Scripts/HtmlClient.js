/****************************** Module Header ******************************\
* Module Name:	HtmlClient.js
* Project:		AzureBingMaps
* Copyright (c) Microsoft Corporation.
* 
* The JavaScript code for the HTML client.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

// Your Bing Maps credential.
var mapCredential = '[your credential]';
var map;
var mouseDownLocation;
// Since the HTML client is hosted in the same Web Role as the service,
// we'll use relative address.
var dataServiceUri = "DataService/TravelDataService.svc";
var items = new Array();
var addedItemIDs = new Array();
var modifiedItemIDs = new Array();
var deletedItemIDs = new Array();
var selectedItem;
var tempID = 0;

// A shortcut namespace for Microsoft.Maps.
var Bing = Microsoft.Maps;

$(document).ready(LoadMap);

function LoadMap() {
    map = new Bing.Map($('#MainMap')[0],
    {
        credentials: mapCredential,
        center: new Bing.Location(31, 121),
        zoom: 4
    });
    // Cannot use click since it fires when panning the map.
    // Bing.Events.addHandler(map, 'click', Map_OnMouseDown);
    Bing.Events.addHandler(map, 'mousedown', Map_OnMouseDown);
    Bing.Events.addHandler(map, 'mouseup', Map_OnMouseUp);

    // Query the data service to obtain data.
    $.ajax(
	{
	    type: 'GET',
	    url: dataServiceUri + '/Travels?$orderby=Time',
	    dataType: 'json',
	    success: LoadDataCompleted,
	    error: function () {
	        $('#ErrorInforamtion').text('Error retreiving data. Please try again later.');
	    }
	});
}

function LoadDataCompleted(result) {
    $(result['d']).each(function () {
        // Trim the '/'s, and display only the date part.
        this.Time = eval('new ' + this.Time.replace(/\//g, ''));
        // Add a pushpin.
        var latLong = new Bing.Location(this.Latitude, this.Longitude);
        var pushpin = new Bing.Pushpin(latLong);
        pushpin.title = this.Place;
        AttachPushpinEvents(pushpin)
        map.entities.push(pushpin);
        items.push({ PartitionKey: this.PartitionKey, RowKey: this.RowKey, Value: this, Pushpin: pushpin });
    });
    ApplyTemplate();
}

function ApplyTemplate() {
    // Remove old data.
    $('#TravelList').children().remove();
    // Apply the jQuery template to the ul.
    $('#travelTemplate').tmpl(items).appendTo('#TravelList');
    // Additional twicks on the UI that cannot be performed in the template.
    $.each($('.DatePicker'), function (index, item) {
        $(item).datepicker({ onSelect: DateChanged, dateFormat: 'yy-mm-dd' });
    });
}

function Map_OnMouseDown(e) {
    mouseDownLocation = new Bing.Point(e.pageX, e.pageY);
}

function Map_OnMouseUp(e) {
    var pixel = new Bing.Point(e.pageX, e.pageY);
    // Only add a pushpin if the user is not panning the map.
    if (mouseDownLocation != null && mouseDownLocation.x == pixel.x && mouseDownLocation.y == pixel.y) {
        var latLong = map.tryPixelToLocation(pixel, Bing.PixelReference.page);
        // Invoke the Location REST service to obtain information of the clicked place.
        $.ajax(
		{
		    url: 'http://dev.virtualearth.net/REST/v1/Locations/' + latLong.latitude + ',' + latLong.longitude + '?o=json&jsonp=LocationCallback&key=' + mapCredential,
		    dataType: 'jsonp',
		    jsonp: 'LocationCallback',
		    success: LocationCallback
		});
    }
}

/// Callback function for the Location REST service.
function LocationCallback(result) {
    if (result.resourceSets.length > 0) {
        var resourceSet = result.resourceSets[0];
        if (resourceSet.resources.length > 0) {
            var resource = resourceSet.resources[0];

            // Generate a temporary GUID for RowKey.
            var id = tempID.toString();
            var id2 = '';
            for (var i = 0; i < 12 - id.length; i++) {
                id2 += '0';
            }
            id2 += id;
            id = '00000000-0000-0000-0000-' + id2;
            tempID++;
            var time = new Date();
            time.setDate(time.getDate() + 1);

            // Add a pushpin.
            var pushpin = new Bing.Pushpin(new Bing.Location(resource.point.coordinates[0], resource.point.coordinates[1]));
            pushpin.title = resource.name;
            AttachPushpinEvents(pushpin)
            map.entities.push(pushpin);
            var item =
            {
                PartitionKey: "[UserName]",
                RowKey: id,
                Place: resource.name,
                Latitude: resource.point.coordinates[0],
                Longitude: resource.point.coordinates[1],
                Time: time
            };
            items.push({ PartitionKey: item.PartitionKey, RowKey: item.RowKey, Value: item, Pushpin: pushpin });
            addedItemIDs.push({ PartitionKey: item.PartitionKey, RowKey: item.RowKey });
            // jQuery Template doesn't support collection notification. So we have to apply the template again.
            ApplyTemplate();
        }
    }
}

// Attach mouse events for the pushpin.
function AttachPushpinEvents(pushpin) {
    Bing.Events.addHandler(pushpin, 'mouseover', Pushpin_MouseOver);
    Bing.Events.addHandler(pushpin, 'mouseout', Pushpin_MouseOut);
}

// When mouse hovers the pushpin, display the info box.
function Pushpin_MouseOver(e) {
    $('#PushpinText').text(e.target.title);
    var pushpinPopup = $('#PushpinPopup');    
    var pixel = map.tryLocationToPixel(e.target.getLocation(), Bing.PixelReference.control);
    pushpinPopup.css('left', pixel.x - 100);
    pushpinPopup.css('top', pixel.y);
    pushpinPopup.animate({ opacity: 1 });
}

// When mouse leaves the pushpin, hide the info box.
function Pushpin_MouseOut() {
    $('#PushpinPopup').animate({ opacity: 0 })
}

// Not used in this version of the sample.
function SelectItem(sender) {
    var partitionKey = $(sender).find(':input[type=hidden]')[0].value;
    var rowKey = $(sender).find(':input[type=hidden]')[1].value;
    selectedItem = SearchItems(partitionKey, rowKey);
    var ul = $(sender.currentTarget).parent();
    ul.children().removeClass('SelectedItem');
    $(sender.currentTarget).addClass('SelectedItem');
}

function DateChanged(dateText, datePicker) {
    var li = datePicker.input.parent().parent();
    var partitionKey = li.find(':input[type=hidden]')[0].value;
    var rowKey = li.find(':input[type=hidden]')[1].value;
    var item = SearchItems(partitionKey, rowKey);
    item.Value.Time = $(this).datepicker('getDate');
    // If the item is not newly created, add it to the modified list.
    if (!IsItemInArray(addedItemIDs, partitionKey, rowKey, false)) {
        modifiedItemIDs.push({ PartitionKey: partitionKey, RowKey: rowKey });
    }
}

function DeleteItem(sender) {
    var li = $(sender).parent().parent();
    var partitionKey = li.find(':input[type=hidden]')[0].value;
    var rowKey = li.find(':input[type=hidden]')[1].value;
    // If the item is not newly created, add it to the deleted list. Otherwise remove it from the added list.
    if (!IsItemInArray(addedItemIDs, partitionKey, rowKey, true)) {
        deletedItemIDs.push({ PartitionKey: partitionKey, RowKey: rowKey });
    }
    var item = SearchItems(partitionKey, rowKey, true);
    map.entities.remove(item.Pushpin);
    li.detach();
}

// Call service to save the data.
function SaveListButton_Click() {
    $(addedItemIDs).each(PostToDS);
    $(modifiedItemIDs).each(PutToDS);
    $(deletedItemIDs).each(DeleteFromDS);
    addedItemIDs = new Array();
    modifiedItemIDs = new Array();
    deletedItemIDs = new Array();
}

// Insert: Do a POST request.
function PostToDS() {
    var item = SearchItems(this.PartitionKey, this.RowKey);
    $.ajax(
    {
        type: 'POST',
        url: dataServiceUri + '/Travels',
        contentType: 'application/json; charset=utf-8',
        data: JSON.stringify(item.Value),
        datatype: 'json',
        error: function () {
            $('#ErrorInforamtion').text('Error retreiving data. Please try again later.');
        }
    });
}

// Update: Do a PUT request.
function PutToDS() {
    var item = SearchItems(this.PartitionKey, this.RowKey);
    $.ajax(
    {
        type: 'PUT',
        url: dataServiceUri + "/Travels(PartitionKey='" + this.PartitionKey + "',RowKey=guid'" + this.RowKey + "')",
        contentType: 'application/json; charset=utf-8',
        data: JSON.stringify(item.Value),
        datatype: 'json',
        error: function () {
            $('#ErrorInforamtion').text('Error retreiving data. Please try again later.');
        }
    });
}

// Delete: Do a DELETE request.
function DeleteFromDS() {
    $.ajax(
    {
        type: 'DELETE',
        url: dataServiceUri + "/Travels(PartitionKey='" + this.PartitionKey + "',RowKey=guid'" + this.RowKey + "')",
        contentType: 'application/json; charset=utf-8',
        datatype: 'json',
        error: function () {
            $('#ErrorInforamtion').text('Error retreiving data. Please try again later.');
        }
    });
}

// Utility function to search the item based on key.
// Remove the item if needed.
function SearchItems(partitionKey, rowKey, remove) {
    for (var i = 0; i < items.length; i++) {
        var item = items[i];
        if (item.PartitionKey == partitionKey && item.RowKey == rowKey) {
            if (remove) {
                items.splice(i, 1);
            }
            return item;
        }
    }
    return null;
}

// Utility function to see if the item is in the list.
// Remove the item if needed.
function IsItemInArray(array, partitionKey, rowKey, remove) {
    for (var i = 0; i < array.length; i++) {
        var item = array[i];
        if (item.PartitionKey == partitionKey && item.RowKey == rowKey) {
            if (remove) {
                array.splice(i, 1);
            }
            return true;
        }
    }
    return false;
}

function formatDate(date) {
    return $.datepicker.formatDate('yy-mm-dd', date);
}