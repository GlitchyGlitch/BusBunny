import { createRouter, createWebHistory, RouteRecordRaw } from "vue-router";
import LiveView from "@/views/LiveView.vue";
import Log from "@/views/Log.vue";

const routes: Array<RouteRecordRaw> = [
  {
    path: "/",
    name: "LiveView",
    component: LiveView,
  },
  {
    path: "/log",
    name: "Log",
    // route level code-splitting
    // this generates a separate chunk (about.[hash].js) for this route
    // which is lazy-loaded when the route is visited.
    component: Log,
  },
];

const router = createRouter({
  history: createWebHistory(process.env.BASE_URL),
  routes,
});

export default router;
