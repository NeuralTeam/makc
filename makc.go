package makc

import (
	"github.com/NeuralTeam/makc/internal/wrapper"
	"github.com/NeuralTeam/makc/internal/wrapper/keyboard"
	"github.com/NeuralTeam/makc/internal/wrapper/mouse"
)

var (
	_Keyboard = keyboard.New()
	_Mouse    = mouse.New()
)

func Keyboard() wrapper.Keyboard {
	return _Keyboard
}

func Mouse() wrapper.Mouse {
	return _Mouse
}
