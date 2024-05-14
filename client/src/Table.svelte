<script lang="ts">
  import {
    gcards_selected, gcards_selected_update, gcards_selected_subscribe
  } from "./table_data.ts";
  import { sendNewGame, sendTry3 } from "./wscon";
  import Canvas from "./Canvas.svelte";
  let canvasComponent;
  export let new_game_enabled;
  const newGame = () => {
    console.log("newGame");
    gcards_selected_update([]);
    sendNewGame();
  }
  export let tstate = -1;
  export let gstate = -1;
  export const setStates = (t, g) => {
    console.log("tstate="+t + " gstate="+g);
    tstate = t;
    gstate = g;
  }
  export let cards;
  export const setCards = (a) => {
    cards = a;
  };
  export const redraw = () => {
    console.log("redraw cards="+cards);
    canvasComponent.setCards(cards);
    canvasComponent.redrawMe();
  }
  let n_selected = 0;
  let _gcards_selected_unsubscribe = gcards_selected_subscribe((v) => {
    console.log("gcards_selected sub... v=" + v + " g="+gstate);
    n_selected = v.length;
    if (n_selected == 3) {
      sendTry3(v, gstate);
    }
  });
  console.log("Table.svelte: end-of-script");
</script>

<div id="divTable">
  <div id=divControl>
    <strong>Table</strong> 
    <button on:click={newGame} disabled={!new_game_enabled}>New Game</button>
    <button on:click={redraw}>Redraw</button>
    <span>{cards.length} cards:</span> <span>{n_selected} selected</span>
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
