// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2011/June/27


var set3333 = ( function () {

    var _o = {};

    _o.array_has_value = function (a, v) {
        return ($.inArray(v, a) != -1);
    };


    _o.popi = function (a, i) {
        var last = a.pop();
        if (i < a.length) {
            a[i] = last;
        }
    };

    _o.acmp = function (a0, a1) {
        var ret = 0;
        var i, m = Math.min(a0.length, a1.length);
        for (i = 0; (ret == 0) & (i < m); ++i) {
            ret = a0[i] - a1[i];
        }
        if (ret == 0) {
            ret = (a0.length - a1.length);
        }
        return ret;
    }

    _o.d2 = function(n) {
        if (n < 10) { n = "0" + n; }
        return n;
    }

    _o.hms = function(epoch) {
        var d = new Date(1000*epoch);
        s = 
            _o.d2((d.getUTCHours())) + ':' +
            _o.d2((d.getUTCMinutes())) + ':' +
            _o.d2((d.getUTCSeconds()));
        return s;
    }


    _o.ymdhms = function(epoch) {
        var d = new Date(1000*epoch);
        s = 
            d.getUTCFullYear() + '/' + 
            _o.d2((d.getUTCMonth() + 1)) + '/' + 
            _o.d2(d.getUTCDate()) + ' ' +
            _o.d2((d.getUTCHours())) + ':' +
            _o.d2((d.getUTCMinutes())) + ':' +
            _o.d2((d.getUTCSeconds()));
        return s;
    }

    _o.same_ymd = function(epochs) {
        var same = true;
        var ymd = function (epoch) {
            var d = new Date(1000*epoch);
            return [d.getUTCFullYear(), d.getUTCMonth(), d.getUTCDate()]
        }
        if (epochs.length > 1) {
            var i;
            var ymd0 = ymd(epochs[0]);
            for (i = 1; same && i < epochs.length; ++i) {
                var ymdi = ymd(epochs[i]);
                same = (_o.acmp(ymd0, ymdi) == 0);
            }
        }
        return same;
    }

    _o.replace_append = function (parent, child) {
        var children = parent.childNodes;

        if (children.length > 0) {
            parent.replaceChild(child, parent.firstChild);
        } else {
            parent.appendChild(child);
        }
    };


    _o.tr_append_td = function (tr, e, kls) {
        var td = document.createElement("td");
        if (kls != undefined) {
            console.log("tr_append_td: Setting className="+kls);
            td.className = kls;
        }
        td.appendChild(e);
        tr.appendChild(td);
        return td;
    };

    _o.tr_append_text = function (tr, t, kls) {
        var e = document.createTextNode(t);
        return _o.tr_append_td(tr, e, kls);
    };


    _o.warning_desktop = function (text, okfunc) {
        $("#warning").dialog("option", "buttons",
            { "OK": function () {
                if (okfunc){
                    console.log("Call okfunc");
                    okfunc();
                }
                $(this).dialog("close");
            }
        });
        $("#warning").html(text);
        $("#warning").dialog("open");
    };

    _o.warning_mobile = function (text, okfunc) {
        console.log("warning_mobile: "+text);
        $("#warn-text").html(text);
        $("#popup-warning").popup({
            afterclose: function( event, ui ) { if (okfunc) { okfunc() } }
        });
        $("#popup-warning").popup("open");
    };

    _o.warning = function (text, okfunc) {
        console.log("warning: "+text);
        if (_o.state.mobile) {
            _o.warning_mobile(text, okfunc);
        } else {
            _o.warning_desktop(text, okfunc);
        }
    };

    _o.warning_code = function (error_code, okfunc) {
        var text = "";
        switch (error_code)
        {
         case _o.c.E3333_BAD_COMMAND:
            text = "Bad Command.";
            break;
         case _o.c.E3333_NO_TABLE:
            text = "No table. Table may have been expired.";
            break;
         case _o.c.E3333_NO_PLAYER:
            text = "No player. may have been expired.";
            break;
         case _o.c.E3333_CLUB_FULL:
            text = "Club full, no more tables available.";
            break;
         case _o.c.E3333_COLLISION:
            text = "Sorry, your action was ignored due to \"cloud collision\". " +
                "Probably your action assumed obsolete game state. " +
                "Some player was faster than you?";
            break;
         case _o.c.E3333_NAME_USED:
            text = "Name already used, or (if re-joining) wrong passcode";
            break;
         case _o.c.E3333_NOT_A_SET:
            text = "Sorry, not a set!";
            break;
         case _o.c.E3333_TABLE_FULL:
            text = "Sorry, table is full!";
            break;
         case _o.c.E3333_TABLE_NOT_AUTHORIZED:
            text = "Sorry, wrong passcode for the table.";
            break;
         case _o.c.E3333_YES_MORE:
            text = "Hmmm... still there is/are more.";
            break;
         case _o.c.E3333_FALSE_NONE:
            text = "Adding 3 cards was not necessary.";
            break;
        }
        _o.warning(text, okfunc);
    };

    _o.state_initial = {
        mobile: false,
        myname: "",
        table_name: "",
        owner: false, // true if I am the table owner
        tstate: -1,
        gstate: -1,
        deck_size: 81,
        game_active: false,
        my_say: "",
        my_say_entry: undefined,
        cards_active_idx: [], // Indices to cards
        cards_selected: [], // Indices to cards_active_idx
        selected_draw_mode: undefined,
        delayed: false,
        delayed_events: [],
    };

    _o.state = $.extend(true, {}, _o.state_initial);

    _o.msgh_tables_status = function (tables) {

        var apid = "club";
        console.log("msgh_tables_status: tables="+tables+"  n="+tables.length);
        if (_o.state.mobile) {
           // var ap = $.mobile.pagecontainer.getActivePage();
           var ap = $("body").pagecontainer("getActivePage");
           apid = $(ap).attr("id");
           console.log("activepage="+ap+", id="+apid);
        }
        var tbl = document.createElement("table");
        for (var ti = 0; ti < tables.length; ti++) {
            var tr = $('<tr>');
            // var button = document.createElement("button");
            var tti = tables[ti];
            var table_name = tti[0];
            var label = table_name + " (" + tti[1] + ")";
            var is_open = (Number(tti[2]) == 1);
            var button = $('<button>')
                .text(table_name + " (" + tti[1] + ")")
                .click(function (s) {
                    return function () { _o.table_join_request(s); };
                }(table_name));
            if (is_open) {
                button
                    .addClass("opentable")
                    .attr({title: "Join table, no passcode needed"});
            } else {
                button
                    .attr({title: "Join table, passcode required"});
            }
            

            tr
                .append($('<td>')
                    .append(button))
                .append($('<td>')
                    .attr({title: "Time the table was created"})
                    .append(' ' + _o.ymdhms(tti[3])).addClass("creation"))
                .append($('<td>')
                    .attr({title: "Time of last action made in the table"})
                    .append(' ' + _o.ymdhms(tti[4])).addClass("lastaction"))
                ;
            if (tti[5] > 0) {
                tr.append($('<td>')
                    .attr({title: "Owner left table"})
                    .append(' ' + _o.ymdhms(tti[5])).addClass("ownerleft"));
            }
            $(tbl).append(tr);
        }
 
        var e = document.getElementById("tables");
        _o.replace_append(e, tbl);
        if (apid !== "club") { // Implies also _o.state.mobile
            $("body").pagecontainer("change", "#"+apid);
        }
    };

    _o.msgh_set_found = function (a3i) {
        console.log("msgh_set_found: a3i="+a3i);
        _o.state.cards_selected = a3i;
        _o.state.selected_draw_mode = _o.c.DRAW_CARD_IS_A_SET;
        _o.board_show(); // For all players!
        _o.state.delayed = true;
        setTimeout(function () {
            _o.state.delayed = false;
            _o.state.cards_selected = [];
            _o.state.selected_draw_mode = _o.c.DRAW_CARD_SELECTED;
            _o.board_show();
            _o.handle_delayed_events();
        }, 600);
    };

    _o.connection_taken = function (data) {
        console.log("connection_taken");
        _o.warning("Connection was taken");
        _o.web_socket.close();
        _o.state = $.extend(true, {}, _o.state_initial);
        _o.web_socket_open();
        _o.set_club_view();
    };

    _o.table_closed = function (data) {
        console.log("table_closed");
        _o.set_club_view();
    };

    _o.tables_status = function () {
        console.log("tables_status");
        _o.web_socket.send(_o.c.S3333_C2S_TBLS);
    };


    _o.players_fill = function (players) {
        var table_players = $('<table>')
            .attr({"id": "tableplayers", "border": "1"});
        var t2s = [_o.ymdhms, _o.ymdhms]; // may change
        var epochs;
        var tr = $('<tr>')
            .append($('<th>')
                .append('Player')
            )
            .append($('<th>')
                .append('Found').addClass('good')
            )
            .append($('<th>')
                .append('Bad Calls').addClass('bad')
            )
            .append($('<th>')
                .append('True None').addClass('good')
            )
            .append($('<th>')
                .append('False None').addClass('bad')
            )
            .append($('<th>')
                .append('Time Joined')
            )
            .append($('<th>')
                .append('Last Action')
            );
        if (!_o.state.mobile) {
            console.log("Adding say column");
            tr.append($('<th>')
                .append('Say/Said')
            );
        }
        table_players.append($('<thead>').append(tr));

        epochs = [new Array(players.line), new Array(players.line)];
        for (var pi = 0; pi < players.length; pi++) {
            var p = players[pi];
            epochs[0][pi] = p["tcreated"];
            epochs[1][pi] = p["taction"];
        }
        if (_o.same_ymd(epochs[0])) { t2s[0] = _o.hms; }
        if (_o.same_ymd(epochs[1])) { t2s[1] = _o.hms; }
        for (var pi = 0; pi < players.length; pi++) {
            var p = players[pi];
            var tr = $("<tr>");
            tr.append($('<td>').append(p["name"]));
            var numbers = p["numbers"];
            for (var i = 0; i < 4; i++) {
                var clss = "number";
                if (numbers[i] > 0) {
                    clss += " " + (i % 2 == 0 ? "good" : "bad");
                }
                tr.append($('<td>')
                    .append(numbers[i])
                    .addClass(clss)
                );
            }
            tr.append($('<td>').append(t2s[0](p["tcreated"])));
            tr.append($('<td>').append(t2s[1](p["taction"])));
            if (!_o.state.mobile) {
                if (p["name"] == _o.state.myname) {
                    if (_o.state.my_say_entry) {
                        _o.state.my_say_entry.parentNode.removeChild(
                            _o.state.my_say_entry);
                    } else {
                        _o.state.my_say_entry = document.createElement("input");
                    }
                    tr.append(_o.state.my_say_entry);
                } else {
                    tr.append(p["say"]);
                }
            }
            table_players.append(tr);
        }
        $("#tableplayers").replaceWith(table_players);
    };


    _o.no_more_button_set = function () {
        if (_o.state.game_active) {
            $("#nomore").attr("disabled", false);
            // var nomore = document.getElementById("nomore");
            if (_o.state.deck_size == 0) {
                $("#nomore")
                    .html("No More")
                    .attr("title", "Click if you think there are\n" +
                        "no more sets for this game.");
            } else {
                if (_o.state.mobile) {
                    $("#nomore").html("Add 3/" + _o.state.deck_size);
                } else {
                    $("#nomore")
                        .html("Add 3")
                        .attr("title", "Add 3 cards, if you think\n" +
                            "the displayed cards have no set.");
                }
            }
        } else {
            $("#nomore").attr("disabled", true);
        }
    };


    _o.info_mobile = function (text, okfunc) {
        console.log("info_mobile: text="+text + ", okfunc="+okfunc);
        $("#info").html(text);
        $("#popup-info").popup({afterclose: function (event, ui) {
            console.log("afterclose");
            if (okfunc) { okfunc(); }
        }});
        $("#popup-info").popup("open");
    };


    _o.game_over_desktop = function (text, okfunc) {
        $("#gameover").html(text);
        $("#gameover").dialog({
            buttons: {
                "OK": function () {
                    $(this).dialog("close");
                    if (okfunc) { okfunc(); }
                }
            }
        });
        $("#gameover").dialog("open");
    };

    _o.game_over_mobile = function (text, okfunc) {
         _o.mobile_page_set("players");
         _o.info_mobile(text, okfunc);
    };


    _o.game_over = function () {
        console.log("game_over: owner="+_o.state.owner);
        var text =
            "<b>Game Over.</b><br>" +
            (_o.state.owner
                ? "Please start <em>New Game</em>."
                : "Wait for table owner to start a new game.");
        // var okfunc = _o.state.owner ? _o.new_game : undefined;
        var okfunc = undefined;
        console.log("game_over: text="+text);
        if (_o.state.mobile) {
            _o.game_over_mobile(text, okfunc);
        } else {
            _o.game_over_desktop(text, okfunc);
        }
    };

    _o.my_say_last = "",

    _o.mobile_page_set = function (page) {
        console.log("mobile_page_set: page=" + page);
        $("body").pagecontainer("change", "#" + page, {changeHash: false});
        $("#" + page + " select").val(page).change();
    }   

    _o.set_club_view = function () {
        console.log("set_club_view");
        if (_o.state.mobile) {
            _o.mobile_page_set("club");
        } else {
            $("#main-tabs").tabs({ active: 0 });
        }
    };

    _o.msgh_new_table = function (result) {
        console.log("msgh_new_table");
        _o.state.owner = true;
        _o.board_show();
        _o.web_socket.send(_o.c.S3333_C2S_TBLS)
        if (_o.state.mobile) {
            // $.mobile.changePage("#table");
            _o.mobile_set_titles();
            _o.mobile_page_set("table");
            // $(".change_page").children('option:selected')
            //    .attr('value', "table");
        } else {
            $("#table").show();
            $("#newgame").show();
            $("#closetable").show();
            $("#main-tabs").tabs({ active: 1 });
        }
    };

    _o.msgh_join_desktop = function (result) {
        console.log("msgh_join_desktop");
        $("#table").show();
        $("#newgame").hide();
        $("#closetable").hide();
        _o.board_show();
        $("#main-tabs").tabs({ active: 1 });
    };

    _o.mobile_set_titles = function () {
        $("#players-title").html("Players of Table: " + _o.state.table_name);
        $("#table-title").html(_o.state.myname + " @ Table owned by " +
            _o.state.table_name);
    };

    _o.msgh_join_mobile = function (result) {
        console.log("msgh_join_mobile");
        _o.mobile_set_titles();
        _o.board_show();
        _o.mobile_page_set("table");
        $("#table select").val("table").change();
    };

    _o.msgh_join = function (result) {
        console.log("msgh_join");
        _o.state.owner = false;
        if (_o.state.mobile) {
            _o.msgh_join_mobile(result);
        } else {
            _o.msgh_join_desktop(result);
        }
    };

    _o.table_nj = function (table_name) {
        var join, pw_flags, s;
        var name = $("#name").val().trim();
        var tpass = $("#tpass").val().trim();
        var upass = $("#upass").val().trim();
        _o.state.myname = name;
        console.log("table_nj, table_name="+table_name + 
            ", name="+name + ", pass="+tpass+", upass="+upass);
        if (name === "") {
            _o.warning("Please enter name");
        } else {
            join = (table_name !== "");
            _o.state.table_name = (join ? table_name : name);
            pw_flags = 1*(!(tpass === "")) + 2*(!(upass === ""));
            console.log("pw_flags="+pw_flags);
            _o.state.game_active = false;
            _o.web_socket.send([
                join ? _o.c.S3333_C2S_JOIN : _o.c.S3333_C2S_NTBL,
                table_name, name, pw_flags, tpass, upass].join(" "));
        }
    }

    _o.new_table = function () {
        _o.table_nj("");
    };

    _o.new_table_dialog = function () {
        console.log("new_table_dialog, mobile?=" + _o.state.mobile);
        if (_o.state.mobile) {
            $("#newtbl-ok").click(function () {
                console.log("newtbl-ok");
                $("#newjointbl").popup("close");
                _o.new_table(); 
            });
            $("#newjointbl").popup("open");
        } else {
            $("#newjointbl").dialog({
                title: "New Table",
                buttons: {
                    "OK": function () {
                        _o.new_table();
                        $(this).dialog("close");
                    },
                    Cancel: function () { $(this).dialog("close"); }
                }
            });
            $("#newjointbl-text").html(
                "Name the table and yourself. " +
                "Choose (optional) passcodes for joiners and yourself."
                );
            $("#newjointbl").dialog("open");
        }
    };


    _o.table_close = function () {
        console.log("table_close: myname="+_o.state.myname);
        _o.web_socket.send(_o.c.S3333_C2S_CLOS);
    };


    _o.table_join = function (table_name) {
        _o.table_nj(table_name);
    };


    _o.table_join_request_desktop = function (table_name) {
        console.log("table_join_request_desktop: table_name=" + table_name);
        $("#newjointbl").dialog({
            title: "Join Table: " + table_name,
            buttons: {
                "OK": function () {
                    _o.table_join(table_name);
                    $(this).dialog("close");
                },
                Cancel: function () { $(this).dialog("close"); }
            }
        });
        $("#newjointbl-text").html(
            "Name yourself. Enter table passcode, " +
            "and choose an (optional) passcode for yourself."
            );
        $("#newjointbl").dialog("open");
    };


    _o.table_join_request_mobile = function (table_name) {
        console.log("table_join_request_mobile: table_name=" + table_name);
        $("#tbl-popup-header").html("Join Table: " + table_name);
        // $("#newjointbl").dialog();
        console.log("Dum-size="+ $("#zbzb").size() + ", newjtbl-size="+
                  $("#newjointbl").size());
        // $("#newjointbl").popup("open"); // Why works only  with setTimeout !?
        setTimeout(function () {
            $("#newjointbl").popup("open");
        }, 0);
        $("#newtbl-ok").click(function () {
            // $("#newjointbl").dialog("close");
            // $.mobile.back();
            // $("body").pagecontainer("change", "#table");
            _o.table_nj(table_name); 
            $("#newjointbl").popup("close");
        });
        //console.log("changePage: newjointbl");
      //$.mobile.changePage("#newjointbl", {transition: "pop", role: "dialog" })
    };


    _o.table_join_request = function (table_name) {
        console.log("table_join_request: table_name=" + table_name);
        if (_o.state.mobile) {
            _o.table_join_request_mobile(table_name);
        } else {
            _o.table_join_request_desktop(table_name);
        }
    };


    _o.cards = (function() {
        var ci, i, j, k, l;
        var crds = new Array(81); // 3^4
        ci = 0;
        for (i = 0; i < 3; i++) {
            for (j = 0; j < 3; j++) {
                for (k = 0; k < 3; k++) {
                    for (l = 0; l < 3; l++) {
                        crds[ci++] = [i, j, k, l];
                    }
                }
            }
        }
        return crds
    })(),
        


    _o.new_game = function () {
        _o.web_socket.send(_o.c.S3333_C2S_GNEW);
    }


    _o.user_add3 = function () {
        _o.web_socket.send(_o.c.S3333_C2S_ADD3 + " " + _o.state.gstate);
    };


    _o.no_more = function () {
        _o.web_socket.send(_o.c.S3333_C2S_NMOR + " " + _o.state.gstate);
    };


    _o.add3_no_more = function () {
        if (_o.state.deck_size == 0) { _o.no_more(); } else { _o.user_add3(); }
    };

    _o.board = {
        n_columns: 1,
        card_width: 1,
        card_height: 1,
        xgap: 0,
        ygap: 0
    };

    _o.golden = (Math.sqrt(5.)+1.)/2.;


    _o.board_n_columns = function (width, height, n) {
        // Decide how many columns to use for drawing
        // For each possible number of columns: columns,
        // We compute the resulted number of rows: rows.
        // Then we compute the sizes of the implied cell,
        // and find the ratio  width/height of the cell.
        // The 'best'  columns number  is the one that gives a ratio
        // That is closest to the golden ratio.
        var best_columns = 1;
        var best_ratio_quality = 0; // Can be 1 at most.
        for (var columns = 1; columns <= n; columns +=1)
        {
            var rows = (n + (columns - 1)) / columns;
            // cell_width = win_width / columns
            // cell_height = win_height / rows
            // cell_ratio = cell_width / cell_height
            var cell_ratio = (width * rows) / (height * columns);
            var ratio_quality = cell_ratio / _o.golden;
            if (ratio_quality > 1.)
            {
                ratio_quality = 1. / ratio_quality;
            }
            // console.log("columns="+columns + ", quality="+ratio_quality);
            if (best_ratio_quality < ratio_quality)
            {
                best_columns = columns;
                best_ratio_quality = ratio_quality;
            }
        }
        return best_columns
    };


    _o.fill_passes = [undefined, undefined, undefined];


    _o.draw_diamond = function (context, x, y, w, h) {
        context.beginPath();
        context.moveTo(x + w/2, y);
        context.lineTo(x + w, y + h/2);
        context.lineTo(x + w/2, y + h);
        context.lineTo(x, y + h/2);
        context.closePath();
    };


    _o.aa_round = function (aa) {
        for (var i = 0; i < aa.length; i++) {
            var a = aa[i];
            for (var j = 0; j < a.length; j++) {
                a[j] = [Math.round(a[j][0]), Math.round(a[j][1])];
            }
        }
    };

    _o.draw_squiggle = function (context, x, y, w, h) {
        // Draw squiggle within the rectangle whose diagonal (x,y) -> (x+w,y+h)"

        // Build curves array(s). Each curve is made of 3 XY-points:
        //    [Control-Point1, Control-Point2, Target-Point]
        // Start with half left contour, top to bottom, counter-clockwise. {
        // We start relative to (x,y).
        var rel_curves_left = [
            [[w/8, 0],      [0,   h/16],   [0,   h/14]],
            [[w/3, h/8],    [w/4, h/4],    [w/5, 3*h/8]],
            [[w/5, h/2],    [0,   5*h/8],  [0,   3*h/4]],
            [[0,   7*h/8],  [w/4, h],      [w/2, h]]
        ];
        _o.aa_round(rel_curves_left);
        // Make the right side curves symmetric.                           }
        var rel_curves_right = []
        for (var i = 0; i < rel_curves_left.length; i++) {
            var xy3_l = rel_curves_left[i];
            var xy3_r = [undefined, undefined, undefined];
            for (var j = 0; j < 3; j++) {
                var xy_l = xy3_l[j];
                var xy_r = [w - xy_l[0], h - xy_l[1]]; // center symmetry
                xy3_r[j] = xy_r;
            }
            rel_curves_right.push(xy3_r);
        }

        var curves = []; // Absolute, add (x,y) to each
        var rel_curves = rel_curves_left.concat(rel_curves_right);
        for (var i = 0; i < rel_curves.length; i++) {
            var xy3_rel = rel_curves[i];
            var xy3 = []
            for (j = 0; j < 3; j++)
            {
                var xy_rel = xy3_rel[j];
                xy3.push([x + xy_rel[0], y + xy_rel[1]])
            }
            curves.push(xy3)
        }
        // The last point is also the first!
        var pt_1st = curves[curves.length - 1][2];

        context.beginPath();
        context.moveTo(pt_1st[0], pt_1st[1]);
        for (var i = 0; i < curves.length; i++) {
            var curve = curves[i];
            context.bezierCurveTo(
                curve[0][0], curve[0][1],
                curve[1][0], curve[1][1],
                curve[2][0], curve[2][1]);
        }
        context.closePath();
    };


    _o.draw_oval = function (context, x, y, w, h) {
        // Draw an ellipse, within a bounding rectangle
        //   (x,y)    The left-top point of the bounding rectangle.
        //   (w,h)    The width and height of the bounding rectangle.
        //   and rotated by rotation_angle, filled by texture

        // Compute center
        var cx = x + w/2, cy = y + h/2;
        // Compute Radii
        var rx = w/2, ry = h/2;

        context.beginPath();
        context.moveTo(cx + rx, cy);
        context.quadraticCurveTo(cx + rx, cy - ry, cx, cy - ry);
        context.quadraticCurveTo(cx - rx, cy - ry, cx - rx, cy);
        context.quadraticCurveTo(cx - rx, cy + ry, cx, cy + ry);
        context.quadraticCurveTo(cx + rx, cy + ry, cx + rx, cy);
        context.closePath();
    };
    
    _o.shape_draw = [undefined, undefined, undefined];

    _o.draw_card = function (context, card, x, y, w, h, draw_mode) {
        // console.log0("draw_card: mode="+draw_mode +
        //  " x="+x+" y="+y+" w="+w+" h="+h);
        var n = card[_o.c.DIM_NUMBER] + 1;
        var symbol = card[_o.c.DIM_SYMBOL];
        var color  = card[_o.c.DIM_COLOR];
        var shading  = card[_o.c.DIM_SHADING];

        var brect_width = w/4, brect_height = 3*h/4;
        var x_gap = (w - (n * brect_width))/(n + 1);
        var brect_x = x + x_gap;
        var brect_y = y + (h - brect_height)/2;

        switch (draw_mode) {
            case _o.c.DRAW_CARD_NORMAL:
                // Draw white
                context.fillStyle = "#fff";
                context.fillRect(x, y, w, h);
                break;
            case _o.c.DRAW_CARD_SELECTED:
            case _o.c.DRAW_CARD_NOT_A_SET:
            case _o.c.DRAW_CARD_IS_A_SET:
                context.fillStyle = _o.c.draw_mode_frame_rgb[draw_mode];
                context.fillRect(x, y, w, h);
                var bw = Math.max(h / 24, 3); // frame width
                context.fillStyle = "#ccc";
                context.fillRect(x + bw, y + bw, w - 2*bw, h - 2*bw);
                break;
        }

        draw_func = _o.shape_draw[symbol];

        // console.log("color="+color);
        var rgbc = _o.c.rgb_colors;
        var fill_passes = _o.fill_passes[shading];
        context.fillStyle = rgbc[color];
        context.strokeStyle = rgbc[color];
        context.lineWidth = Math.max(brect_width/10, 4);
        // var pattern_striped = context.createPattern(offscreen, "repeat");

        for (var r = 0; r < n; r++) {
            context.fillStyle = rgbc[color];
            for (var fpi = 0; fpi < fill_passes.length; fpi++) {
                draw_func(
                    context, Math.round(brect_x), Math.round(brect_y),
                    Math.round(brect_width), Math.round(brect_height));
                if (fill_passes[fpi]) {
                    if (shading == _o.c.SHADING_STRIPED) {
                        context.fillStyle = _o.c.pattern_stripes[color];
                    } else {
                        context.fillStyle = rgbc[color];
                    }
                    context.fill();
                } else {
                    context.fillStyle = rgbc[color];
                    context.stroke();
                }
            }
            brect_x += brect_width + x_gap
        }
    };


    _o.pattern_stripes_set = function (card_height) {
        var pat_height = Math.max(card_height / 24, 10);
        var stripe_height = (3*pat_height)/5;
        for (var ci = 0; ci < 3; ci++) {
            var offscreen = document.createElement("canvas");
            offscreen.width = 8;
            offscreen.height = pat_height;
            var ctx = offscreen.getContext("2d")
            ctx.fillStyle = _o.c.rgb_colors[ci];
            ctx.fillRect(0, 0, 8, stripe_height);
            _o.c.pattern_stripes[ci] =
                ctx.createPattern(offscreen, "repeat");
        }
    };


    _o.card_get_draw_mode = function(ci) {
        var draw_mode = (_o.array_has_value(_o.state.cards_selected, ci) 
            ? _o.state.selected_draw_mode : _o.c.DRAW_CARD_NORMAL);
        return draw_mode;
    }

    _o.board_clear = function () {
        console.log("board_clear");
        var bw = $("#boardwrap");
        var position = bw.position();
        var winw = $(window).width(), winh = $(window).height();
        var canvas = $("#board")[0];
        var context = canvas.getContext("2d"); // Get 2D drawing context
        canvas.width = 9*(winw - 2*position.left)/10;
        canvas.height = 9*(winh - position.top)/10;
        console.log("window: "+winw + "x" + winh + 
                    ", canvas: "+canvas.width +"x"+canvas.height);
        context.fillStyle = "#242";
        context.fillStyle = "#777";
        context.fillRect(0, 0, canvas.width, canvas.height);
        return { canvas: canvas, context: context };
    }

    _o.board_show = function () {
        console.log("board_show");
        var bret = _o.board_clear();
        var canvas = bret.canvas;
        var context = bret.context;
        var width = canvas.width, height = canvas.height;

        var n_cards = _o.state.cards_active_idx.length;
        var columns = _o.board.n_columns = 
            _o.board_n_columns(width, height, n_cards);
        var rows = Math.floor((n_cards + columns - 1) / columns);
        rows = Math.max(rows, 1); // Avoid zero-division
        console.log("n_cards="+n_cards+", rows="+rows+", cols="+columns);

        var q = 0.95;
        if (width/columns < (_o.golden * height)/rows) {
            _o.board.card_width = Math.round(q * width / columns);
            _o.board.card_height = Math.round(_o.board.card_width / _o.golden);
        } else {
            _o.board.card_height = Math.round(q * height / rows);
            _o.board.card_width = Math.round(_o.golden * _o.board.card_height);
        }
        _o.pattern_stripes_set(_o.board.card_height);


        var xgap = (width - (columns * _o.board.card_width)) / (columns + 1);
        var ygap = (height - (rows * _o.board.card_height)) / (rows + 1);
        _o.board.xgap = xgap;    _o.board.ygap = ygap;

        var xn = xgap, yn = ygap;
        var xi = 0
        var x = xgap, y = ygap;
        for (var ci = 0; ci < n_cards; ci++) {
            var card = _o.cards[_o.state.cards_active_idx[ci]];
            _o.draw_card(context, card, x, y, _o.board.card_width,
                _o.board.card_height, _o.card_get_draw_mode(ci));
            xi += 1;
            if (xi < columns) {
                x += _o.board.card_width + xgap;
            } else {
                xi = 0;
                x = xgap;
                y += _o.board.card_height + ygap;
            }
        }
    };


    _o.cheat_tip = function () {
        console.log("cheat_tip:");
        var n = _o.state.cards_active_idx.length;
        for (var i = 0; i < n; i++) {
            var i_card = _o.cards[_o.state.cards_active_idx[i]]
            for (var j = i + 1; j < n; j++) {
                var j_card = _o.cards[_o.state.cards_active_idx[j]]
                for (var k = j + 1; k < n; k++) {
                    var k_card = _o.cards[_o.state.cards_active_idx[k]]
                    var is_set = true;
                    for (var d = 0; is_set && d < 4; d++)
                    {
                        s = i_card[d] + j_card[d] + k_card[d];
                        is_set = ((s % 3) == 0);
                    }
                    if (is_set) {
                        console.log("is_set: ["+i+", "+j+", "+k+"]");
                    }
                }
            }
        }
    }

    _o.state_update = function (rstate) {
        console.log("state_update");
        if (!_o.state.owner && _o.state.mobile ) {
            console.log("non owner close info and show table");
            $('[data-role="popup"]').popup("close");
            _o.mobile_page_set("table");
        }
        if (_o.state.tstate < rstate['tstate']) {
            _o.state.tstate = rstate['tstate'];
            _o.players_fill(rstate['players']);
        }
        if (_o.state.gstate < rstate['gstate']) {
	    var old_game_active = _o.state.game_active;
            _o.state.gstate = rstate['gstate'];
            _o.state.cards_active_idx = rstate['active'];
            _o.state.deck_size = rstate['deck'];
            $("#indeck").text(_o.state.deck_size);
            _o.state.game_active = rstate['gactive'];
            _o.no_more_button_set();
            _o.board_show();
            if (old_game_active && !_o.state.game_active) {
                _o.game_over();
            }
            _o.cheat_tip()
        }
    },

    // redraw just one card
    _o.board_draw_cardi = function (ci) {
        console.log("board_draw_cardi: ci="+ci);
        var canvas = $("#board")[0];
        var context = canvas.getContext("2d"); // Get 2D drawing context
        var b = _o.board;
        var xwg = (b.card_width + b.xgap);
        var yhg = (b.card_height + b.ygap);
        var x = b.xgap + (ci % b.n_columns) * xwg;
        var y = b.ygap + Math.floor(ci / b.n_columns) * yhg;
        var card = _o.cards[_o.state.cards_active_idx[ci]];
        _o.draw_card(context, card, x, y, b.card_width, b.card_height,
            _o.card_get_draw_mode(ci));
    };


    _o.xy2card_index = function (x, y) {
        var b = _o.board;
        var column = (x - b.xgap) / (b.card_width + b.xgap);
        var row    = (y - b.ygap) / (b.card_height + b.ygap);
        column = Math.max(Math.floor(column), 0);
        row = Math.max(Math.floor(row), 0);
        var i = b.n_columns * row + column;
        if (i >= _o.state.cards_active_idx.length) { i = -1; }
        return i;
    };


    _o.select_revert = function () {
        _o.state.cards_selected.pop();
        board_show();
    };


    _o.user_try3 = function () {
        console.log("user_try3");
        _o.web_socket.send(_o.c.S3333_C2S_TRY3 + " " + _o.state.gstate + " " + 
            _o.state.cards_selected.join(" "));
    };


    _o.card_select = function (e) {
        var mouseX = e.pageX - this.offsetLeft;
        var mouseY = e.pageY - this.offsetTop;
        var ci = _o.xy2card_index(mouseX, mouseY);
        console.log("card_select: mouse=("+mouseX+","+mouseY+"), ci="+ci);
        if (ci >= 0) {
            var si = $.inArray(ci, _o.state.cards_selected);
            if (si == -1) {
                _o.state.cards_selected.push(ci);
                if (_o.state.cards_selected.length == 3) {
                    _o.user_try3();
                }
            } else {
                _o.popi(_o.state.cards_selected, si);
            }
            _o.board_draw_cardi(ci);
        }
    };

    _o.draw_selected = function () { $.each(_o.state.cards_selected,
        function (i, ci) { _o.board_draw_cardi(ci); }) 
    };

    _o.bad_set_show = function() {
        _o.state.selected_draw_mode = _o.c.DRAW_CARD_NOT_A_SET; // for redraw
        _o.draw_selected();
    };    

    _o.message_handlers = undefined; // set in initnew Array(_o.c.E3333_S2C_N);

    _o.dialogs_init = function () {
        $("#gameover").dialog({
            autoOpen: false,
            modal: true,
            open: function (event, ui) {
                jQuery('.ui-dialog-titlebar-close').hide();
            },
            closeOnEscape: false,
        });


        $("#newjointbl").dialog({
            autoOpen: false,
            width: 400, height: 300,
            modal: true,

        });

        $("#warning").dialog({
            title: "Warning",
            autoOpen: false,
            width: 400, height: 200,
            modal: true,
        });
    };


    _o.help = function () {
        var winw = $(window).width(), winh = $(window).height();
        $("#help").dialog({width: winw/2, height: 2*winh/3});
        console.log("help");
        $("#help").dialog("open");
    };

    _o.event_handler_now = function (evt) {
        console.log("evt.data=" + evt.data);
        var edata = JSON.parse(evt.data);
        var rc = edata['rc'];
        console.log("edata="+edata+", rc="+rc);
        if (rc === undefined) {
            rc = _o.c.E3333_BAD_COMMAND;
        }
        if (rc != _o.c.E3333_OK) {
            console.log("not _o.c.E3333_OK");
            if (rc == _o.c.E3333_NOT_A_SET) {
                console.log("Not a set");
                _o.bad_set_show();
                _o.warning_code(rc, function () {
                     console.log("dm:=selected");
                     _o.state.selected_draw_mode = _o.c.DRAW_CARD_SELECTED;
                     _o.state.cards_selected.pop();
                     _o.board_show();
                });
            } else {
                _o.warning_code(rc);
            }
        } else {
            var cmd = edata['cmd'];
            cmd = Number(cmd); // delete hhis line !?!
            var h = _o.message_handlers[cmd];
            console.log("cmd="+cmd+", h?="+(!!(h)));
            if (h) {
                h(edata['result']);
            }
        }
    };

    _o.handle_delayed_events = function () {
        console.log("handle_delayed_events");
        while ((!_o.state.delayed) && (_o.state.delayed_events.length > 0)) {
            _o.event_handler_now( _o.state.delayed_events.shift() );
        }
    };

    _o.event_handler = function (evt) {
        console.log("event_handler");
        if (_o.state.delayed) {
            console.log("Delayed event");
            _o.state.delayed_events.push(evt);
        } else {
            _o.event_handler_now(evt);
        }
    };

    _o.web_socket_open = function () {
        win_hostname =  window.location.hostname
        if (win_hostname == "") { win_hostname = "localhost"; }
        var host = "ws://" + win_hostname + ":" + _o.c.CS3333_PORT + "/ws";
        console.log("host="+host)
        _o.web_socket = new WebSocket(host);
        console.log("readyState=" + _o.web_socket.readyState);
        _o.web_socket.onopen = function () {
            console.log("websocket.onopen");
            _o.tables_status();
        };
        _o.web_socket.onmessage = _o.event_handler;
    };

    _o.init_common = function () {
        var name = "3333-Game Club", host;

        _o.fill_passes[_o.c.SHADING_FILL] = [true];
        _o.fill_passes[_o.c.SHADING_STRIPED] = [true, false];
        _o.fill_passes[_o.c.SHADING_OPEN] = [false];

        _o.shape_draw[_o.c.SYM_DIAMOND] = _o.draw_diamond;
        _o.shape_draw[_o.c.SYM_SQUIGGLE] = _o.draw_squiggle;
        _o.shape_draw[_o.c.SYM_OVAL] = _o.draw_oval;

        _o.message_handlers = new Array(_o.c.E3333_S2C_N);
        _o.message_handlers[_o.c.E3333_S2C_GSTATE] = _o.state_update;
        _o.message_handlers[_o.c.E3333_S2C_NTBL] = _o.msgh_new_table;
        _o.message_handlers[_o.c.E3333_S2C_JOIN] = _o.msgh_join;
        _o.message_handlers[_o.c.E3333_S2C_TBLS] = _o.msgh_tables_status;
        _o.message_handlers[_o.c.E3333_S2C_SET_FOUND] = _o.msgh_set_found;
        _o.message_handlers[_o.c.E3333_S2C_CONNECTION_TAKEN] =
            _o.connection_taken;
        _o.message_handlers[_o.c.E3333_S2C_TABLE_CLOSED] = _o.table_closed;

        _o.c.draw_mode_frame_rgb[_o.c.DRAW_CARD_NORMAL] = "#888"; // unused
        _o.c.draw_mode_frame_rgb[_o.c.DRAW_CARD_SELECTED] = "#111";
        _o.c.draw_mode_frame_rgb[_o.c.DRAW_CARD_NOT_A_SET] = "#f00";
        _o.c.draw_mode_frame_rgb[_o.c.DRAW_CARD_IS_A_SET] = "#2c3";

        _o.state.selected_draw_mode = _o.c.DRAW_CARD_SELECTED;

        $("#clubtitle").html('<span class="bigletters">' + name +
            "</span> (β " + _o.c.version + ")");
        $("#newtbl").click(this.new_table_dialog);
        $("#club-refresh").click(this.tables_status);
        $("#newgame").click(this.new_game);
        $("#nomore").click(this.add3_no_more);
        $("#nomore").attr("disabled", true);
        $("#board").click(_o.card_select);

        _o.web_socket_open();
    }

    _o.init_desktop = function () {

        console.log("_o.init_desktop");
        $("#tabs-help").load("help.html");


        this.dialogs_init();
        $("#main-tabs").tabs();
        _o.init_common();
        $("#table").hide();
        // $("#board").click(_o.card_select);
        $(window).resize(_o.board_show);

        $("#boardwrap").resize(function () {
            var bw = $("#boardwrap");
            console.log("bw.resize: w="+bw[0].offsetWidth + 
              ", h="+bw[0].offsetHeight);
        });
          
    };

    _o.init_mobile = function () {
        console.log("init_mobile");

        $(".change_page").change(function() {
            var v = $(this).children('option:selected').attr('value');
            console.log("v=" + v);
            var curr = $("body").pagecontainer("getActivePage").attr("id");
            console.log("curr=" + curr);
            if (v != curr) {
                _o.mobile_page_set(v);
            }
        });

        _o.state.mobile = true;
        // $("#help-content").load("help.html");
        _o.init_common();
        $("#popup-warning").popup();
        $("#popup-info").popup();
        $(window).resize(_o.board_show);
        $(window).on( "orientationchange", function( event ) {
             console.log("device in " + event.orientation + " mode");
             // _o.board_show();
        });
    }


    _o.init = function () {
        console.log("location.pathname="+location.pathname);
        var m3333 = (location.pathname.indexOf("m3333.html") >= 0);
        var lsearch = location.search.substr(1);
        var kvs = lsearch.split('&');
        var mobile = -1;
        for (var i = 0; i < kvs.length; i++) {
            var kv = kvs[i].split('=');
            if (kv.length == 2) {
                if (kv[0] === 'mobile') { mobile = kv[1]; }
            }
        }

        if ((screen.width < 1000) && (!m3333) && (mobile != 0)) {
            // document.location = "mobile.html";
            location.replace("m3333.html?mobile=1");
        } else{
            $("#closetable").click(_o.table_close);
            if (m3333) { _o.init_mobile(); } else { _o.init_desktop(); }
        }
    };

    _o.c = {
        // 4 Dimensions
        DIM_NUMBER: 0,
        DIM_SYMBOL: 1,
        DIM_COLOR: 2,
        DIM_SHADING: 3,

        SYM_DIAMOND: 0,
        SYM_SQUIGGLE: 1,
        SYM_OVAL: 2,

        COLOR_RED: 0,
        COLOR_GREEN: 1,
        COLOR_PURPLE: 2,

        SHADING_FILL: 0,
        SHADING_STRIPED: 1,
        SHADING_OPEN: 2,

        DRAW_CARD_NORMAL: 0,
        DRAW_CARD_SELECTED: 1,
        DRAW_CARD_NOT_A_SET: 2,
        DRAW_CARD_IS_A_SET: 3,

        draw_mode_frame_rgb: new Array(4),        

        rgb_colors: ["#e21", "#382", "#a3f"],
        pattern_stripes: [undefined, undefined, undefined],

        // On purpose. almost at end-of-file to maintain most line-numbers.
        // Magic include of filtered common-consts.py
    };

    return _o;
})();

$(document).ready(function () {
    set3333.init();
});
