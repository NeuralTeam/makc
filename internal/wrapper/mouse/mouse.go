package mouse

import (
	"github.com/NeuralTeam/makc/internal/makc"
	"github.com/NeuralTeam/makc/pkg/types"
	"github.com/NeuralTeam/makc/pkg/types/buttons"
	"strings"
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

func (m *Mouse) GetButtonState(button buttons.Button) (buttonState types.State) {
	for _, buttonState = range types.States {
		if makc.GetMouseEventState(button.String() + buttonState.String()) {
			m.Buttons.Store(button, buttonState)
			break
		}
	}
	return
}

func (m *Mouse) ButtonsRange(f func(k buttons.Button, v types.State) bool) {
	m.Buttons.Range(func(k, v any) bool {
		switch k := k.(type) {
		case buttons.Button:
			if v, ok := v.(types.State); ok {
				return f(k, v)
			}
		default:
			break
		}
		return true
	})
}

func (m *Mouse) GetFirstButton() (firstButton buttons.Button) {
	r := makc.GetFirstMouseEvent()
	for _, e := range buttons.Buttons {
		if strings.Contains(r, e.String()) {
			firstButton = e
		}
	}
	return
}

func (m *Mouse) Click(button buttons.Button) {
	switch button {
	case buttons.Left:
		makc.LeftMouseButtonClick()
		break
	case buttons.Right:
		makc.RightMouseButtonClick()
		break
	case buttons.Middle:
		makc.MiddleMouseButtonClick()
		break
	default:
		break
	}
}

func (m *Mouse) Move(point types.Point, relative, humanly bool) {
	if humanly {
		makc.MoveMouseHumanly(point.X, point.Y)
	} else {
		makc.MoveMouse(point.X, point.Y, relative)
	}
}
