<template>
  <div class="live-table">
    <div class="record" v-for="(record, pid) in liveTable" :key="pid">
      <div class="record__pid">{{ pid }}</div>
      <div class="record__data">
        <div
          class="record__databyte"
          v-for="(byte, index) in record.data"
          :key="index"
          :class="{ 'record__databyte--changed': record.diff[index] }"
        >
          {{ byte }}
        </div>
      </div>
      <!-- <div
        class="record__data"
        v-for="(flag, index) in record.diff"
        :key="index"
      >
        {{ flag }}
      </div> -->
      <div>{{ record.ts }}</div>
      <div>{{ record.int }}</div>
      <div>{{ record.count }}</div>
    </div>
  </div>
</template>

<script lang="ts">
import { Vue } from "vue-class-component";
import { useStore } from "@/store";

export default class LiveTable extends Vue {
  store = useStore();

  get liveTable(): number {
    return this.store.state.liveTable;
  }
}
</script>

<style scoped lang="scss">
.record {
  display: flex;
  font-family: monospace;
  gap: 5px;
  &__data {
    display: flex;
    gap: 2px;
  }
  &__databyte {
    &--changed {
      background-color: #aaa;
    }
  }
}
</style>
