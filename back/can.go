package main

type CanLiveFrame struct {
	Pid    string
	Data   []string
	Ctrl   []bool
	Print  []string
	Ts     int
	Tsdiff int
	Count  int
}

type CanLiveTable map[string]CanLiveFrame

func (f *CanLiveFrame) Parse() { //TODO: figure out parsing

}
