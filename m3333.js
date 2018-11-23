console.log("Yotam 1\n");

document.addEventListener('prechange', function(event) {
  console.log("Yotam addEventListener\n");
});

function getUrlVars() {
    var vars = {};
    var parts = window.location.href.replace(/[?&]+([^=&]+)=([^&]*)/gi, 
        function(m,key,value) {
            vars[key] = value;
    });
    return vars;
}

gelem = function(idname) { 
  var ret = document.getElementById(idname); 
  if (ret === null) {
    console.log('gelem: not (yet?) found: "' + idname + '"');
  }
  return ret;
};

ymdhms = function () {
  var d2 = function(n) {
    if (n < 10) { n = '0' + n; }
    return n;
  }

  var d = new Date();
  s = d.getUTCFullYear() + '/' + 
    d2((d.getUTCMonth() + 1)) + '/' + 
    d2(d.getUTCDate()) + ' ' +
    d2((d.getUTCHours())) + ':' +
    d2((d.getUTCMinutes())) + ':' +
    d2((d.getUTCSeconds()));
  return s;
};

console_all_elements = function () {
  var all = document.getElementsByTagName("*");
  for (var i=0, max=all.length; i < max; i++) {
    console.log('element[' + i + '] = ' + all[i] + ' id='+ all[i].id);
  }
}

var hideDialog = function (id) {
  gelem(id).hide();
};

var hidePopover = function (popid) {
  gelem(popid).hide();
};

show_tree = function (root, depth, ci) {
  var indent = ''
  for (var i = 0; i < depth; ++i) { indent += '  '; }
  if (root.id !== undefined) {
    console.log(indent + '[' + ci + '] ' + root + ' id=' + root.id
      + ', w='+root.width + ', h='+root.height 
      + ', cw='+root.clientWidth + ', ch='+root.clientHeight
      // + ', iw='+root.innerWidth + ', ih='+root.innerHeight 
      // + ', ow='+root.outerWidth + ', oh='+root.outerHeight
    );
  }
  for (var i = 0; (depth < 8) && (i < root.childNodes.length); i++) {
    var e = root.childNodes[i];
    show_tree(e, depth + 1, i);
  }
}

init_ui = function (_o) {

  // { Add UI constants
  // 4 Dimensions
  _o.c.DIM_NUMBER = 0;
  _o.c.DIM_SYMBOL = 1;
  _o.c.DIM_COLOR = 2;
  _o.c.DIM_SHADING = 3;

  _o.c.SYM_DIAMOND = 0;
  _o.c.SYM_SQUIGGLE = 1;
  _o.c.SYM_OVAL = 2;

  _o.c.COLOR_RED = 0;
  _o.c.COLOR_GREEN = 1;
  _o.c.COLOR_PURPLE = 2;

  _o.c.SHADING_FILL = 0;
  _o.c.SHADING_STRIPED = 1;
  _o.c.SHADING_OPEN = 2;

  _o.c.DRAW_CARD_NORMAL = 0;
  _o.c.DRAW_CARD_SELECTED = 1;
  _o.c.DRAW_CARD_NOT_A_SET = 2;
  _o.c.DRAW_CARD_IS_A_SET = 3;

  _o.c.draw_mode_frame_rgb = new Array(4);
  _o.c.draw_mode_frame_rgb[_o.c.DRAW_CARD_NORMAL] = "#888"; // unused
  _o.c.draw_mode_frame_rgb[_o.c.DRAW_CARD_SELECTED] = "#111";
  _o.c.draw_mode_frame_rgb[_o.c.DRAW_CARD_NOT_A_SET] = "#f00";
  _o.c.draw_mode_frame_rgb[_o.c.DRAW_CARD_IS_A_SET] = "#2c3";

  _o.c.rgb_colors = ["#e21", "#382", "#a3f"];
  _o.c.pattern_stripes = [undefined, undefined, undefined];
  // } Added UI constants

  _o.init_canvas = function (_o) {
    console.log('init table page');
    var canvas = gelem('canvas');
    console.log('canvas=' + canvas);
    url_canvas = getUrlVars()['canvas'];
    console.log('url_canvas='+url_canvas);
    if (url_canvas == 'no') { // for css debug
      return;
    }
    if (canvas === null)
    {
      show_tree(document.body, 0, 0);
      var wrap = gelem('board-wrap');
      console.log('Create canvas @ ' + wrap);
      canvas = document.createElement('canvas');
      canvas.setAttribute('id', "board");
      wrap.appendChild(canvas);
    }
    bw = gelem('board-wrap');
    console.log('board-wrap: w=' + bw.width + ', h=' + bw.height);
    console.log(' .. client: w=' + bw.clientWidth + ', h=' + bw.clientHeight);
    console.log('y1 canvas=' + canvas);
    console.log('canvas: w=' + canvas.width + ', h='+canvas.height);
    console.log('canvasClient: w=' + canvas.clientWidth + 
      ', h='+canvas.clientHeight);
    // Make it visually fill the positioned parent
    if (false) {
      canvas.style.width ='100%';
      canvas.style.height='100%';
      // ...then set the internal size to match
      canvas.width  = canvas.offsetWidth;
      canvas.height = canvas.offsetHeight;
    }
    canvas.width = bw.clientWidth - 2;
    canvas.height = bw.clientHeight - 2;
    var context = canvas.getContext('2d');
    context.fillStyle = "#242";
    context.fillRect(0, 0, canvas.width, canvas.height);
    context.fillStyle = "#6ab";
    var w = canvas.width, h = canvas.height;
    context.fillRect(0, 0, w/8, h/8);
    context.fillRect(7*w/8, 0, w, h/8);
    context.fillRect(0, 7*h/8, w/8, h);
    context.fillRect(7*w/8, 7*h/8, w, h);
    console.log('canvas: w=' + canvas.width + ', h='+canvas.height);
    return canvas;
  };

  document.addEventListener('init', function(event) {
    var page = event.target;
    console.log('page=' + page);
    console.log('page,id=' + page.id);
    if (page.matches('#table')) {
      _o.init_canvas(_o);
      _o.ui_completed = true; // may need stronger condition
      _o.init_server(_o);
    }
  });

  _o.warning = function (text, okfunc) {
    var text_set = function(text) { gelem('warning-content').innerHTML = text; }
    console.log('warning: '+text);
    var dialog = gelem('warning-dialog');
    if (dialog) {
      text_set(text);
      dialog.show();
    } else {
      ons.createElement('warning.html', { append: true })
      .then(function(dialog) {
        text_set(text);
        dialog.show();
        gelem('b-warning-ok').onclick = function () {
          gelem('warning-dialog').hide();
          if (okfunc) { okfunc(); }
          console.log('warning done');
        };
      });
    }
  };

  _o.warning_code = function (error_code, okfunc) {
    _o.warning('error_code='+error_code, okfunc);
  };

  _o.show_new_table_dialog = function(_o) {

    var cb = function (_o) {
      console.log('create-table');
      var name = gelem('table-name').value;
      var owner_pw = gelem('owner-password').value;
      var table_pw = gelem('table-password').value;
      var pw_flags = 1*(!(table_pw === "")) + 2*(!(owner_pw === ""));
      console.log('name='+name + ', owner_pw='+owner_pw + 
        ', table_pw='+table_pw);
      _o.web_socket.send([
        _o.c.S3333_C2S_NTBL, name, pw_flags, table_pw, owner_pw].join(" "));
    };

    var dialog = gelem('dialog-new-table');
    if (dialog) {
      dialog.show();
    } else {
      ons.createElement('new-table.html', { append: true })
        .then(function(dialog) {
          dialog.show();
          // console_all_elements();
          ss = ['table-name', 'owner-password', 'table-password'];
          for (si in ss) {
            var s = ss[si];
            var b = gelem('bi-' + s);
            console.log('si='+si + ', b='+b.id);
            b.onclick = (function () { 
              var slocal = s;
              var blocal = b;
              return function () {
                console.log('clicked: ' + blocal.id);
                gelem('popinfo-' + slocal).show(blocal);
              };
            })();
          };
          gelem('b-create-table').onclick = function () { cb(_o); }
        });
    }
  };

  _o.board_clear = function () {
    var position = 6; // gelem('boardwrap').position();
    // var winw = $(window).width(), winh = $(window).height();
    var canvas = gelem("board");
    var context = canvas.getContext('2d'); // Get 2D drawing context
    console.log('window: iw='+window.innerWidth + ', ih='+window.innerHeight);
    // console.log('canvas: iw='+canvas.innerWidth + ', ih='+canvas.innerHeight);
    // console.log('window: w='+window.width + ', h='+window.height);
    console.log('canvas: w='+canvas.width + ', h='+canvas.height);
    // canvas.width = (9*window.innerWidth)/10;
    // canvas.height = (9*window.innerHeight)/10;
    // canvas.width = 180; canvas.height = 220;
    console.log('board_clear: canvas: w='+canvas.width + ', h='+canvas.height);
    context.fillStyle = "#242";
    context.fillStyle = "#777";
    context.fillRect(0, 0, canvas.width, canvas.height);
    if (true) { // debug
      var w = canvas.width, h = canvas.height;
      context.fillStyle = "#6ab";
      context.fillRect(0, 0, w/8, h/8);
      context.fillRect(7*w/8, 0, w, h/8);
      context.fillRect(0, 7*h/8, w/8, h);
      context.fillRect(7*w/8, 7*h/8, w, h);
    }
    return { canvas: canvas, context: context };
  }

  _o.board_show = function () {
    var board_n_columns = function (width, height, n) {
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

    var pattern_stripes_set = function (_o, card_height) {
      var pat_height = Math.max(card_height / 24, 10);
      var stripe_height = (3*pat_height)/5;
      for (var ci = 0; ci < 3; ci++) {
        var offscreen = document.createElement('canvas');
        offscreen.width = 8;
        offscreen.height = pat_height;
        var ctx = offscreen.getContext('2d')
        ctx.fillStyle = _o.c.rgb_colors[ci];
        ctx.fillRect(0, 0, 8, stripe_height);
        _o.c.pattern_stripes[ci] = ctx.createPattern(offscreen, 'repeat');
      }
    };

    _o.board = {
        n_columns: 1,
        card_width: 1,
        card_height: 1,
        xgap: 0,
        ygap: 0
    };
    var bret = _o.board_clear();
    var canvas = bret.canvas;
    var context = bret.context;
    var width = canvas.width, height = canvas.height;

    var n_cards = _o.state.cards_active_idx.length;
    var columns = _o.board.n_columns = 
        board_n_columns(width, height, n_cards);
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
    pattern_stripes_set(_o, _o.board.card_height);

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

  gelem('b-new-table').onclick = function () { _o.show_new_table_dialog(_o); };
};


init_server = function (_o) {
  console.log('init_server');

  _o.init_state = function (_9) { 
    _o.state = {
      myname: "",
      table_name: "",
      owner: false, // true if I am the table owner
      tstate: -1,
      gstate: -1,
      deck_size: 81,
      game_active: false,
      cards_active_idx: [], // Indices to cards
      cards_selected: [], // Indices to cards_active_idx
      selected_draw_mode: undefined,
      delayed: false,
      delayed_events: [],
    };
  };
  _o.init_state();

  _o.msgh_new_table = function (_o, result) {
    console.log('msgh_new_table: ...');
    hideDialog('dialog-new-table');
    _o.state.owner = true;
    _o.board_show();
    // _o.web_socket.send(_o.c.S3333_C2S_TBLS)
    // _o.mobile_set_titles();
    // _o.mobile_page_set("table");
  };

  _o.init_handlers = function (_o) {
    _o.message_handlers = new Array(_o.c.E3333_S2C_N);
    _o.message_handlers[_o.c.E3333_S2C_GSTATE] = _o.state_update;
    _o.message_handlers[_o.c.E3333_S2C_NTBL] = _o.msgh_new_table;
    _o.message_handlers[_o.c.E3333_S2C_JOIN] = _o.msgh_join;
    _o.message_handlers[_o.c.E3333_S2C_TBLS] = _o.msgh_tables_status;
    _o.message_handlers[_o.c.E3333_S2C_SET_FOUND] = _o.msgh_set_found;
    _o.message_handlers[_o.c.E3333_S2C_CONNECTION_TAKEN] =
        _o.connection_taken;
    _o.message_handlers[_o.c.E3333_S2C_TABLE_CLOSED] = _o.table_closed;
  }
  _o.init_handlers(_o);

  _o.tables_status = function (_o) {
  };

  _o.event_handler_now = function (_o, evt) {
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
          h(_o, edata['result']);
        }
    }
  };

  _o.handle_delayed_events = function (_o) {
     console.log("handle_delayed_events");
     while ((!_o.state.delayed) && (_o.state.delayed_events.length > 0)) {
         _o.event_handler_now(_o, _o.state.delayed_events.shift() );
     }
  };

  _o.event_handler = function (_o, evt) {
     console.log("event_handler");
     if (_o.state.delayed) {
         console.log("Delayed event");
         _o.state.delayed_events.push(evt);
     } else {
         _o.event_handler_now(_o, evt);
     }
  };

  var win_hostname = window.location.hostname
  if (win_hostname == "") { win_hostname = "localhost"; }
  var host = "ws://" + win_hostname + ":" + _o.c.CS3333_PORT + "/ws";
  console.log("host="+host)
  _o.web_socket = new WebSocket(host);
  console.log("web_socket.readyState=" + _o.web_socket.readyState);
  _o.web_socket.onopen = function () {
      console.log("websocket.onopen");
      _o.tables_status();
  };
  _o.web_socket.onmessage = function (evt) { _o.event_handler(_o, evt); };
}

document.addEventListener("DOMContentLoaded", function (event) {
  console.log('DOMContentLoaded called: ' + ymdhms());
  var _o = {}
  _o.c = m3333_consts;
  _o.ui_completed = false;
  _o.init_server = init_server;
  init_ui(_o);
  // init_server(_o);
});

console.log('End of 3.js');
