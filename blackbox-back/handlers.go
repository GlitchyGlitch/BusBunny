package main

import (
	"github.com/gorilla/websocket"
	"log"
	"net/http"
	"time"
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
				"pid": "01F",
				"data": ["0A", "12", "BA"],
				"ctrl": [true, true, true, false, false, false],
				"ts": 123,
				"int": 100,
				"count": 20
			}`))
			if err != nil {
				return
			}
			time.Sleep(100 * time.Millisecond)
			err = wsc.Send([]byte(`{
				"pid": "01F",
				"data": ["AC", "12", "BA"],
				"ctrl": [true, false, false, false, false, false],
				"ts": 123,
				"int": 100,
				"count": 20
			}`))
			if err != nil {
				return
			}
			time.Sleep(100 * time.Millisecond)
			err = wsc.Send([]byte(`{
				"pid": "AF1",
				"data": ["AC", "12", "BA", "FF"],
				"ctrl": [true, false, false, false, false, false],
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
