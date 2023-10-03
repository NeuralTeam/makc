package makc

import (
	"github.com/NeuralTeam/makc/internal/wrapper"
	"github.com/NeuralTeam/makc/internal/wrapper/keyboard"
	"github.com/NeuralTeam/makc/internal/wrapper/mouse"
)

var (
	Keyboard wrapper.Keyboard
	Mouse    wrapper.Mouse
)

func init() {
	Keyboard, Mouse = keyboard.New(), mouse.New()
}
