<script>
  import { createEventDispatcher } from 'svelte';
  const dispatch = createEventDispatcher();
  export let tabs;
  export let tabActiveIndex;
</script>


<div class="navbar">
  <ul>
    {#each {length: tabs.length} as _, i}
      <button on:click={() => dispatch("tabChange", i) }>
	<div class:active={i === tabActiveIndex}>{tabs[i]}</div>
      </button>
    {/each}
  </ul>
</div>

<style>
  .navbar {
    margin-bottom: 4%;
  }
  ul {
    display: flex;
    justify-content: center;
    padding: 0;
    list-style-type: none;
  }
  button {
    margin: 0 16px;
    font-size: 18px;
    color: #555;
    cursor: pointer;
  }
  .active {
    color: #208020;
    border-bottom: 2px solid #208020;
    padding-bottom: 8px;
  }
</style>
