package makc

import (
	"log"
	"time"
)

func init() {
	go func() {
		if ok, err := Initialize(); err != nil || !ok {
			log.Fatalln("failed to initialize:", err.Error())
		}
		defer Terminate()

		for {
			select {
			case <-time.After(time.Hour):
				continue
			}
		}
	}()
}
