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
  <bold>{players_data.length} Players</bold>
  <span>&emsp; &emsp;</span>
  <button on:click={refresh}>Refresh</button>
  <div>
    <center>
      <table>
	<tr>
	  <th>Name</th>
	  <th colspan="2">&emsp;&emsp;Sets</th>
	  <th colspan="2">&emsp;Add 3 calls</th>
	  <th colspan="2">No More</th>
	  <th>Joined</th>
	  <th>Last</th>
	</tr>
	<tr>
	  <th></th>
	  <th class="good">
	    Good <span class="tooltip">Number of good Set calls</span>
	  </th>
	  <th class="bad">
	    Wrong <span class="tooltip">Number of wrong Set calls</span>
	  </th>
	  <th class="good">
	    Needed <span class="tooltip">Number of needed Add-3 requests</span>
	  </th>
	  <th class="bad">
	    Excess <span class="tooltip">Number of unneeded Add-3 requests</span>
	  </th>
	  <th class="good">
	    True <span class="tooltip">Number right "No More" calls</span>
	  </th>
	  <th class="bad">
	    False <span class="tooltip">Number wrong "No More" calls</span>
	  </th>
	</tr>
	{#each players_data as player}
	  <tr>
	    <td>{player["name"]}</td>
	    <td class="good">{player["numbers"][0]}</td>
	    <td class="bad">{player["numbers"][1]}</td>
	    <td class="good">{player["numbers"][2]}</td>
	    <td class="bad">{player["numbers"][3]}</td>
	    <td class="good">{player["numbers"][4]}</td>
	    <td class="bad">{player["numbers"][5]}</td>
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
  .good {
    color: #171
  }
  .bad {
    color: #e33
  }
  .tooltip {
    visibility: hidden;
    background-color: black;
    color: #fff;
    text-align: center;
    border-radius: 5px;
    padding: 5px;
    position: absolute;
    z-index: 1;
    opacity: 0;
    transition: opacity 0.3s;
  }

  th:hover .tooltip {
    visibility: visible;
    opacity: 1;
  }
</style>
