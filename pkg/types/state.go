package types

const (
	up   = `up`
	down = `down`
)

type State int

const (
	Up State = iota
	Down
)

var (
	States = []State{
		Up,
		Down,
	}
)

func (s State) String() string {
	return [...]string{
		up,
		down,
	}[s]
}

func (s State) Bool() bool {
	return [...]bool{
		false,
		true,
	}[s]
}
