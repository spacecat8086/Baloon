#include "main.h"

HDC hBackDC;
HDC hSpriteDC;
HBITMAP hBackBmp;
HBITMAP hSpriteBmp;
BITMAPINFO spriteImage;
HBRUSH hSpriteBrush = (HBRUSH)(COLOR_WINDOW + 1);
HBRUSH hBackBrush = (HBRUSH)(COLOR_HOTLIGHT + 1);
RECT clientRect;

SPRITE sprite = { 320, 240, IMAGE_WIDTH, IMAGE_HEIGHT };
DWORD prevTime, ticks;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR pCmdLine, int nCmdShow)
{
    prevTime = GetTickCount();

    const wchar_t *wcName = L"MainWindowClass";
    WNDCLASSEX wc = { };

    wc.lpszClassName = wcName;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.cbSize = sizeof (WNDCLASSEX);

    RegisterClassEx(&wc);

    HWND hWindow = CreateWindowEx(
        0,
        wcName,
        L"Window",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 640, 480,
        NULL,
        NULL,
        hInstance,
        NULL);

    if (hWindow == NULL)
    {
        return 42;
    }
    
    ShowWindow(hWindow, nCmdShow);

    int iResult;
    MSG msg = { };
    
    while ((iResult = GetMessage(&msg, NULL, 0, 0)) > 0)
    {
        DispatchMessage(&msg);
    }
    return iResult;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;        
    HDC dc;

    // Params obtained by the WM_MOUSEWHEEL message
    int fwKeys;
    int zDelta;

    switch (uMsg)
    {
    case WM_CREATE :
        GetClientRect(hwnd, &clientRect);
        dc = GetDC(hwnd);

        // Create back buffer
        hBackDC = CreateCompatibleDC(dc);
        hBackBmp = CreateCompatibleBitmap(dc, clientRect.right, clientRect.bottom);
        SelectObject(hBackDC, hBackBmp);

        // Load sprite image
        hSpriteBmp = LoadImage(NULL, SPRITE_PATH, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        hSpriteDC = CreateCompatibleDC(dc);
        SelectObject(hSpriteDC, hSpriteBmp);
  
        ReleaseDC(hwnd, dc);
        
        Timer(hwnd);
        return 0;

    case WM_DESTROY :
        PostQuitMessage(0);
        return 0;

    case WM_SIZE :
        LooseControl(&sprite);
        Resize(hwnd);
        return 0;

    // To avoid contols getting stuck
    case WM_KILLFOCUS :
        LooseControl(&sprite);
        return 0;
    
    // Same story
    case WM_MOVE :
        LooseControl(&sprite);
        return 0;

    case WM_PAINT :        
        Redraw();

        // Swap buffers
        dc = BeginPaint(hwnd, &ps);
        BitBlt(dc, 0, 0, clientRect.right, clientRect.bottom, hBackDC, 0, 0, SRCCOPY);
        EndPaint(hwnd, &ps);       

        UpdateFrameCounter();
        return 0;

    case WM_TIMER :
        Timer(hwnd);
        return 0;

    case WM_KEYDOWN :
        ReadKeys(wParam, TRUE, &sprite.actions);        
        return 0;

    case WM_KEYUP :
        ReadKeys(wParam, FALSE, &sprite.actions);
        return 0;

    case WM_MOUSEWHEEL :
        fwKeys = GET_KEYSTATE_WPARAM(wParam);
        zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
        MouseWheel(fwKeys, zDelta / WHEEL_DELTA);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void Redraw()
{    
    // Draw backround
    FillRect(hBackDC, &clientRect, hBackBrush);

    UpdateSprite(&sprite);
    CheckBounds(&sprite);
    DrawSprite(&sprite);
}

void Timer(HWND hwnd)
{
    // Actual tick count value is restricted by timer resolution
    SetTimer(hwnd, 1, 1000 / TICK_COUNT, NULL);
    InvalidateRect(hwnd, NULL, FALSE);
    //UpdateScreen(hwnd);
}

void Resize(HWND hwnd)
{
    // Create new back buffer bitmap that fits resized window
    GetClientRect(hwnd, &clientRect);
    HDC hdc = GetDC(hwnd);
    HBITMAP hNewBmp = CreateCompatibleBitmap(hdc, clientRect.right, clientRect.bottom);
    SelectObject(hBackDC, hNewBmp);
    ReleaseDC(hwnd, hdc);

    // And dispose of the old one
    DeleteObject((HGDIOBJ)hBackBmp);
    hBackBmp = hNewBmp;
}

/* Same handler for key press and release.
Particular action is specified in the 2-nd param */
void ReadKeys(int key, BOOL isDown, ACTIONS *control)
{
    switch (key)
    {
    case VK_UP :
        control->up = isDown;
        break;
        
    case VK_DOWN :
        control->down = isDown;
        break;
        
    case VK_LEFT :
        control->left = isDown;
        break;
        
    case VK_RIGHT :
        control->right = isDown;
        break;        
    }
}

void MouseWheel(int keys, int steps)
{
    int axis;

    if (keys & MK_SHIFT)
    {
        axis = PUNCH_HORIZONTAL;
    }
    else
    {
        axis = PUNCH_VERTICAL;
    }
    PunchSprite(&sprite, axis, steps);
}

void DrawSprite(SPRITE *s)
{
    // Copy sprite image to the back buffer
    TransparentBlt(hBackDC, s->x - s->width / 2, s->y - s->height / 2, s->width, s->height, hSpriteDC, 0, 0, 50, 117, RGB(255, 255, 255));
}

void CheckBounds(SPRITE *s)
{
    /* On collision with screen bounds, corresponding sptite's speed component is inverted 
    and sprite is moved back into the screen*/
    if (s->x < clientRect.left + s->width / 2)
    {
        s->x = clientRect.left + s->width / 2;
        s->vx *= -(1 - COLLISION_SLOWDOWN);
    }
    
    if (s->x > clientRect.right - s->width / 2)
    {
        s->x = clientRect.right - s->width / 2;
        s->vx *= -(1 - COLLISION_SLOWDOWN);
    }
    
    if (s->y < clientRect.top + s->height / 2)
    {
        s->y = clientRect.top + s->height / 2;
        s->vy *= -(1 - COLLISION_SLOWDOWN);
    }
    
    if (s->y > clientRect.bottom - s->height / 2)
    {
        s->y = clientRect.bottom - s->height / 2;
        s->vy *= -(1 - COLLISION_SLOWDOWN);
    }
}

void UpdateFrameCounter()
{
    DWORD now = GetTickCount();

    if (now - prevTime < 1000)
    {
        ticks++;
    }
    else
    {
        prevTime = now;
        printf("%d\n", ticks);
        ticks = 0;
    }
}

/* Currently unused.
Alternative way of painting the window.
Call this inside the timer handler instead of 
calling InvalidateRect and handlind WM_PAINT message */
void UpdateScreen(HWND hwnd)
{
        Redraw();

        // Swap buffers
        HDC dc = GetDC(hwnd);
        BitBlt(dc, 0, 0, clientRect.right, clientRect.bottom, hBackDC, 0, 0, SRCCOPY);
        ReleaseDC(hwnd, dc);   

        UpdateFrameCounter();
}
