#include <windows.h>
#include <math.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ChildWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"HelloAPI";
LPCTSTR ChildClassName = L"ChildWin";


#define IDC_BUTTON 1001
#define BOARD_SIZE 60
#define DIR_E 1
#define DIR_S 2
#define DIR_W 3
#define DIR_N 4
#define RESULT_OK 1
#define RESULT_FEED 2
#define UPDATE_WALL 3
#define CELL_SIZE 20

int gameBoard[BOARD_SIZE][BOARD_SIZE];
int score = 0;
int length = 3;
int speed = 1;
int direction = DIR_E;
int cellSIze = CELL_SIZE;
void initBoard();
void gameLoop();
void renderBoard();
int updateBoard();
void changeDir(int dir);



int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpszCmdParam,int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);     //���������� Ŭ���� ���

	WndClass.lpfnWndProc = ChildWndProc;      //���ϵ� ������ ���ν���
	WndClass.lpszClassName = ChildClassName; //���ϵ� ������ Ŭ�����̸�
	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass,			//������Ŭ���� �̸�
		L"��ǻ�Ͱ��а� 2�г�",			    //������Ÿ��Ʋ
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,   //�����콺Ÿ��
		200, 200,							//�����찡 ���϶� X Y��ǥ
		600, 600,							//�������� ���� ����				
		(HWND)NULL,							//�θ������� �ڵ�
		(HMENU)NULL,						//�����찡 ������ �޴��ڵ�
		hInstance,							//�ν��Ͻ��ڵ�
		NULL);								//������ ������

	ShowWindow(hWnd, nCmdShow);

	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage,
	WPARAM wParam, LPARAM lParam)
{
	LPCTSTR text = L"������ 202234-243458";
	switch (iMessage) {
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		TextOut(hdc, 100, 100, text, lstrlen(text));
		EndPaint(hWnd, &ps);
		return 0;
	}
	case WM_CREATE:
	{
		HWND hChildWnd = CreateWindow(
			ChildClassName,     		// ���ϵ� ������ Ŭ���� �̸� 
			L"���ϵ� ������",            	// ������ Ÿ��Ʋ 
			WS_OVERLAPPEDWINDOW | WS_CHILD,  // ������  ��Ÿ�� 
			150,       		// ������ ���� �� x ��ǥ 
			150,       		// ������ ���� �� y ��ǥ 
			260,       		// ������ ��
			200,       		// ������ ����
			hWnd,         		// �θ� ������
			(HMENU)1000,        	// ���ϵ� ������ID 
			g_hInst,           		// �ν��Ͻ� �ڵ� 
			(LPVOID)NULL);      	// ������ ������

		if (!hChildWnd) 	return -1;

		HWND hButton = CreateWindow(
			L"button",
			L"��������",
			WS_CHILD | WS_VISIBLE,
			20, 400, 100, 30,
			hWnd,
			(HMENU)IDC_BUTTON,
			g_hInst,
			(LPVOID)NULL);

		ShowWindow(hChildWnd, SW_SHOW);
		initBoard();
		return 0;
	}
	case WM_COMMAND:
		if (LOWORD(wParam) == IDC_BUTTON) {
			MessageBox(hWnd, L"�������� ��ư�� Ŭ���Ǿ���", L"��������", MB_OK | MB_ICONINFORMATION);
		}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
LRESULT CALLBACK ChildWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LPCTSTR text = L"������ 202234-243458";
	switch (message)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		TextOut(hdc, 10, 10, text, lstrlen(text));
		EndPaint(hWnd, &ps);
		return 0;
	}

	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

/*****************************************************************************/
/* Game Code                                                                 */
/*****************************************************************************/

void initBoard() {
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			gameBoard[i][j] = 0;
		}
	}
	int boardCenter = floor(BOARD_SIZE / 2);
}

