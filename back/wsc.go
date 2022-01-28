package main

import (
	"errors"
	"github.com/gorilla/websocket"
)

type WSController struct {
	conn      *websocket.Conn
	done      chan bool
	ChIn      chan []byte
	ChOut     chan []byte
	InBufLen  int
	OutBufLen int
}

func (c *WSController) Send(data []byte) error {
	select {
	case <-c.done:
		return errors.New("connection closed")
	default:
		c.ChOut <- data
	}
	return nil
}

func (c *WSController) Receive() ([]byte, error) {
	msg, ok := <-c.ChIn
	if !ok {
		return nil, errors.New("cannot receive, disconnected") //TODO: make proper error handling
	}
	return msg, nil
}

func (c *WSController) ReadPump() {
	defer func() { close(c.done) }()
	for {
		mt, msg, err := c.conn.ReadMessage()
		if err != nil {
			return
		}
		if mt != websocket.TextMessage {
			continue
		}
		c.ChIn <- msg
	}
}

func (c *WSController) WritePump() {
	defer func() { close(c.done) }()
	for {
		msg, ok := <-c.ChOut
		if !ok {
			c.conn.WriteMessage(websocket.CloseMessage, []byte{})
			return
		}
		if err := c.conn.WriteMessage(websocket.TextMessage, msg); err != nil {
			return
		}
	}
}

func NewWSController(conn *websocket.Conn, done chan bool, inBufLen, outBufLen int) *WSController {
	return &WSController{
		conn:      conn,
		done:      done,
		InBufLen:  inBufLen,
		OutBufLen: outBufLen,
		ChIn:      make(chan []byte, inBufLen),
		ChOut:     make(chan []byte, outBufLen),
	}
}
