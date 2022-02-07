interface IfaceLiveRecord {
  id: string;
  data: [string];
  print: string;
  ctrl: [boolean];
  ts: number;
  int: number;
  count: number;
}

interface IfaceLiveTable {
  [key: string]: IfaceLiveRecord;
}

export { IfaceLiveRecord, IfaceLiveTable };
