<script lang="ts">
  import {
    Header,
    DataTable,
    HeaderNav,
    HeaderNavItem,
    HeaderNavMenu,
    SkipToContent,
    Content,
    Grid,
    Row,
    Column,
  } from "carbon-components-svelte";

  let isSideNavOpen = false;
</script>


<Header company="Polar Lab™" platformName="BusBunny" bind:isSideNavOpen>
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
        <DataTable
  title="Can Live Traffic"
  description="esp-32-asddf-asdsfd-asdagfgd-asdfgh"
  headers={[
    { key: "canId", value: "ID", width: "50px"},
    { key: "byte1", value: "Data", width: "50px"},
    { key: "byte2", value: "", width: "50px" },
    { key: "byte3", value: "", width: "50px" },
    { key: "byte4", value: "", width: "50px" },
    { key: "byte5", value: "", width: "50px" },
    { key: "byte6", value: "", width: "50px" },
    { key: "byte7", value: "", width: "50px" },
    { key: "byte8", value: "", width: "50px" },
    { key: "printable", value: "Printable" },

  ]}
  rows={[
    {
      id: "a",
      canId: "A3",
      byte1: "A5",
      byte2: "B5",
      byte3: "B9",
      byte4: "B9",
      byte5: "B9",
      byte6: "B9",
      byte7: "B9",
      byte8: "B9",
      printable: "BC..AA.."
    },
    {
      id: "b",
      canId: "A1",
      byte1: "A5",
      byte2: "98",
      byte3: "B9",
      byte4: "B9",
      byte5: "B9",
      byte6: "B9",
      byte7: "B9",
      byte8: "B9",
      printable: "BC..AA.."
    },
    {
      id: "c",
      canId: "A2",
      byte1: "32",
      byte2: "1F",
      byte3: "4D",
      byte4: "B9",
      byte5: "B9",
      byte6: "B9",
      byte7: "B9",
      byte8: "B9",
      printable: "BC..AA.."
    },
    {
      id: "d",
      canId: "A6",
      byte1: "A5",
      byte2: "7F",
      byte3: "B9",
      byte4: "B9",
      byte5: "B9",
      byte6: "B9",
      byte7: "B9",
      byte8: "B9",
      printable: "BC..AA.."
    },
    {
      id: "e",
      canId: "A4",
      byte1: "A5",
      byte2: "1B",
      byte3: "B9",
      byte4: "B9",
      byte5: "B9",
      byte6: "B9",
      byte7: "B9",
      byte8: "B9",
      printable: "BC..AA.."
    },
    {
      id: "f",
      canId: "A5",
      byte1: "A5",
      byte2: "E2",
      byte3: "4D",
      byte4: "B9",
      byte5: "B9",
      byte6: "B9",
      byte7: "B9",
      byte8: "B9",
      printable: "BC..AA.."
    },
  ]}
/>
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
