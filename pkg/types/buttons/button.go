package buttons

import (
	"github.com/NeuralTeam/makc/internal/makc"
)

const (
	leftbutton   = makc.LeftButton
	rightbutton  = makc.RightButton
	middlebutton = makc.MiddleButton
	sidebutton   = makc.SideButton
)

type Button int

const (
	Left Button = iota
	Right
	Middle
	Side
)

var (
	Buttons = []Button{
		Left,
		Right,
		Middle,
		Side,
	}
)

func (s Button) String() string {
	return [...]string{
		leftbutton,
		rightbutton,
		middlebutton,
		sidebutton,
	}[s]
}
