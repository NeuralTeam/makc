package main

import (
	"github.com/NeuralTeam/makc"
	"github.com/NeuralTeam/makc/pkg/keyboard"
	"log"
	"time"
)

func main() {
	if ok, err := makc.Initialize(); err != nil || !ok {
		log.Fatalln("failed to initialize:", err.Error())
	}
	defer makc.Terminate()

	k := keyboard.New()
	t := time.NewTicker(time.Millisecond)
	defer t.Stop()
	for {
		select {
		case <-t.C:
			for _, e := range keyboard.Keys {
				_ = k.GetKeyState(e)
				//log.Printf("state: %v", s)
			}
			k.Keys.Range(func(k interface{}, v interface{}) bool {
				b := v.(bool)
				if b {
					log.Printf(
						"%v: %v",
						k, keyboard.BoolToState(b),
					)
				}
				return true
			})
		}
	}
}
