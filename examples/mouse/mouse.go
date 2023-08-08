package main

import (
	"github.com/NeuralTeam/makc"
	"github.com/NeuralTeam/makc/pkg/mouse"
	"github.com/NeuralTeam/makc/pkg/types"
	"log"
	"time"
)

func main() {
	if ok, err := makc.Initialize(); err != nil || !ok {
		log.Fatalln("failed to initialize:", err.Error())
	}
	defer makc.Terminate()

	m := mouse.New()
	t := time.NewTicker(time.Millisecond)
	defer t.Stop()
	for {
		select {
		case <-t.C:
			for _, e := range mouse.Buttons {
				_ = m.GetButtonState(e)
				//log.Printf("state: %v", s)
			}
			m.Buttons.Range(func(k, v interface{}) bool {
				switch v := v.(type) {
				case types.State:
					log.Printf(
						"%v: %v",
						k, v,
					)
					if v.Bool() && k == mouse.Right {
						makc.MoveMouse(10, 10, true)
						log.Printf("pointer: %v", m.GetPointer())
					}
				}
				return true
			})
		}
	}
}
