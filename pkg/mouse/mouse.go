package mouse

import (
	"github.com/NeuralTeam/makc"
	"github.com/NeuralTeam/makc/pkg/utils"
	"strings"
	"sync"
)

type State string
type Button [2]State

const (
	down State = "down"
	up   State = "up"
)

var (
	states = []State{
		down,
		up,
	}
)

var (
	Left = Button{
		makc.LeftButtonDown,
		makc.LeftButtonUp,
	}
	Right = Button{
		makc.RightButtonDown,
		makc.RightButtonUp,
	}
	Middle = Button{
		makc.MiddleButtonDown,
		makc.MiddleButtonUp,
	}
	Side = Button{
		makc.SideButtonDown,
		makc.SideButtonUp,
	}
	Buttons = []Button{
		Left,
		Right,
		Middle,
		Side,
	}
)

const (
	LeftButton   State = "leftbutton"
	RightButton  State = "rightbutton"
	MiddleButton State = "middlebutton"
	SideButton   State = "sidebutton"
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

func (m *Mouse) GetPointer() utils.Point {
	x, y := makc.GetMousePos()
	m.Pointer = utils.Point{X: x, Y: y}
	return m.Pointer
}

func (m *Mouse) GetButtonState(button Button) (state bool) {
	for _, e := range button {
		for _, s := range states {
			e, s := string(e), string(s)
			if strings.Contains(e, s) && makc.GetMouseEventState(e) {
				e = strings.TrimSuffix(e, s)
				state = m.stateToBool(State(s))
				m.Buttons.Store(State(e), state)
				break
			}
		}
	}
	return
}

func (m *Mouse) stateToBool(state State) (result bool) {
	if state == down {
		result = true
	}
	return
}
