import { InjectionKey } from "vue";
import { createStore, useStore as baseUseStore, Store } from "vuex";
import { IfaceLiveTable, IfaceLiveRecord } from "@/types/IfaceLive";

interface State {
  isBackendUp: boolean;
  liveTable: IfaceLiveTable;
}

export const key: InjectionKey<Store<State>> = Symbol();

export const store = createStore<State>({
  state: {
    isBackendUp: false,
    liveTable: {},
  },
  mutations: {
    liveTablePush(state, record: IfaceLiveRecord): void {
      state.liveTable[record.id] = record;
      console.log(state.liveTable);
    },
    turnBackendState(state): void {
      state.isBackendUp = !state.isBackendUp;
    },
  },
  actions: {
    turnBackendState({ commit }): void {
      commit("turnBackendState");
    },
    liveTablePush({ commit }, record: IfaceLiveRecord): void {
      commit("liveTablePush", record);
    },
  },
});

const socket = new WebSocket("ws://localhost:9825/front");

socket.onopen = () => {
  store.dispatch("turnBackendState");
};
socket.onmessage = (event) => {
  const record = JSON.parse(event.data);
  store.dispatch("liveTablePush", record);
};
/* eslint-disable @typescript-eslint/no-explicit-any */
export function useStore(): any {
  return baseUseStore(key);
}
