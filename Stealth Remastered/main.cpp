#include "main.h"

HMODULE g_hModule = NULL;
//void MouseClick()
//{
//	/*INPUT mouseBuffer;
//	mouseBuffer.mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN);
//	SendInput(1, &mouseBuffer, sizeof(mouseBuffer));
//
//	Sleep(40);
//
//	mouseBuffer.mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP);
//	SendInput(1, &mouseBuffer, sizeof(mouseBuffer));*/
//	//pKeyHook->g_GameKeyState[BUTTON_CIRCLE] = { 0xFF, true };
//	SHORT key;
//	UINT mappedkey;
//	INPUT input = { 0 };
//	key = VkKeyScan('i');
//	mappedkey = MapVirtualKey(LOBYTE(key), 0);
//	input.type = INPUT_KEYBOARD;
//	input.ki.dwFlags = KEYEVENTF_SCANCODE;
//	input.ki.wScan = mappedkey;
//	SendInput(1, &input, sizeof(input));
//	Sleep(10);
//	pSAMP->addMessageToChat(0xFFFFFFFF, "Mouse click");
//	//pSAMP->addMessageToChat(0xFFFFFFFF, "Mouse click");
//	input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
//	SendInput(1, &input, sizeof(input));
//}



void mainThread()
{
	pConfig = new CConfig("GTS");
	pSAMP = new CSAMP();

	while (!pSAMP->tryInit())
		Sleep(100);

	pHooks = new CHooks();
	pD3DHook = new CD3DHook();
	pAimbot = new CAimbot();
	pVisuals = new CVisuals();
	//pRainbow = new CRainbow();
	pRakClient = new CRakClient();
	pSAMP->isInited = true;
	
	ExitThread(0);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReasonForCall, LPVOID lpReserved)
{
	switch (dwReasonForCall)
	{
		case DLL_PROCESS_ATTACH:
		{
			g_hModule = hModule;
			DisableThreadLibraryCalls(hModule);
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)mainThread, NULL, NULL, NULL);
			break;
		}
	}
	return TRUE;
}