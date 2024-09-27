<script lang="ts">
  import { onMount } from "svelte";
  import { DrawWaitingForGame, drawTable, handleClick } from "./canvas.ts";
  let canvasElement: HTMLCanvasElement;
  console.log("Canvas: not yet mounted");
  onMount(() => {
      console.log("Canvas: mounted");
      // redrawMe();
      WriteWaitingForGame();
      // get canvas context
    // let ctx = canvasElement.getContext("2d");
   if (false) {
    canvasElement.addEventListener("click", function(event) {
      const rect = canvasElement.getBoundingClientRect();
      const x = event.clientX - rect.left;
      const y = event.clientY - rect.top;
      console.log("Click coordinates relative to canvas: X -", x, ", Y -", y);
    });
   }
  });
  export let cards;
  export const setCards = (a) => {
    console.log("setCards a=", a);
    cards = a;
    // console.log("setCards end")
  };
  // export let cards_selected; // LUT to cards, max 3 
  export function WriteWaitingForGame() {
    // console.log("redrawMe");
    DrawWaitingForGame(canvasElement);
  }
  export function redrawMe() {
    // console.log("redrawMe");
    drawTable(canvasElement, cards, -1, -1);
  }
  const handleClickHigh = (e) => {
    console.log("handleClickHigh: e=", e);
    handleClick(e, canvasElement, cards);
  }
</script>

<canvas bind:this={canvasElement} on:click={handleClickHigh}/>

<style>
  canvas {
    padding: 2px;
    margin: 2px 2px;
    width: 100%dvh;
    /* height: 300px; */
    /* max-height: 100vh; */
    height: 200px;
    max-height: 100%;
    background: #aba;
    flex-grow: 1;
    border: 3px solid black;
    /* padding-left: 20px; */
    /* margin-left: 30px; */
  }
</style>
