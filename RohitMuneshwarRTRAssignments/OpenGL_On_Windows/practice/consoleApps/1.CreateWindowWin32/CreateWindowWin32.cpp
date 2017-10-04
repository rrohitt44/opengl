#include<Windows.h>

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLIne, int iCmdShow){
	WNDCLASSEX wndclass;
	TCHAR AppName[] = TEXT("Window Custom");
	TCHAR WinName[] = TEXT("My Sweet Window");
	HWND hwnd;
	MSG msg;
	//initialize window class
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = AppName;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	//initialization complete
	
	RegisterClassEx(&wndclass); //Register Class
	
	//create window in memory
	hwnd = CreateWindow(
	AppName,
	WinName,
	WS_OVERLAPPEDWINDOW,
	CW_USEDEFAULT,
	CW_USEDEFAULT,
	CW_USEDEFAULT,
	CW_USEDEFAULT,
	NULL,
	NULL,
	hInstance,
	NULL
	);
	
	ShowWindow(hwnd, iCmdShow); //Show Window
	UpdateWindow(hwnd); // color/paint the background of the window
	
	//message loop
	
	//get message first
	while(GetMessage(&msg, NULL, 0, 0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		//translate and dispatch message
	}
	return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam){
	switch(iMsg){
		case WM_DESTROY:
			PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}