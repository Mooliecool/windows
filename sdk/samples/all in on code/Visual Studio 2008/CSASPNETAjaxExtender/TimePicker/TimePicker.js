/// <reference name="MicrosoftAjax.js"/>


Type.registerNamespace("CSASPNETAjaxExtender");

CSASPNETAjaxExtender.TimePicker = function(element) {
    CSASPNETAjaxExtender.TimePicker.initializeBase(this, [element]);
    //element property 

    this._holder = null;
    this._backgroundImg = null;
    this._closeImg = null;
    this._minImg = null;
    this._hourImg = null;
    this._centerImg = null;
    this._ampm = null;
    this._errorSpan = null;

    //property
    this._cssClass = null;
    this._timeType = null;
    //handler
    this._popupControl_clickHandler = null;
    this._element_focusHandler = null;
    this._element_keyupHandler = null;
    this._element_blurHandler = null;
    this._closeImg_mousedownHandler = null;
    this._ampm_mousedownHandler = null;
    this._mousedownHandler = null;
    this._mousemoveHandler = null;
    this._mouseupHandler = null;

    //
    this._movedPointName = null;
    this._selectedTime = null;
    this._isOpened = null;

}

CSASPNETAjaxExtender.TimePicker.prototype = {


    //initialize
    initialize: function() {
        CSASPNETAjaxExtender.TimePicker.callBaseMethod(this, 'initialize');
        this.createDelegates();
        $addHandlers(this.get_element(), {
            "focus": this._element_focusHandler,
            "keyup": this._element_keyupHandler,
            "blur": this._element_blurHandler
        }, this);
    },
    dispose: function() {
        this.clearDelegates();
        $clearHandlers(this.get_element());
        CSASPNETAjaxExtender.TimePicker.callBaseMethod(this, 'dispose');
    },
    createDelegates: function() {
        this._element_focusHandler = Function.createDelegate(this, this._element_onfocus);
        this._element_keyupHandler = Function.createDelegate(this, this._element_onkeyup);
        this._element_blurHandler = Function.createDelegate(this, this._element_onblur);
        this._closeImg_mousedownHandler = Function.createDelegate(this, this._closeImg_onmousedown);
        this._ampm_mousedownHandler = Function.createDelegate(this, this._ampm_onmousedown);
        this._mousedownHandler = Function.createDelegate(this, this._onmousedown);
        this._mousemoveHandler = Function.createDelegate(this, this._onmousemove);
        this._mouseupHandler = Function.createDelegate(this, this._onmouseup);

    },
    clearDelegates: function() {
        this._element_focusHandler = null;
        this._element_keyupHandler = null;
        this._element_blurHandler = null;
        this._closeImg_mousedownHandler = null;
        this._ampm_mousedownHandler = null;
        this._mousedownHandler = null;
        this._mousemoveHandler = null;
        this._mouseupHandler = null;
    },

    //const
    AMPM_CssClass: "ampm",
    BackGroundImg_CssClass: "backgroundClock",
    CenterImg_CssClass: "centerPoint",
    MinImg_CssClass: "minutePointer",
    HourImg_CssClass: "hourPointer",
    CloseImg_CssClass: "closeButton",
    CssClass: 'timePicker',
    DetectableGridValueRage: 1, //detectable range when you select a pointer

    //size of every element, if you want to custom the picture about clock and pointers, you need modify these variables
    BackgroundImgSize: { OffsetWidth: 123, OffsetHeight: 123 },
    HourImgSize: { OffsetWidth: 58, OffsetHeight: 58 },
    MinImgSize: { OffsetWidth: 96, OffsetHeight: 97 },
    CenterImgSize: { OffsetWidth: 6, OffsetHeight: 5 },
    CloseImgSize: { OffsetWidth: 12, OffsetHeight: 12 },
    TimeType: { H12: 0, H24: 1 },
    AMPM: { AM: 'AM', PM: 'PM' },


    _element_onfocus: function(e) {
        //event of TargetControl onfocus
        this._ensureTimePicker();
        this.show();
    },

    _element_onkeyup: function(e) {
        //event of TargetControl onkeyup
        if (!this._isValidTime(this.get_element().value)) {
            if (this._errorSpan != null)
                this._errorSpan.innerHTML = 'time type is invilid';
            return;
        }
        if (this._holder.style.display != 'none' && this._holder.style.visibility != 'hidden')
            this.set_selectedTime(this.get_element().value);
    },

    _element_onblur: function(e) {
        //event of TargetControl onblur
        if (!this._isValidTime(this.get_element().value)) {
            if (this._errorSpan != null)
                this._errorSpan.innerHTML = 'time type is invilid';
        }
    },

    _closeImg_onmousedown: function(e) {
        //event of close image mousedown
        this.hide();
        e.preventDefault();
        e.stopPropagation();
        return false;
    },

    _ampm_onmousedown: function(e) {
        //Event of ampm hyperlink mousedown. To stop propagation in this.mousedown, we have to define mousedown to deal with
        //instead of onclick event.
        ampm_innerHTML = (this._ampm.innerHTML == "" || this._ampm.innerHTML == this.AMPM.PM) ? this.AMPM.AM : this.AMPM.PM;
        var oldTime = this.get_selectedTime(this.TimeType.H12);
        var newTime = oldTime.split(' ')[0] + ' ' + ampm_innerHTML;

        //select a new time value
        this.set_selectedTime(newTime);
        //set the value of targetcontrol to a time
        this.get_element().value = this._convertTimeTypeByString(newTime, this.get_timeType());


        e.preventDefault(); //prevent default event to fire
        e.stopPropagation(); //stop propagation so that it won't generate child element to fire mousedown event meanwhile
        return false;

    },

    _onmousedown: function(e) {
        //event of hoder mousedown
        var gridValue = this._getGridValueByCursorCoordinate(e);
        var oldTime = this.get_selectedTime(this.TimeType.H24).split(':');

        if (this._isTimeInRange(parseInt(oldTime[1], 10), { from: gridValue - this.DetectableGridValueRage, to: gridValue + this.DetectableGridValueRage })) {

            this._movedPointName = "min";
            //$addHandlers
            $addHandler(this._holder, 'mousemove', this._mousemoveHandler);
            $addHandler(this._holder, 'mouseup', this._mouseupHandler);

        }
        else if (this._isTimeInRange((parseInt(oldTime[0], 10) % 12) * 5, { from: gridValue - this.DetectableGridValueRage, to: gridValue + this.DetectableGridValueRage })) {
            this._movedPointName = "hour";
            //$addHandlers
            $addHandler(this._holder, 'mousemove', this._mousemoveHandler);
            $addHandler(this._holder, 'mouseup', this._mouseupHandler);

        }
        else {

        }

    },


    _onmousemove: function(e) {
        //event of hoder mousemove
        var gridValue = this._getGridValueByCursorCoordinate(e);

        var movedValue = this._movedPointName == "min" ? gridValue : Math.round(gridValue / 5) % 12;
        var oldTime = this.get_selectedTime(this.TimeType.H24).split(':');
        var ampm = this._ampm.innerHTML;
        var hours, mins;
        if (this._movedPointName === 'min') {
            hours = parseInt(oldTime[0], 10) % 12;
            mins = movedValue;
        }
        else {
            hours = movedValue;
            mins = parseInt(oldTime[1], 10);
        }

        hours = (hours == 0) ? 12 : hours;
        var newTime = hours + ":" + mins + ' ' + ampm;
        this.set_selectedTime(newTime);
        if (this._movedPointName === 'min')
            this.get_element().value = this._convertTimeType(hours, mins, ampm, this.get_timeType());
        else
            this.get_element().value = this._convertTimeType(hours, mins, ampm, this.get_timeType());

        e.preventDefault();
        e.stopPropagation();
        return false;
    },

    _onmouseup: function(e) {
        //event of hoder mouseup
        //Release mousemove and mouseup event of holder object in this event
        var cache;
        cache = this._holder._events['mousemove'];
        if (cache instanceof Array)
            $removeHandler(this._holder, 'mousemove', this._mousemoveHandler);
        cache = this._holder._events['mouseup'];
        if (cache instanceof Array)
            $removeHandler(this._holder, 'mouseup', this._mouseupHandler);

        if (this._movedPointName === "hour")
            this.raiseHourSelectionChanged();
        if (this._movedPointName === "min")
            this.raiseMinuteSelectionChanged();

        e.preventDefault();
        e.stopPropagation();
        return false;
    },

    _isTimeInRange: function(time, range) {

        var x = (range.from + 60) % 60;
        var y = (range.to + 60) % 60;
        if (y >= x) {
            if (time >= x && time <= y)
                return true;
            else
                return false;
        }
        else {
            if (time >= x || time <= y)
                return true;
            else
                return false;
        }
    },

    _getGridValueByCursorCoordinate: function(e) {
        /// <summary>
        /// Get the value of the time calibration that mouse's on
        /// </summary>
        /// <param name="handler" type="Function">
        /// The handler to remove from the event.
        /// </param>
        /// <returns />
        var cursor_coordinate = { 'x': e.clientX, 'y': e.clientY };
        var backgroundImg_coordinate = { 'x': Sys.UI.DomElement.getLocation(this._backgroundImg).x, 'y': Sys.UI.DomElement.getLocation(this._backgroundImg).y }
        var x = cursor_coordinate.x - backgroundImg_coordinate.x - this._backgroundImg.offsetWidth / 2;
        var y = cursor_coordinate.y - backgroundImg_coordinate.y - this._backgroundImg.offsetHeight / 2;


        if (x == 0 && y == 0) {
            return 0;
        }
        else if (x == 0) {
            if (y < 0)
                return 0;
            else
                return 30;
        }
        else if (y == 0) {
            if (x < 0)
                return 45;
            else
                return 15;
        }
        var basedAngle = Math.acos(Math.abs(y) / Math.sqrt(x * x + y * y)) * 180 / Math.PI;

        var angle; //= ((y / Math.abs(y) + 1) * 90 - x * y / Math.abs(x * y) * basedAngle + 360) % 360;

        if (y > 0 && x > 0)
            angle = 180 - basedAngle;
        else if (y > 0 && x < 0)
            angle = 180 + basedAngle;
        else if (y < 0 && x > 0)
            angle = basedAngle;
        else if (y < 0 && x < 0)
            angle = (-basedAngle + 360) % 360;

        var gridValue = this._getGridValueByAngle(angle);

        return gridValue;
    },

    _getGridValueByAngle: function(angle) {

        return parseInt(angle / 6);
    },


    _ensureTimePicker: function() {
        //Create TimePicker elements    
        if (this._holder == null || this._holder.childNodes.length === 0) {

            //holder
            this._holder = NewElement("div", {
                'style': { 'position': 'absolute', 'zIndex': 39999 }
            });
            document.forms[0].appendChild(this._holder);

            //background image
            this._backgroundImg = NewElement("div", { 'style': {
                'position': 'absolute',
                'height': this.BackgroundImgSize.OffsetHeight + 'px',
                'width': this.BackgroundImgSize.OffsetWidth + 'px',
                'zIndex': 40001
            }
            });


            this._holder.appendChild(this._backgroundImg);

            //To fix the issu ein IE6, embed an iframe into TimePicker.
            //			if (window.ie6){
            //				var this._iframe = NewElement("iframe", 
            //                             {'src':'about:Blank',
            //											'style':{'width':this.options.clockSize.width,
            //												'position':'absolute',
            //												'z-index':40000,
            //												'height':this.options.clockSize.height},
            //											'frameborder':0
            //										});

            //		        this._holder.appendChild(iframe);				
            //            }


            //close image
            this._closeImg = NewElement("div", { 'style': {
                'position': 'absolute',
                'zIndex': 40006,
                'height': this.CloseImgSize.OffsetHeight + 'px',
                'width': this.CloseImgSize.OffsetWidth + 'px',
                'marginLeft': (this.BackgroundImgSize.OffsetWidth - 12) + "px",
                'cursor': 'pointer'
            }
            });

            this._holder.appendChild(this._closeImg);


            //min image
            this._minImg = NewElement("div", {
                'style': {
                    'position': 'absolute',
                    'zIndex': 40002,
                    'width': this.MinImgSize.OffsetWidth + "px",
                    'height': this.MinImgSize.OffsetHeight + "px",
                    'marginTop': (this.BackgroundImgSize.OffsetHeight - this.MinImgSize.OffsetHeight) / 2 + 'px',
                    'marginLeft': (this.BackgroundImgSize.OffsetWidth - this.MinImgSize.OffsetWidth) / 2 + 'px'

                }
            });

            this._holder.appendChild(this._minImg);

            //hour image
            this._hourImg = NewElement("div", {
                'style': {
                    'position': 'absolute',
                    'zIndex': 40003,
                    'width': this.HourImgSize.OffsetWidth + "px",
                    'height': this.HourImgSize.OffsetHeight + "px",
                    'marginTop': (this.BackgroundImgSize.OffsetHeight - this.HourImgSize.OffsetHeight) / 2 + 'px',
                    'marginLeft': (this.BackgroundImgSize.OffsetWidth - this.HourImgSize.OffsetWidth) / 2 + 'px'

                }
            });

            this._holder.appendChild(this._hourImg);

            //center image
            this._centerImg = NewElement("div", {

                'style': {
                    'position': 'absolute', 'zIndex': 40004,
                    'width': this.CenterImgSize.OffsetWidth + "px",
                    'height': this.CenterImgSize.OffsetHeight + "px",
                    'marginTop': (this.BackgroundImgSize.OffsetHeight - this.CenterImgSize.OffsetHeight) / 2 + 'px',
                    'marginLeft': (this.BackgroundImgSize.OffsetWidth - this.CenterImgSize.OffsetWidth) / 2 + 'px'
                }
            });

            this._holder.appendChild(this._centerImg);

            //am pm
            this._ampm = NewElement("a", {
                'href': '#',
                'innerHTML': this.AMPM.AM,
                'style': {
                    'position': 'absolute',
                    'zIndex': 40005,
                    'display': 'block',
                    'marginTop': (this.BackgroundImgSize.OffsetHeight + (this._backgroundImg.offsetHeight / 4)) / 2 + 'px',
                    'marginLeft': (this.BackgroundImgSize.OffsetHeight - 20) / 2 + 'px'
                }
            });
            this._holder.appendChild(this._ampm);
            //Assign css class on the elements.
            Sys.UI.DomElement.addCssClass(this._holder, this.get_cssClass());
            Sys.UI.DomElement.addCssClass(this._backgroundImg, this.BackGroundImg_CssClass);
            Sys.UI.DomElement.addCssClass(this._closeImg, this.CloseImg_CssClass);
            Sys.UI.DomElement.addCssClass(this._minImg, this.MinImg_CssClass);
            Sys.UI.DomElement.addCssClass(this._hourImg, this.HourImg_CssClass);
            Sys.UI.DomElement.addCssClass(this._centerImg, this.CenterImg_CssClass);
            Sys.UI.DomElement.addCssClass(this._ampm, this.AMPM_CssClass);
            //Define the event handler for holder and child elements.
            $addHandler(this._closeImg, "mousedown", this._closeImg_mousedownHandler);
            $addHandler(this._ampm, "mousedown", this._ampm_mousedownHandler);
            $addHandler(this._holder, "mousedown", this._mousedownHandler);

        }
        this._ensureLocation();
    },

    _ensureLocation: function() {
        //set location to picker
        var textbox = this.get_element();
        var textbox_location = Sys.UI.DomElement.getLocation(textbox);
        Sys.UI.DomElement.setLocation(this._holder, textbox_location.x, textbox_location.y + textbox.offsetHeight);

    },

    show: function() {
        //Pop out the TimePicker
        if (!this._isOpened) {
            var eventArgs = new Sys.CancelEventArgs();
            this.raiseShowing(eventArgs);
            if (eventArgs.get_cancel()) {
                return;
            }
            this.set_selectedTime(this.get_element().value);
            this._holder.style.display = "block";
            this._isOpened = true;
            this.raiseShown();
        }

    },

    hide: function() {
        //Close the TimePicker
        if (this._isOpened) {

            var eventArgs = new Sys.CancelEventArgs();
            this.raiseHiding(eventArgs);
            if (eventArgs.get_cancel()) {
                return;
            }
            this._holder.style.display = "none";
            this._isOpened = false;
            this.raiseHidden();
        }
    },


    //event handler
    add_showing: function(handler) {
        /// <summary>
        /// Adds an event handler for the <code>showiwng</code> event.
        /// </summary>
        /// <param name="handler" type="Function">
        /// The handler to add to the event.
        /// </param>
        /// <returns />

        this.get_events().addHandler("showing", handler);
    },
    remove_showing: function(handler) {
        /// <summary>
        /// Removes an event handler for the <code>showing</code> event.
        /// </summary>
        /// <param name="handler" type="Function">
        /// The handler to remove from the event.
        /// </param>
        /// <returns />

        this.get_events().removeHandler("showing", handler);
    },
    raiseShowing: function(eventArgs) {
        /// <summary>
        /// Raise the showing event
        /// </summary>
        /// <param name="eventArgs" type="Sys.CancelEventArgs" mayBeNull="false">
        /// Event arguments for the showing event
        /// </param>
        /// <returns />

        var handler = this.get_events().getHandler('showing');
        if (handler) {
            handler(this, eventArgs);
        }
    },

    add_shown: function(handler) {
        /// <summary>
        /// Adds an event handler for the <code>shown</code> event.
        /// </summary>
        /// <param name="handler" type="Function">
        /// The handler to add to the event.
        /// </param>
        /// <returns />

        this.get_events().addHandler("shown", handler);
    },
    remove_shown: function(handler) {
        /// <summary>
        /// Removes an event handler for the <code>shown</code> event.
        /// </summary>
        /// <param name="handler" type="Function">
        /// The handler to remove from the event.
        /// </param>
        /// <returns />

        this.get_events().removeHandler("shown", handler);
    },
    raiseShown: function() {
        /// <summary>
        /// Raise the <code>shown</code> event
        /// </summary>
        /// <returns />

        var handlers = this.get_events().getHandler("shown");
        if (handlers) {
            handlers(this, Sys.EventArgs.Empty);
        }
    },

    add_hiding: function(handler) {
        /// <summary>
        /// Adds an event handler for the <code>hiding</code> event.
        /// </summary>
        /// <param name="handler" type="Function">
        /// The handler to add to the event.
        /// </param>
        /// <returns />

        this.get_events().addHandler("hiding", handler);
    },
    remove_hiding: function(handler) {
        /// <summary>
        /// Removes an event handler for the <code>hiding</code> event.
        /// </summary>
        /// <param name="handler" type="Function">
        /// The handler to remove from the event.
        /// </param>
        /// <returns />

        this.get_events().removeHandler("hiding", handler);
    },
    raiseHiding: function(eventArgs) {
        /// <summary>
        /// Raise the hiding event
        /// </summary>
        /// <param name="eventArgs" type="Sys.CancelEventArgs" mayBeNull="false">
        /// Event arguments for the showing event
        /// </param>
        /// <returns />

        var handler = this.get_events().getHandler('hiding');
        if (handler) {
            handler(this, eventArgs);
        }
    },

    add_hidden: function(handler) {
        /// <summary>
        /// Adds an event handler for the <code>hidden</code> event.
        /// </summary>
        /// <param name="handler" type="Function">
        /// The handler to add to the event.
        /// </param>
        /// <returns />

        this.get_events().addHandler("hidden", handler);
    },
    remove_hidden: function(handler) {
        /// <summary>
        /// Removes an event handler for the <code>hidden</code> event.
        /// </summary>
        /// <param name="handler" type="Function">
        /// The handler to remove from the event.
        /// </param>
        /// <returns />

        this.get_events().removeHandler("hidden", handler);
    },
    raiseHidden: function() {
        /// <summary>
        /// Raise the <code>hidden</code> event
        /// </summary>
        /// <returns />

        var handlers = this.get_events().getHandler("hidden");
        if (handlers) {
            handlers(this, Sys.EventArgs.Empty);
        }
    },

    add_hourSelectionChanged: function(handler) {
        /// <summary>
        /// Adds an event handler for the <code>hourSelectionChanged</code> event.
        /// </summary>
        /// <param name="handler" type="Function">
        /// The handler to add to the event.
        /// </param>
        /// <returns />

        this.get_events().addHandler("hourSelectionChanged", handler);
    },
    remove_hourSelectionChanged: function(handler) {
        /// <summary>
        /// Removes an event handler for the <code>hourSelectionChanged</code> event.
        /// </summary>
        /// <param name="handler" type="Function">
        /// The handler to remove from the event.
        /// </param>
        /// <returns />

        this.get_events().removeHandler("hourSelectionChanged", handler);
    },
    raiseHourSelectionChanged: function() {
        /// <summary>
        /// Raise the hourSelectionChanged event
        /// </summary>
        /// <param name="eventArgs" type="Sys.CancelEventArgs" mayBeNull="false">
        /// Event arguments for the showing event
        /// </param>
        /// <returns />

        var handler = this.get_events().getHandler('hourSelectionChanged');
        if (handler) {
            handler(this, Sys.EventArgs.Empty);
        }
    },

    add_minuteSelectionChanged: function(handler) {
        /// <summary>
        /// Adds an event handler for the <code>minuteSelectionChanged</code> event.
        /// </summary>
        /// <param name="handler" type="Function">
        /// The handler to add to the event.
        /// </param>
        /// <returns />

        this.get_events().addHandler("minuteSelectionChanged", handler);
    },
    remove_minuteSelectionChanged: function(handler) {
        /// <summary>
        /// Removes an event handler for the <code>minuteSelectionChanged</code> event.
        /// </summary>
        /// <param name="handler" type="Function">
        /// The handler to remove from the event.
        /// </param>
        /// <returns />

        this.get_events().removeHandler("minuteSelectionChanged", handler);
    },
    raiseMinuteSelectionChanged: function() {
        /// <summary>
        /// Raise the minuteSelectionChanged event
        /// </summary>
        /// <param name="eventArgs" type="Sys.CancelEventArgs" mayBeNull="false">
        /// Event arguments for the showing event
        /// </param>
        /// <returns />

        var handler = this.get_events().getHandler('minuteSelectionChanged');
        if (handler) {
            handler(this, Sys.EventArgs.Empty);
        }
    },



    //properties
    get_timeType: function() {
        return this._timeType;
    },

    set_timeType: function(val) {
        if (this._timeType !== val) {
            this._timeType = val;
            this.raisePropertyChanged('timeType');
        }
    },



    get_errorSpan: function() {
        return this._errorSpan;
    },

    set_errorSpan: function(val) {
        if (this._errorSpan !== val) {
            this._errorSpan = val;
            this.raisePropertyChanged('errorSpan');
        }
    },

    get_cssClass: function() {
        if (this._cssClass === "")
            this._cssClass = this.CssClass;
        return this._cssClass;
    },

    set_cssClass: function(val) {
        if (this._cssClass !== val) {
            this._cssClass = val;
            this.raisePropertyChanged('cssClass');
        }
    },




    get_selectedTime: function(timeType) {
        if (this._selectedTime == null || this._selctedTime === "")
            this._selectedTime = "0:00";
        this._selectedTime = this._convertTimeTypeByString(this._selectedTime, timeType);

        return this._selectedTime;
    },

    set_selectedTime: function(val) {
        if (val == null || val === "")
            val = "0:00";
        val = val.trim();
        if (!this._isValidTime(val))
            return;
        //As default internal, it converts to 24 hours format to count.
        val = this._convertTimeTypeByString(val, this.TimeType.H24);

        var gottenTime = this.get_selectedTime(this.TimeType.H24);
        if (gottenTime !== val) {

            var oldTime = gottenTime.split(':');
            var newTime = val.split(":");

            if ((parseInt(oldTime[0], 10) % 12) !== (parseInt(newTime[0], 10) % 12)) {
                //hour img change
                var hourPoint = parseInt(newTime[0], 10) % 12;
                var left = this.HourImgSize.OffsetWidth * hourPoint;

                if (this._hourImg != null) {
                    this._hourImg.style.backgroundPosition = -left + 'px 0';
                }

            }
            if (oldTime[1] !== newTime[1]) {
                //min img change
                var minPoint = parseInt(newTime[1], 10);
                var left = this.MinImgSize.OffsetWidth * minPoint;
                if (this._minImg != null) {
                    this._minImg.style.backgroundPosition = -left + 'px 0';
                }

            }

            //ampm change
            this._ampm.innerHTML = parseInt(newTime[0], 10) >= 12 ? this.AMPM.PM : this.AMPM.AM;

            this._selectedTime = val;
            this.raisePropertyChanged('selectedTime');
        }
    },

    _isValidTime: function(val) {
        //check if it is a valid time format
        if (!this._isFormatedTime(val, this.TimeType.H24) && !this._isFormatedTime(val, this.TimeType.H12))
            return false;
        return true;

    },

    _isFormatedTime: function(val, timeType) {
        var bo = true;

        if (this.TimeType.H12 === timeType)
            bo = val.match(new RegExp('^(1[0-2]|0?[1-9]):([0-5]?[0-9]) (AM|PM|am|pm)$'));
        else if (this.TimeType.H24 === timeType)
            bo = val.match(new RegExp('^(2[0-3]|[01]?[0-9]):([0-5]?[0-9])$'));
        return bo;
    },

    _convertTimeTypeByString: function(val, timeType) {
        var valstring = val.split(' ');
        var time = valstring[0].split(':');
        var ampm = valstring[1];
        return this._convertTimeType(parseInt(time[0], 10), parseInt(time[1], 10), ampm, timeType);
    },

    _convertTimeType: function(hour, min, ampm, timeType) {
        //if ampm is empty, it will be treated as H24
        var d = new Date();
        var convertedTime;

        if (typeof (ampm) == 'string' && ampm != "" && hour == 12)
            hour = 0;


        if (ampm === this.AMPM.PM)
            d.setHours(hour + 12, min);
        else
            d.setHours(hour, min);

        if (timeType === this.TimeType.H12) {
            converedTime = d.format("h:mm tt");
        }
        else if (timeType === this.TimeType.H24) {
            converedTime = d.format("H:mm");
        }
        else {
            throw new Error('timeType error'); 
        }
        return converedTime;
    }


}

CSASPNETAjaxExtender.TimePicker.registerClass('CSASPNETAjaxExtender.TimePicker', Sys.UI.Behavior);

if (typeof (Sys) !== 'undefined') Sys.Application.notifyScriptLoaded();


//create element method, which is static
function NewElement(tag, properties) {
   var element = document.createElement(tag);
   for (var propertyName in properties) {
       AssignProperties(element, propertyName, properties[propertyName]);   
   }

   return element;
}
function AssignProperties(el, propertyName, propertyValue) { 
   if (typeof (propertyValue) === "string" || typeof (propertyValue) === "number") 
       el[propertyName] = propertyValue;
   if (typeof (propertyValue) === "object") 
    {
        for(var childpropertyName in propertyValue)
        {
            AssignProperties(el[propertyName], childpropertyName, propertyValue[childpropertyName]);
        }
    }
}


