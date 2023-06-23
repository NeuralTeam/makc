package mouse

import "github.com/NeuralTeam/makc"

type Button [2]State

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
