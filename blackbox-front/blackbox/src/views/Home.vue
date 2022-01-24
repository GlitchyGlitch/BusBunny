<template>
  <div class="home"><LiveTable /></div>
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
