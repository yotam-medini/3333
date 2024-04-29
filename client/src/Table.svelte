<script lang="ts">
  import Canvas from "./Canvas.svelte";
  let canvasComponent;
  const redraw = () => {
    console.log("redraw");
    canvasComponent.redrawMe();
  }
  export let cards;
  // let selected_card_indices = [];
</script>

<div>
  <strong>Table</strong> <button on:click={redraw}>Redraw</button>
  <div>cards: {cards}</div>
</div>
<Canvas {cards} bind:this={canvasComponent} />

<style>
</style>
