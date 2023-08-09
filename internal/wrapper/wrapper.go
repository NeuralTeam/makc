package wrapper

import (
	"github.com/NeuralTeam/makc/pkg/types"
	"github.com/NeuralTeam/makc/pkg/types/buttons"
	"github.com/NeuralTeam/makc/pkg/types/keys"
)

type Keyboard interface {
	GetKeyState(key keys.Key) (keyState types.State)
	GetFirstKey() (firstKey keys.Key)
	KeysRange(f func(k keys.Key, v types.State) bool)
}

type Mouse interface {
	GetPointer() (pointer types.Point)
	GetButtonState(button buttons.Button) (buttonState types.State)
	GetFirstButton() (firstButton buttons.Button)
	ButtonsRange(f func(k buttons.Button, v types.State) bool)
	Click(button buttons.Button)
	Move(point types.Point, relative, humanly bool)
}
