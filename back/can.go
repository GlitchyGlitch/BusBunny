package main

type IfaceFrame struct {
	Id     string
	Data   []string
	Ctrl   []bool
	Print  []string
	Ts     int
	Tsdiff int
	Count  int
}
type IfaceLiveFrame struct {
	Id     string
	Data   []string
	Ctrl   []bool
	Print  []string
	Ts     int
	Tsdiff int
	Count  int
}

type IfaceLogTable map[string]IfaceFrame
type IfaceLiveTable map[string]IfaceLiveFrame

func (f *IfaceFrame) Parse() { //TODO: figure out parsing

}
