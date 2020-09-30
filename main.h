#ifndef _MAIN_H_
#define _MAIN_H_

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <stdio.h>
#include "sprite.h"

#define TICK_COUNT 65
#define SPRITE_PATH L"res\\pic.bmp"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void UpdateScreen(HWND hwnd);
void Timer(HWND hwnd);
void Resize(HWND hwnd);
void ReadKeys(int key, BOOL isDown, ACTIONS *control);
void MouseWheel(int keys, int steps);
void Redraw();
void CheckBounds(SPRITE *s);
void DrawSprite(SPRITE *s);
void UpdateFrameCounter();

#endif