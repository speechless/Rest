#include "stdafx.h"

bool _keepAlive = true;
bool _pressKey = false;

class KeyboardHook
{
private:
	HANDLE hMessageLoop;
	static KeyboardHook* pKeyboardHook;
public:
	KeyboardHook();
	static KeyboardHook *GetInstance ();
	void Start();
	void End();
	HANDLE GetThread();
	static DWORD WINAPI Messageloop(LPVOID);
	static LRESULT CALLBACK LowLevelKeyboardProc( int nCode, WPARAM wParam, LPARAM lParam );
};

KeyboardHook* KeyboardHook::pKeyboardHook = NULL;

KeyboardHook::KeyboardHook()
{
}

KeyboardHook* KeyboardHook::GetInstance()
{
	if (pKeyboardHook == NULL)
		pKeyboardHook = new KeyboardHook;

	return pKeyboardHook;
}
void KeyboardHook::Start()
{
	this->hMessageLoop = CreateThread(NULL, NULL, KeyboardHook::Messageloop, NULL, NULL, NULL);
}
void KeyboardHook::End()
{
	TerminateThread(this->hMessageLoop, 0);
}
HANDLE KeyboardHook::GetThread()
{
	return hMessageLoop;
}
DWORD WINAPI KeyboardHook::Messageloop(LPVOID)
{
	MSG msg;
	HINSTANCE appInstance = GetModuleHandle(NULL);

	SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, appInstance, 0);

	while( GetMessage(&msg, NULL, 0, 0 ) > 0 )
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}
LRESULT CALLBACK KeyboardHook::LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	KBDLLHOOKSTRUCT* pKeyBoard = reinterpret_cast<KBDLLHOOKSTRUCT *>(lParam);

	bool isEndEnabled = GetKeyState(VK_END) < 0;
	bool isHomeEnabled = GetKeyState(VK_HOME) < 0;

	if (isEndEnabled == true)
		_keepAlive = false;
	
	if (isHomeEnabled == true)
		_pressKey = true;

	return 0;
}

void PressNum0 ()
{
	INPUT ip;

	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0;
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;

	ip.ki.wVk = VK_NUMPAD0;

	ip.ki.dwFlags = 0;
	SendInput(1, &ip, sizeof(INPUT));

	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));
}

int main (int argc, char *argv[])
{
	KeyboardHook *hook = KeyboardHook::GetInstance();
	std::cout << "Keyboard Hook obtained\n";

	hook->Start();
	std::cout << "Key Logger started\n";

	std::cout << "\n"
		"  Instructions:\n"
		"    set FFXIV:ARR as your active window\n"
		"    press HOME to start spamming NUM 0\n"
		"    press END to quit program.\n";

	while (_keepAlive == true)
	{
		if (_pressKey == true)
			PressNum0();
		
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	hook->End();

	return 0;
}
