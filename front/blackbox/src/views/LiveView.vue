<template>
  <div class="live-view">
    <div class="live-table">
      <LiveTable />
    </div>
  </div>
</template>

<script lang="ts">
import { Options, Vue } from "vue-class-component";
import { useStore } from "@/store";
import LiveTable from "@/components/LiveTable.vue";

@Options({
  components: {
    LiveTable,
  },
})
export default class Home extends Vue {
  store = useStore();

  get liveTable(): number {
    return this.store.state.liveTable;
  }
}
</script>
<style scoped lang="scss">
.live-view {
  display: grid;
  grid-template-columns: repeat(12, 1fr);
  grid-template-rows: repeat(7, 1fr);
  gap: 20px;
  padding: 20px;
}
.live-table {
  grid-column: 1/8;
}
</style>
