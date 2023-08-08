package main

import (
	"github.com/NeuralTeam/makc"
	"github.com/NeuralTeam/makc/pkg/keyboard"
	"github.com/NeuralTeam/makc/pkg/types"
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
				switch v := v.(type) {
				case types.State:
					if !v.Bool() {
						break
					}
					log.Printf(
						"%v: %v",
						k, v,
					)
				}
				return true
			})
		}
	}
}
