/*****************************************
		NanoShell Operating System
	      (C) 2022 iProgramInCpp
           Raycaster application

             Main source file
******************************************/
#include <nsstandard.h>
#include "game.h"

void SLogMsg(const char*, ...);


Window* g_pWindow = NULL;

void RequestRepaint(Window* pWindow);

int ScreenWidth = 640, ScreenHeight = 480;

//sub-millis timing?
const char* GetGameName();
void Update (int deltaTime);
void Render (int deltaTime);
void Init   ();

bool gKeyboardState[128];

bool IsKeyDown (int keyCode)
{
	return gKeyboardState [keyCode];
}

extern bool WantCaptureMouse();
extern int mouseX, mouseY;
extern bool mouseL, mouseR, setPause;
static bool capture;
static int lastDeltaTime = 1;
int lastX = -1, lastY = -1;
void CALLBACK WndProc (Window* pWindow, int messageType, long parm1, long parm2)
{
	switch (messageType)
	{
		case EVENT_SIZE: {
			ScreenWidth  = GET_X_PARM(parm1);
			ScreenHeight = GET_Y_PARM(parm1);
			OnSize (ScreenWidth, ScreenHeight);
			break;
		}
		case EVENT_KEYRAW: {
			unsigned char key_code = (unsigned char) parm1;
			if (key_code != 0xE0)
				gKeyboardState[key_code & 0x7F] = !(key_code & 0x80);
			break;
		}
		case EVENT_MOVECURSOR: {
			int actualX = GET_X_PARM(parm1);
			int actualY = GET_Y_PARM(parm1);
			
			if (lastX != -1) {
				mouseX += (actualX - lastX);
				mouseY += (actualY - lastY);
			}
			
			lastX = actualX;
			lastY = actualY;
			break;
		}
		case EVENT_UPDATE: {
			Render (lastDeltaTime);
			break;
		}
		case EVENT_CLICKCURSOR:
			mouseL = true;
			break;
		case EVENT_RIGHTCLICK:
			mouseR = true;
			break;
		case EVENT_RELEASECURSOR:
			mouseL = false;
			break;
		case EVENT_RIGHTCLICKRELEASE:
			mouseR = false;
			break;
		case EVENT_SETFOCUS:
			capture = WantCaptureMouse();
			setPause = false;
			break;
		case EVENT_KILLFOCUS:
			capture = false;
			setPause = true;
			break;
		default:
			DefaultWindowProc (pWindow, messageType, parm1, parm2);
			break;
	}
}

void ResetToMiddle()
{
	if (capture) {
		Rectangle rc;
		GetWindowRect(g_pWindow, &rc);
		SetMousePos((rc.left + rc.right) / 2, (rc.top + rc.bottom) / 2);
		lastX = (rc.left + rc.right) / 2 - rc.left;
		lastY = (rc.top + rc.bottom) / 2 - rc.top;
	}
}

static bool SetupWindow (const char* TITLE)
{
	Window* pWindow = CreateWindow (TITLE, 200,200, ScreenWidth, ScreenHeight, WndProc, WF_ALWRESIZ | WF_SYSPOPUP);
	if (!pWindow)
	{
		SLogMsg ("Could not create window");
		return false;
	}
	
	g_pWindow = pWindow;
	
	return true;
}

int main()
{
	if (!SetupWindow(GetGameName()))
		return 1;
	
	Init();
	
	int nextTickIn;
	int lastTC = GetTickCount();
	bool fix = false;
	while (1)
	{
		int target = 16 + fix;
		fix ^= 1;
		
		// update game
		int deltaTime = GetTickCount() - lastTC;
		
		lastTC = GetTickCount();
		
		int tc=GetTickCount();
		Update (deltaTime);
		int diff=GetTickCount()-tc;
		LogMsg("this update took %d ms.  %d.%02d fps estimated.", diff, 1000/(diff?diff:1), 100000/(diff?diff:1)%100);
		
		nextTickIn = GetTickCount() + target - (GetTickCount()-lastTC);
		lastDeltaTime = deltaTime;
		
		// repaint the window
		//RequestRepaint (g_pWindow);
		RegisterEvent (g_pWindow, EVENT_UPDATE, 0, 0);
		
		// Handle window messages.
		bool result = HandleMessages (g_pWindow);
		if (!result)
		{
			g_pWindow = NULL;
			break;
		}
		
		while (nextTickIn > GetTickCount());
	}
	
	CleanUp();
	
	return 0;
}
