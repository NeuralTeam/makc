package keyboard

import (
	"github.com/NeuralTeam/makc"
	"sync"
)

type Key string

const (
	K1     Key = makc.Key1
	K2     Key = makc.Key2
	K3     Key = makc.Key3
	K4     Key = makc.Key4
	K5     Key = makc.Key5
	K6     Key = makc.Key6
	K7     Key = makc.Key7
	K8     Key = makc.Key8
	K9     Key = makc.Key9
	K0     Key = makc.Key0
	Minus  Key = makc.KeyMinus
	Equals Key = makc.KeyEquals

	F1  Key = makc.KeyF1
	F2  Key = makc.KeyF2
	F3  Key = makc.KeyF3
	F4  Key = makc.KeyF4
	F5  Key = makc.KeyF5
	F6  Key = makc.KeyF6
	F7  Key = makc.KeyF7
	F8  Key = makc.KeyF8
	F9  Key = makc.KeyF9
	F10 Key = makc.KeyF10
	F11 Key = makc.KeyF11
	F12 Key = makc.KeyF12

	A Key = makc.KeyA
	S Key = makc.KeyS
	D Key = makc.KeyD
	F Key = makc.KeyF
	G Key = makc.KeyG
	H Key = makc.KeyH
	J Key = makc.KeyJ
	K Key = makc.KeyK
	L Key = makc.KeyL
	Q Key = makc.KeyQ
	W Key = makc.KeyW
	E Key = makc.KeyE
	R Key = makc.KeyR
	T Key = makc.KeyT
	Y Key = makc.KeyY
	U Key = makc.KeyU
	I Key = makc.KeyI
	O Key = makc.KeyO
	P Key = makc.KeyP
	Z Key = makc.KeyZ
	X Key = makc.KeyX
	C Key = makc.KeyC
	V Key = makc.KeyV
	B Key = makc.KeyB
	N Key = makc.KeyN
	M Key = makc.KeyM

	LeftSquareBracket  Key = makc.KeyLeftSquareBracket
	RightSquareBracket Key = makc.KeyRightSquareBracket
	BackQuote          Key = makc.KeyBackQuote
	Backslash              = makc.KeyBackslash

	Semicolon    Key = makc.KeySemicolon
	SingleQuote  Key = makc.KeySingleQuote
	Comma        Key = makc.KeyComma
	Dot          Key = makc.KeyDot
	QuestionMark     = makc.KeyQuestionMark

	Escape       Key = makc.KeyEscape
	Delete       Key = makc.KeyDelete
	Tab          Key = makc.KeyTab
	Enter        Key = makc.KeyEnter
	Control      Key = makc.KeyControl
	ControlLeft  Key = makc.KeyControlLeft
	ControlRight Key = makc.KeyControlRight
	Shift        Key = makc.KeyShift
	ShiftLeft    Key = makc.KeyShiftLeft
	ShiftRight   Key = makc.KeyShiftRight
	Space        Key = makc.KeySpace
	Backspace    Key = makc.KeyBackspace
	Capslock     Key = makc.KeyCapslock
	Insert       Key = makc.KeyInsert
	Printscreen  Key = makc.KeyPrintscreen
	End          Key = makc.KeyEnd
	Home         Key = makc.KeyHome
	Menu         Key = makc.KeyMenu
	AltLeft      Key = makc.KeyAltLeft
	AltRight     Key = makc.KeyAltRight
)

var (
	Keys = []Key{
		K1,
		K2,
		K3,
		K4,
		K5,
		K6,
		K7,
		K8,
		K9,
		K0,
		Minus,
		Equals,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		A,
		S,
		D,
		F,
		G,
		H,
		J,
		K,
		L,
		Q,
		W,
		E,
		R,
		T,
		Y,
		U,
		I,
		O,
		P,
		Z,
		X,
		C,
		V,
		B,
		N,
		M,
		LeftSquareBracket,
		RightSquareBracket,
		BackQuote,
		Backslash,
		Semicolon,
		SingleQuote,
		Comma,
		Dot,
		QuestionMark,
		Escape,
		Delete,
		Tab,
		Enter,
		Control,
		ControlLeft,
		ControlRight,
		Shift,
		ShiftLeft,
		ShiftRight,
		Space,
		Backspace,
		Capslock,
		Insert,
		Printscreen,
		End,
		Home,
		Menu,
		AltLeft,
		AltRight,
	}
)

type Keyboard struct {
	Keys sync.Map
}

func (k *Keyboard) GetKeyState(key Key) (state bool) {
	state = makc.GetKeyState(string(key))
	k.Keys.Store(key, state)
	return
}
