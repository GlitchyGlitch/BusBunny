<template>
  <div class="nav">
    <img src="@/assets/logo.svg" alt="" />
    <div class="nav__menu">
      <router-link to="/">Live</router-link>
      <router-link to="/log">Log</router-link>
    </div>
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
  display: flex;
  align-items: center;
  justify-content: space-between;
  background-color: #111;

  &__menu {
    display: flex;
    gap: 20px;
  }
}
</style>
