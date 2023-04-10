// windows audio out.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "windows audio out.h"

#include <Windows.h>
#include <math.h>
#include <mmsystem.h>
#include <stdio.h>
#pragma comment(lib, "Winmm.lib")
#define OUT_BUFFER_SIZE		16384
#include "audio.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSAUDIOOUT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSAUDIOOUT));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSAUDIOOUT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSAUDIOOUT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static BOOL bShutOff, bClosing;
    static HWAVEOUT hWaveOut; /* device handle */
    static PBYTE pBuffer1, pBuffer2;
    static PWAVEHDR pWaveHdr1, pWaveHdr2;
    static WAVEFORMATEX waveformat;

    switch (message)
    {
    case WM_CREATE:
    {
        if (hWaveOut == NULL) {
            pWaveHdr1 = (PWAVEHDR)malloc(sizeof(WAVEHDR));
            pWaveHdr2 = (PWAVEHDR)malloc(sizeof(WAVEHDR));
            pBuffer1 = (PBYTE)malloc(OUT_BUFFER_SIZE);
            pBuffer2 = (PBYTE)malloc(OUT_BUFFER_SIZE);
            if (!pWaveHdr1 || !pWaveHdr2 || !pBuffer1 || !pBuffer2) {
                if (!pWaveHdr1) free(pWaveHdr1);
                if (!pWaveHdr2) free(pWaveHdr2);
                if (!pBuffer1)  free(pBuffer1);
                if (!pBuffer2)  free(pBuffer2);
                fprintf(stderr, "Error allocating memory!\n");
                return TRUE;
            }
            bShutOff = FALSE;

            //   MMRESULT result;  /* for waveOut return values */
            waveformat.nSamplesPerSec = 44100; /* sample rate */
            waveformat.wBitsPerSample = 16;   /* sample size */
            waveformat.nChannels = 2;   /* channels  */
            waveformat.cbSize = 0; /* size of _extra_ info */
            waveformat.wFormatTag = WAVE_FORMAT_PCM;
            waveformat.nBlockAlign = (waveformat.wBitsPerSample >> 3) * waveformat.nChannels;
            waveformat.nAvgBytesPerSec = waveformat.nBlockAlign * waveformat.nSamplesPerSec;
            if (waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveformat, (DWORD)hWnd, 0, CALLBACK_WINDOW) != MMSYSERR_NOERROR) {
                free(pWaveHdr1);	free(pWaveHdr2);	free(pBuffer1);	free(pBuffer2);
                hWaveOut = NULL;
                fprintf(stderr, "unable to open WAVE_MAPPER device\n");
                ExitProcess(1);
            }
            pWaveHdr1->lpData = (LPSTR)pBuffer1;
            pWaveHdr1->dwBufferLength = OUT_BUFFER_SIZE;
            pWaveHdr1->dwBytesRecorded = 0;
            pWaveHdr1->dwUser = 0;
            pWaveHdr1->dwFlags = 0;
            pWaveHdr1->dwLoops = 1;
            pWaveHdr1->lpNext = NULL;
            pWaveHdr1->reserved = 0;
            waveOutPrepareHeader(hWaveOut, pWaveHdr1, sizeof(WAVEHDR));

            pWaveHdr2->lpData = (LPSTR)pBuffer2;
            pWaveHdr2->dwBufferLength = OUT_BUFFER_SIZE;
            pWaveHdr2->dwBytesRecorded = 0;
            pWaveHdr2->dwUser = 0;
            pWaveHdr2->dwFlags = 0;
            pWaveHdr2->dwLoops = 1;
            pWaveHdr2->lpNext = NULL;
            pWaveHdr2->reserved = 0;
            waveOutPrepareHeader(hWaveOut, pWaveHdr2, sizeof(WAVEHDR));
        }
    }
    break;

    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        bShutOff = TRUE;
        PostQuitMessage(0);
        break;
    case MM_WOM_OPEN:
        FillAudioBuffer(pBuffer1);	waveOutWrite(hWaveOut, pWaveHdr1, sizeof(WAVEHDR));
        FillAudioBuffer(pBuffer2);	waveOutWrite(hWaveOut, pWaveHdr2, sizeof(WAVEHDR));
        return TRUE;
    case MM_WOM_DONE:
        if (bShutOff) { waveOutClose(hWaveOut);	return TRUE; }
        FillAudioBuffer((PBYTE)((PWAVEHDR)lParam)->lpData);
        waveOutWrite(hWaveOut, (PWAVEHDR)lParam, sizeof(WAVEHDR));
        return TRUE;
    case MM_WOM_CLOSE:
        waveOutUnprepareHeader(hWaveOut, pWaveHdr1, sizeof(WAVEHDR));
        waveOutUnprepareHeader(hWaveOut, pWaveHdr2, sizeof(WAVEHDR));
        free(pWaveHdr1);	free(pWaveHdr2);	free(pBuffer1);	free(pBuffer2);
        hWaveOut = NULL;
        return TRUE;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
