package main

import (
	"github.com/NeuralTeam/makc"
	"github.com/NeuralTeam/makc/pkg/types"
	"github.com/NeuralTeam/makc/pkg/types/keys"
	"log"
	"time"
)

func main() {
	k := makc.Keyboard()

	log.Println("waiting for keyboard key...")
	log.Printf("pressed key: %v", k.GetFirstKey())

	for {
		select {
		case <-time.After(time.Millisecond):
			for _, e := range keys.Keys {
				_ = k.GetKeyState(e)
				//log.Printf("state: %v", s)
			}
			k.KeysRange(func(k keys.Key, v types.State) bool {
				if !v.Bool() {
					return true
				}
				log.Printf(
					"%v: %v",
					k, v,
				)
				return true
			})
		}
	}
}
