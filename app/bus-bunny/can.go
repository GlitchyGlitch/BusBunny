package main

type CANRecord struct {
	CanId string   `json:"canId"`
	Count int      `json:"count"`
	Data  []string `json:"data"`
	Print string   `json:"print"`
}
