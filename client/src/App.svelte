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
  import { TableStatus, CallBackIdx } from "./consts";
  
  const tabChange = (e) => {
    tabActiveIndex = e.detail;
  }
  let tabs = ['Club', 'Table', 'Players', 'Help'];
  let tabActiveIndex = 0;
  let i2tab = ainvert(tabs);
  $: tabActiveName = tabs[tabActiveIndex];
  console.log("tabs: " + tabs);
  console.log("i2tab: " + i2tab);
  console.log("i2tab[Table]=" + i2tab["Table"]);

  const updateTableStatus = (table_staus: TableStatus) => {
    console.log("table_staus=" + table_staus);
    tabActiveIndex = i2tab['Table']
    cards = [];
    if (table_staus == TableStatus.None) {
      new_game_enabled = false;
    } else if (table_staus == TableStatus.Own) {
      new_game_enabled = true;
    } else if (table_staus == Table_staus.Join) {
      new_game_enabled = false;
    } else {
      console.log("Error: unexpected table_staus=" + table_staus);
    }
  };
  set_callback(CallBackIdx.ITableStatus, updateTableStatus);

  set_callback(CallBackIdx.ISetCards, (a: number[]) => {
    cards = a;
    console.log("cards=" + cards);
  });

  let cards = [2, 3, 5, 43, 59, 79];
  let new_game_enabled = false;
  // let cards_selected = [1, 4];
</script>

<main>
  <h1>Set Game / beta 4 </h1>
  <NavBar {tabs} {tabActiveIndex} on:tabChange={tabChange} />
  {#if tabActiveName === 'Club'}
    <Club />
  {:else if tabActiveName === 'Table'}
    <Table {cards} {new_game_enabled} />
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
