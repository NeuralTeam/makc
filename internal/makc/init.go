package makc

import (
	"os"
	"os/signal"
)

func init() {
	if err := Initialize(); err != nil {
		panic(err)
	}
	go func() {
		defer Terminate()

		interrupt := make(chan os.Signal, 1)
		signal.Notify(interrupt, os.Interrupt)
		<-interrupt
	}()
}
