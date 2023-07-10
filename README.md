# BusBunny

## Versioning

Structure of protocol cannot be changed within the generation. Version of software, firmware and hardware is described by generation number `>=0`, version number `0 - 65535` and prefix

- `vs` for software eg. `vs1.125`
- `vf` for firmware eg. `vf1.21`
- `vh` for hardware eg. `vh1.3228`

## Backend-frontend communcation

Protocol uses websockets.

### Live CAN PDU:

```json
{
  "id": "01F",
  "data": ["41", "41", "41", "41", "..", "..", "..", ".."],
  "ctrl": [true, true, true, false, false, false],
  "print": "AAAA....",
  "ts": 123,
  "tsdiff": 100,
  "count": 20
}
```

## Interface comunication

For now interface starts to send `simplified standard CAN PDUs` out of the box, as soon as it is connected to bus and computer

<!-- ### Describtion

Interface provides accesspoint with DHCP, DNS and TCP server. Client device connects to it and starts communication with handshake. After that connection mode has to be set with control frame.

### Handshake

Interface -> Bakcend

```
| 16  | 16  | 16  | 128    |
| --- | --- | --- | ------ |
| ST  | FWV | HWV | SERIAL |
```

| Field  | Length | Describtion              |
| ------ | ------ | ------------------------ |
| ST     | 16     | SelfTest error code      |
| FWV    | 16     | FirmWere Version as code |
| HWV    | 16     | HardWare Version as code |
| SERIAL | 128    | Serial number of device  |

### Control frame

Interface <- Backend

```
| 8   | 1024  |
| --- | ----- |
| ACT | PARAM |
```

| Field | Length | Describtion          |
| ----- | ------ | -------------------- |
| ACT   | 8      | Code of action       |
| PARAM | 1024   | Parameter for action |

### Modes

Mode is set by action `0x00`. In param of action there is 16 btis of mode code first and additional data next.

`PARAM` field in control frame:

```
| Code     | Additional data | Filling  |
| -------- | --------------- | -------- |
| 00000001 | 1111            | 0000.... |
```

| Mode                   | Abbreviation | Code(16) | Additional data | Describtion                                                                                            |
| ---------------------- | ------------ | -------- | --------------- | ------------------------------------------------------------------------------------------------------ |
| Simpified standard CAN | CANSilmple   | 0x01     | Bus speed (8)   | CAN frame without start, end, delimeters, reserved bits and CRC (error detection handled by interface) |

### Actions

Actions are used in control frames for configuring device. Actions can take up to 1024 bits of parameter.

| Acion       | Code | `PARAM` format                             |
| ----------- | ---- | ------------------------------------------ |
| Change mode | 0x01 | Mode code(16) \| Additional data(variable) |
 -->

### Simplified standard CAN PDU

Interface <-> Backend

```
| 16  | 29  | 1   | 1   | 1   | 4   | 64   |
| --- | --- | --- | --- | --- | --- | ---- |
| LEN | ID  | RTR | IDE | ACK | DLC | DATA |
```


# Interactive idf mode
```docker run --rm -v $PWD:/blackbox -w /blackbox -it espressif/idf:release-v4.4```