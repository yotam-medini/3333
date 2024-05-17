<script lang="ts">
  import { CallBackIdx, game_state_empty } from "./consts";
  import {
    gcards_selected, gcards_selected_update, gcards_selected_subscribe
  } from "./table_data.ts";
  import { set_callback, sendNewGame, sendTry3 } from "./wscon";
  import Canvas from "./Canvas.svelte";
  let canvasComponent;
  export let new_game_enabled;
  const newGame = () => {
    console.log("newGame");
    gcards_selected_update([]);
    sendNewGame();
  }
  export let player_name = "";
  export let table_name = "";
  // set_callback(CallBackIdx.iSetNames, (p: string, t: string) => {
  //   player_name = p;
  //   table_name = t;
  // });
  export let game_state;
  export let cards;
  let numbers = [0, 0, 0, 0]
  export let tstate = 0;
  export let gstate = 0;
  $: {
    console.log("reactive: game_state=", game_state);
    if (gstate != game_state["gstate"]) {
      gstate = game_state["gstate"];
      console.log("new gstate=" + gstate);
      if (gstate > 0) {
        cards = game_state["active"]
        gcards_selected_update([]);
        redraw();
        numbers = game_state["players"][player_name]["numbers"]
      }
    }
    if (tstate != game_state["tstate"]) {
      tstate = game_state["tstate"];
      console.log("new tstate=" + tstate);
      if (tstate > 0) {
        numbers = game_state["players"][player_name]["numbers"]
      }
    }
  }
  // export const setStates = (t, g) => {
  //   // console.log("tstate="+t + " gstate="+g);
  //   // tstate = t;
  //   // gstate = g;
  // }
  // export const setCards = (a) => {
  //   // cards = a;
  // };
  export const add3 = () => {
    console.log("add3");
  }
  export const redraw = () => {
    console.log("redraw cards="+cards);
    canvasComponent.setCards(cards);
    canvasComponent.redrawMe();
  };
  let n_selected = 0;
  let _gcards_selected_unsubscribe = gcards_selected_subscribe((v) => {
    console.log("gcards_selected sub... v=" + v + " g="+gstate);
    n_selected = v.length;
    if (n_selected == 3) {
      sendTry3(v, gstate);
    }
  });
  // stats
  let n_calls_good = 0;
  let n_calls_bad = 0;
  let n_add3_good = 0
  let n_add3_bad = 0;
  console.log("Table.svelte: end-of-script");
</script>

<div id="divTable">
  <div id=divControl>
    <strong>Table: {player_name}@{table_name}</strong> 
    <button on:click={newGame} disabled={!new_game_enabled}>New Game</button>
    <button on:click={add3}>Add 3 Cards</button>
    <button on:click={redraw}>Redraw</button>
    <span>{cards.length} cards:</span> <span>{n_selected} selected</span>
    <span>Stats: {numbers}</span>
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
