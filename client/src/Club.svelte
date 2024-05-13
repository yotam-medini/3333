<script lang="ts">
  import Dialog from './Dialog.svelte'
  let dialog
  let player_name = ''; // if owner - then also table name
  let table_password = '';
  let player_password = '';
  const createTable = () => {
    console.log("name="+player_name + " tpw="+table_password +
                " ppw="+player_password);
  };
</script>

<div>
  <h2>Club</h2>
  <button on:click={() => dialog.showModal()}>New Table</button>
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
</style>
