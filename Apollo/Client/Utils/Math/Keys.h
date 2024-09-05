#pragma once

namespace Keys
{
	// Special keys
	static const int NONE = 7;
	static const int BACKSPACE = 8;
	static const int TAB = 9;
	static const int ENTER = 13;
	static const int SHIFT = 16;
	static const int CTRL = 17;
	static const int ALT = 18;
	static const int PAUSE = 19;
	static const int CAPSLOCK = 20;
	static const int ESC = 27;
	static const int SPACEBAR = 32;
	static const int PAGEUP = 33;
	static const int PAGEDOWN = 34;
	static const int END = 35;
	static const int HOME = 36;
	static const int ARROW_LEFT = 37;
	static const int ARROW_UP = 38;
	static const int ARROW_RIGHT = 39;
	static const int ARROW_DOWN = 40;
	static const int PRINTSCRN = 44;
	static const int INSERT = 45;
	static const int DELETE_KEY = 46;

	// Number keys
	static const int NUM_0 = 48;
	static const int NUM_1 = 49;
	static const int NUM_2 = 50;
	static const int NUM_3 = 51;
	static const int NUM_4 = 52;
	static const int NUM_5 = 53;
	static const int NUM_6 = 54;
	static const int NUM_7 = 55;
	static const int NUM_8 = 56;
	static const int NUM_9 = 57;

	// Alphabet keys
	static const int A = 65;
	static const int B = 66;
	static const int C = 67;
	static const int D = 68;
	static const int E = 69;
	static const int F = 70;
	static const int G = 71;
	static const int H = 72;
	static const int I = 73;
	static const int J = 74;
	static const int K = 75;
	static const int L = 76;
	static const int M = 77;
	static const int N = 78;
	static const int O = 79;
	static const int P = 80;
	static const int Q = 81;
	static const int R = 82;
	static const int S = 83;
	static const int U = 85;
	static const int V = 86;
	static const int W = 87;
	static const int X = 88;
	static const int Y = 89;
	static const int Z = 90;

	// Function keys
	static const int F1 = 112;
	static const int F2 = 113;
	static const int F3 = 114;
	static const int F4 = 115;
	static const int F5 = 116;
	static const int F6 = 117;
	static const int F7 = 118;
	static const int F8 = 119;
	static const int F9 = 120;
	static const int F10 = 121;
	static const int F11 = 122;
	static const int F12 = 123;

	// Numpad keys
	static const int NUMPAD_0 = 96;
	static const int NUMPAD_1 = 97;
	static const int NUMPAD_2 = 98;
	static const int NUMPAD_3 = 99;
	static const int NUMPAD_4 = 100;
	static const int NUMPAD_5 = 101;
	static const int NUMPAD_6 = 102;
	static const int NUMPAD_7 = 103;
	static const int NUMPAD_8 = 104;
	static const int NUMPAD_9 = 105;
	static const int NUMPAD_MULTIPLY = 106;
	static const int NUMPAD_ADD = 107;
	static const int NUMPAD_SUBTRACT = 109;
	static const int NUMPAD_DECIMAL = 110;
	static const int NUMPAD_DIVIDE = 111;

	// Other keys
	static const int SEMICOLON = 186;
	static const int EQUAL = 187;
	static const int COMMA = 188;
	static const int DASH = 189;
	static const int PERIOD = 190;
	static const int FORWARDSLASH = 191;
	static const int GRAVE = 192;
	static const int LBRACKET = 219;
	static const int BACKSLASH = 220;
	static const int RBRACKET = 221;
	static const int APOSTROPHE = 222;
};

const char* const KeyName[] =
{
	"Unknown",
"VK_LBUTTON",
"VK_RBUTTON",
"VK_CANCEL",
"VK_MBUTTON",
"VK_XBUTTON1",
"VK_XBUTTON2",
"Unknown",
"VK_BACK",
"TAB",
"Unknown",
"Unknown",
"VK_CLEAR",
"RETURN",
"Unknown",
"Unknown",
"Shift",
"CTRL",
"VK_MENU",
"VK_PAUSE",
"VK_CAPITAL",
"VK_KANA",
"Unknown",
"VK_JUNJA",
"VK_FINAL",
"VK_KANJI",
"Unknown",
"ESCAPE",
"VK_CONVERT",
"VK_NONCONVERT",
"VK_ACCEPT",
"VK_MODECHANGE",
"SPACE",
"PRIOR",
"NEXT",
"END",
"HOME",
"LEFT",
"UP",
"RIGHT",
"DOWN",
"VK_SELECT",
"VK_PRINT",
"VK_EXECUTE",
"VK_SNAPSHOT",
"INSERT",
"DELETE",
"VK_HELP",
"0",
"1",
"2",
"3",
"4",
"5",
"6",
"7",
"8",
"9",
"Unknown",
"Unknown",
"Unknown",
"Unknown",
"Unknown",
"Unknown",
"Unknown",
"A",
"B",
"C",
"D",
"E",
"F",
"G",
"H",
"I",
"J",
"K",
"L",
"M",
"N",
"O",
"P",
"Q",
"R",
"S",
"T",
"U",
"V",
"W",
"X",
"Y",
"Z",
"VK_LWIN",
"VK_RWIN",
"VK_APPS",
"Unknown",
"VK_SLEEP",
"NUMPAD0",
"NUMPAD1",
"NUMPAD2",
"NUMPAD3",
"NUMPAD4",
"NUMPAD5",
"NUMPAD6",
"NUMPAD7",
"NUMPAD8",
"NUMPAD9",
"VK_MULTIPLY",
"VK_ADD",
"VK_SEPARATOR",
"VK_SUBTRACT",
"VK_DECIMAL",
"VK_DIVIDE",
"F1",
"F2",
"F3",
"F4",
"F5",
"F6",
"F7",
"F8",
"F9",
"F10",
"F11",
"F12",
"VK_F13",
"VK_F14",
"VK_F15",
"VK_F16",
"VK_F17",
"VK_F18",
"VK_F19",
"VK_F20",
"VK_F21",
"VK_F22",
"VK_F23",
"VK_F24",
"Unknown",
"Unknown",
"Unknown",
"Unknown",
"Unknown",
"Unknown",
"Unknown",
"Unknown",
"VK_NUMLOCK",
"VK_SCROLL",
"VK_OEM_NEC_EQUAL",
"VK_OEM_FJ_MASSHOU",
"VK_OEM_FJ_TOUROKU",
"VK_OEM_FJ_LOYA",
"VK_OEM_FJ_ROYA",
"Unknown",
"Unknown",
"Unknown",
"Unknown",
"Unknown",
"Unknown",
"Unknown",
"Unknown",
"Unknown",
"LeftShift",
"RightShift",
"VK_LCONTROL",
"VK_RCONTROL",
"VK_LMENU",
"VK_RMENU",
"Browser Back",
"Browser Forward",
"Browser Refresh",
"Browser Stop",
"Browser Search",
"Browser Favourites",
"Browser Home",
"Volume Mute",
"Volume Down",
"Volume Up",
"Media_Next",
"Media_Prev",
"Media_Stop",
"Media_Pause",
"Mail",
"Media",
"App1",
"App2",
"Unknown",
"Unknown",
"OEM_1",
"PLUS",
"COMMA",
"MINUS",
"DOT",
"OEM_2",
"OEM_3"
};