<script lang="ts">
  import {
    Header,
    HeaderNav,
    HeaderNavItem,
    HeaderNavMenu,
    SkipToContent,
    Content,
    Grid,
    Row,
    Column,
  } from "carbon-components-svelte";
  import CANLiveTable from './components/CANLiveTable.svelte';
  let isSideNavOpen = false;

</script>


<Header company="Polar Labs™" platformName="BusBunny" bind:isSideNavOpen>
  <svelte:fragment slot="skip-to-content">
    <SkipToContent />
  </svelte:fragment>
  <HeaderNav>
    <HeaderNavItem href="/" text="Link 1" />
    <HeaderNavItem href="/" text="Link 2" />
    <HeaderNavItem href="/" text="Link 3" />
    <HeaderNavMenu text="Menu">
      <HeaderNavItem href="/" text="Link 1" />
      <HeaderNavItem href="/" text="Link 2" />
      <HeaderNavItem href="/" text="Link 3" />
    </HeaderNavMenu>
  </HeaderNav>
</Header>

<Content>
  <Grid>
    <Row>
      <Column>
        <CANLiveTable/>
      </Column>
    </Row>
  </Grid>
</Content>

<style>
  @import "carbon-components-svelte/css/g90.css";
  :global(body) {
    height: calc(100vh - 48px);
  }
  :global(.bx--data-table) {
    font-family: 'IBM Plex Mono';
  }
</style>
