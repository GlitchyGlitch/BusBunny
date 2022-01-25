<template>
  <div class="live-table">
    <div class="record" v-for="(record, pid) in liveTable" :key="pid">
      <div class="record__pid">{{ pid }}</div>
      <div>{{ record.ts }}</div>
      <div>{{ record.int }}</div>
      <div>{{ record.count }}</div>
      <div class="record__data">
        <div class="record__databyte" v-for="byte in record.data" :key="byte">
          <!-- Little hack for fluent indication of changes -->
          <div class="change-indicator">{{ byte }}</div>
        </div>
      </div>
      <!-- <div
        class="record__data"
        v-for="(flag, index) in record.diff"
        :key="index"
      >
        {{ flag }}
      </div> -->
    </div>
  </div>
</template>

<script lang="ts">
import { Vue } from "vue-class-component";
import { useStore } from "@/store";
import { CANLiveRecord } from "@/types/CANLive";

export default class LiveTable extends Vue {
  store = useStore();

  get liveTable(): CANLiveRecord {
    return this.store.state.liveTable;
  }
}
</script>

<style scoped lang="scss">
.record {
  font-family: monospace;
  display: flex;
  gap: 5px;

  &__data {
    display: flex;
    gap: 2px;
  }
}
.change-indicator {
  animation: 1s change-blink;
}

@keyframes change-blink {
  from {
    background-color: rgb(1, 139, 47);
  }
  to {
    background-color: transparent;
  }
}
</style>
