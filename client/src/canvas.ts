const GOLDEN: number = (Math.sqrt(5.)+1.)/2.;

type CanvasCtx = {
  ctx: CanvasRenderingContext2D;
  width: number;
  height: number;
};

type DrawPlan = {
  columns: number;
  rows: number;
  card_width: number;
  card_height: number;
};

type CardDrawInfo = {
  cctx: CanvasCtx;
  card: number;
  x: number;
  y: number;
  w: number;
  h: number;
};

function determineNumberOfColumns(cctx: CanvasCtx, n_cards: number): number {
  let best_columns: number = 1;
  var best_ratio_quality: number = 0.; // Can be 1. at most.
  for (let columns: number = 1; columns <= n_cards; columns +=1) {
    let rows: number = (n_cards + (columns - 1)) / columns;
    // cell_width = win_width / columns
    // cell_height = win_height / rows
    // cell_ratio = cell_width / cell_height
    let cell_ratio: number = (cctx.width * rows) / (cctx.height * columns);
    var ratio_quality = cell_ratio * GOLDEN; // portrait => 1./GOLDEN
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
  console.log("w="+cctx.width + " h="+cctx.height + " n_cards="+n_cards +
    " => best_columns=" + best_columns);
  return best_columns
}

function computeDrawPlan(cctx: CanvasCtx, n_cards: number) : DrawPlan {
  let columns: number = determineNumberOfColumns(cctx, n_cards);
  let rows = Math.floor((n_cards + columns - 1) / columns);
  console.log("n_cards="+n_cards + " columns=" + columns + " rows="+rows);
  let q: number = 11./12.; // card-pixels per table pixles ratio
  let card_width: number = Math.round(q * cctx.width / columns);
  let card_height: number = Math.round(q * cctx.height / rows);
  if (card_height < GOLDEN * card_width) {
     card_width = Math.round(card_height / GOLDEN);
  } else {
     card_height = GOLDEN * card_width;
  }
  return { columns: columns, rows: rows,
    card_width: card_width, card_height: card_height};
}

function drawCard(cdi: CardDrawInfo) {
  console.log("drawCard cdi: " + cdi); console.log(cdi);
  cdi.cctx.ctx.fillStyle = "#fff";
  cdi.cctx.ctx.fillRect(cdi.x, cdi.y, cdi.w, cdi.h);
}

export function drawTable(canvas : HTMLCanvasElement, cards) {
  console.log("drawTable: canvas=" + canvas + " cards=" + cards);
  // let p: HTMLCanvasElement = canvas.parentElement;
  // console.log("p="+p + " name="+p.nodeName);
  // let brect = p.getBoundingClientRect()
  // console.log(brect);
  let brect = canvas.getBoundingClientRect();
  console.log(brect);
  canvas.width = Math.floor(brect.width);
  canvas.height = Math.floor(brect.height);
  let ctx = canvas.getContext("2d");
  console.log("ctx=" + ctx);
  console.log("width=" + canvas.width + " height=" + canvas.height);
  console.log("scroll: width=" + canvas.scrollWidth +
	      " height=" + canvas.scrollHeight);
  const cctx: CanvasCtx = { 
    ctx: ctx, width: canvas.width, height: canvas.height, };
  cctx.ctx.fillStyle = '#2a3';
  cctx.ctx.fillRect(0, 0, cctx.width, cctx.height);
  cctx.ctx.fillStyle = '#734';
  cctx.ctx.fillRect(cctx.width/7, cctx.height/5, cctx.width/5, cctx.height/7);
  let n_columns = determineNumberOfColumns(cctx, cards.length);
  let dp: Drawtable = computeDrawPlan(cctx, cards.length);
  console.log(dp);
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
    // draw_card(cctx, card, x, y, card_width, card_height
    const cdi: CardDrawInfo = {
      cctx: cctx, card: cards[ci], x: x, y: y, 
      w: dp.card_width, h: dp.card_height};
    console.log("draw_card: card=" + cards[ci] + " @=("+x + ", "+y+")");
    drawCard(cdi);
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
