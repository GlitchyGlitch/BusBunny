<template>
  <div class="nav">
    <router-link to="/">Live</router-link> |
    <router-link to="/about">Log</router-link>
  </div>
  <router-view />
</template>

<style lang="scss">
@use "scss/normalization" as *;
@use "scss/typography" as *;
#app {
  height: 100vh;
  background-color: #000;
}
.nav {
  padding: 20px;
}
</style>
