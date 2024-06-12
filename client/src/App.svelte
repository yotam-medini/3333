<script lang="ts">
  import svelteLogo from './assets/svelte.svg'
  import viteLogo from '/vite.svg'
  import Counter from './lib/Counter.svelte'

  import NavBar from "./NavBar.svelte";
  import Club from "./Club.svelte";
  import Table from "./Table.svelte";
  import Players from "./Players.svelte";
  import Help from "./Help.svelte";
  import { ainvert } from "./utils";
  import { web_socket, set_callback } from "./wscon";
  import { TableStatus, CallBackIdx, game_state_empty } from "./consts";
  import { gcards_selected_update } from "./table_data.ts";
  import { find_set } from "./hint";
  let game_state = game_state_empty;
  set_callback(CallBackIdx.iSetGameState, (gs) => {
    game_state = gs;
  });
  
  const tabChange = (e) => {
    tabActiveIndex = e.detail;
    console.log("tabActiveIndex=" + tabActiveIndex)
  }
  let tabs = ['Club', 'Table', 'Players', 'Help'];
  let tabActiveIndex = 0;
  let i2tab = ainvert(tabs);
  $: tabActiveName = tabs[tabActiveIndex];
  console.log("tabs: " + tabs);
  console.log("i2tab: " + i2tab);
  console.log("i2tab[Table]=" + i2tab["Table"]);
  let tableComponent = null;

  const updateTableStatus = (table_status: TableStatus) => {
    console.log("table_status=" + table_status);
    tabActiveIndex = i2tab['Table']
    setStates(tstate, gstate);
    cards = [];
    if (table_status == TableStatus.None) {
      new_game_enabled = false;
    } else if (table_status == TableStatus.Own) {
      new_game_enabled = true;
    } else if (table_status == Table_status.Join) {
      new_game_enabled = false;
    } else {
      console.log("Error: unexpected table_status=" + table_status);
    }
  };
  set_callback(CallBackIdx.ITableStatus, updateTableStatus);

  set_callback(CallBackIdx.ISetCards, (a: number[]) => {
    cards = a;
    console.log("cards=" + cards);
    find_set(a);
    // tableComponent.setCards(cards);
    // tableComponent.redraw();
  });

  set_callback(CallBackIdx.iSetDeck, (n: number) => {
    deck = n;
    console.log("deck=" + n);
  });

  set_callback(CallBackIdx.INotASet, () => {
    console.log("gcards_selected_update([])")
    gcards_selected_update([]);
  });

  let player_name = "";
  let table_name = "";
  set_callback(CallBackIdx.iSetNames, (p: string, t: string) => {
    console.log("p="+p + " t="+t);
    player_name = p;
    table_name = t;
  });
  export let tstate = -1;
  export let gstate = -1;
  export const setStates = (t, g) => {
    tstate = t;
    gstate = g;
    console.log("tstate="+t + " gstate="+g + " tableComp="+tableComponent);
    // if (tableComponent !== null) {
    //   tableComponent.setStates(t, g);
    // }
  }
  set_callback(CallBackIdx.ISetStates, setStates);

  let cards = [];
  let deck = 0;
  let new_game_enabled = false;

  let tables = []
  set_callback(CallBackIdx.IClubTables, (json_tables) => {
    let _tables = [];
    let scr_index = 0;
    for (let table_name in json_tables) {
      let summary = json_tables[table_name];
      summary["name"] = table_name;
      summary["scr_index"] = scr_index;
      scr_index++;
      _tables.push(summary);
    }
    console.log(_tables);
    tables = _tables;
  });
</script>

<main>
  <h1>Set Game / beta 4 </h1>
  <NavBar {tabs} {tabActiveIndex} on:tabChange={tabChange} />
  {#if tabActiveName === 'Club'}
    <Club {tables} />
  {:else if tabActiveName === 'Table'}
    <Table {player_name} {table_name} {game_state} {cards} {new_game_enabled}
	   {deck}
           bind:this={tableComponent}
    />
  {:else if tabActiveName === 'Players'}
    <Players />
  {:else if tabActiveName === 'Help'}
    <Help />
  {:else}
    <div>Error: unexpected {tabActiveName}</div>
  {/if}
  <div id="dummyFooter">dummy footer</div>
</main>

<style>
  main {
    display: flex;
    flex-direction: column;
    width: 90%v;
    /* height: 80%; */
    height: 100dh;
    max-height: 100%;
    background: #aaa;
    padding: 1px;
    border: 3px solid red;
  }
  h1 {
    flex-grow: 0;
    background: #bbe;
  }
  #dummyFooter {
    min-height: 0px;
    flex-grow: 0;
    background: #d8a
  }
</style>
