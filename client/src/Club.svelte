<script lang="ts">
  import { sendClubRefresh, sendNewTable, sendJoin} from "./wscon";
  import { epoch_ymdhms } from "./utils";
  import Modal from './Modal.svelte';
  import IdInput from './IdInput.svelte';
  let show_new_table_modal = false;
  let show_join_modal = false;
  let player_name = ''; // if owner - then also table name
  let table_name = '';
  let table_password = '';
  let player_password = '';
  export let tables = [];
  const createTable = () => {
    console.log("name="+player_name + " tpw="+table_password +
                " ppw="+player_password);
    sendNewTable(player_name, table_password, player_password);
    show_new_table_modal = false;
  };
  const refresh = () => {
    sendClubRefresh();
  }
  const joinDialogOpen = (tbl_name) => {
    table_name = tbl_name;
    console.log("join: table_name=" + table_name);
    show_join_modal = true;
  }
  const joinTable = () => {
    console.log("table"+table_name + " name="+player_name +
                " tpw="+table_password + " ppw="+player_password);
    sendJoin(table_name, player_name, table_password, player_password);
    show_join_modal = false;
  }
  let sort_key_last = "";
  let sort_dir_inc = true;
  const sortBy = (key) => {
    console.log("sortBy key="+key);
    if (key == sort_key_last) {
      sort_dir_inc = !sort_dir_inc;
    }
    let tables_copy = [];
    for (let i = 0; i < tables.length; ++i) {
      tables_copy.push(tables[i]);
    }
    tables_copy.sort((t0, t1) => {
      let ret = 0;
      if (t0[key] == t1[key]) {
        ret = t1["scr_index"] - t0["scr_index"];
      } else {
        if ((t0[key] < t1[key]) == sort_dir_inc) {
          ret = -1;
        } else {
          ret = 1;
        }
      }
      return ret;
    });
    sort_key_last = key;
    tables = tables_copy;
  }
</script>

<div>
  <div class="ClubControl">
    <button on:click={() => (show_new_table_modal = true)}>New Table</button>
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
              <button on:click={() => joinDialogOpen(summary["name"])}>
		Join
              </button>
           </td>
          </tr>
        {/each}
      </table>
    </center>
  </div>
</div>
<Modal title="New Table" bind:showModal={show_new_table_modal}>
  <slot/>
  <table>
    <tr>
      <td>Table Name</td>
      <td><IdInput bind:value={player_name}/></td>
    </tr>
    <tr>
      <td>Table Password</td>
      <td><IdInput bind:value={table_password}/></td>
    </tr>
    <tr>
      <td>Player Password</td>
      <td><IdInput bind:value={player_password}/></td>
    </tr>
  </table>
  <button class="create" on:click={createTable}>Create Table</button>
</Modal>
<Modal title="Join Table {table_name}" bind:showModal={show_join_modal}>
  <slot/>
  <table>
    <tr>
      <td>Player Name</td>
      <td><IdInput bind:value={player_name}/></td>
    </tr>
    <tr>
      <td>Table Password</td>
      <td><IdInput bind:value={table_password}/></td>
    </tr>
    <tr>
      <td>Player Password</td>
      <td><IdInput bind:value={player_password}/></td>
    </tr>
  </table>
  <button class="join" on:click={joinTable}>Join</button>
</Modal>
<style>
  .create, .join {
    color: #262;
    background-color: #ccc;
  }
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
