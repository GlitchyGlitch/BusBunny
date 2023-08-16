package main

import (
	"bytes"
	"embed"
	"encoding/json"
	"log"
	"net/http"
	"os"
	"time"

	"github.com/gorilla/mux"
	"github.com/gorilla/websocket"
	"github.com/wailsapp/wails/v2"
	"github.com/wailsapp/wails/v2/pkg/options"
	"github.com/wailsapp/wails/v2/pkg/options/assetserver"
)

//go:embed all:frontend/dist
var assets embed.FS

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
		var buffer bytes.Buffer
		encoder := json.NewEncoder(&buffer)
		var can CANRecord
		can.CanId = "0FD"
		can.Count = 26
		can.Print = "asfgh"
		can.Data = []string{"as", "as", "as", "as", "as", "as", "as", "as"}
		encoder.Encode(can)
		for {
			err := wsc.Send(buffer.Bytes())
			if err != nil {
				println("here")
				return
			}
			time.Sleep(5 * time.Second)
		}
	}
}
func main() {

	r := mux.NewRouter()
	u := &websocket.Upgrader{}
	u.CheckOrigin = func(r *http.Request) bool {
		return true
	}

	r.HandleFunc("/front", NewFrontHandler(u))
	http.Handle("/", r)

	s := http.Server{
		Addr:    "localhost:9825",
		Handler: r,
		//TODO: ErrorLog:     l.StandardLogger(&hclog.StandardLoggerOptions{}), // set the logger for the server
		ReadTimeout:  5 * time.Second,
		WriteTimeout: 10 * time.Second,
		IdleTimeout:  120 * time.Second,
	}

	go func() {
		log.Println("Starting server on port 9825")

		err := s.ListenAndServe()
		if err != nil {
			os.Exit(1)
		}
	}()
	// Create an instance of the app structure
	app := NewApp()

	// Create application with options
	err := wails.Run(&options.App{
		Title:  "PLRC - BusBunny",
		Width:  1200,
		Height: 600,
		AssetServer: &assetserver.Options{
			Assets: assets,
		},
		OnStartup: app.startup,
		Bind: []interface{}{
			app,
		},
	})

	if err != nil {
		println("Error:", err.Error())
	}
}
