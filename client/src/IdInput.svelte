<script>
  export let value = '';

  function handleInput(event) {
    value = value.replace(/[^a-zA-Z0-9]/g, '');
  }
</script>

<input type="text" bind:value={value} on:input={handleInput}>
