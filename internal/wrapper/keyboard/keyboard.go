package keyboard

import (
	"github.com/NeuralTeam/makc/internal/makc"
	"github.com/NeuralTeam/makc/pkg/types"
	"github.com/NeuralTeam/makc/pkg/types/keys"
	"strings"
	"sync"
)

type Keyboard struct {
	Keys sync.Map
}

func New() (k *Keyboard) {
	k = new(Keyboard)
	return
}

func (k *Keyboard) GetKeyState(key keys.Key) (keyState types.State) {
	if s := makc.GetKeyState(key.String()); s {
		keyState = types.Down
	}
	k.Keys.Store(key, keyState)
	return
}

func (k *Keyboard) KeysRange(f func(k keys.Key, v types.State) bool) {
	k.Keys.Range(func(k, v any) bool {
		switch k := k.(type) {
		case keys.Key:
			if v, ok := v.(types.State); ok {
				return f(k, v)
			}
		default:
			break
		}
		return true
	})
}

func (k *Keyboard) GetFirstKey() (firstKey keys.Key) {
	r := makc.GetFirstKey()
	for _, e := range keys.Keys {
		if strings.Contains(r, e.String()) {
			firstKey = e
		}
	}
	return
}
