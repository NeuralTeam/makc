package main

import (
	"github.com/NeuralTeam/makc"
	"github.com/NeuralTeam/makc/pkg/types"
	"github.com/NeuralTeam/makc/pkg/types/buttons"
	"log"
	"time"
)

func main() {
	m := makc.Mouse()

	log.Println("waiting for mouse button...")
	firstButton := m.GetFirstButton()
	log.Printf("pressed button: %v", firstButton)

	for {
		select {
		case <-time.After(time.Millisecond):
			for _, e := range buttons.Buttons {
				_ = m.GetButtonState(e)
				//log.Printf("state: %v", s)
			}
			m.ButtonsRange(func(k buttons.Button, v types.State) bool {
				if !v.Bool() {
					return true
				}
				log.Printf(
					"%v: %v",
					k, v,
				)
				if k == firstButton {
					m.Move(
						types.Point{X: 10, Y: 10},
						true, false,
					)
					log.Printf("pointer: %v", m.GetPointer())
				}
				return true
			})
		}
	}
}
