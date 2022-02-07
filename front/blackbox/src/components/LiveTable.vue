<template>
  <div class="live-table">
    <div class="record">
      <div class="record__id">ID</div>
      <div>Ts</div>
      <div>Int</div>
      <div>Count</div>
      <div class="record__data">Data</div>
      <div class="">Printable</div>
    </div>
    <div class="record" v-for="(record, id) in liveTable" :key="id">
      <div class="record__id">{{ id }}</div>
      <div>{{ record.ts }}</div>
      <div>{{ record.int }}</div>
      <div>{{ record.count }}</div>
      <div class="record__data">
        <div
          class="record__databyte"
          v-for="(byte, index) in record.data"
          :key="index"
        >
          <!-- Little hack for fluent indication of changes -->
          <div class="change-indicator" :key="byte">{{ byte }}</div>
        </div>
      </div>
      <div>{{ record.print }}</div>
    </div>
  </div>
</template>

<script lang="ts">
import { Vue } from "vue-class-component";
import { useStore } from "@/store";
import { IfaceLiveRecord } from "@/types/IfaceLive";

export default class LiveTable extends Vue {
  store = useStore();

  get liveTable(): IfaceLiveRecord {
    return this.store.state.liveTable;
  }
}
</script>

<style scoped lang="scss">
.live-table {
  background-color: #222;
  padding: 20px;
  font-family: "JetBrains Mono", monospace;
}

.record {
  display: grid;
  grid-template-columns: repeat(4, 65px) 220px 150px;
  gap: 5px;

  &__data {
    display: flex;
    gap: 5px;
  }
}
.change-indicator {
  border: transparent solid 1px;

  animation: 1.2s change-blink;
}

@keyframes change-blink {
  from {
    background-color: #4200ff;
    border: #777 solid 1px;
  }
  to {
    background-color: transparent;
    border: transparent solid 1px;
  }
}
</style>
