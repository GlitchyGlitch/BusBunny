# BlackBox

## Backend forntend communication prorocol

Protocol is used for communication through websockets.

### CAN frames example

Live:

```json
{
  "pid": "01F",
  "data": ["41", "41", "41", "41", "..", "..", ".."],
  "ctrl": [true, true, true, false, false, false],
  "print": "AAAA....",
  "ts": 123,
  "int": 100,
  "count": 20
}
```
