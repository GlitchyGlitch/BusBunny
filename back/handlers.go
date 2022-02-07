package main

import (
	"log"
	"net/http"
	"time"

	"github.com/gorilla/websocket"
)

func NewFrontHandler(upgrader *websocket.Upgrader) func(w http.ResponseWriter, r *http.Request) {
	return func(w http.ResponseWriter, r *http.Request) {
		done := make(chan bool)

		c, err := upgrader.Upgrade(w, r, nil)
		if err != nil {
			log.Println("Websocket upgrade error")
			return
		}
		defer c.Close()

		wsc := NewWSController(c, done, 512, 512)

		go wsc.WritePump()
		go wsc.ReadPump()
		for {
			err := wsc.Send([]byte(`{
				"id": "01F",
				"data": ["0A", "12", "BA", "0A", "..", "..", ".."],
				"ctrl": [true, true, true, false, false, false],
				"print": ".Aśc.<",
				"ts": 123,
				"int": 100,
				"count": 20
			}`))
			if err != nil {
				return
			}
			time.Sleep(1700 * time.Millisecond)
			err = wsc.Send([]byte(`{
				"id": "01F",
				"data": ["0A", "FF", "BA", "0A", "FF", "..", ".."],
				"ctrl": [true, true, true, false, false, false],
				"print": ".Arc.?",
				"ts": 123,
				"int": 100,
				"count": 20
			}`))
			if err != nil {
				return
			}
			time.Sleep(1700 * time.Millisecond)
			err = wsc.Send([]byte(`{
				"id": "612",
				"data": ["0A", "FF", "BA", "0A", "FF", "..", ".."],
				"ctrl": [true, true, true, false, false, false],
				"print": ".Arc.?",
				"ts": 123,
				"int": 100,
				"count": 20
			}`))
			if err != nil {
				return
			}
			time.Sleep(100 * time.Millisecond)
		}
	}
}
