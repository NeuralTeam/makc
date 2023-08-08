package mouse

import (
	"github.com/NeuralTeam/makc"
	"github.com/NeuralTeam/makc/pkg/types"
	"sync"
)

type Mouse struct {
	Pointer types.Point
	Buttons sync.Map
}

func New() (m *Mouse) {
	m = new(Mouse)
	return
}

func (m *Mouse) GetPointer() (pointer types.Point) {
	x, y := makc.GetMousePos()
	pointer = types.Point{X: x, Y: y}
	m.Pointer = pointer
	return
}

func (m *Mouse) GetButtonState(button Button) (buttonState types.State) {
	for _, buttonState = range types.States {
		if makc.GetMouseEventState(button.String() + buttonState.String()) {
			m.Buttons.Store(button, buttonState)
			break
		}
	}
	return
}
