package main

import (
	"github.com/NeuralTeam/makc"
	"github.com/NeuralTeam/makc/pkg/mouse"
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
				b := v.(bool)
				log.Printf(
					"%v: %v",
					k, mouse.BoolToState(b),
				)
				if b && k == mouse.RightButton {
					makc.MoveMouse(5, 5, true)
					log.Printf("pointer: %v", m.GetPointer())
				}
				return true
			})
		}
	}
}
