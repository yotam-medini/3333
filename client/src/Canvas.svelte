<script lang="ts">
    import { onMount } from "svelte";
    import { drawTable } from "./canvas.ts";
    let canvasElement: HTMLCanvasElement;
    console.log("Canvas: not yet mounted");
 if (false) {
    onMount(() => {
        console.log("Canvas: mounted");
        // get canvas context
        let ctx = canvasElement.getContext("2d");

        // draw line
        ctx.beginPath();
        ctx.moveTo(10, 20); // line will start here
        ctx.lineTo(10, 30); // line ends here
        ctx.stroke(); // draw it
    });
 }
    export let cards;
    export function redrawMe() {
      console.log("redrawMe");
      drawTable(canvasElement, cards);
    }
</script>

<canvas bind:this={canvasElement}/>

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
    padding-left: 20px;
    margin-left: 30px;
  }
</style>
