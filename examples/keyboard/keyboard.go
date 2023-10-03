package main

import (
	"github.com/NeuralTeam/makc"
	"github.com/NeuralTeam/makc/pkg/types"
	"github.com/NeuralTeam/makc/pkg/types/keys"
	"log"
	"time"
)

func main() {
	log.Println("waiting for keyboard key...")
	log.Printf("pressed key: %v", makc.Keyboard.GetFirstKey())

	for range time.Tick(time.Millisecond) {
		for _, e := range keys.Keys {
			_ = makc.Keyboard.GetKeyState(e)
			//log.Printf("state: %v", s)
		}
		makc.Keyboard.KeysRange(func(k keys.Key, v types.State) bool {
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
