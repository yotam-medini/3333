<!-- Modal.svelte (by ChatGPT) -->
<script>
  export let showModal = false;
  export let title = "";
</script>

{#if showModal}
<div class="modal">
  <div class="modal-content">
    <h2>{title}</h2>
    <slot></slot> <!-- Slot for modal content -->
    <button class="but" on:click={() => (showModal = false)}>Close</button>
  </div>
</div>
{/if}

<style>
  .but {
    color: #e22;
    background-color: #ccc;
  }
  .modal {
    position: fixed;
    top: 0;
    left: 0;
    width: 100%;
    height: 100%;
    background: rgba(0, 0, 0, 0.5);
    display: flex;
    justify-content: center;
    align-items: center;
  }

  .modal-content {
    background: white;
    padding: 20px;
    border-radius: 5px;
  }
</style>
