<script lang="ts">
  import {
    gcards_selected, gcards_selected_update, gcards_selected_subscribe
  } from "./table_data.ts";
  import Canvas from "./Canvas.svelte";
  let canvasComponent;
  const redraw = () => {
    console.log("redraw");
    canvasComponent.redrawMe();
  }
  export let cards;
  let n_selected = 0;
  let _gcards_selected_unsubscribe = gcards_selected_subscribe((v) => {
    console.log("gcards_selected sub... v=" + v);
    n_selected = v.length;
  });
</script>

<div id="divTable">
  <div id=divControl>
    <strong>Table</strong> <button on:click={redraw}>Redraw</button>
    <span>cards: {cards}</span> <span>{n_selected} selected</span>
  </div>
  <Canvas {cards} bind:this={canvasComponent} />
</div>

<style>
  #divTable {
    width: 90%vh;
    /* height: 100%; */
    height: 100dvh;
    display: flex;
    flex-direction: column;
    gap: 6px;
    flex-grow: 1;
    background: #aae;
    padding-left: 10px;
    padding-right: 20px;
  }
  #divControl {
    display: flex;
    justify-content: center;
    gap: 10px;
    flex-grow: 0;
  }
</style>
