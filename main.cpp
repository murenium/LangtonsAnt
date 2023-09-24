#include <windows.h>
#include <windowsx.h>
#include <iostream>
#include <pthread.h>

using namespace std;

const int SCREEN_X = GetSystemMetrics(SM_CXSCREEN);
const int SCREEN_Y = GetSystemMetrics(SM_CYSCREEN);
const int WND_X = 800;
const int WND_Y = 600;
int step_count;

HDC dc;

int dx = 400;
int dy = 300;

// Colors
const COLORREF white = RGB(255, 255, 255);
const COLORREF black = RGB(0, 0, 0);
const COLORREF lcolor = RGB(255, 0, 0); // Ant color

pthread_t ants[1024] = {}; // Ants.
// Max ants count - 1024

bool main_window[800][600] = {}; // Massive with pixels

// 2D vector tools
struct vec2 {
    int x;
    int y;
};

vec2 Vec2(int x, int y) {
    vec2 temp;
    temp.x = x;
    temp.y = y;
    return temp;
}

// Rotation functions...
int Plus90(int rotation) {
    if(rotation == 360) return 90;
    else return rotation+=90;
}
int Min90(int rotation) {
    if(rotation == 0) return 270;
    else return rotation-=90;
}

vec2 GetVec2ByRotation(vec2 old, int rotation) {
    if((rotation == 0)||(rotation == 360)) old.x--;
    else if(rotation == 90) old.y--;
    else if(rotation == 180) old.x++;
    else if(rotation == 270) old.y++;
    return old;
}

// Logic & drawing
void* render(void*) {
    vec2 pos = Vec2(dx, dy);
    int rotation = 0;
    for(int i = 0; i <= step_count; i++) {
        if((pos.x >= 800)||(pos.y >= 600)||(pos.x <= 0)||(pos.y <= 0)) { // Checking for errors
            cout << "error" << endl;
            pthread_exit(NULL);
        }
        if(main_window[pos.x][pos.y]) { // On white square
            main_window[pos.x][pos.y] = false;
            SetPixel(dc, pos.x, pos.y, black);
            rotation = Plus90(rotation);
            pos = GetVec2ByRotation(pos, rotation);
        }
        else { // On black square
            main_window[pos.x][pos.y] = true;
            SetPixel(dc, pos.x, pos.y, white);
            rotation = Min90(rotation);
            pos = GetVec2ByRotation(pos, rotation);
        }
        SetPixel(dc, pos.x, pos.y, lcolor);
        cout << endl << "X=" << pos.x << "   Y=" << pos.y << "   R=" << rotation << "   Step N" << i << " ~ "  << (float(i)/float(step_count))*100 << '%' << endl; // Status
        //Sleep(10);
    }
    pthread_exit(NULL);
}

void FillWindow() {
    for(int i = 0; i <= 800; i++) {
        for(int i2 = 0; i2 <= 600; i2++) {
            if(main_window[i][i2] == true) SetPixel(dc, i, i2, white);
            else SetPixel(dc, i, i2, black);
            //else SetPixel(dc, i, i2, RGB(127, 127, 127));
        }
    }
}

// Window event handling function
LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_LBUTTONDOWN: // Spawn ants by mouse click
        dx = GET_X_LPARAM(lParam);
        dy = GET_Y_LPARAM(lParam);
        pthread_create(&ants[sizeof(ants)/sizeof(ants[0])+1], NULL, &render, NULL);
        break;
    case WM_KEYDOWN:
        if(wParam == 70) FillWindow();
        break;
    default:
        DefWindowProcA(hwnd, msg, wParam, lParam);
        break;
    }
}

int main()
{
    cout << "init...";
    // Filling an array
    for(int i = 0; i >= 800; i++) {
        for(int i2 = 0; i >= 600; i2++) {
            main_window[i][i2] = true;
        }
    }
    memset(&main_window, 1, sizeof(main_window));
    WNDCLASS wcl; // Window class setting
    memset(&wcl, 0, sizeof(WNDCLASS));
    wcl.lpszClassName = "gui";
    wcl.lpfnWndProc = WndProc;
    wcl.hbrBackground = (HBRUSH) CreateSolidBrush(RGB(127, 127, 127)); // Background color
    RegisterClass(&wcl);
    cout << "ok" << endl << "Step count: ";
    string step_count_s;
    cin >> step_count_s;
    step_count = stoi(step_count_s);
    HWND hwnd = CreateWindow("gui", "test", WS_OVERLAPPEDWINDOW, 100, 100, WND_X, WND_Y, NULL, NULL, NULL, NULL); // Creating window
    dc = GetDC(hwnd);
    ShowWindow(hwnd, SW_SHOWNORMAL);
    SetActiveWindow(hwnd);
    UpdateWindow(hwnd);
    //pthread_t thr;
    //if(pthread_create(&thr, NULL, &render, NULL) != 0) return 1;
    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0)){ // Window loop
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0; // End...
}
