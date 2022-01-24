import { InjectionKey } from "vue";
import { createStore, useStore as baseUseStore, Store } from "vuex";
import { CANLiveTable, CANLiveRecord } from "@/types/CANLive";

interface State {
  isBackendUp: boolean;
  liveTable: CANLiveTable;
}

export const key: InjectionKey<Store<State>> = Symbol();

export const store = createStore<State>({
  state: {
    isBackendUp: false,
    liveTable: {},
  },
  mutations: {
    liveTablePush(state, record: CANLiveRecord): void {
      state.liveTable[record.pid] = record;
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
    liveTablePush({ commit }, record: CANLiveRecord): void {
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
