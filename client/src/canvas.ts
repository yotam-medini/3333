import { 
  gcards_selected, gcards_selected_update, gcards_selected_subscribe 
} from './table_data';

const GOLDEN: number = (Math.sqrt(5.)+1.)/2.;

console.log("GOLDEN="+GOLDEN);

let _gcards_selected = []
let _gcards_selected_unsubscribe = gcards_selected_subscribe((v) => {
  console.log("gcards_selected.subscribe v=" + v);
  _gcards_selected = v;
});

let cleanupFunction;
function initialize() {
  console.log("initialize");
  cleanupFunction = function() {
    console.log("cleanupFunction");
    _gcards_selected_unsubscribe();
  };
  // window.addEventListener('beforeunload', cleanupFunction); // Example event
  window.addEventListener('beforeunload', (event) => {
    console.log("beforeunload");
    cleanupFunction();
    // Cancel the event as stated by the standard.
    event.preventDefault();
    // Chrome requires returnValue to be set.
    event.returnValue = '';
  });
}
initialize();

type CanvasCtx = {
  ctx: CanvasRenderingContext2D;
  width: number;
  height: number;
  rgb_colors: [string, string, string];
};

type DrawPlan = {
  columns: number;
  rows: number;
  card_width: number;
  card_height: number;
  pattern_stripes: CanvasPattern[];
};

type Rect = {
  x: number;
  y: number;
  w: number;
  h: number;
};

type CardDrawInfo = {
  cctx: CanvasCtx;
  draw_plan: DrawPlan;
  card: number;
  x: number;
  y: number;
  selected: bool;
};

type QR = {q: number, r: number; };
 
function divmod(n: number, d: number): QR {
  let r = n % d;
  let q = (n - r)/d;
  return {q: q, r: r};
}

function determineNumberOfColumns(cctx: CanvasCtx, n_cards: number): number {
  let best_columns: number = 1;
  var best_ratio_quality: number = 0.; // Can be 1. at most.
  for (let columns: number = 1; columns <= n_cards; columns +=1) {
    let rows: number = Math.floor((n_cards + (columns - 1)) / columns);
    // cell_width = win_width / columns
    // cell_height = win_height / rows
    // cell_ratio = cell_width / cell_height
    let cell_ratio: number = (cctx.width * rows) / (cctx.height * columns);
    var ratio_quality = cell_ratio / GOLDEN;
    if (ratio_quality > 1.) {
      ratio_quality = 1. / ratio_quality;
    }
    // console.log("columns="+columns + " rows="+rows +
    //   " cell_ratio="+cell_ratio + ", quality="+ratio_quality);
    if (best_ratio_quality < ratio_quality) {
      best_columns = columns;
      best_ratio_quality = ratio_quality;
    }
  }
  // console.log("w="+cctx.width + " h="+cctx.height + " n_cards="+n_cards +
  //   " => best_columns=" + best_columns + " # GOLDEN="+GOLDEN);
  return best_columns
}

function generatePatternStripes(card_height: interner, rgb_colors):
    CanvasPattern[] {
  let pat_height = Math.max(Math.round(card_height / 24), 10);
  let stripe_height = Math.round((3*pat_height)/5);
  let pattern_stripes: CanvasPattern[] = [undefined, undefined, undefined];
  for (let ci:number = 0; ci < 3; ci++) {
    var offscreen = document.createElement("canvas");
    offscreen.width = 8;
    offscreen.height = pat_height;
    let ctx = offscreen.getContext("2d")
    ctx.fillStyle = rgb_colors[ci];
    ctx.fillRect(0, 0, 8, stripe_height);
    let pattern_stripe: CanvasPattern = ctx.createPattern(offscreen, "repeat");
    // console.log("ci="+ci + " pattern_stripe...");
    // console.log(pattern_stripe)
    pattern_stripes[ci] = pattern_stripe;
  }
  return pattern_stripes;
}

function computeDrawPlan(cctx: CanvasCtx, n_cards: number) : DrawPlan {
  let columns: number = determineNumberOfColumns(cctx, n_cards);
  let rows = Math.floor((n_cards + columns - 1) / columns);
  // console.log("n_cards="+n_cards + " columns=" + columns + " rows="+rows);
  let q: number = 11./12.; // card-pixels per table pixles ratio
  let card_width: number = Math.round(q * cctx.width / columns);
  let card_height: number = Math.round(q * cctx.height / rows);
  if (card_width < GOLDEN * card_height) {
    card_height = Math.round(card_width / GOLDEN);
  } else {
    card_width = Math.round(GOLDEN * card_height);
  }
  let pattern_stripes: CanvasPattern[] =
    generatePatternStripes(card_height, cctx.rgb_colors);
  return { columns: columns, rows: rows,
    card_width: card_width, card_height: card_height,
    pattern_stripes: pattern_stripes};
}

function drawDiamond(ctx: CanvasRenderingContext2D, rect: Rect) {
  // console.log("drawDiamond: rect=", rect); 
  ctx.beginPath();
  ctx.moveTo(rect.x + half(rect.w), rect.y);
  ctx.lineTo(rect.x + rect.w, rect.y + half(rect.h));
  ctx.lineTo(rect.x + half(rect.w), rect.y + rect.h);
  ctx.lineTo(rect.x, rect.y + half(rect.h));
  ctx.closePath();
}

function aa_round(aa: number[][]) { 
  for (let i: number = 0; i < aa.length; i++) {
    var a = aa[i];
    for (let j:number = 0; j < a.length; j++) {
      a[j] = [Math.round(a[j][0]), Math.round(a[j][1])];
    }
  }
}

function drawSquiggle(ctx: CanvasRenderingContext2D, rect: Rect) {
  let x: number = rect.x;
  let y: number = rect.y;
  let w: number = rect.w;
  let h: number = rect.h;
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
  for (let i: number = 0; i < rel_curves_left.length; i++) {
    var xy3_l = rel_curves_left[i];
    var xy3_r = [undefined, undefined, undefined];
    for (var j: number = 0; j < 3; j++) {
      var xy_l = xy3_l[j];
      var xy_r = [w - xy_l[0], h - xy_l[1]]; // center symmetry
      xy3_r[j] = xy_r;
    }
    rel_curves_right.push(xy3_r);
  }

  var curves = []; // Absolute, add (x,y) to each
  var rel_curves = rel_curves_left.concat(rel_curves_right);
  for (let i: number = 0; i < rel_curves.length; i++) {
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

  ctx.beginPath();
  ctx.moveTo(pt_1st[0], pt_1st[1]);
  for (let i: number = 0; i < curves.length; i++) {
    var curve = curves[i];
    ctx.bezierCurveTo(
      curve[0][0], curve[0][1],
      curve[1][0], curve[1][1],
      curve[2][0], curve[2][1]);
  }
  ctx.closePath();
}

function half(n: number): number {
  return Math.round(n/2);
}

function drawOval(ctx: CanvasRenderingContext2D, rect: Rect) {
  // console.log("drawOval: rect=", rect); 
  // Draw an ellipse, within a bounding rectangle
  //   (x,y)    The left-top point of the bounding rectangle.
  //   (w,h)    The width and height of the bounding rectangle.
  //   and rotated by rotation_angle, filled by texture

  // Compute center
  let cx: number = rect.x + half(rect.w), cy = rect.y + half(rect.h);
  // Compute Radii
  let rx = half(rect.w), ry = half(rect.h);

  ctx.beginPath();
  ctx.moveTo(cx + rx, cy);
  ctx.quadraticCurveTo(cx + rx, cy - ry, cx, cy - ry);
  ctx.quadraticCurveTo(cx - rx, cy - ry, cx - rx, cy);
  ctx.quadraticCurveTo(cx - rx, cy + ry, cx, cy + ry);
  ctx.quadraticCurveTo(cx + rx, cy + ry, cx + rx, cy);
  ctx.closePath();
}

function drawCard(cdi: CardDrawInfo) {
  // console.log("drawCard cdi: ", cdi);
  let ctx: CanvasRenderingContext2D = cdi.cctx.ctx; // abbreviation
  ctx.fillStyle = "#fff";
  if (cdi.selected) {
    ctx.fillStyle = "#111";
    ctx.fillRect(cdi.x, cdi.y,
      cdi.draw_plan.card_width, cdi.draw_plan.card_height);
    let bw: number = Math.max(Math.round(cdi.draw_plan.card_height/24), 3);
    ctx.fillStyle = "#ccc";
    ctx.fillRect(cdi.x + bw, cdi.y + bw,
      cdi.draw_plan.card_width - 2*bw, cdi.draw_plan.card_height - 2*bw);
  } else {
    ctx.fillRect(cdi.x, cdi.y, 
      cdi.draw_plan.card_width, cdi.draw_plan.card_height);
  }
  // let shading, color, symbol, cnumber
  let qr: QR = divmod(cdi.card, 3);
  let shading: number = qr.r;
  qr = divmod(qr.q, 3);
  let color: number = qr.r;
  qr = divmod(qr.q, 3);
  let symbol: number = qr.r;
  let n_symbols: number = qr.q + 1;
  // console.log("card="+cdi.card + " n_symbols="+n_symbols + " symbol="+symbol +
  //  " color="+color + " shading="+shading)

  let draw_symbol = [drawDiamond, drawSquiggle, drawOval][symbol];
  // let rgb = ["#e21", "#382", "#a3f"][color];
  let rgb = cdi.cctx.rgb_colors[color];
  let fill_passes: bool[][] = [
    [true],        // filled
    [true, false], // stripped
    [false]        // open
  ][shading];
  // console.log("rgb="+rgb + " fill_passes="+fill_passes);
  ctx.fillStyle = rgb;
  ctx.strokeStyle = rgb;
  ctx.lineWidth = Math.max(Math.round(cdi.draw_plan.card_width/40), 4);
  // console.log("card_width="+cdi.draw_plan.card_width + " lineWidth=" + ctx.lineWidth)

  // symbol bounding box
  let rect : Rect = {x: 0, y: 0,
    w: Math.round(cdi.w/4), h: Math.round(3*cdi.h/4)};
  // console.log("[0] rect: ", rect);
  let x_gap: number = Math.round((cdi.w - n_symbols * rect.w)/(n_symbols + 1));
  // console.log("cdi.w="+cdi.w + " rect.w="+rect.w + " n_symbols="+n_symbols +
  //  " x_gap="+x_gap);
  rect.x = cdi.x + x_gap;
  rect.y = cdi.y + half(cdi.h - rect.h);
  // console.log("[1] rect: ", rect);
  
  for (let r: int = 0; r < n_symbols; ++r) {
    for (let fill_pass: bool of fill_passes) {
      ctx.fillStyle = rgb;
      // console.log("r="+r + " fill_pass="+fill_pass + " ... symbol");
      draw_symbol(ctx, rect);
      if (fill_pass) {
        ctx.fillStyle = (shading == 1 ? cdi.draw_plan.pattern_stripes[color] : rgb);
        ctx.fill();
      } else {
        ctx.fillStyle = rgb;
        ctx.stroke();
      }
    }
    rect.x += rect.w + x_gap;
  }  
}

export function drawTable(
    canvas : HTMLCanvasElement,
    cards: number[],
    click_x: number,
    click_y: number) {
  // console.log("drawTable: canvas=" + canvas + " cards=" + cards + 
  //   " gcards_selected=" + _gcards_selected);
  // let p: HTMLCanvasElement = canvas.parentElement;
  // console.log("p="+p + " name="+p.nodeName);
  // let brect = p.getBoundingClientRect()
  // console.log(brect);
  let brect = canvas.getBoundingClientRect();
  // console.log(brect);
  if (click_x == -1) {
    canvas.width = Math.floor(brect.width);
    canvas.height = Math.floor(brect.height);
  }
  let ctx = canvas.getContext("2d");
  // console.log("ctx=" + ctx); console.log(ctx);
  // console.log("width=" + canvas.width + " height=" + canvas.height);
  // console.log("scroll: width=" + canvas.scrollWidth +
  //   " height=" + canvas.scrollHeight);
  const cctx: CanvasCtx = { 
    ctx: ctx, width: canvas.width, height: canvas.height, 
    rgb_colors: ["#e21", "#382", "#a3f"], };
  if (click_x == -1) {
    cctx.ctx.fillStyle = '#2a3';
    cctx.ctx.fillRect(0, 0, cctx.width, cctx.height);
  }
  let n_columns = determineNumberOfColumns(cctx, cards.length);
  let dp: DrawPlan = computeDrawPlan(cctx, cards.length);
  // console.log(dp);
  let xgap: number = Math.round((cctx.width - (dp.columns * dp.card_width)) / 
    (dp.columns + 1));
  let ygap: number = Math.round((cctx.height - (dp.rows * dp.card_height)) / 
    (dp.rows + 1));
  let xn: number = xgap;
  let yn: number  = ygap;
  let xi: number = 0;
  let x: number = xgap;
  let y: number = ygap;
  for (let ci: number = 0; ci < cards.length; ci++) {
    let drawMe: bool = (click_x === -1);
    // console.log("click_x="+click_x + " ci="+ci + " [0]drawMe="+drawMe);
    // draw_card(cctx, card, x, y, card_width, card_height
    let isel: number = _gcards_selected.indexOf(ci);
    // console.log("ci="+ci + " isel="+isel);
    let selected: bool = (isel != -1);
    if ((x <= click_x) && (click_x <= x + dp.card_width) &&
        (y <= click_y) && (click_y <= y + dp.card_height)) {
       drawMe = true;
       if (selected) {
         _gcards_selected.splice(isel, 1);
       } else {
         _gcards_selected.push(ci);
       }
       selected = !selected;
       // updateSelectedCards(_gcards_selected);
       gcards_selected_update(_gcards_selected);
    }
    // console.log("click_x="+click_x + " ci="+ci + " drawMe="+drawMe);
    if (drawMe) {
      const cdi: CardDrawInfo = {
        cctx: cctx, draw_plan: dp, card: cards[ci], x: x, y: y, 
        w: dp.card_width, h: dp.card_height,
        selected: selected};
      // console.log("draw_card: card=" + cards[ci] + " @=("+x + ", "+y+")");
      // console.log("cdi=", cdi);
      drawCard(cdi);
    }
    xi += 1;
    if (xi < n_columns) {
      x += dp.card_width + xgap;
    } else {
      xi = 0;
      x = xgap;
      y += dp.card_height + ygap;
    }
  }
}

export function handleClick(e,
    canvas : HTMLCanvasElement,
    cards: number[],
    click_x: number,
    click_y: number) {
  // console.log("handleClick: e=", e);
  // console.log("e.target=", e.target);
  let brect = e.target.getBoundingClientRect()
  // console.log("brect: ", brect);
  let x = e.clientX - brect.left;
  let y = e.clientY - brect.top;
  console.log("handleClick: x="+x + " y="+y);
  let computer_style = getComputedStyle(e.target);
  // console.log("getComputedStyle: ", computer_style);
  drawTable(canvas, cards, x, y)
}

export function DrawWaitingForGame(canvas: HTMLCanvasElement) {
  let ctx = canvas.getContext("2d");
  ctx.font = "12px serif";
  let w = canvas.width;
  let h = canvas.height;
  let left = w/8;
  let h3 = h/3;
  console.log("w="+w + " h="+h);
  ctx.fillText("Wait for Table owner", left, h3);
  ctx.fillText("To start a New Game", left, 2*h3);
}
