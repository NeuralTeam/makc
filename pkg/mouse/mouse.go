package mouse

import (
	"github.com/NeuralTeam/makc"
	"github.com/NeuralTeam/makc/pkg/utils"
	"strings"
	"sync"
)

var (
	ButtonStates = []State{
		LeftButton,
		RightButton,
		MiddleButton,
		SideButton,
	}
)

type Mouse struct {
	Pointer utils.Point
	Buttons sync.Map
}

func New() (k *Mouse) {
	k = new(Mouse)
	return
}

func (m *Mouse) GetPointer() utils.Point {
	x, y := makc.GetMousePos()
	m.Pointer = utils.Point{X: x, Y: y}
	return m.Pointer
}

func (m *Mouse) GetButtonState(button Button) (b bool) {
	for _, e := range button {
		for _, s := range States {
			e, s := string(e), string(s)
			if strings.Contains(e, s) && makc.GetMouseEventState(e) {
				e = strings.TrimSuffix(e, s)
				b = StateToBool(State(s))
				m.Buttons.Store(State(e), b)
				break
			}
		}
	}
	return
}
