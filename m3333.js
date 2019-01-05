console.log("Yotam 1\n");

document.addEventListener('prechange', function(event) {
  console.log("Yotam addEventListener\n");
});

popi = function (a, i) {
  var last = a.pop();
  if (i < a.length) {
    a[i] = last;
  }
};


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

function get_longest_tr(parent, celltag) {
  let tr_longest = null;
  let trs = parent.getElementsByTagName('tr');
  let cells_max = 0;
  for (let i = 0; i < trs.length; i++) { 
    let tr = trs[i]; 
    let cells = tr.getElementsByTagName(celltag);
    if (cells_max < cells.length) {
      cells_max = cells.length;
      tr_longest  = tr;
    }
  }
  return tr_longest;
}

function table_align_head_body(tbl_id) {
  let table = document.getElementById(tbl_id);
  let thead = table.getElementsByTagName('thead');
  let tbody = table.getElementsByTagName('tbody');
  let header_tr = get_longest_tr(thead[0], 'th');
  let body_tr = get_longest_tr(tbody[0], 'td');

  console.log('header_tr='+header_tr + ', body_tr='+body_tr);
  let ths = header_tr.getElementsByTagName("th");
  let tds = [];
  if (body_tr !== null) {
    tds = body_tr.getElementsByTagName("td");
  }
  let ncols = Math.min(ths.length, tds.length);
  for (var ti = 0; ti < ncols; ++ti) {
    let cell_big = ths[ti];
    let cell_small = tds[ti];
    let wbig = cell_big.offsetWidth;
    let wsmall= cell_small.offsetWidth;
    if (wsmall != wbig) {
      if (wbig < wsmall) {
	let tcell = cell_big; cell_big = cell_small; cell_small = tcell;
      }
      let style = window.getComputedStyle(cell_big, null);
      let style_width = style.getPropertyValue("width");
      let sstyle = window.getComputedStyle(cell_small, null);
      cell_small.setAttribute('width', style_width)
    }
  }
}

init_ui = function (_o) {

  console.log('init_ui');
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
    url_canvas = getUrlVars()['board'];
    console.log('url_canvas='+url_canvas);
    if (url_canvas == 'no') { // for css debug
      return;
    }
    if (canvas === null)
    {
      // show_tree(document.body, 0, 0);
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
    canvas.onclick = function (e) { _o.card_select(e); };
    console.log('canvas: w=' + canvas.width + ', h='+canvas.height);
    return canvas;
  };

  function new_game(_o) {
    console.log('New game');
    _o.web_socket.send(_o.c.S3333_C2S_GNEW);
  }

  function add3(_o) {
    console.log('add3');
    _o.web_socket.send(_o.c.S3333_C2S_ADD3 + " " + _o.state.gstate);
  }

  function no_more(_o) {
    console.log('no_more');
    _o.web_socket.send(_o.c.S3333_C2S_NMOR + " " + _o.state.gstate);
  }

  function game_over(_o) {
    info(_o, 'Game Over!', function () {
      gelem('done').disabled = true;
    });
  }

  document.addEventListener('init', function(event) {
    var page = event.target;
    console.log('page=' + page);
    console.log('page,id=' + page.id);
    _o.init_server(_o);
    if (page.matches('#table')) {
      gelem('start').onclick = function () { new_game(_o); }
      gelem('add3').onclick = function () { add3(_o); }
      gelem('done').onclick = function () { no_more(_o); }
      gelem('start').disabled = true;
      gelem('add3').disabled = true;
      gelem('done').disabled = true;
      _o.init_canvas(_o);
      _o.ui_completed = true; // may need stronger condition
    }
    if (page.matches('#club')) {
      console.log('init club page');
      table_align_head_body('tables-table');
    }
  });

  function info_dialog_show(_o, dialog, text, okfunc) {
    console.log('info_dialog_show: ' + text);
    gelem('info-content').innerHTML = text;
    dialog.show();
    gelem('b-info-ok').onclick = function () {
      gelem('info-dialog').hide();
      if (okfunc) { okfunc(); }
      console.log('info done');
    };
  }

  function info(_o, text, okfunc) {
    console.log('info: ' + text);
    let dialog  = gelem('info-dialog');
    if (dialog) {
      info_dialog_show(_o, dialog, text, okfunc);
    } else {
      ons.createElement('info.html', { append: true })
      .then(function(dialog) {
        info_dialog_show(_o, dialog, text, okfunc);
      });
    }
  }

  function warning_dialog_show(_o, dialog, text, okfunc) {
    gelem('warning-content').innerHTML = text;
    dialog.show();
    gelem('b-warning-ok').onclick = function () {
      gelem('warning-dialog').hide();
      if (okfunc) { okfunc(); }
      console.log('warning done');
    };
  }

  _o.warning = function (text, okfunc) {
    console.log('warning: '+text);
    var dialog = gelem('warning-dialog');
    if (dialog) {
      warning_dialog_show(_o, dialog, text, okfunc);
    } else {
      ons.createElement('warning.html', { append: true })
      .then(function(dialog) {
        warning_dialog_show(_o, dialog, text, okfunc);
      });
    }
  };

  _o.warning_code = function (_o, error_code, okfunc) {
    console.log('error_code='+error_code);
    var text = ''; // _o.warning_code_to_text[error_code];
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
    console.log('error_code='+error_code + ', text: ' + text);
    _o.warning(text, okfunc);
  };

  function refresh_club(_o) {
    console.log('refresh_club');
    _o.web_socket.send(_o.c.S3333_C2S_TBLS);
  }

  function show_new_table_dialog(_o) {

    var cb = function (_o) {
      console.log('create-table');
      var name = gelem('table-name').value;
      var owner_pw = gelem('owner-password').value;
      var table_pw = gelem('table-password').value;
      var pw_flags = 1*(!(table_pw === "")) + 2*(!(owner_pw === ""));
      console.log('name='+name + ', owner_pw='+owner_pw + 
        ', table_pw='+table_pw);
      _o.state.myname = _o.state.table_name = name;
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

  _o.join_table = function (_o, table_name) {
    console.log('table_name='+table_name);
    _o.state.table_name = table_name
    var cb = function (_o) {
      console.log('join-table');
      var name = gelem('guest-name').value;
      var guest_pw = gelem('guest-password').value;
      var table_pw = gelem('jtable-password').value;
      var pw_flags = 1*(!(table_pw === "")) + 2*(!(guest_pw === ""));
      console.log('name='+name + ', guest_pw='+guest_pw + 
        ', table_pw='+table_pw);
      _o.state.myname =name;
      _o.web_socket.send([
        _o.c.S3333_C2S_JOIN, _o.state.table_name, name, pw_flags, table_pw,
        guest_pw].join(' '));
    };

    var dialog = gelem('dialog-join-table');
    if (dialog) {
      dialog.show();
    } else {
      ons.createElement('join-table.html', { append: true })
        .then(function(dialog) {
          dialog.show();
          ss = ['guest-name', 'guest-password', 'jtable-password'];
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
          gelem('b-join-table').onclick = function () { cb(_o); }
        });
    }
  }

  _o.card_get_draw_mode = function(ci) {
    var draw_mode = (_o.state.cards_selected.includes(ci) 
      ? _o.state.selected_draw_mode : _o.c.DRAW_CARD_NORMAL);
    return draw_mode;
  }

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

  _o.golden = (Math.sqrt(5.)+1.)/2.;

  function draw_diamond(context, x, y, w, h) {
    context.beginPath();
    context.moveTo(x + w/2, y);
    context.lineTo(x + w, y + h/2);
    context.lineTo(x + w/2, y + h);
    context.lineTo(x, y + h/2);
    context.closePath();
  };

  function aa_round(aa) {
    for (var i = 0; i < aa.length; i++) {
      var a = aa[i];
      for (var j = 0; j < a.length; j++) {
	a[j] = [Math.round(a[j][0]), Math.round(a[j][1])];
      }
    }
  };

  function draw_squiggle(context, x, y, w, h) {
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
    aa_round(rel_curves_left);
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
      for (j = 0; j < 3; j++) {
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

  function draw_oval(context, x, y, w, h) {
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

  var shape_draw = [undefined, undefined, undefined];
  shape_draw[_o.c.SYM_DIAMOND] = draw_diamond;
  shape_draw[_o.c.SYM_SQUIGGLE] = draw_squiggle;
  shape_draw[_o.c.SYM_OVAL] = draw_oval;

  function draw_card(context, card, x, y, w, h, draw_mode) {
    // console.log("draw_card: mode="+draw_mode +
    // " x="+x+" y="+y+" w="+w+" h="+h);
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

    draw_func = shape_draw[symbol];

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

  function stats_show(_o, rstate) {
    console.log('myname='+_o.state.myname);
    gelem('deck').innerHTML = rstate['deck'];
    var players = rstate['players'];
    for (var pi = 0; pi < players.length; ++pi) {
      var player = players[pi];
      console.log(player);
      if (player.name == _o.state.myname) {
        var nums = player['numbers'];
        gelem('table-found').innerHTML = nums[0] + nums[2];
        gelem('table-bad-calls').innerHTML = nums[1] + nums[3];
      }
    }
  }

  _o.board_show = function (_o) {
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
      console.log('pattern_stripes_set');
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
    console.log('width='+width + ', height='+height);

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
    console.log(_o.board);
    pattern_stripes_set(_o, _o.board.card_height);

    var xgap = (width - (columns * _o.board.card_width)) / (columns + 1);
    var ygap = (height - (rows * _o.board.card_height)) / (rows + 1);
    _o.board.xgap = xgap;    _o.board.ygap = ygap;
    console.log('xgap='+xgap + ', ygap='+ygap);

    var xn = xgap, yn = ygap;
    var xi = 0
    var x = xgap, y = ygap;
    for (var ci = 0; ci < n_cards; ci++) {
      var card = _o.cards[_o.state.cards_active_idx[ci]];
      draw_card(context, card, x, y, _o.board.card_width,
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

  function players_update(_o, rstate) {
    var table = gelem('players-table');
    var players = rstate['players'];
    var rows = table.getElementsByTagName('tr');
    for (var ri = table.rows.length - 1; ri > 0; --ri) {
      table.deleteRow(ri);
    }
    for (var pi = 0; pi < players.length; ++pi) {
      var p = players[pi];
      var row = table.insertRow(pi + 1);
      row.insertCell(0).innerHTML = p['name'];
      for (var ni = 0; ni < 4; ++ni) {
        row.insertCell(ni + 1).innerHTML = p['numbers'][ni];
      }
      row.insertCell(5).innerHTML = p['tcreated'];
      row.insertCell(6).innerHTML = p['taction'];
    }
  }

  function cheat_tip(_o) {
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

  _o.state_update = function (_o, rstate) {
    console.log('state_update'); console.log(rstate);
    if (_o.state.tstate < rstate['tstate']) {
      _o.state.tstate = rstate['tstate'];
      // _o.players_fill(rstate['players']);
      players_update(_o, rstate);
      stats_show(_o, rstate);
    }
    if (_o.state.gstate < rstate['gstate']) {
      var old_game_active = _o.state.game_active;
      _o.state.gstate = rstate['gstate'];
      _o.state.cards_active_idx = rstate['active'];
      if ((rstate['deck'] == 0) && (_o.state.deck_size > 0)) {
        gelem('add3').disabled = true;
        gelem('done').disabled = false;
      }
      _o.state.deck_size = rstate['deck'];
      if (rstate['gactive'] && !_o.state.game_active) {
        gelem('add3').disabled = false;
        gelem('done').disabled = true;
      }
      _o.state.game_active = rstate['gactive'];
      _o.board_show(_o);
      if (old_game_active && !_o.state.game_active) {
        game_over(_o);
      }
      cheat_tip(_o)
    }
  };

  // redraw just one card
  _o.board_draw_cardi = function (ci) {
    console.log("board_draw_cardi: ci="+ci);
    var canvas = gelem('board');
    var context = canvas.getContext("2d"); // Get 2D drawing context
    var b = _o.board;
    var xwg = (b.card_width + b.xgap);
    var yhg = (b.card_height + b.ygap);
    var x = b.xgap + (ci % b.n_columns) * xwg;
    var y = b.ygap + Math.floor(ci / b.n_columns) * yhg;
    var card = _o.cards[_o.state.cards_active_idx[ci]];
    draw_card(context, card, x, y, b.card_width, b.card_height,
      _o.card_get_draw_mode(ci));
  };


  function xy2card_index(_o, x, y) {
    var b = _o.board;
    var column = (x - b.xgap) / (b.card_width + b.xgap);
    var row    = (y - b.ygap) / (b.card_height + b.ygap);
    column = Math.max(Math.floor(column), 0);
    row = Math.max(Math.floor(row), 0);
    var i = b.n_columns * row + column;
    console.log('x='+x + ', y='+y + ', column='+column + ', row='+row +
        ', n_columns='+b.n_columns + ', i='+i);
    if (i >= _o.state.cards_active_idx.length) { i = -1; }
    return i;
  };


    _o.user_try3 = function () {
        console.log("user_try3");
        _o.web_socket.send(_o.c.S3333_C2S_TRY3 + " " + _o.state.gstate + " " + 
            _o.state.cards_selected.join(" "));
    };


  _o.card_select = function (e) {
    console.log('card_select'); console.log(e);
    var rect = e.target.getBoundingClientRect();
    console.log('rect'); console.log(rect); 
    var ci = xy2card_index(_o, e.clientX - rect.left, e.clientY - rect.top);
    console.log("card_select: mouse=("+e.clientX+","+e.clientY+"), ci="+ci);
    if (ci >= 0) {
      var si = _o.state.cards_selected.indexOf(ci);
      if (si == -1) {
        _o.state.cards_selected.push(ci);
        if (_o.state.cards_selected.length == 3) {
          _o.user_try3();
        }
      } else {
        popi(_o.state.cards_selected, si);
      }
      _o.board_draw_cardi(ci);
    }
  };


  gelem('b-new-table').onclick = function () { show_new_table_dialog(_o); };
  gelem('reresh').onclick = function () { refresh_club(_o); };
  console.log('init_ui done');
};


init_server = function (_o) {
  console.log('init_server');

  _o.init_state = function (_o) { 
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
      selected_draw_mode: _o.c.DRAW_CARD_SELECTED,
      delayed: false,
      delayed_events: [],
    };
  };
  _o.init_state(_o);

  function show_table_name (_o) {
    gelem('tab-table-name').innerHTML = _o.state.table_name;
    gelem('players-table-name').innerHTML = _o.state.table_name;
  }

  _o.msgh_new_table = function (_o, result) {
    console.log('msgh_new_table: ...');
    console.log(result);
    hideDialog('dialog-new-table');
    _o.state.owner = true;
    gelem('start').disabled = false;
    _o.board_show(_o);
    show_table_name(_o);
    gelem('tabbar').setActiveTab(1); // From Club tab to Table tab
    // _o.web_socket.send(_o.c.S3333_C2S_TBLS)
    // _o.mobile_set_titles();
    // _o.mobile_page_set("table");
  };

  _o.msgh_join = function (_o, result) {
    console.log('msgh_join');
    hideDialog('dialog-join-table')
    gelem('tabbar').setActiveTab(1); // From Club tab to Table tab
  };

  _o.msgh_tables_status = function (_o, result) {
    console.log(result);
    console.log('result.length='+result.length);
    let tbody = gelem('tables-tbody');
    let rows = tbody.getElementsByTagName('tr');
    for (let ri = rows.length - 1; ri >= 0; --ri) {
      tbody.deleteRow(ri);
    }
    for (let ri = 0; ri < result.length; ++ri) {
      let tblinf = result[ri];
      let table_name = tblinf[0];
      let row = tbody.insertRow(ri);
      let button = ons._util.createElement("<ons-button>Join</ons-button>");
      button.onclick = function () { 
        console.log('Join table: ' + table_name);
        _o.join_table(_o, table_name);
      };
      let td = row.insertCell(0);
      td.appendChild(button);
      row.insertCell(1).innerHTML = table_name;
      row.insertCell(2).innerHTML = tblinf[1];
      row.insertCell(3).innerHTML = tblinf[3];
      row.insertCell(4).innerHTML = tblinf[4];
    }
    table_align_head_body('tables-table');
  };

  _o.msgh_set_found = function (_o, a3i) {
    console.log("msgh_set_found: a3i="+a3i);
    _o.state.cards_selected = a3i;
    _o.state.selected_draw_mode = _o.c.DRAW_CARD_IS_A_SET;
    _o.board_show(_o); // For all players!
    _o.state.delayed = true;
    setTimeout(function () {
      console.log('after found shown');
      _o.state.delayed = false;
      _o.state.cards_selected = [];
      _o.state.selected_draw_mode = _o.c.DRAW_CARD_SELECTED;
      _o.board_show(_o);
      _o.handle_delayed_events(_o);
    }, 600);
  };

  var init_gfx_modes = function (_o) {
    _o.fill_passes = [undefined, undefined, undefined];
    _o.fill_passes[_o.c.SHADING_FILL] = [true];
    _o.fill_passes[_o.c.SHADING_STRIPED] = [true, false];
    _o.fill_passes[_o.c.SHADING_OPEN] = [false];
  }
  init_gfx_modes(_o);

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
  };
  _o.init_handlers(_o);


  console.log(_o.message_handlers);

  function tables_status (_o) {
    console.log('tables_status ask server');
    _o.web_socket.send(_o.c.S3333_C2S_TBLS)
  };

  function draw_selected(_o) {
    for (var i = 0; i < _o.state.cards_selected.length; ++i) {
       _o.board_draw_cardi(_o.state.cards_selected[i]);
    }
  };

  function bad_set_show(_o) {
      _o.state.selected_draw_mode = _o.c.DRAW_CARD_NOT_A_SET; // for redraw
      draw_selected(_o);
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
        bad_set_show(_o);
        _o.warning_code(_o, rc, function () {
          console.log("dm:=selected");
          _o.state.selected_draw_mode = _o.c.DRAW_CARD_SELECTED;
          _o.state.cards_selected.pop();
          _o.board_show(_o);
        });
      } else {
        _o.warning_code(_o, rc);
      }
    } else {
      var cmd = edata['cmd'];
      cmd = Number(cmd); // delete hhis line !?!
      var h = _o.message_handlers[cmd];
      console.log("cmd="+cmd+", h?="+(!!(h)));
      if (h) {
        h(_o, edata['result']);
      } else {
        console.log(_o.message_handlers);
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

  function onconnect(_o, f, timeout) {
    console.log('readyState='+ _o.web_socket.readyState + ', timeout='+timeout);
    if (timeout > 0) {
      if (_o.web_socket.readyState == 1) {
        f(_o);
      } else {
        setTimeout(function () { onconnect(_o, f, timeout - 1); });
      }
    } 
  }

  var win_hostname = window.location.hostname
  if (win_hostname == "") { win_hostname = "localhost"; }
  var host = "ws://" + win_hostname + ":" + _o.c.CS3333_PORT + "/ws";
  console.log("host="+host)
  _o.web_socket = new WebSocket(host);
  console.log("web_socket.readyState=" + _o.web_socket.readyState);
  _o.web_socket.onopen = function () {
      console.log("websocket.onopen: readyState="+ _o.web_socket.readyState);
      onconnect(_o, tables_status, 10);
  };
  _o.web_socket.onmessage = function (evt) { _o.event_handler(_o, evt); };
}

init_cards = function() {
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
  return crds;
}

document.addEventListener("DOMContentLoaded", function (event) {
  console.log('DOMContentLoaded called: ' + ymdhms());
  var _o = {}
  _o.c = m3333_consts;
  _o.cards = init_cards();
  _o.ui_completed = false;
  _o.init_server = init_server;
  init_ui(_o);
  // init_server(_o);
});

// document.addEventListener('init', function(event) {
//   console.log('listener: init')
//   var page = event.target;
//   if (page.matches('#table')) {
//     console.log('listener: init: table');
//   }
// });

console.log('End of 3.js');
