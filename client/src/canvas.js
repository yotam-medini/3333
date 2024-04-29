export function drawTable(canvas, cards) {
  console.log("drawTable: canvas=" + canvas + " cards=" + cards);
  let ctx = canvas.getContext("2d");
  console.log("ctx=" + ctx);
  console.log("width=" + canvas.width + " height=" + canvas.height);
  ctx.fillStyle = '#777';
  ctx.fillRect(0, 0, canvas.width, canvas.height);
}
