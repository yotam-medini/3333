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

  let cards = [2, 3, 5];
</script>

<main>
  <h1>Set Game / beta 4 </h1>
  <NavBar {tabs} {tabActiveIndex} on:tabChange={tabChange} />
  {#if tabActiveName === 'Club'}
    <Club />
  {:else if tabActiveName === 'Table'}
    <Table {cards} />
  {:else if tabActiveName === 'Players'}
    <Players />
  {:else if tabActiveName === 'Help'}
    <Help />
  {:else}
    <div>Error: unexpected {tabActiveName}</div>
  {/if}

</main>

<style>
  body {
    width: 100%;
    height: 100%;
    background: yellow;  
  }
  main {
    width: 80%;
    height: 80%;
    background: #aaa;  
  }
</style>
