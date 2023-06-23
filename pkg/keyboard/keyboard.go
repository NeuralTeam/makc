package keyboard

import (
	"github.com/NeuralTeam/makc"
	"sync"
)

type Keyboard struct {
	Keys sync.Map
}

func New() (k *Keyboard) {
	k = new(Keyboard)
	return
}

func (k *Keyboard) GetKeyState(key Key) (b bool) {
	b = makc.GetKeyState(string(key))
	k.Keys.Store(key, b)
	return
}
