interface CANLiveRecord {
  pid: string;
  data: [string];
  diff: [boolean];
  ctrl: [boolean];
  ts: number;
  int: number;
  count: number;
}

interface CANLiveTable {
  [key: string]: CANLiveRecord;
}

export { CANLiveRecord, CANLiveTable };
