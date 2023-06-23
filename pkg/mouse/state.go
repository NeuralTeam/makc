package mouse

type State string

const (
	Down State = "down"
	Up   State = "up"
)

var (
	States = []State{
		Down,
		Up,
	}
)

func BoolToState(b bool) (state State) {
	state = Up
	if b {
		state = Down
	}
	return
}

func StateToBool(state State) (b bool) {
	if state == Down {
		b = true
	}
	return
}
