<script lang="ts">
    import { DataTable } from 'carbon-components-svelte';
    import type { DataTableRow } from 'carbon-components-svelte/types/DataTable/DataTable.svelte';
    import { onMount } from 'svelte';
  
    let headers = [
      { key: "count", value: "Ct", width: "48px"},
      { key: "id", value: "ID", width: "58px"},
      { key: "b1", value: "01", width: "48px"},
      { key: "b2", value: "02", width: "48px"},
      { key: "b3", value: "03", width: "48px"},
      { key: "b4", value: "04", width: "48px"},
      { key: "b5", value: "05", width: "48px"},
      { key: "b6", value: "06", width: "48px"},
      { key: "b7", value: "07", width: "48px"},
      { key: "b8", value: "08", width: "48px"},
      { key: "print", value: "Printable" },
    ];

    let ws: WebSocket;
    let messages: DataTableRow[] = [];

    interface CanRecord {
      count: number,
      id: string,
      print: string,
      b1: string,
      b2: string,
      b3: string,
      b4: string,
      b5: string,
      b6: string,
      b7: string,
      b8: string,
    }
    interface CanPDU {
      count: number,
      canId: string,
      print: string,
      data: string[8]
    }

    let convertCanFrame: (canFrame: string) => CanRecord = (canFrame: string) => {
      let canFrameObj: CanPDU = JSON.parse(canFrame);
      let result: CanRecord = {
        count: canFrameObj.count,
        id: canFrameObj.canId,
        print: canFrameObj.print,
        b1: canFrameObj.data[0],
        b2: canFrameObj.data[1],
        b3: canFrameObj.data[2],
        b4: canFrameObj.data[3],
        b5: canFrameObj.data[4],
        b6: canFrameObj.data[5],
        b7: canFrameObj.data[6],
        b8: canFrameObj.data[7]
      }
      return result
    }

    let handle = (event) => {
        const canFrame = convertCanFrame(event.data);
        const index = messages.findIndex(f => f.id == canFrame.id);
        if (index == -1) {
          messages.push(canFrame);
          return
        }
        messages[index] = canFrame;
      };

    onMount(() => {
      ws = new WebSocket('ws://localhost:9825/front');
  
      ws.addEventListener("message", handle);
    });

  
</script>
  
<style>
  /* Stylizacja dla twojego komponentu Svelte */
</style>
  
<div>
  <DataTable
    title="Can Live Traffic"
    description="esp-32-asddf-asdsfd-asdagfgd-asdfgh"
    headers={headers}
    rows={messages} />
</div>
  