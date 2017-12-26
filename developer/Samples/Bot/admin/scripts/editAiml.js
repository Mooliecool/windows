/***************************************
 * http://www.program-o.com
 * PROGRAM O
 * Version: 2.5.3
 * FILE: editAiml.js
 * AUTHOR: Elizabeth Perreau and Dave Morton
 * DATE: 05-11-2013
 * DETAILS: UI for aiml edition
 ***************************************/
YUI().use("datatable-base", "datatable-message", "datatable-sort", "datatable-mutable",
    "datatable-datasource", "datasource-io", "datasource-jsonschema", "paginator",
    "overlay", "event-valuechange", function(Y) {

        var currentEditionCell, currentRequest, table, Paginator, paginator,
            dataSource, overlay, filterTimer, saveTimer,
            fields = ['topic', 'thatpattern', 'pattern', 'template', 'filename'],
            url = "editAiml.php";

        // Create a widget to render pages (1 2 3 ... 4 5 6 ... 80 81 82)
        Paginator = Y.Base.create('my-paginator', Y.Widget, [Y.Paginator], {
            renderUI: function() {
                var numbers = '', i, LIMIT = 4,
                    page = this.get("page"),
                    totalPages = this.get('totalPages');

                for (i = 1; i <= totalPages; i++) {                             // Render pagination links
                    if (i < LIMIT || i > totalPages - LIMIT
                        || (i > page - LIMIT && i < page + LIMIT)) {
                        numbers += '<a href="#" ' + ((page === i) ? "class='selected' " : "") + 'data-page="' + i + '">' + i + '</a>, ';
                    }
                    else if (i === LIMIT || i === totalPages - LIMIT) {
                        numbers += " ... ";
                    }
                }
                var li = numbers.lastIndexOf(', ');
                numbers = numbers.substring(0, li);
                this.get('boundingBox').setContent(numbers);
            },
            bindUI: function() {
                this.get('boundingBox').delegate('click', function(e) {         // let's not go to the page, just update internally
                    e.halt(true);
                    load(parseInt(e.currentTarget.getAttribute("data-page")));
                }, 'a', this);
                this.after(['pageChange', "totalItemsChange", "itemsPerPage"], this.renderUI);
            }
        });

        // Render pagination
        paginator = new Paginator().render(".editaiml-table");

        // Create a datasource for json data
        dataSource = new Y.DataSource.IO({source: url + "?action=search"})
            .plug(Y.Plugin.DataSourceJSONSchema, {
                schema: {
                    metaFields: {result: "results", totalItems: "total_records", start_index: "start_index", page: "page", page_size: "page_size"},
                    resultListLocator: "results",
                    resultFields: ["id", "thatpattern", "pattern", "template", "topic", "filename"]
                }
            });

        // Render the table
        table = new Y.DataTable({
            width: "99.7%",
            columns: [
              {
                label: " ",
                className: "delete-row",
                allowHTML: true,
                emptyCellValue: '<div class="delete-row" title="Delete This Row">&nbsp;</div>'
              },
              {key: "topic", label: "Topic", sortable: true},
              {key: "thatpattern", label: "Previous bot response", sortable: true},
              {key: "pattern", label: "User input", sortable: true},
              {key: "template", label: "Bot response", sortable: true},
              {key: "filename", label: "File", sortable: true}
            ]
          })
          .plug(Y.Plugin.DataTableDataSource, {datasource: dataSource})
          .render(".editaiml-table");

        // Reload table every time it's sorted
        table.after("sort", load);

        // Append filters
        table.get("boundingBox").one("thead").append("<tr><th class='yui3-datatable-header yui3-datatable-cell'></th>"
            + Y.Array.map(fields, function(i) {
                return "<th class='yui3-datatable-header yui3-datatable-cell'><input placeholder='filter' class='filter-" + i + "'/></th>";
            }).join("") + "</tr>");

        // Filters changed event
        Y.one(".editaiml-table").delegate("valueChange", function() {
            filterTimer && filterTimer.cancel();
            filterTimer = Y.later(400, this, load);
        }, "th input");

        // Handle row deletion
        table.get("boundingBox").delegate("click", function(e) {
            var delRow = confirm('Are you sure you want to delete this row? you can\'t take it back if you change your mind!');
            if (!delRow) return false;
            var row = table.getRecord(e.currentTarget);
            table.removeRow(row);
            e.halt(true);
            Y.io(url + "?action=del&id=" + row.get("id"), {
                on: {
                    failure: function() {
                        alert("An error occured deleting row, please reload page");
                    }
                }
            });
        }, "td.delete-row");

        // Field edition
        table.get("boundingBox").delegate("click", function(e) {
            overlay.setStdModContent('body', "<textarea>" + e.currentTarget.getContent() + "</textarea>");
            currentEditionCell = e.currentTarget;
            alignOverlay(e.currentTarget);
            overlay.get("boundingBox").one("textarea").select().focus();
        }, "td.yui3-datatable-cell");

        overlay = new Y.Overlay({
            zIndex: 10,
            visible: false
        }).render();

        overlay.get("boundingBox").delegate("valueChange", function() {
            var key = table.getColumn(currentEditionCell).key,
                value = this.one("textarea").get("value");

            if (key === "topic" || key === "thatpattern" || key === "pattern") {
                value = value.toUpperCase();
            }

            table.getRecord(currentEditionCell).set(key, value);
            alignOverlay(currentEditionCell);                                   // Align the overlay again since the size of the cell may have changed
            currentEditionCell.modified = true;

            saveTimer && saveTimer.cancel();
            saveTimer = Y.later(2000, this, save, [currentEditionCell]);        // Save changes after 2 sec without activity
        }, "textarea");
        overlay.get("boundingBox").delegate("blur", function() {                // Or if the textarea is unfocused
            save(currentEditionCell);
            overlay.hide();
        }, "textarea");

        function save(cell) {
            if (cell.modified) {
                cell.modified = false;
                Y.one(".search-msg").setContent("Saving...");
                Y.io(url + "?action=update", {
                    method: 'POST',
                    data: table.getRecord(currentEditionCell).toJSON(),
                    on: {
                        success: function() {
                            Y.one(".search-msg").setContent("Changes saved");
                        },
                        failure: onError
                    }
                });
            }
        }

        function onError(e) {
            alert("Error saving object. Please reload page.");
            console && console.log(e);
        }

        function alignOverlay(cell) {
            overlay.setAttrs({
                align: {
                    node: cell,
                    points: ["tl", "tl"]
                },
                width: cell.getComputedStyle("width"),
                height: cell.getComputedStyle("height")
            }).show();
        }

        function load(page) {
            overlay.hide();
            table.set("data", null).showMessage("loadingMessage");

            if (Y.DataSource.Local.transactions[currentRequest]) {              // Abort any existing request
                Y.DataSource.Local.transactions[currentRequest].abort();
                Y.DataSource.Local.transactions[currentRequest] = null;         // @hack Remove reference since YUI won't do it
            }

            var sortBy = table.get("sortBy") ? "&sort=" + Y.Object.keys(table.get("sortBy")[0])[0]
                + "&sortOrder=" + (Y.Object.values(table.get("sortBy")[0])[0] === 1 ? "DESC" : "ASC") : "",
                filters = Y.Array.map(fields, function(i) {
                    return "&" + i + "=" + Y.one(".filter-" + i).get("value");
                }).join("");

            currentRequest = table.datasource.load({
                request: "&group=" + (Y.Lang.isNumber(page) ? page : paginator.get("page"))
                    + filters + sortBy,
                callback: {
                    success: function(e) {
                        table.datasource.onDataReturnInitializeTable(e);
                        paginator.set('page', e.response.meta.page)
                            .set('itemsPerPage', e.response.meta.page_size)
                            .set("totalItems", e.response.meta.totalItems);
                    },
                    failure: function(e) {
                        if (e.data && e.data.status) {                          // Do not stop for canceled requests
                            table.showMessage('Error getting data: ' + e.response || e);
                        }
                    }
                }
            });
        }

        // @hack Modified to allow transaction id retrieval (to abort them later
        Y.Plugin.DataTableDataSource.prototype.load = function(config) {
            config = config || {};
            config.request = config.request || this.get("initialRequest");
            config.callback = config.callback || {
                success: Y.bind(this.onDataReturnInitializeTable, this),
                failure: Y.bind(this.onDataReturnInitializeTable, this),
                argument: this.get("host").get("state") //TODO
            };

            var ds = (config.datasource || this.get("datasource"));
            if (ds) {
                return ds.sendRequest(config);                                  // Added return
            }
        };

        // Insert new aiml
        table.get("boundingBox").one("table").append("<tfoot>"
            + "<tr><td></td><td colspan='5' style='padding-top:2px'>Teach new knowledge</td></tr>"
            + "<tr><td></td><td><textarea rows='1' placeholder='Topic'></textarea></td>"
            + "<td><textarea rows='1' placeholder='Previous bot response'></textarea></td>"
            + "<td><textarea rows='1' placeholder='User input'></textarea></td>"
            + "<td><textarea rows='1' placeholder='Bot response'></textarea></td>"
            + "<td><button>Save</button></td></tr>"
            + "</tfoot>");
        table.get("boundingBox").delegate("click", function() {
            var texts = Y.all("tfoot textarea");
            if (texts.item(2).get("value") === "" || texts.item(3).get("value") === "") {
                alert("You must enter a user input and bot response.");
                return;
            }
            Y.one(".search-msg").setContent("Saving...");
            Y.io(url + "?action=add", {
                method: 'POST',
                data: {
                    topic: texts.item(0).get("value"),
                    thatpattern: texts.item(1).get("value"),
                    pattern: texts.item(2).get("value"),
                    template: texts.item(3).get("value")
                },
                on: {
                    success: function() {
                        load(paginator.get("totalPages"));
                        Y.one(".search-msg").setContent("All changes saved");
                    },
                    failure: onError
                }
            });
            texts.set("value", "");
        }, "button");

        // Load the first page
        load(1);
    });
    