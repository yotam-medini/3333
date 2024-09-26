<script lang="ts">
  import { sendPlayersRefresh } from "./wscon";
  import { epoch_ymdhms } from "./utils";
  export let players_data = [];
  const refresh = () => {
    console.log("refresh");
    sendPlayersRefresh();
  }
</script>

<div>
  <h2>{players_data.length} Players</h2>
  <div class="PlayersControl">
    <button on:click={refresh}>Refresh</button>
  </div>
  <div>
    <center>
      <table>
	<tr>
	  <th>Name</th>
	  <th colspan="2">&emsp;&emsp;Sets</th>
	  <th colspan="2">&emsp;Add 3 calls</th>
	  <th colspan="2">No More&emsp;&emsp;</th>
	  <th>Joined</th>
	  <th>Last</th>
	</tr>
	<tr>
	  <th></th>
	  <th>Good</th>
	  <th>Wrong</th>
	  <th>Needed</th>
	  <th>Excess</th>
	  <th>True</th>
	  <th>False</th>
	</tr>
	{#each players_data as player}
	  <tr>
	    <td>{player["name"]}</td>
	    <td>{player["numbers"][0]}</td>
	    <td>{player["numbers"][1]}</td>
	    <td>{player["numbers"][2]}</td>
	    <td>{player["numbers"][3]}</td>
	    <td>{player["numbers"][4]}</td>
	    <td>{player["numbers"][5]}</td>
            <td>{epoch_ymdhms(player["tcreated"])}</td>
            <td>{epoch_ymdhms(player["taction"])}</td>
	  </tr>
	{/each}
      </table>
    </center>
  </div>
</div>

<style>
  th, td {
    padding: 8px;
    text-align: left;
    border-left: 1px solid #ddd;
    border-right: 1px solid #ddd;
    border-top: 1px solid #ddd;
    border-bottom: 1px solid #ddd;
  }
</style>
