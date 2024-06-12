<script lang="ts">
  import Dialog from './Dialog.svelte';
  import { sendClubRefresh, sendNewTable } from "./wscon";
  import { epoch_ymdhms } from "./utils";
  let dialog;
  let player_name = ''; // if owner - then also table name
  let table_password = '';
  let player_password = '';
  export let tables = [];
  const createTable = () => {
    console.log("name="+player_name + " tpw="+table_password +
                " ppw="+player_password);
    sendNewTable(player_name, table_password, player_password);
    dialog.close();
  };
  const refresh = () => {
    sendClubRefresh();
  }
  const join = (table_name) => {
    console.log("join: table_name=" + table_name);
  }
  const sortBy = (key) => {
    console.log("sortBy key="+key);
  }
</script>

<div>
  <div class="ClubControl">
    <button on:click={() => dialog.showModal()}>New Table</button>
    <button on:click={refresh}>Refresh</button>
  </div>
  <div>
    <center>{tables.length} Tables</center>
    <center>
      <table>
        <tr>
          <th><button on:click={() => sortBy("name")}>Name</button></th>
          <th><button on:click={() => sortBy("players")}>Players</button></th>
          <th><button on:click={() => sortBy("tcreated")}>Created</button></th>
          <th><button on:click={() => sortBy("taction")}>Action</button></th>
          <th>Join</th>
        </tr>
        {#each tables as summary}
          <tr>
            <td>{summary["name"]}</td>
            <td>{summary["players"]}</td>
            <td>{epoch_ymdhms(summary["tcreated"])}</td>
            <td>{epoch_ymdhms(summary["taction"])}</td>
            <td>
              <button on:click={() => join(summary["name"])}>
		Join
              </button>
           </td>
          </tr>
        {/each}
      </table>
    </center>
  </div>
  <Dialog bind:dialog on:close={() => console.log('closed')}>
    <table>
      <tr>
        <td>Table Name</td>
        <td><input bind:value={player_name}/></td>
      </tr>
      <tr>
        <td>Table Password</td>
        <td><input bind:value={table_password}/></td>
      </tr>
      <tr>
        <td>Player Password</td>
        <td><input bind:value={player_password}/></td>
      </tr>
    </table>
    <button on:click={createTable}>Create Table</button>  (esc to close)
  </Dialog>
</div>

<style>
  th {
    padding-top: 10px;
    padding-bottom: 10px;
    padding-left: 20px;
    padding-right: 20px;
  }
  td {
    padding-top: 2px;
    padding-bottom: 2px;
    padding-left: 20px;
    padding-right: 20px;
  }
</style>
