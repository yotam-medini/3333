<script lang="ts">
    import { onMount } from "svelte";
    import { drawTable } from "./canvas.js";
    let canvasElement: HTMLCanvasElement;
    console.log("Canvas: not yet mounted");
    onMount(() => {
        console.log("Canvas: mounted");
        // get canvas context
        let ctx = canvasElement.getContext("2d");

        // draw line
        ctx.beginPath();
        ctx.moveTo(10, 20); // line will start here
        ctx.lineTo(150, 100); // line ends here
        ctx.stroke(); // draw it
    });
    export let cards;
    export function redrawMe() {
      console.log("redrawMe");
      drawTable(canvasElement, cards);
    }
</script>

<canvas bind:this={canvasElement}/>
