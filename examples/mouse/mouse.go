package main

import (
	"github.com/NeuralTeam/makc"
	"github.com/NeuralTeam/makc/pkg/types"
	"github.com/NeuralTeam/makc/pkg/types/buttons"
	"log"
	"time"
)

func main() {
	log.Println("waiting for mouse button...")
	firstButton := makc.Mouse.GetFirstButton()
	log.Printf("pressed button: %v", firstButton)

	for range time.Tick(time.Millisecond) {
		for _, e := range buttons.Buttons {
			_ = makc.Mouse.GetButtonState(e)
			//log.Printf("state: %v", s)
		}
		makc.Mouse.ButtonsRange(func(k buttons.Button, v types.State) bool {
			if !v.Bool() {
				return true
			}
			log.Printf(
				"%v: %v",
				k, v,
			)
			if k == firstButton {
				makc.Mouse.Move(
					types.Point{X: 10, Y: 10},
					true, false,
				)
				log.Printf("pointer: %v", makc.Mouse.GetPointer())
			}
			return true
		})
	}
}
