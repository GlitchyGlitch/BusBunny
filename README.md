# BlackBox

## Backend forntend communication prorocol

Protocol is used for communication through websockets.

### CAN frames example

Live:

```json
{
  "pid": "01F",
  "data": ["0A", "AF", "FA"],
  "diff": [false, true, false, false, false, false, false, false],
  "ctrl": [true, true, true, false, false, false],
  "ts": 123,
  "int": 100,
  "count": 20
}
```
