/// <reference name="MicrosoftAjax.js"/>


Type.registerNamespace("CSASPNETAjaxScriptControl");

CSASPNETAjaxScriptControl.Schedule = function(element) {
    CSASPNETAjaxScriptControl.Schedule.initializeBase(this, [element]);
    //element property 
    this._toolContainer = null;
    this._calendarContainer = null;

    //property
    this._servicePath = null;
    this._updateServiceMethod = null;
    this._deleteServiceMethod = null;

    //handler
    this._calendarMouseOverHandler = null;
    this._calendarlMouseOutHandler = null;
    this._cellContentMouseUpHandler = null;
    //

    this._updateWebRequest = null;
    this._deleteWebRequest = null;
    
    this._dropPanelClientIDCollection = null;
    this._dateTimeFieldName = null;
    this._titleFieldName = null;
    this._descriptionFieldName = null;
    this._deletingDropPanel = null;
    this._loadingImg = null;// it will apply only one loadingimg for all droppanel
}

CSASPNETAjaxScriptControl.Schedule.prototype = {
    initialize: function() {
        CSASPNETAjaxScriptControl.Schedule.callBaseMethod(this, 'initialize');

        this.createDelegates();
        //create handlers
        var cellTable = this.get_calendarContainer();
        $addHandler(cellTable, "mouseover", this._calendarMouseOverHandler);
        $addHandler(cellTable, "mouseout", this._calendarMouseOutHandler);
        for (var r = 2; r < 7; r++) {
            var calendarTr = this.get_calendarContainer().rows[r];
            for (var i = 0; i < 7; i++) {
                var calendarCellTd = calendarTr.childNodes[i];

                var calendarCellContent = calendarCellTd;
                $addHandler(calendarCellContent, "mouseup", this._cellContentMouseUpHandler);
            }
        }
        //set closed event callback of dropPanel 
        var dropPanel = $("." + this.get_element().className + " .dropPanel");
        for (var m = 0; m < dropPanel.length; m++) {
            var id = dropPanel[m].id;
            CSASPNETAjaxScriptControl$Schedule$SetSender(this);
            setTimeout("$find('" + id + "').add_closed(CSASPNETAjaxScriptControl$Schedule$ClosedCallBack)", 0); // callback to static method
        }

        this._initializeBody();
    },
    _initializeBody: function() {
        var body = document.body;
        body.oncontextmenu = function() { return false; };
        body.ondragstart = function() { return false; };
        body.onselectstart = function() { return false; };
        body.onselect = function() {
            if (document.selection)
                document.selection.empty();
            else if (window.getSelection())
                window.getSelection().removeAllRanges();
        };
        body.oncopy = function() {
            if (document.selection)
                document.selection.empty();
            else if (window.getSelection())
                window.getSelection().removeAllRanges();
        };
        body.onbeforecopy = function() { return false; };
        body.onmouseup = function() {
            if (document.selection)
                document.selection.empty();
            else if (window.getSelection())
                window.getSelection().removeAllRanges();
        };
    },
    dispose: function() {
        this.clearDelegates();
        $clearHandlers(this.get_calendarContainer());
        CSASPNETAjaxScriptControl.Schedule.callBaseMethod(this, 'dispose');
    },
    createDelegates: function() {
        this._calendarMouseOverHandler = Function.createDelegate(this, this._onCalendarMouseOver);
        this._calendarMouseOutHandler = Function.createDelegate(this, this._onCalendarMouseOut);
        this._cellContentMouseUpHandler = Function.createDelegate(this, this._onCellContentMouseUp);

    },

    clearDelegates: function() {
        this._calendarMouseOverHandler = null;
        this._calendarMouseOutHandler = null;
        this._cellContentMouseUpHandler = null;
    },

    moveDropPanel: function() {
        var _droppanel = this.get_activeDropPanel();
        //if (_droppanel)
        _droppanel.moveTo(this.__obj);
    },

    _onCalendarMouseOver: function(e) {
        var aa = this._getFirstParentNodeFromClassName(e.target, "calendarCellContent");
        if (aa != null) {
            var activeDropPanel = this.get_activeDropPanel();
            if (activeDropPanel != null && this._updateWebRequest == null) {
                var obj = this._getFirstParentNodeFromClassName(e.target, this.get_calendarCellContentCssClass());

                if (obj != null) {
                    this.__obj = obj;
                    
                    //if the curor is keeping on the droppanel during mouseout and mouseover, 
                    //mouseout will not be triggered. So we can use settimeout to delay the 
                    //droppanel move and ensure mouseout triggering.
                    setTimeout(Function.createDelegate(this, this.moveDropPanel), 0);

                }
            }
        }

        var obj = this._getFirstParentNodeFromClassName(e.target, "calendarCelltd");
        if (obj != null) {
            var height = $(obj).find("table:first>tbody:first>tr:eq(1)>td:first>div:first")[0].offsetHeight;

            obj.className = "calendarCelltd_hover";
            this._adjustCurrentRowStyle(obj, height);
        }

    },

    _adjustCurrentRowStyle: function(obj, basedHeight) {

        var offsetHeight = Math.max($(obj).find("table:first>tbody:first>tr:eq(1)>td:first>div:first")[0].offsetHeight, basedHeight);
        var calendarTdCollection = $(obj).siblings();

        for (var i = 0; i < calendarTdCollection.length; i++) {
            $(calendarTdCollection[i]).find("table:first>tbody:first>tr:eq(1)>td:first>div:first")[0].style.height = offsetHeight - 1 + "px";
        }
    },

    _onCalendarMouseOut: function(e) {
    
        var obj = this._getFirstParentNodeFromClassName(e.target, "calendarCelltd_hover");
        var event = window.event || e, relatedTarget = event.toElement || event.relatedTarget;
        while (relatedTarget && relatedTarget != obj)
            relatedTarget = relatedTarget.parentNode;
        if (!relatedTarget) {
            if (obj != null) {
                var offsetHeight = $(obj).find("table:first>tbody:first>tr:eq(1)>td:first>div:first")[0].offsetHeight

                var calendarTdCollection = $(obj).siblings();
                for (var i = 0; i < calendarTdCollection.length; i++) {
                    $(calendarTdCollection[i]).find("table:first>tbody:first>tr:eq(1)>td:first>div:first")[0].style.height = "";

                }
                obj.className = "calendarCelltd";
            }
        }

    },
    _getFirstParentNodeFromClassName: function(targetElement, className) {
        if (targetElement === undefined || targetElement == null)
            return null;

        if (targetElement.className == className)
            return targetElement;
        else {
            return this._getFirstParentNodeFromClassName(targetElement.parentNode, className);

        }

    },

    _appendDropPanelLoading: function(dropPanel) {
        if (this._loadingImg == null) {
            var _imgCrl = NewElement("div");
            this._loadingImg = NewElement("div", {
                'className': 'loadingImg'
            });
            this._loadingImg.appendChild(_imgCrl);
        }

        dropPanel.get_titleContainer().style.display = 'none';
        dropPanel.get_headerContainer().insertBefore(this._loadingImg, dropPanel.get_closeImage());
        this._loadingImg.style.display = 'block';

    },
    _removeDropPanelLoading: function(dropPanel) {

        this._loadingImg.style.display = 'none';
        dropPanel.get_titleContainer().style.display = 'block';
        dropPanel.get_headerContainer().removeChild(this._loadingImg);
    },
    _dropPanelClosed: function(sender, e) {
        //no one updating, no one deleting

        if (this._deletingDropPanel == null) {
            this._deletingDropPanel = sender;

            this._appendDropPanelLoading(sender);

            // Invoke the web service
            var params = new Object;
            params["key"] = sender.get_fromContextKey().key;
            this._deleteWebRequest = Sys.Net.WebServiceProxy.invoke(
                this.get_servicePath(),
                this.get_deleteServiceMethod(),
                false,
                params,
                Function.createDelegate(this, this._onMethodComplete),
                Function.createDelegate(this, this._onMethodFailed),
                null);
            $common.updateFormToRefreshATDeviceBuffer();
        }
    },
    _onCellContentMouseUp: function(e) {
        var activeDropPanel = this.get_activeDropPanel();
        if (activeDropPanel != null && this._updateWebRequest == null) {

            var cellContentObj = this._getFirstParentNodeFromClassName(e.target, this.get_calendarCellContentCssClass());
            if (cellContentObj == undefined || cellContentObj == null)
                return;
            var toDate = $(cellContentObj).attr("relatedDate");

            if (activeDropPanel.get_fromContextKey().datetime === toDate) {
                activeDropPanel.set_isMoving(false);
                this.set_activeDropPanel(null);
                return;
            }
            activeDropPanel.set_toContextKey(toDate);

            this._appendDropPanelLoading(activeDropPanel);

            // Invoke the web service
            var params = new Object;
            params["key"] = activeDropPanel.get_fromContextKey().key;
            params["updateFieldName"] = this.get_dateTimeFieldName();
            params["updateValue"] = toDate;
            this._updateWebRequest = Sys.Net.WebServiceProxy.invoke(this.get_servicePath(), this.get_updateServiceMethod(), false, params, Function.createDelegate(this, this._onMethodComplete), Function.createDelegate(this, this._onMethodFailed), null);
            $common.updateFormToRefreshATDeviceBuffer();

        }

    },

    _onMethodComplete: function(result, context) {


        if (this._updateWebRequest != null) {
            this._updateWebRequest = null;

            var activeDropPanel = this.get_activeDropPanel();

            this._updateDateTimePicker(activeDropPanel.get_fromContextKey().datetime, activeDropPanel.get_toContextKey());

            activeDropPanel.get_fromContextKey().datetime = activeDropPanel.get_toContextKey();
            this._removeDropPanelLoading(activeDropPanel);
            activeDropPanel.set_isMoving(false);
            this.set_activeDropPanel(null);
        }
        if (this._deleteWebRequest != null) {
            this._deleteWebRequest = null;
            this._removeDropPanelLoading(this._deletingDropPanel);
            this._deletingDropPanel.get_element().parentNode.removeChild(this._deletingDropPanel.get_element());
            this._deletingDropPanel.dispose();
            this._deletingDropPanel = null;

        }
    },

    _updateDateTimePicker: function(fromDate, toDate) {
        var frd = fromDate.split(' ')[0];
        var tod = toDate.split(' ')[0]
        var calendarCellContents = $(this.get_calendarContainer()).find(">tbody>tr:gt(2)>td").find(">table>tbody>tr:eq(1)").find(">td:first>div");
        var dateTimePickerTds = $(this.get_dateTimePicker()).find("tbody>tr:gt(2)>td");
        {
            calendarCellContents.each(function(n) {
                if ($(this).attr("relatedDate") === frd)

                    if ($(this).find("div").size() === 0) {
                    $(dateTimePickerTds).each(function(i) {
                        if ($(this).attr("RelatedDate") === frd) {
                            var dayA = $(this).find(">A:first")[0];
                            if (frd.split('-')[2] == dayA.innerHTML) {
                                dayA.style.color = "#666666";
                                dayA.style.fontWeight = "normal";
                                return false;
                            }

                        }

                    });
                    return false;

                }
            });

        }
        {
            calendarCellContents.each(function(n) {
                if ($(this).attr("relatedDate") === tod) {


                    $(dateTimePickerTds).each(function(i) {
                        if ($(this).attr("RelatedDate") === tod) {
                            var dayA = $(this).find(">A:first")[0];
                            if (tod.split('-')[2] == dayA.innerHTML) {
                                dayA.style.color = "black";
                                dayA.style.fontWeight = "bold";
                                return false;
                            }

                        }

                    });
                    return false;

                }
            });

        }



    },

    _onMethodFailed: function(err, response, context) {

        this._updateWebRequest = null;
        var activeDropPanel = this.get_activeDropPanel();
        this._removeDropPanelLoading(activeDropPanel);

        throw new Error(err);
        //TODO: move back
    },

    get_activeDropPanel: function() {

        if (CSASPNETAjaxScriptControl.DropPanel !== undefined)
            return CSASPNETAjaxScriptControl$DropPanel$DropPanelManager$get_ActiveDropPanel();
        else
            return null;

    },
    set_activeDropPanel: function(val) {
        if (CSASPNETAjaxScriptControl.DropPanel !== undefined)
            CSASPNETAjaxScriptControl$DropPanel$DropPanelManager$set_ActiveDropPanel(val);

    },


    //propety

    get_calendarContainer: function() {
        return this._calendarContainer;
    },

    set_calendarContainer: function(val) {
        if (this._calendarContainer !== val) {
            this._calendarContainer = val;
            this.raisePropertyChanged('calendarContainer');
        }
    },

    get_toolContainer: function() {
        return this._toolContainer;
    },

    set_toolContainer: function(val) {
        if (this._toolContainer !== val) {
            this._toolContainer = val;
            this.raisePropertyChanged('toolContainer');
        }
    },

    get_dateTimePicker: function() {
        return this._dateTimePicker;
    },

    set_dateTimePicker: function(val) {
        if (this._dateTimePicker !== val) {
            this._dateTimePicker = val;
            this.raisePropertyChanged('dateTimePicker');
        }
    },
    /*filedName*/
    get_dateTimeFieldName: function() {
        return this._dateTimeFieldName;
    },

    set_dateTimeFieldName: function(val) {
        if (this._dateTimeFieldName !== val) {
            this._dateTimeFieldName = val;
            this.raisePropertyChanged('dateTimeFieldName');
        }
    },

    get_titleFieldName: function() {
        return this._titleFieldName;
    },

    set_titleFieldName: function(val) {
        if (this._titleFieldName !== val) {
            this._titleFieldName = val;
            this.raisePropertyChanged('titleFieldName');
        }
    },

    get_descriptionFieldName: function() {
        return this._descriptionFieldName;
    },

    set_descriptionFieldName: function(val) {
        if (this._descriptionFieldName !== val) {
            this._descriptionFieldName = val;
            this.raisePropertyChanged('descriptionFieldName');
        }
    },

    /*service info*/
    get_servicePath: function() {
        return this._servicePath;
    },

    set_servicePath: function(val) {
        if (this._servicePath !== val) {
            this._servicePath = val;
            this.raisePropertyChanged('servicePath');
        }
    },

    get_updateServiceMethod: function() {
        return this._updateServiceMethod;
    },

    set_updateServiceMethod: function(val) {
        if (this._updateServiceMethod !== val) {
            this._updateServiceMethod = val;
            this.raisePropertyChanged('updateServiceMethod');
        }
    },

    get_deleteServiceMethod: function() {
        return this._deleteServiceMethod;
    },

    set_deleteServiceMethod: function(val) {
        if (this._deleteServiceMethod !== val) {
            this._deleteServiceMethod = val;
            this.raisePropertyChanged('deleteServiceMethod');
        }
    },

    get_dropPanelClientIDCollection: function() {
        return this._dropPanelClientIDCollection;
    },

    set_dropPanelClientIDCollection: function(val) {
        if (this._dropPanelClientIDCollection !== val) {
            this._dropPanelClientIDCollection = val;
            this.raisePropertyChanged('dropPanelClientIDCollection');
        }

    },
    get_calendarCellContentCssClass: function() {
        return this._calendarCellContentCssClass;
    },

    set_calendarCellContentCssClass: function(val) {
        if (this._calendarCellContentCssClass !== val) {
            this._calendarCellContentCssClass = val;
        }

    }

}
CSASPNETAjaxScriptControl.Schedule.registerClass('CSASPNETAjaxScriptControl.Schedule', Sys.UI.Control);

if (typeof (Sys) !== 'undefined') Sys.Application.notifyScriptLoaded();



//**** To build a child scriptcontrol client event, we have to use settime to set the callback, because the child scriptcontrol will be registered after this control.
//**** Then we have to build a static method as the callback of client event of child scriptcontrol
//**** This static method will call back to the propotype method in class again so that it looks like droppanel closed event to call the callbackfunction in schedule class propotype directly.
//**** In fact, we have to call it through static method, unless we have idea to set the scriptocntrol register order in render phase.
function CSASPNETAjaxScriptControl$Schedule$ClosedCallBack(sender,e) {
    var activeSchedule=CSASPNETAjaxScriptControl$Schedule$GetSender();
    activeSchedule._dropPanelClosed(sender, e);// this will point to window
}

function CSASPNETAjaxScriptControl$Schedule$SetSender(sender) {
    this._activeSchedule = sender;
}
function CSASPNETAjaxScriptControl$Schedule$GetSender() {
    return this._activeSchedule;
}
