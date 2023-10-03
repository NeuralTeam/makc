/*
*Unauthorized copying of this file, via any medium is strictly prohibited
*Proprietary and confidential
*Written by Maksim Akhmetdinov <aivaki@vk.com>, February 2023
*
*Low-level mouse and keyboard control for Windows
*/


// IS_WINDOWS checks if we are in Windows
//
#if !defined(IS_WINDOWS) && (defined(WIN32) || defined(_WIN32) || \
    defined(__WIN32__) || defined(__WINDOWS__) || defined(__CYGWIN__))
#define IS_WINDOWS
#endif /* IS_WINDOWS */


#ifndef MAKC_C
#define MAKC_C
#if defined(IS_WINDOWS)

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4047)
#pragma warning(disable : 4028)
#pragma warning(disable : 4717)

#include <Windows.h>
#include <process.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>

#pragma region Low level hook flags

#if (_WIN32_WINNT >= 0x0400)

/* Low level hook flags */
#define WH_KEYBOARD_LL					13
#define WH_MOUSE_LL						14


/* LLKHF and LLMHF injected flags */
#define LLKHF_INJECTED					0x00000010
#define LLKHF_LOWER_IL_INJECTED			0x00000002

#define LLMHF_INJECTED					0x00000001
#define LLMHF_LOWER_IL_INJECTED			0x00000002

#endif /* _WIN32_WINNT >= 0x0400 */


/* Keyboard flags */
#define INPUT_KEYBOARD					1

#define WM_KEYUP						0x0101
#define WM_SYSKEYUP						0x0105

#define WM_KEYDOWN						0x0100
#define WM_SYSKEYDOWN					0x0104


/* Mouse flags */
#define INPUT_MOUSE						0

#define MOUSEEVENTF_MOVE				0x0001
#define MOUSEEVENTF_MOVE_NOCOALESCE     0x2000
#define MOUSEEVENTF_LEFTDOWN			0x0002
#define MOUSEEVENTF_LEFTUP				0x0004
#define MOUSEEVENTF_RIGHTDOWN			0x0008
#define MOUSEEVENTF_RIGHTUP				0x0010
#define MOUSEEVENTF_MIDDLEDOWN			0x0020
#define MOUSEEVENTF_MIDDLEUP			0x0040
#define MOUSEEVENTF_XDOWN				0x0080
#define MOUSEEVENTF_XUP					0x0100
#define MOUSEEVENTF_WHEEL               0x0800
#define MOUSEEVENTF_HWHEEL              0x01000
#define MOUSEEVENTF_ABSOLUTE            0x8000
#define MOUSEEVENTF_VIRTUALDESK         0x4000

#define SPI_GETMOUSESPEED				0x0070

#define WM_LBUTTONDOWN                  0x0201
#define WM_LBUTTONUP                    0x0202
#define WM_RBUTTONDOWN                  0x0204
#define WM_RBUTTONUP                    0x0205
#define WM_MBUTTONDOWN                  0x0207
#define WM_MBUTTONUP                    0x0208

#if (_WIN32_WINNT >= 0x0500)
#define WM_XBUTTONDOWN                  0x020B
#define WM_XBUTTONUP                    0x020C

#define XBUTTON							131072
#endif /* _WIN32_WINNT >= 0x0500 */


/* Screen flags */
#define SM_CXSCREEN						0
#define SM_CYSCREEN						1


/* Other flags */
#define WM_INPUT                        0x00FF

#pragma endregion


#pragma region Local global variables

// MOUSE_EVENT_CODE is enum of the WM codes of mouse button events
//
typedef enum _MOUSEEVENTCODE {
	button_LeftButtonDown = WM_LBUTTONDOWN,
	button_LeftButtonUp = WM_LBUTTONUP,
	//
	button_RightButtonDown = WM_RBUTTONDOWN,
	button_RightButtonUp = WM_RBUTTONUP,
	//
	button_MiddleButtonDown = WM_MBUTTONDOWN,
	button_MiddleButtonUp = WM_MBUTTONUP,
	//
	button_SideButtonDown = WM_XBUTTONDOWN,
	button_SideButtonUp = WM_XBUTTONUP,
} MOUSEEVENTCODE;

// KEYCODE is enum of key VK codes (check KBDLLHOOKSTRUCT)
//
typedef enum _KEYBOARDKEYCODE {
	key_NULL = 0x00, /* Don't use! */
	//
	key_1 = 0x31,
	key_2 = 0x32,
	key_3 = 0x33,
	key_4 = 0x34,
	key_5 = 0x35,
	key_6 = 0x36,
	key_7 = 0x37,
	key_8 = 0x38,
	key_9 = 0x39,
	key_0 = 0x30,
	key_MinusAndUnderscore = VK_OEM_MINUS,
	key_EqualsAndPlus = VK_OEM_PLUS,
	//
	key_F1 = VK_F1,
	key_F2 = VK_F2,
	key_F3 = VK_F3,
	key_F4 = VK_F4,
	key_F5 = VK_F5,
	key_F6 = VK_F6,
	key_F7 = VK_F7,
	key_F8 = VK_F8,
	key_F9 = VK_F9,
	key_F10 = VK_F10,
	key_F11 = VK_F11,
	key_F12 = VK_F12,
	//
	key_A = 0x41,
	key_S = 0x53,
	key_D = 0x44,
	key_F = 0x46,
	key_G = 0x47,
	key_H = 0x48,
	key_J = 0x4A,
	key_K = 0x4B,
	key_L = 0x4C,
	key_Q = 0x51,
	key_W = 0x57,
	key_E = 0x45,
	key_R = 0x52,
	key_T = 0x54,
	key_Y = 0x59,
	key_U = 0x55,
	key_I = 0x49,
	key_O = 0x4F,
	key_P = 0x50,
	key_Z = 0x5A,
	key_X = 0x58,
	key_C = 0x43,
	key_V = 0x56,
	key_B = 0x42,
	key_N = 0x4E,
	key_M = 0x4D,
	//
	key_LeftSquareBracketAndLeftCurlyBrace = VK_OEM_4,
	key_RightSquareBracketAndRightCurlyBrace = VK_OEM_6,
	key_BackQuoteAndTilde = VK_OEM_3,
	key_BackslashAndVerticalLine = VK_OEM_5,
	//
	key_Semicolon = VK_OEM_1,
	key_SingleQuoteAndDoubleQuote = VK_OEM_7,
	key_Comma = VK_OEM_COMMA,
	key_Dot = VK_OEM_PERIOD,
	key_QuestionMark = VK_OEM_2,
	//
	key_Esc = VK_ESCAPE,
	key_Delete = VK_DELETE,
	key_Tab = VK_TAB,
	key_Enter = VK_RETURN,
	key_Control = VK_CONTROL,
	key_LeftControl = VK_LCONTROL,
	key_RightControl = VK_RCONTROL,
	key_Shift = VK_SHIFT,
	key_LeftShift = VK_LSHIFT,
	key_RightShift = VK_RSHIFT,
	key_Space = VK_SPACE,
	key_Backspace = VK_BACK,
	key_Capslock = VK_CAPITAL,
	key_Insert = VK_INSERT,
	key_PrintScreen = VK_SNAPSHOT,
	key_End = VK_END,
	key_Home = VK_HOME,
	key_Alt = VK_MENU,
	key_LeftAlt = VK_LMENU,
	key_RightAlt = VK_RMENU,
} KEYBOARDKEYCODE;


typedef enum _INJECTEDINPUTMOUSEOPTIONS {
	_none = 0,
	_move = MOUSEEVENTF_MOVE,
	_moveNoCoalesce = MOUSEEVENTF_MOVE_NOCOALESCE,
	_virtualDesk = MOUSEEVENTF_VIRTUALDESK,
	_absolute = MOUSEEVENTF_ABSOLUTE,
	_leftDown = MOUSEEVENTF_LEFTDOWN,
	_leftUp = MOUSEEVENTF_LEFTUP,
	_rightDown = MOUSEEVENTF_RIGHTDOWN,
	_rightUp = MOUSEEVENTF_RIGHTUP,
	_middleDown = MOUSEEVENTF_MIDDLEDOWN,
	_middleUp = MOUSEEVENTF_MIDDLEUP,
	_xDown = MOUSEEVENTF_XDOWN,
	_xUp = MOUSEEVENTF_XUP,
	_wheel = MOUSEEVENTF_WHEEL,
	_hWheel = MOUSEEVENTF_HWHEEL
} INJECTEDINPUTMOUSEOPTIONS;

typedef struct _INJECTEDINPUTMOUSE {
	int deltaX, deltaY;
	unsigned int mouseData;
	INJECTEDINPUTMOUSEOPTIONS dwFlags;
	unsigned int time;
	void* dwExtraInfo;
} INJECTEDINPUTMOUSE;

typedef struct _MOUSE {
	/* Data for export */
	int MousePosX, MousePosY;
	int MouseSpeed;

	/* Low-level data */
	struct {
		struct {
			HHOOK hMouseHook;
			BOOL mouseHookActive;
			MSLLHOOKSTRUCT mouseStruct;
		} HOOK;

		struct {
			BOOL mouseInputActive;
			POINT mouseLastMovement;
			INJECTEDINPUTMOUSE mouseInput;
			FARPROC injectMouseInputProc;
		} INPUT;

		struct {
			// Mouse left and right buttons
			BOOL mouseLeftButtonDownEvent, mouseRightButtonDownEvent;
			BOOL mouseLeftButtonUpEvent, mouseRightButtonUpEvent;
			// Mouse side buttons
			BOOL mouseSideButton1DownEvent, mouseSideButton1UpEvent;
			BOOL mouseSideButton2DownEvent, mouseSideButton2UpEvent;
			// Mouse middle button
			BOOL mouseMiddleButtonDownEvent, mouseMiddleButtonUpEvent;
		} BUTTONS;
	} LLDATA;
} MOUSE;
/* Global MOUSE */
static MOUSE mouseData;


typedef struct _KEYBOARDKEYS {
	// Low-level hook data
	KBDLLHOOKSTRUCT keyboardStruct;
	// Key event
	WPARAM keyboardEvent;
	// Added key
	KEYBOARDKEYCODE keyboardKey;
	// State of the added key
	BOOL keyboardKeyActive;
} KEYBOARDKEYS;

typedef struct {
	/* Only low-level data */
	struct {
		// Global hook state
		BOOL keyboardHookActive;
		// Global hook
		HHOOK hKeyboardHook;

		// Data for send input
		INPUT keyboardInput;

		// All keys
		KEYBOARDKEYS keyboardKeys[256];
		// Number of keys added
		INT32 keyboardNumberOfKeys;
	} LLDATA;
} KEYBOARD;
/* Global KEYBOARD */
static KEYBOARD keyboardData;


typedef struct _SCREEN {
	// Screen size
	INT32 screenWidth, screenHeight;
} SCREEN;
/* Global SCREEN */
SCREEN screenData;


typedef struct _GMSG {
	// MSG for receiving system messages
	MSG msg;
	// MSG state
	BOOL msgActive;
} GMSG;
/* Global GMSG */
static GMSG gMsg;


typedef void(__cdecl* _beginthread_proc_type)(void*);

typedef UINT8  uint8_t;
typedef uint8_t* uint8_t_ptr;
typedef UINT64 uint64_t;

typedef uint8_t_ptr(__stdcall* SKIPHOOK)(uint64_t address);
typedef bool(__stdcall* INJECTMOUSEINPUT)(INJECTEDINPUTMOUSE* inputs, int count);
typedef LRESULT(__stdcall* CALLNEXTHOOKEX)(HHOOK hhk, int nCode, WPARAM wParam, LPARAM lParam);
typedef BOOL(__stdcall* UNHOOKWINDOWSHOOKEX)(HHOOK hhk);
typedef HHOOK(__stdcall* SETWINDOWSHOOKEX)(int idHook, HOOKPROC lpfn, HINSTANCE hmod, DWORD dwThreadId);
typedef BOOL(__stdcall* GETMESSAGE)(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax);
typedef BOOL(__stdcall* TRANSLATEMESSAGE)(const MSG* lpMsg);
typedef LRESULT(__stdcall* DISPATCHMESSAGE)(const MSG* lpMsg);
typedef BOOL(__stdcall* POSTMESSAGE)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
typedef int(__stdcall* GETSYSTEMMETRICS)(int nIndex);

typedef struct __SKIPHOOK {
	FARPROC skipHookProc;
	SKIPHOOK skipHook;
	INJECTMOUSEINPUT injectMouseInput;
	CALLNEXTHOOKEX callNextHookEx;
	UNHOOKWINDOWSHOOKEX unhookWindowsHookEx;
	SETWINDOWSHOOKEX setWindowsHookEx;
	GETMESSAGE getMessage;
	TRANSLATEMESSAGE translateMessage;
	DISPATCHMESSAGE dispatchMessage;
	POSTMESSAGE postMessage;
	GETSYSTEMMETRICS getSystemMetrics;
} _SKIPHOOK;
/* Global SKIPHOOK */
static _SKIPHOOK skipHook;

#pragma endregion


#pragma region Local functions

// _DEBUG_PRINTF displays debug information
//
#ifdef _DEBUG
#define _DEBUG_PRINTF(p, i) printf(p, i)
#else
#define _DEBUG_PRINTF(p, i)
#endif

// generateRandomValue returns a random number up to the specified maximum
//
int __fastcall generateRandomValue(int min_value, int max_value) {
	if (max_value == 0) {
		return 0;
	}
	srand((unsigned int)time(NULL));
	int random = rand() % max_value;
	if (random < min_value) {
		random = min_value;
	}
	return random;
}

/*
* _  _  ___   ___  _  __
*| || |/ _ \ / _ \| |/ /
*| __ | |_| | |_| |   |
*|_||_|\___/ \___/|_|\_\
 */

INJECTMOUSEINPUT InjectMouseInput(INJECTEDINPUTMOUSE* inputs, int count) {
	INJECTMOUSEINPUT inject_mouse_input = (INJECTMOUSEINPUT)mouseData.LLDATA.INPUT.injectMouseInputProc;
	return (INJECTMOUSEINPUT)inject_mouse_input(inputs, count);
}

void __fastcall initSkipHook(FARPROC skip_hook) {
    skipHook.skipHookProc = skip_hook;
    //
    skipHook.skipHook = (SKIPHOOK)skipHook.skipHookProc;
	skipHook.injectMouseInput = (INJECTMOUSEINPUT)skipHook.skipHook((uint64_t)InjectMouseInput);
	skipHook.callNextHookEx = (CALLNEXTHOOKEX)skipHook.skipHook((uint64_t)CallNextHookEx);
	skipHook.unhookWindowsHookEx = (UNHOOKWINDOWSHOOKEX)skipHook.skipHook((uint64_t)UnhookWindowsHookEx);
	skipHook.setWindowsHookEx = (SETWINDOWSHOOKEX)skipHook.skipHook((uint64_t)SetWindowsHookEx);
	skipHook.getMessage = (GETMESSAGE)skipHook.skipHook((uint64_t)GetMessage);
	skipHook.translateMessage = (TRANSLATEMESSAGE)skipHook.skipHook((uint64_t)TranslateMessage);
	skipHook.dispatchMessage = (DISPATCHMESSAGE)skipHook.skipHook((uint64_t)DispatchMessage);
	skipHook.postMessage = (POSTMESSAGE)skipHook.skipHook((uint64_t)PostMessage);
	skipHook.getSystemMetrics = (GETSYSTEMMETRICS)skipHook.skipHook((uint64_t)GetSystemMetrics);
}

 // handleMouseEvents handles mouse event
 //
void __fastcall handleMouseEvents(WPARAM mouse_event) {
	switch (mouse_event) {

	case WM_LBUTTONDOWN: /* Left mouse button down */
		mouseData.LLDATA.BUTTONS.mouseLeftButtonDownEvent = true;
		Sleep(50);
		mouseData.LLDATA.BUTTONS.mouseLeftButtonDownEvent = false;

		_DEBUG_PRINTF("\n%s", "Mouse hook: Left mouse button down");
		break;
	case WM_RBUTTONDOWN: /* Right mouse button down */
		mouseData.LLDATA.BUTTONS.mouseRightButtonDownEvent = true;
		Sleep(50);
		mouseData.LLDATA.BUTTONS.mouseRightButtonDownEvent = false;

		_DEBUG_PRINTF("\n%s", "Mouse hook: Right mouse button down");
		break;


	case WM_LBUTTONUP: /* Left mouse button up */
		mouseData.LLDATA.BUTTONS.mouseLeftButtonUpEvent = true;
		Sleep(50);
		mouseData.LLDATA.BUTTONS.mouseLeftButtonUpEvent = false;

		_DEBUG_PRINTF("\n%s", "Mouse hook: Left mouse button up");
		break;
	case WM_RBUTTONUP: /* Right mouse button up */
		mouseData.LLDATA.BUTTONS.mouseRightButtonUpEvent = true;
		Sleep(50);
		mouseData.LLDATA.BUTTONS.mouseRightButtonUpEvent = false;

		_DEBUG_PRINTF("\n%s", "Mouse hook: Right mouse button up");
		break;


	case WM_MBUTTONDOWN: /* Middle mouse button down */
		mouseData.LLDATA.BUTTONS.mouseMiddleButtonDownEvent = true;
		Sleep(50);
		mouseData.LLDATA.BUTTONS.mouseMiddleButtonDownEvent = false;

		_DEBUG_PRINTF("\n%s", "Mouse hook: Middle mouse button down");
		break;
	case WM_MBUTTONUP: /* Middle mouse button up */
		mouseData.LLDATA.BUTTONS.mouseMiddleButtonUpEvent = true;
		Sleep(50);
		mouseData.LLDATA.BUTTONS.mouseMiddleButtonUpEvent = false;

		_DEBUG_PRINTF("\n%s", "Mouse hook: Middle mouse button up");
		break;


	case WM_XBUTTONDOWN: /* Side mouse buttons down */
		/* XBUTTON1 */
		if (mouseData.LLDATA.HOOK.mouseStruct.mouseData == (DWORD)XBUTTON) {
			mouseData.LLDATA.BUTTONS.mouseSideButton1DownEvent = true;
			Sleep(50);
			mouseData.LLDATA.BUTTONS.mouseSideButton1DownEvent = false;
		}
		/* XBUTTON2 or other */
		else {
			mouseData.LLDATA.BUTTONS.mouseSideButton2DownEvent = true;
			Sleep(50);
			mouseData.LLDATA.BUTTONS.mouseSideButton2DownEvent = false;
		}

		_DEBUG_PRINTF("\n%s", "Mouse hook: Side mouse button down");
		break;
	case WM_XBUTTONUP: /* Side mouse buttons up */
		/* XBUTTON1 */
		if (mouseData.LLDATA.HOOK.mouseStruct.mouseData == (DWORD)XBUTTON) {
			mouseData.LLDATA.BUTTONS.mouseSideButton1UpEvent = true;
			Sleep(50);
			mouseData.LLDATA.BUTTONS.mouseSideButton1UpEvent = false;
		}
		/* XBUTTON2 or other */
		else {
			mouseData.LLDATA.BUTTONS.mouseSideButton2UpEvent = true;
			Sleep(50);
			mouseData.LLDATA.BUTTONS.mouseSideButton2UpEvent = false;
		}

		_DEBUG_PRINTF("\n%s", "Mouse hook: Side mouse button up");
		break;

		/* There is no WM_MOUSEMOVE and WM_MOUSEWHEEL because the resource consumption is too high */

	default:
		break;
	}

	_endthread();
}

// handleKeyboardEvents handles keyboard events
//
void __fastcall handleKeyboardEvents(KEYBOARDKEYCODE vk_key) {
	int key_number;
	for (key_number = 0; key_number < keyboardData.LLDATA.keyboardNumberOfKeys + 1; key_number++) {
		if (keyboardData.LLDATA.keyboardKeys[key_number].keyboardStruct.vkCode == (DWORD)vk_key
			&& keyboardData.LLDATA.keyboardKeys[key_number].keyboardStruct.vkCode == (DWORD)keyboardData.LLDATA.keyboardKeys[key_number].keyboardKey) {

			switch (keyboardData.LLDATA.keyboardKeys[key_number].keyboardEvent) {

			case WM_KEYDOWN: case WM_SYSKEYDOWN: /* Keyboard key down */
				if (keyboardData.LLDATA.keyboardKeys[key_number].keyboardKeyActive == FALSE) {
					keyboardData.LLDATA.keyboardKeys[key_number].keyboardKeyActive = TRUE;
				}

				_DEBUG_PRINTF("\nKeyboard hook: (bool) State of the key: %d", keyboardData.LLDATA.keyboardKeys[key_number].keyboardKeyActive);
				_DEBUG_PRINTF("\nKeyboard hook: (vk code) Current key down: %d", keyboardData.LLDATA.keyboardKeys[key_number].keyboardStruct.vkCode);
				break;


			case WM_KEYUP: case WM_SYSKEYUP: /* Keyboard key up */
				if (keyboardData.LLDATA.keyboardKeys[key_number].keyboardKeyActive == TRUE) {
					keyboardData.LLDATA.keyboardKeys[key_number].keyboardKeyActive = FALSE;
				}

				_DEBUG_PRINTF("\nKeyboard hook: (bool) State of the key: %d", keyboardData.LLDATA.keyboardKeys[key_number].keyboardKeyActive);
				_DEBUG_PRINTF("\nKeyboard hook: (vk code) Current key up: %d", keyboardData.LLDATA.keyboardKeys[key_number].keyboardStruct.vkCode);
				break;


			default:
				break;
			}
		}
	}

	_endthread();
}

// lowLevelMouseHookCallback catches and processes mouse flags
//
// Returns CallNextHookEx for mouse hook create
//
LRESULT CALLBACK lowLevelMouseHookCallback(int n_code, WPARAM w_param, MSLLHOOKSTRUCT* l_param) {
	/* Removing 'unnecessary' flags */
	if ((l_param->flags & LLMHF_INJECTED) != 0) {
		l_param->flags &= ~LLMHF_INJECTED;
	}
	if ((l_param->flags & LLMHF_LOWER_IL_INJECTED) != 0) {
		l_param->flags &= ~LLMHF_LOWER_IL_INJECTED;
	}
	mouseData.LLDATA.HOOK.mouseStruct.flags = l_param->flags;
	mouseData.LLDATA.INPUT.mouseInput.mouseData = (unsigned int)((LPARAM)l_param);

	/* If n_code is less than zero, the hook procedure must pass the message to the CallNextHookEx
	function without further processing and should return the value returned by CallNextHookEx */
	if (n_code < 0) {
		return skipHook.callNextHookEx(NULL, n_code, w_param, (int)((LPARAM)l_param));
	}

	/* WM_MOUSEMOVE and WM_MOUSEWHEEL consume too many resources, so they also immediately return CallNextHookEx */
	if (w_param == WM_MOUSEMOVE || w_param == WM_MOUSEWHEEL) {
		/* Set mouse X and Y position immediately */
		if ((int)l_param->pt.x < 0) {
			l_param->pt.x = 0;
		}
		if ((int)l_param->pt.y < 0) {
			l_param->pt.y = 0;
		}
		mouseData.MousePosX = (int)l_param->pt.x;
		mouseData.MousePosY = (int)l_param->pt.y;

		return skipHook.callNextHookEx(NULL, n_code, w_param, (int)((LPARAM)l_param));
	}

	/* Writing data and creating the thread */
	mouseData.LLDATA.HOOK.mouseStruct = *l_param;
	_beginthread((_beginthread_proc_type)handleMouseEvents, 0, (void*)w_param);

#if defined(_DEBUG)
	/* LLMHF_INJECTED and LLMHF_LOWER_IL_INJECTED flags detector */
	if (mouseData.LLDATA.HOOK.mouseStruct.flags & LLMHF_INJECTED
		|| mouseData.LLDATA.HOOK.mouseStruct.flags & LLMHF_LOWER_IL_INJECTED) {
		_DEBUG_PRINTF("\n%s", "Mouse hook: LLMHF_INJECTED or LLMHF_LOWER_IL_INJECTED flag detected");
	}
#endif

	return skipHook.callNextHookEx(NULL, n_code, w_param, (int)((LPARAM)l_param));
}

// lowLevelKeyboardHookCallback catches and processes keyboard flags
//
// Returns CallNextHookEx for keyboard hook create
//
LRESULT CALLBACK lowLevelKeyboardHookCallback(int n_code, WPARAM w_param, KBDLLHOOKSTRUCT* l_param) {
	/* Removing 'unnecessary' flags */
	if ((l_param->flags & LLKHF_INJECTED) != 0) {
		l_param->flags &= ~LLKHF_INJECTED;
	}
	if ((l_param->flags & LLKHF_LOWER_IL_INJECTED) != 0) {
		l_param->flags &= ~LLKHF_LOWER_IL_INJECTED;
	}

	/* If n_code is less than zero, the hook procedure must pass the message to the CallNextHookEx
	function without further processing and should return the value returned by CallNextHookEx */
	if (n_code < 0) {
		return skipHook.callNextHookEx(NULL, n_code, w_param, (int)((LPARAM)l_param));
	}

	/* Checking for correctness, writing data and creating the thread */
	int key_number;
	for (key_number = 0; key_number < keyboardData.LLDATA.keyboardNumberOfKeys + 1; key_number++) {
		if (l_param->vkCode == (DWORD)keyboardData.LLDATA.keyboardKeys[key_number].keyboardKey) {

			keyboardData.LLDATA.keyboardKeys[key_number].keyboardStruct = *l_param;
			keyboardData.LLDATA.keyboardKeys[key_number].keyboardEvent = w_param;

			_beginthread((_beginthread_proc_type)handleKeyboardEvents, 0, (void*)keyboardData.LLDATA.keyboardKeys[key_number].keyboardKey);

			break;
		}
	}

#if defined(_DEBUG)
	/* LLKHF_INJECTED and LLKHF_LOWER_IL_INJECTED flags detector */
	if (keyboardData.LLDATA.keyboardKeys[key_number].keyboardStruct.flags & LLKHF_INJECTED
		|| keyboardData.LLDATA.keyboardKeys[key_number].keyboardStruct.flags & LLKHF_LOWER_IL_INJECTED) {
		_DEBUG_PRINTF("\n%s", "Keyboard hook: LLKHF_INJECTED or LLKHF_LOWER_IL_INJECTED flag detected");
	}
#endif

	return skipHook.callNextHookEx(NULL, n_code, w_param, (int)((LPARAM)l_param));
}

// unhookWindowsHook calls the UnhookWindowsHookEx to unhook created hooks
//
// Returns true if unhook was executed, otherwise returns false
//
bool __fastcall unhookWindowsHook(void) {
	if (!mouseData.LLDATA.HOOK.hMouseHook && !keyboardData.LLDATA.hKeyboardHook) {
		return false;
	}

	if (mouseData.LLDATA.HOOK.hMouseHook) {
	    _beginthread((_beginthread_proc_type)skipHook.unhookWindowsHookEx, 0, (void*)mouseData.LLDATA.HOOK.hMouseHook);
	}
	if (keyboardData.LLDATA.hKeyboardHook) {
	    _beginthread((_beginthread_proc_type)skipHook.unhookWindowsHookEx, 0, (void*)keyboardData.LLDATA.hKeyboardHook);
	}
	return true;
}

// setWindowsMouseHook calls the SetWindowsHookEx to create mouse hook
//
// Returns true if a hook was created, otherwise returns false
//
bool __fastcall setWindowsMouseHook(void) {
	if (mouseData.LLDATA.HOOK.hMouseHook) {
		bool unhook = unhookWindowsHook();
		if (!unhook) {
			_DEBUG_PRINTF("\n%s", "Mouse hook: Failed to unhook");
			return false;
		}
	}

	mouseData.LLDATA.HOOK.hMouseHook = skipHook.setWindowsHookEx(WH_MOUSE_LL, (HOOKPROC)lowLevelMouseHookCallback, NULL, 0);
	if (!mouseData.LLDATA.HOOK.hMouseHook) {
		_DEBUG_PRINTF("\n%s", "Mouse hook: Failed to hook");
		return false;
	}
	return true;
}

// setWindowsKeyboardHook calls SetWindowsHookEx to create keyboard hook
//
// Returns true if a hook was created, otherwise returns false
//
bool __fastcall setWindowsKeyboardHook(void) {
	if (keyboardData.LLDATA.hKeyboardHook) {
		bool unhook = unhookWindowsHook();
		if (!unhook) {
			_DEBUG_PRINTF("\n%s", "Keyboard hook: Failed to unhook");
			return false;
		}
	}

	keyboardData.LLDATA.hKeyboardHook = skipHook.setWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)lowLevelKeyboardHookCallback, NULL, 0);
	if (!keyboardData.LLDATA.hKeyboardHook) {
		_DEBUG_PRINTF("\n%s", "Keyboard hook: Failed to hook");
		return false;
	}
	return true;
}

// getMessage calls the GetMessage to receive messages indefinitely
//
// (without this, hooks will not work, see docs.microsoft.com/windows/win32/winmsg/about-hooks)
//
void __fastcall getMessage(void) {
	BOOL msg_result;

	if (msg_result = skipHook.getMessage(&gMsg.msg, GetConsoleWindow(), WM_INPUT, WM_INPUT) != 0) {
		if (msg_result == -1) {
			_DEBUG_PRINTF("\n%s", "Input hook: The GetMessage function returned the value -1");
			return;
		}
		/* Translate virtual-key message into character message */
		skipHook.translateMessage(&gMsg.msg);
		/* Send message */
		skipHook.dispatchMessage(&gMsg.msg);
	}

	/* Thread termination */
	_endthread();
}

// interceptMouseAndKeyboardHook creates the necessary hooks and calls the getMessage
//
void __fastcall interceptMouseAndKeyboardHook(void) {
	/* Removing the active key states */
	int key_number;
	for (key_number = 0; key_number < keyboardData.LLDATA.keyboardNumberOfKeys + 1; key_number++) {
		if (keyboardData.LLDATA.keyboardKeys[key_number].keyboardKeyActive == TRUE) {
			keyboardData.LLDATA.keyboardKeys[key_number].keyboardKeyActive = FALSE;
		}
	}

	/* Removes the set hooks, which are now false */
	if (!mouseData.LLDATA.HOOK.mouseHookActive || !keyboardData.LLDATA.keyboardHookActive) {
		bool unhook = unhookWindowsHook();
		if (!unhook) {
			_DEBUG_PRINTF("\n%s", "Input hook: Some of the hooks are not running");
		}
	}

	/* Creating hooks */
	bool set = false;
	if (mouseData.LLDATA.HOOK.mouseHookActive) {
		set = setWindowsMouseHook();
		assert(set);
	}
	if (keyboardData.LLDATA.keyboardHookActive) {
		set = setWindowsKeyboardHook();
		assert(set);
	}

	/* End point */
	gMsg.msgActive = TRUE;
	getMessage();
}


// zeroMouseData resets all values of the structure
//
void __fastcall zeroMouseData(MOUSE* mouse_data) {
	mouse_data->MousePosX = 0;
	mouse_data->MousePosY = 0;
	mouse_data->MouseSpeed = 0;
	//
	mouse_data->LLDATA.HOOK.hMouseHook = NULL;
	//
	mouse_data->LLDATA.INPUT.mouseInput.deltaX = 0;
	mouse_data->LLDATA.INPUT.mouseInput.deltaY = 0;
	mouse_data->LLDATA.INPUT.mouseInput.dwExtraInfo = NULL;
	mouse_data->LLDATA.INPUT.mouseInput.dwFlags = _none;
	mouse_data->LLDATA.INPUT.mouseInput.mouseData = 0;
	mouse_data->LLDATA.INPUT.mouseInput.time = 0;
	//
	mouse_data->LLDATA.HOOK.mouseHookActive = FALSE;
	mouse_data->LLDATA.INPUT.mouseInputActive = FALSE;
	//
	mouse_data->LLDATA.INPUT.mouseLastMovement.x = 0;
	mouse_data->LLDATA.INPUT.mouseLastMovement.y = 0;
	//
	mouse_data->LLDATA.BUTTONS.mouseLeftButtonDownEvent = FALSE;
	mouse_data->LLDATA.BUTTONS.mouseLeftButtonUpEvent = FALSE;
	//
	mouse_data->LLDATA.BUTTONS.mouseMiddleButtonDownEvent = FALSE;
	mouse_data->LLDATA.BUTTONS.mouseMiddleButtonUpEvent = FALSE;
	//
	mouse_data->LLDATA.BUTTONS.mouseRightButtonDownEvent = FALSE;
	mouse_data->LLDATA.BUTTONS.mouseRightButtonUpEvent = FALSE;
	//
	mouse_data->LLDATA.BUTTONS.mouseSideButton1DownEvent = FALSE;
	mouse_data->LLDATA.BUTTONS.mouseSideButton1UpEvent = FALSE;
	//
	mouse_data->LLDATA.BUTTONS.mouseSideButton2DownEvent = FALSE;
	mouse_data->LLDATA.BUTTONS.mouseSideButton2UpEvent = FALSE;
	//
	mouse_data->LLDATA.HOOK.mouseStruct.dwExtraInfo = 0;
	mouse_data->LLDATA.HOOK.mouseStruct.flags = 0;
	mouse_data->LLDATA.HOOK.mouseStruct.mouseData = 0;
	mouse_data->LLDATA.HOOK.mouseStruct.pt.x = 0;
	mouse_data->LLDATA.HOOK.mouseStruct.pt.y = 0;
	mouse_data->LLDATA.HOOK.mouseStruct.time = 0;
}

// initMouse initializes the memory for the mouse data
//
void __fastcall initMouse(BOOL mouse, FARPROC mouse_input) {
	zeroMouseData(&mouseData);
	mouseData.LLDATA.HOOK.mouseHookActive = mouse;
	mouseData.LLDATA.INPUT.injectMouseInputProc = mouse_input;
	SystemParametersInfo(SPI_GETMOUSESPEED, 0, &mouseData.MouseSpeed, 0);
}

// initKeyboardKeyByIndex initializes the memory for the key
//
void __fastcall initKeyboardKeyByNumber(INT32 key_number) {
	KEYBOARDKEYCODE key_null = key_NULL;
	//
	keyboardData.LLDATA.keyboardKeys[key_number].keyboardKey = key_null;
	keyboardData.LLDATA.keyboardKeys[key_number].keyboardKeyActive = FALSE;
	keyboardData.LLDATA.keyboardKeys[key_number].keyboardEvent = 0;
	//
	keyboardData.LLDATA.keyboardKeys[key_number].keyboardStruct.dwExtraInfo = 0;
	keyboardData.LLDATA.keyboardKeys[key_number].keyboardStruct.flags = 0;
	keyboardData.LLDATA.keyboardKeys[key_number].keyboardStruct.scanCode = 0;
	keyboardData.LLDATA.keyboardKeys[key_number].keyboardStruct.time = 0;
	keyboardData.LLDATA.keyboardKeys[key_number].keyboardStruct.vkCode = 0;
}

// initKeyboard initializes the memory for the keyboard data
//
void __fastcall initKeyboard(BOOL keyboard) {
    keyboardData.LLDATA.keyboardHookActive = keyboard;
    //
    initKeyboardKeyByNumber(0);
    //
	keyboardData.LLDATA.keyboardInput.type = INPUT_KEYBOARD;
	keyboardData.LLDATA.keyboardInput.hi.uMsg = 0;
	keyboardData.LLDATA.keyboardInput.hi.wParamH = 0;
	keyboardData.LLDATA.keyboardInput.hi.wParamL = 0;
	//
	keyboardData.LLDATA.keyboardInput.ki.dwExtraInfo = 0;
	keyboardData.LLDATA.keyboardInput.ki.dwFlags = 0;
	keyboardData.LLDATA.keyboardInput.ki.time = 0;
	keyboardData.LLDATA.keyboardInput.ki.wScan = 0;
	keyboardData.LLDATA.keyboardInput.ki.wVk = 0;
	//
	keyboardData.LLDATA.keyboardInput.mi.time = 0;
	keyboardData.LLDATA.keyboardInput.mi.dwExtraInfo = 0;
	keyboardData.LLDATA.keyboardInput.mi.mouseData = 0;
	keyboardData.LLDATA.keyboardInput.mi.dx = 0;
	keyboardData.LLDATA.keyboardInput.mi.dy = 0;
}

/*					 _
* ___  ___ _ __   __| |
*| __|/ _ \  _ \ / _  |
*\__ \  __/ | | | (_| |
*|___/\___|_| |_|\____|
*/

// Static data
//
#define PI 3.14159265358979323846
#define SQRT2 sqrt(2)
#define SQRT3 sqrt(3)
#define SQRT5 sqrt(5)

// calculateMouseDistance returns the distance between points
//
double __fastcall calculateMouseDistance(double x_one, double y_one, double x_two, double y_two) {
	return sqrt(pow(x_two - x_one, 2) + pow(y_two - y_one, 2));
}

// mouseCoordinateToAbsolute calculates absolute coordinate from the usual
//
int __fastcall mouseCoordinateToAbsolute(int pos, int width_or_height) {
	return (pos * 65536) / width_or_height + 1;
}

// sendMouseMove moves or sets the cursor position
//
// If the relative value is false, the cursor sets to the specified coordinates
// If the relative value is true, the cursor moves relative to the specified coordinates
//
void __fastcall sendMouseMove(MOUSE* mouse_data) {
	/* Informing that the input is being sent */
	mouseData.LLDATA.INPUT.mouseInputActive = TRUE;

	/* Absolute or non-absolute mouse movement */
	if (mouse_data->LLDATA.INPUT.mouseInput.dwFlags == (_move | _absolute)) {
		/* Negative values are undesirable */
		if (mouse_data->MousePosX < 0) {
			mouse_data->MousePosX = 0;
		}
		//
		if (mouse_data->MousePosY < 0) {
			mouse_data->MousePosY = 0;
		}
		mouse_data->MousePosX = mouseCoordinateToAbsolute(mouse_data->MousePosX, screenData.screenWidth);
		mouse_data->MousePosY = mouseCoordinateToAbsolute(mouse_data->MousePosY, screenData.screenHeight);

		mouseData.LLDATA.INPUT.mouseInput.deltaX = mouse_data->MousePosX;
		mouseData.LLDATA.INPUT.mouseInput.deltaY = mouse_data->MousePosY;
	}
	else if (mouse_data->LLDATA.INPUT.mouseInput.dwFlags == _move) {
		mouseData.LLDATA.INPUT.mouseInput.deltaX = mouse_data->MousePosX;
		mouseData.LLDATA.INPUT.mouseInput.deltaY = mouse_data->MousePosY;
	}

	/* Injecting input data */
	skipHook.injectMouseInput(&mouseData.LLDATA.INPUT.mouseInput, 1);
	
	/* Informing that the input is no longer being sent */
	mouseData.LLDATA.INPUT.mouseInputActive = FALSE;

	/* Clearing the memory */
	SecureZeroMemory(&mouse_data, sizeof(mouse_data));
}

// sendHumanlyMouseMove calls the sendMouseMove with a special algorithm of "human" mouse movement
//
void __fastcall sendHumanlyMouseMove(MOUSE* mouse_data) {
	/* Negative values are undesirable */
	if (mouse_data->MousePosX < 0) {
		mouse_data->MousePosX = 0;
	}
	//
	if (mouse_data->MousePosY < 0) {
		mouse_data->MousePosY = 0;
	}

	/* The algorithm is already configured */
	const double end_pos_x = (double)mouse_data->MousePosX;
	const double end_pos_y = (double)mouse_data->MousePosY;
	const double end_pos_offset = 4.00;
	double pos_x = (double)mouseData.MousePosX;
    double pos_y = (double)mouseData.MousePosY;
	//
	const double wind = PI/SQRT5;
	const double gravity = PI*SQRT2;
	const double max_step = PI*SQRT2;
	//
	double hypot_distance = 0.00;
	double mouse_distance = 0.00;
	//
	double velo_x = 0.00;
	double velo_y = 0.00;
	//
	double wind_x = 0.00;
	double wind_y = 0.00;

	/* Last hypot distance for detect mouse movement */
	double last_hypot_distance = (double)(screenData.screenHeight + screenData.screenWidth);

	do {
		/* If the input is already being sent, then we don't need to do anything */
		if (mouseData.LLDATA.INPUT.mouseInputActive) {
			break;
		}

		/* Calculating the hypotenuse */
		hypot_distance = hypot(pos_x - end_pos_x, pos_y - end_pos_y);
		/* If the current distance is greater than the previous one, then the user has moved the mouse */
		if (hypot_distance > last_hypot_distance / 2) {
			break;
		}
		last_hypot_distance = hypot_distance;

		/* Calculating the wind */
		double wind_mag = min(wind, hypot_distance);
		/* Calculate the remaining distance */
		mouse_distance = calculateMouseDistance(round(pos_x), round(pos_y), round(end_pos_x), round(end_pos_y));
		if (mouse_distance < 1) {
			mouse_distance = 1;
		}

		double step = max_step;
		double d = round(round(mouse_distance) * 0.3) / 7;
		double rh = round(hypot_distance);
		if (d > max_step ||
			d < max_step / 2) {
			d = max_step;
		}
		if (d <= rh) {
			step = d;
		}
		else {
			step = rh;
		}

		wind_x = wind_x / SQRT3 + (2 * generateRandomValue(1, (int)(round(wind_mag))) + 1 - wind_mag) / SQRT5;
		wind_y = wind_y / SQRT3 + (2 * generateRandomValue(1, (int)(round(wind_mag))) + 1 - wind_mag) / SQRT5;
		//
		velo_x += wind_x + gravity * (end_pos_x - pos_x) / hypot_distance;
		velo_y += wind_y + gravity * (end_pos_y - pos_y) / hypot_distance;
		//
		const double random_dist = step / 2.00 + (double)generateRandomValue(1, (int)(round(step) / 2));
		const double velo_mag = sqrt(velo_x * velo_x + velo_y * velo_y);
		//
		velo_x = (velo_x / velo_mag) * random_dist;
		velo_y = (velo_y / velo_mag) * random_dist;
		//
		if (isnan(velo_x) || isnan(velo_y)) {
			break;
		}
		//
		const double position_x = round(velo_x);
        const double position_y = round(velo_y);
		//
		if (pos_x != position_x ||
			pos_y != position_y) {
			mouse_data->MousePosX = (int)position_x;
			mouse_data->MousePosY = (int)position_y;
			sendMouseMove(mouse_data);
		}
	} while (last_hypot_distance >= end_pos_offset); /* Exiting if the cursor is in the specified position */

	/* Clearing the memory */
	SecureZeroMemory(&mouse_data, sizeof(mouse_data));
}

// sendMouseEvent sends input data along with the specified button
//
void __fastcall sendMouseEvent(MOUSEEVENTCODE mouse_button) {
	MOUSE mouse_data = mouseData;
	mouse_data.LLDATA.INPUT.mouseInput.deltaX = 0;
	mouse_data.LLDATA.INPUT.mouseInput.deltaY = 0;
	//
	switch (mouse_button) {
	case button_LeftButtonDown: /* Mouse left button down */
		mouse_data.LLDATA.INPUT.mouseInput.dwFlags = _leftDown;
		break;
	case button_LeftButtonUp: /* Mouse left button up */
		mouse_data.LLDATA.INPUT.mouseInput.dwFlags = _leftUp;
		break;


	case button_RightButtonDown: /* Mouse right button down */
		mouse_data.LLDATA.INPUT.mouseInput.dwFlags = _rightDown;
		break;
	case button_RightButtonUp: /* Mouse right button up */
		mouse_data.LLDATA.INPUT.mouseInput.dwFlags = _rightUp;
		break;


	case button_MiddleButtonDown: /* Mouse middle button down */
		mouse_data.LLDATA.INPUT.mouseInput.dwFlags = _middleDown;
		break;
	case button_MiddleButtonUp: /* Mouse middle button up */
		mouse_data.LLDATA.INPUT.mouseInput.dwFlags = _middleUp;
		break;

	default:
		return;
	}

	/* Injecting input data */
	skipHook.injectMouseInput(&mouse_data.LLDATA.INPUT.mouseInput, 1);

	/* Clearing the memory */
	SecureZeroMemory(&mouse_data, sizeof(mouse_data));
}

#pragma endregion


#pragma region Import functions

/*
* _  _  ___   ___  _  __
*| || |/ _ \ / _ \| |/ /
*| __ | |_| | |_| |   |
*|_||_|\___/ \___/|_|\_\
 */

// CreateMouseAndKeyboardHook sets the necessary parameters and calls the interceptMouseAndKeyboardHook
//
// It is necessary to call this once during the entire operation of the program, otherwise unexpected problems may occur
//
// Returns true value if successful, otherwise false
//
bool __stdcall CreateMouseAndKeyboardHook(bool mouse, bool keyboard, FARPROC mouse_input, FARPROC skip_hook) {
	/* If both parameters are incorrect, then stops working */
	if (!mouse && !keyboard) {
		_DEBUG_PRINTF("\n%s", "Input hook: Two false values were passed, canceling the launch");
		return false;
	}

	initMouse((BOOL)mouse, mouse_input);
	initKeyboard((BOOL)keyboard);
	initSkipHook(skip_hook);

	/* Closing previous MSG threads if they exist */
	{
		gMsg.msg.hwnd = NULL;
		gMsg.msg.lParam = 0;
		gMsg.msg.message = 0;
		gMsg.msg.pt.x = 0;
		gMsg.msg.pt.y = 0;
		gMsg.msg.time = 0;
		gMsg.msg.wParam = 0;
		while (gMsg.msgActive != FALSE) {
			gMsg.msgActive = FALSE;
			skipHook.postMessage(GetConsoleWindow(), WM_QUIT, 0, 0);
		}
	}
	/* Getting the screen data (as a dependency) */
	{
		screenData.screenWidth = (INT32)skipHook.getSystemMetrics(SM_CXSCREEN);
		screenData.screenHeight = (INT32)skipHook.getSystemMetrics(SM_CYSCREEN);
		if (screenData.screenWidth <= 0 || screenData.screenHeight <= 0) {
			_DEBUG_PRINTF("\n%s", "Makc: Incorrect screen data, there may be errors");
			return false;
		}
	}

	/* Starting a separate thread */
	_beginthread((_beginthread_proc_type)interceptMouseAndKeyboardHook, 0, NULL);

	return true;
}

// GetMousePositionX returns the mouse position by X
//
int __stdcall GetMousePositionX() {
	return mouseData.MousePosX;
}

// GetMousePositionY returns the mouse position by Y
//
int __stdcall GetMousePositionY() {
	return mouseData.MousePosY;
}
// GetMouseSpeed returns the mouse speed of the operating system
//
int __stdcall GetMouseSpeed() {
	return mouseData.MouseSpeed;
}

// GetMouseEventByCode returns the status of the mouse button event by its event code (check MOUSEEVENTCODE)
//
bool __stdcall GetMouseEventStateByCode(MOUSEEVENTCODE event_code) {
	switch (event_code) {

	case WM_LBUTTONDOWN: /* Left mouse button down */
		return (bool)mouseData.LLDATA.BUTTONS.mouseLeftButtonDownEvent;
		break;
	case WM_RBUTTONDOWN: /* Right mouse button down */
		return (bool)mouseData.LLDATA.BUTTONS.mouseRightButtonDownEvent;
		break;


	case WM_LBUTTONUP: /* Left mouse button up */
		return (bool)mouseData.LLDATA.BUTTONS.mouseLeftButtonUpEvent;
		break;
	case WM_RBUTTONUP: /* Right mouse button up */
		return (bool)mouseData.LLDATA.BUTTONS.mouseRightButtonUpEvent;
		break;


	case WM_MBUTTONDOWN: /* Middle mouse button down */
		return (bool)mouseData.LLDATA.BUTTONS.mouseMiddleButtonDownEvent;
		break;
	case WM_MBUTTONUP: /* Middle mouse button up */
		return (bool)mouseData.LLDATA.BUTTONS.mouseMiddleButtonUpEvent;
		break;


	case WM_XBUTTONDOWN: /* Side mouse buttons down */
		/* XBUTTON1 */
		if (mouseData.LLDATA.HOOK.mouseStruct.mouseData == (DWORD)XBUTTON) {
			return (bool)mouseData.LLDATA.BUTTONS.mouseSideButton1DownEvent;
		}
		/* XBUTTON2 or other */
		else {
			return (bool)mouseData.LLDATA.BUTTONS.mouseSideButton2DownEvent;
		}
		break;
	case WM_XBUTTONUP: /* Side mouse buttons up */
		/* XBUTTON1 */
		if (mouseData.LLDATA.HOOK.mouseStruct.mouseData == (DWORD)XBUTTON) {
			return (bool)mouseData.LLDATA.BUTTONS.mouseSideButton1UpEvent;
		}
		/* XBUTTON2 or other */
		else {
			return (bool)mouseData.LLDATA.BUTTONS.mouseSideButton2UpEvent;
		}
		break;

	default:
		break;
	}

	return false;
}

// AddKeyboardKeyToHandle adds a key to the hook to track its state
//
int __stdcall AddKeyboardKeyToHandle(KEYBOARDKEYCODE vk_key) {
	/* Forced return if the key has already been added */
	if (keyboardData.LLDATA.keyboardNumberOfKeys > 0) {
		int key_number;
		for (key_number = 0; key_number < keyboardData.LLDATA.keyboardNumberOfKeys + 1; key_number++) {
			if (keyboardData.LLDATA.keyboardKeys[key_number].keyboardKey == vk_key) {
				return key_number;
			}
		}
	}

	/* Alternately sets the index */
	++keyboardData.LLDATA.keyboardNumberOfKeys;

	/* Initialize the key */
	if (keyboardData.LLDATA.keyboardNumberOfKeys > 0) {
		initKeyboardKeyByNumber(keyboardData.LLDATA.keyboardNumberOfKeys);
		keyboardData.LLDATA.keyboardKeys[keyboardData.LLDATA.keyboardNumberOfKeys].keyboardKey = vk_key;
	}

	/* Return the index of the key for further work with it */
	return (int)keyboardData.LLDATA.keyboardNumberOfKeys;
}

// DeleteLastAddedKeyboardKey deletes the last key in the array
//
void __stdcall DeleteLastAddedKeyboardKey(void) {
	if (keyboardData.LLDATA.keyboardNumberOfKeys > 0) {
		/* Reset all key data by index */
		initKeyboardKeyByNumber(keyboardData.LLDATA.keyboardNumberOfKeys);
		/* Reducing the number of keys */
		--keyboardData.LLDATA.keyboardNumberOfKeys;
	}
}

// DeleteAllAddedKeyboardKeys deletes all added keys in the array
//
void __stdcall DeleteAllAddedKeyboardKeys(void) {
	if (keyboardData.LLDATA.keyboardNumberOfKeys > 0) {
		int key_number;
		for (key_number = 0; key_number < keyboardData.LLDATA.keyboardNumberOfKeys + 1; key_number++) {
			/* Reset all key data by index */
			initKeyboardKeyByNumber(keyboardData.LLDATA.keyboardNumberOfKeys);
			/* Reducing the number of keys */
			--keyboardData.LLDATA.keyboardNumberOfKeys;
		}
	}
}

// GetKeyboardKeyStateByNumber returns the status of the keyboard key by its number
//
// NOTE: AddKeyboardKeyToHandle returns the number of the added key
//
bool __stdcall GetKeyboardKeyStateByNumber(int key_number) {
	return (bool)keyboardData.LLDATA.keyboardKeys[key_number].keyboardKeyActive;
}

// StopMouseAndKeyboardFlagsHook calls the unhookWindowsHook in a thread
//
// Need to call this on program exit
//
void __stdcall StopMouseAndKeyboardHook() {
	bool unhook = unhookWindowsHook();
	if (!unhook) {
		_DEBUG_PRINTF("\n%s", "Input hook: Failed to unhook");
	}

	/* Clearing the memory */
	//
	SecureZeroMemory(&screenData, sizeof(screenData));
	SecureZeroMemory(&mouseData, sizeof(mouseData));
	SecureZeroMemory(&keyboardData, sizeof(keyboardData));
	SecureZeroMemory(&gMsg, sizeof(gMsg));
}

/*					 _
* ___  ___ _ __   __| |
*| __|/ _ \  _ \ / _  |
*\__ \  __/ | | | (_| |
*|___/\___|_| |_|\____|
*/

// MouseMove calls the sendMouseMove
//
void __stdcall MouseMove(int pos_x, int pos_y, bool relative) {
	MOUSE mouse_data = mouseData;
	//
	mouse_data.MousePosX = pos_x;
	mouse_data.MousePosY = pos_y;
	//
	mouse_data.LLDATA.INPUT.mouseInput.deltaX = mouse_data.MousePosX;
	mouse_data.LLDATA.INPUT.mouseInput.deltaY = mouse_data.MousePosY;
	//
	mouse_data.LLDATA.INPUT.mouseInput.dwFlags = _move;
	if (!relative) {
		mouse_data.LLDATA.INPUT.mouseInput.dwFlags = (_move | _absolute);
	}
	//
	sendMouseMove(&mouse_data);
}

// MoveMouseHumanly calls the sendHumanlyMouseMove
//
void __stdcall MouseMoveHumanly(int pos_x, int pos_y) {
	MOUSE mouse_data = mouseData;
	//
	mouse_data.MousePosX = pos_x;
	mouse_data.MousePosY = pos_y;
	//
	mouse_data.LLDATA.INPUT.mouseInput.deltaX = mouse_data.MousePosX;
	mouse_data.LLDATA.INPUT.mouseInput.deltaY = mouse_data.MousePosY;
	//
	mouse_data.LLDATA.INPUT.mouseInput.dwFlags = _move;
	//
	sendHumanlyMouseMove(&mouse_data);
}

// MouseLeftButtonClick calls the sendMouseEvent twice, creating a left-click
//
void __stdcall MouseLeftButtonClick() {
	sendMouseEvent(button_LeftButtonDown);
	Sleep(generateRandomValue(2, 10));
	sendMouseEvent(button_LeftButtonUp);
}

// MouseRightButtonClick calls the sendMouseEvent twice, creating a right-click
//
void __stdcall MouseRightButtonClick() {
	sendMouseEvent(button_RightButtonDown);
	Sleep(generateRandomValue(2, 10));
	sendMouseEvent(button_RightButtonUp);
}

// MouseMiddleButtonClick calls the sendMouseEvent twice, creating a middle-click
//
void __stdcall MouseMiddleButtonClick() {
	sendMouseEvent(button_MiddleButtonDown);
	Sleep(generateRandomValue(2, 10));
	sendMouseEvent(button_MiddleButtonUp);
}

#pragma endregion

#endif /* IS_WINDOWS */
#endif /* MAKC_C */