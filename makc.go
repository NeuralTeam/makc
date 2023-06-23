package makc

// #include "makc_c.h"
import "C"
import (
	_ "embed"
	"errors"
	"github.com/NeuralTeam/kernel"
	"github.com/NeuralTeam/kernel/pkg/dll"
	"github.com/jchv/go-winloader"
	"log"
	"strings"
	"unsafe"
)

/*
* _  _  ___   ___  _  __
*| || |/ _ \ / _ \| |/ /
*| __ | |_| | |_| | ' |
*|_||_|\___/ \___/|_|\_\
 */

//go:embed pkg/skip_hook/Release/skip_hook.dll
var skipHookBytes []byte
var skipHookModule winloader.Module

// Initialize initializes everything and creates hooks for handling mouse and keyboard events
//
// # Returns true if successful, otherwise false
//
// IMPORTANT: it is undesirable to call this function twice.
// This may lead to unforeseen consequences
func Initialize() (result bool, err error) {
	k, err := kernel.New(dll.New("user32.dll"))
	if err != nil {
		err = errors.New("user32.dll could not be loaded")
		return
	}

	if skipHookModule, err = winloader.LoadFromMemory(skipHookBytes); err != nil {
		err = errors.New("skip_hook.dll could not be loaded")
		return
	}
	h := skipHookModule.Proc("?SkipHook@@YAPEAE_K@Z").Addr()
	if h == 0 {
		err = errors.New("skip_hook.dll is missing required procedure")
		return
	}

	skipHook := *(*C.FARPROC)(unsafe.Pointer(&h))
	mouseInput := *(*C.FARPROC)(unsafe.Pointer(&k.NewProc("InjectMouseInput").Address))
	result = bool(C.CreateMouseAndKeyboardHook(
		C.bool(true), C.bool(true),
		mouseInput, skipHook),
	)
	return
}

// Terminate clears the memory, stops created hooks
//
// NOTE: it is advisable to call this when exiting the program
func Terminate() {
	C.StopMouseAndKeyboardHook()
	if err := skipHookModule.Free(); err != nil {
		log.Fatalln("skip_hook.dll is unable to free")
	}
}

/*
* __  __  ___  _   _ ___  ___
*|  \/  |/ _ \| | | / __|| __|
*| |\/| | |_| | | | \__ \| _|
*|_|  |_|\___/ \___/|___/|___|
 */

// Mouse event names
const (
	LeftButtonDown = "leftbuttondown"
	LeftButtonUp   = "leftbuttonup"

	RightButtonDown = "rightbuttondown"
	RightButtonUp   = "rightbuttonup"

	MiddleButtonDown = "middlebuttondown"
	MiddleButtonUp   = "middlebuttonup"

	SideButtonDown = "sidebuttondown"
	SideButtonUp   = "sidebuttonup"
)

// mouseEventCodeByName contains fields with values from C
var mouseEventCodeByName = map[string]C.MOUSEEVENTCODE{
	LeftButtonDown: C.button_LeftButtonDown,
	LeftButtonUp:   C.button_LeftButtonUp,

	RightButtonDown: C.button_RightButtonDown,
	RightButtonUp:   C.button_RightButtonUp,

	MiddleButtonDown: C.button_MiddleButtonDown,
	MiddleButtonUp:   C.button_MiddleButtonUp,

	SideButtonDown: C.button_SideButtonDown,
	SideButtonUp:   C.button_SideButtonUp,
}

// MouseEvents contains all mouse event names
var MouseEvents = []string{
	LeftButtonDown,
	LeftButtonUp,

	RightButtonDown,
	RightButtonUp,

	MiddleButtonDown,
	MiddleButtonUp,

	SideButtonDown,
	SideButtonUp,
}

// GetMouseEventState returns a boolean value by the specified mouse event name
//
// Returns true if the event occurred, otherwise false
func GetMouseEventState(event string) (state bool) {
	var k []string
	k = append(k, event, strings.ToLower(event))
	for _, e := range k {
		if result, ok := mouseEventCodeByName[e]; ok {
			state = bool(C.GetMouseEventStateByCode(result))
			break
		}
	}
	return
}

// GetFirstMouseEvent tracks and returns the first mouse event name that occurred
//
// NOTE: this is a loop that runs until a mouse event occurs
func GetFirstMouseEvent() (event string) {
	for event == "" {
		for _, e := range MouseEvents {
			if GetMouseEventState(e) ||
				GetMouseEventState(strings.ToLower(e)) {
				event = e
				break
			}
		}
	}
	return
}

// GetMousePos returns the mouse position by X and Y
func GetMousePos() (x, y int) {
	return int(C.GetMousePositionX()), int(C.GetMousePositionY())
}

// GetMouseSpeed returns the OS mouse speed
func GetMouseSpeed() (speed int) {
	return int(C.GetMouseSpeed())
}

// MoveMouse moves the mouse to the specified position
//
// # If the relative value is false, the mouse sets to the specified coordinates
//
// If the relative value is true, the mouse moves relative to the specified coordinates
func MoveMouse(posX int, posY int, relative bool) {
	go C.MouseMove(C.int(posX), C.int(posY), C.bool(relative))
}

// MoveMouseHumanly moves the mouse relative to the specified coordinates
// with a special algorithm of "human" mouse movement
func MoveMouseHumanly(posX int, posY int) {
	go C.MouseMoveHumanly(C.int(posX), C.int(posY))
}

// LeftMouseButtonClick clicks the left mouse button
func LeftMouseButtonClick() {
	go C.MouseLeftButtonClick()
}

// RightMouseButtonClick clicks the right mouse button
func RightMouseButtonClick() {
	go C.MouseRightButtonClick()
}

// MiddleMouseButtonClick clicks the middle mouse button
func MiddleMouseButtonClick() {
	go C.MouseMiddleButtonClick()
}

/*
* _  _  __ _   _ ___  ___   _   ___ ___
*| |/ / __\ \ / / _ )/ _ \ /_\ | _ \   \
*|   |  _| \ V /| _ \ |_| / _ \|   / |) |
*|_|\_\___| |_| |___/\___/_/ \_\_|_\___/
 */

// Keyboard key names
const (
	Key1      = "1"
	Key2      = "2"
	Key3      = "3"
	Key4      = "4"
	Key5      = "5"
	Key6      = "6"
	Key7      = "7"
	Key8      = "8"
	Key9      = "9"
	Key0      = "0"
	KeyMinus  = "-"
	KeyEquals = "="

	KeyF1  = "f1"
	KeyF2  = "f2"
	KeyF3  = "f3"
	KeyF4  = "f4"
	KeyF5  = "f5"
	KeyF6  = "f6"
	KeyF7  = "f7"
	KeyF8  = "f8"
	KeyF9  = "f9"
	KeyF10 = "f10"
	KeyF11 = "f11"
	KeyF12 = "f12"

	KeyA = "a"
	KeyS = "s"
	KeyD = "d"
	KeyF = "f"
	KeyG = "g"
	KeyH = "h"
	KeyJ = "j"
	KeyK = "k"
	KeyL = "l"
	KeyQ = "q"
	KeyW = "w"
	KeyE = "e"
	KeyR = "r"
	KeyT = "t"
	KeyY = "y"
	KeyU = "u"
	KeyI = "i"
	KeyO = "o"
	KeyP = "p"
	KeyZ = "z"
	KeyX = "x"
	KeyC = "c"
	KeyV = "v"
	KeyB = "b"
	KeyN = "n"
	KeyM = "m"

	KeyLeftSquareBracket  = "["
	KeyRightSquareBracket = "]"
	KeyBackQuote          = "`"
	KeyBackslash          = "\\"

	KeySemicolon    = ";"
	KeySingleQuote  = "'"
	KeyComma        = ","
	KeyDot          = "."
	KeyQuestionMark = "?"

	KeyEscape       = "escape"
	KeyDelete       = "delete"
	KeyTab          = "tab"
	KeyEnter        = "enter"
	KeyControl      = "control"
	KeyControlLeft  = "controlleft"
	KeyControlRight = "controlright"
	KeyShift        = "shift"
	KeyShiftLeft    = "shiftleft"
	KeyShiftRight   = "shiftright"
	KeySpace        = "space"
	KeyBackspace    = "backspace"
	KeyCapslock     = "capslock"
	KeyInsert       = "insert"
	KeyPrintscreen  = "printscreen"
	KeyEnd          = "end"
	KeyHome         = "home"
	KeyMenu         = "menu"
	KeyAltLeft      = "altleft"
	KeyAltRight     = "altright"
)

// keyboardKeyCodeByName contains fields with values from C
var keyboardKeyCodeByName = map[string]C.KEYBOARDKEYCODE{
	Key1:      C.key_1,
	Key2:      C.key_2,
	Key3:      C.key_3,
	Key4:      C.key_4,
	Key5:      C.key_5,
	Key6:      C.key_6,
	Key7:      C.key_7,
	Key8:      C.key_8,
	Key9:      C.key_9,
	Key0:      C.key_0,
	KeyMinus:  C.key_MinusAndUnderscore,
	KeyEquals: C.key_EqualsAndPlus,

	KeyF1:  C.key_F1,
	KeyF2:  C.key_F2,
	KeyF3:  C.key_F3,
	KeyF4:  C.key_F4,
	KeyF5:  C.key_F5,
	KeyF6:  C.key_F6,
	KeyF7:  C.key_F7,
	KeyF8:  C.key_F8,
	KeyF9:  C.key_F9,
	KeyF10: C.key_F10,
	KeyF11: C.key_F11,
	KeyF12: C.key_F12,

	KeyA: C.key_A,
	KeyS: C.key_S,
	KeyD: C.key_D,
	KeyF: C.key_F,
	KeyG: C.key_G,
	KeyH: C.key_H,
	KeyJ: C.key_J,
	KeyK: C.key_K,
	KeyL: C.key_L,
	KeyQ: C.key_Q,
	KeyW: C.key_W,
	KeyE: C.key_E,
	KeyR: C.key_R,
	KeyT: C.key_T,
	KeyY: C.key_Y,
	KeyU: C.key_U,
	KeyI: C.key_I,
	KeyO: C.key_O,
	KeyP: C.key_P,
	KeyZ: C.key_Z,
	KeyX: C.key_X,
	KeyC: C.key_C,
	KeyV: C.key_V,
	KeyB: C.key_B,
	KeyN: C.key_N,
	KeyM: C.key_M,

	KeyLeftSquareBracket:  C.key_LeftSquareBracketAndLeftCurlyBrace,
	KeyRightSquareBracket: C.key_RightSquareBracketAndRightCurlyBrace,
	KeyBackQuote:          C.key_BackQuoteAndTilde,
	KeyBackslash:          C.key_BackslashAndVerticalLine,

	KeySemicolon:    C.key_Semicolon,
	KeySingleQuote:  C.key_SingleQuoteAndDoubleQuote,
	KeyComma:        C.key_Comma,
	KeyDot:          C.key_Dot,
	KeyQuestionMark: C.key_QuestionMark,

	KeyEscape:       C.key_Esc,
	KeyDelete:       C.key_Delete,
	KeyTab:          C.key_Tab,
	KeyEnter:        C.key_Enter,
	KeyControl:      C.key_Control,
	KeyControlLeft:  C.key_LeftControl,
	KeyControlRight: C.key_RightControl,
	KeyShift:        C.key_Shift,
	KeyShiftLeft:    C.key_LeftShift,
	KeyShiftRight:   C.key_RightShift,
	KeySpace:        C.key_Space,
	KeyBackspace:    C.key_Backspace,
	KeyCapslock:     C.key_Capslock,
	KeyInsert:       C.key_Insert,
	KeyPrintscreen:  C.key_PrintScreen,
	KeyEnd:          C.key_End,
	KeyHome:         C.key_Home,
	KeyMenu:         C.key_Alt,
	KeyAltLeft:      C.key_LeftAlt,
	KeyAltRight:     C.key_RightAlt,
}

// KeyboardKeys contains all keyboard key names
var KeyboardKeys = []string{
	Key1,
	Key2,
	Key3,
	Key4,
	Key5,
	Key6,
	Key7,
	Key8,
	Key9,
	Key0,
	KeyMinus,
	KeyEquals,

	KeyF1,
	KeyF2,
	KeyF3,
	KeyF4,
	KeyF5,
	KeyF6,
	KeyF7,
	KeyF8,
	KeyF9,
	KeyF10,
	KeyF11,
	KeyF12,

	KeyA,
	KeyS,
	KeyD,
	KeyF,
	KeyG,
	KeyH,
	KeyJ,
	KeyK,
	KeyL,
	KeyQ,
	KeyW,
	KeyE,
	KeyR,
	KeyT,
	KeyY,
	KeyU,
	KeyI,
	KeyO,
	KeyP,
	KeyZ,
	KeyX,
	KeyC,
	KeyV,
	KeyB,
	KeyN,
	KeyM,

	KeyLeftSquareBracket,
	KeyRightSquareBracket,
	KeyBackQuote,
	KeyBackslash,

	KeySemicolon,
	KeySingleQuote,
	KeyComma,
	KeyDot,
	KeyQuestionMark,

	KeyEscape,
	KeyDelete,
	KeyTab,
	KeyEnter,
	KeyControl,
	KeyControlLeft,
	KeyControlRight,
	KeyShift,
	KeyShiftLeft,
	KeyShiftRight,
	KeySpace,
	KeyBackspace,
	KeyCapslock,
	KeyInsert,
	KeyPrintscreen,
	KeyEnd,
	KeyHome,
	KeyMenu,
	KeyAltLeft,
	KeyAltRight,
}

// GetKeyState returns a boolean value by the specified keyboard key name
//
// Returns true if the key is pressed, otherwise false
func GetKeyState(key string) (state bool) {
	var k []string
	k = append(k, key, strings.ToLower(key))
	for _, e := range k {
		if result, ok := keyboardKeyCodeByName[e]; ok {
			state = bool(C.GetKeyboardKeyStateByNumber(C.AddKeyboardKeyToHandle(result)))
			break
		}
	}
	return
}

// GetFirstKey tracks and returns the first keyboard key pressed
//
// NOTE: this is a loop that runs until the key is pressed
func GetFirstKey() (key string) {
	for key == "" {
		for _, e := range KeyboardKeys {
			if GetKeyState(e) ||
				GetKeyState(strings.ToLower(e)) {
				key = e
				break
			}
		}
	}
	C.DeleteAllAddedKeyboardKeys()
	return
}
