package keyboard

import (
	"github.com/NeuralTeam/makc"
	"github.com/NeuralTeam/makc/pkg/types"
	"sync"
)

type Keyboard struct {
	Keys sync.Map
}

func New() (k *Keyboard) {
	k = new(Keyboard)
	return
}

func (k *Keyboard) GetKeyState(key Key) (keyState types.State) {
	if s := makc.GetKeyState(key.String()); s {
		keyState = types.Down
	}
	k.Keys.Store(key, keyState)
	return
}
