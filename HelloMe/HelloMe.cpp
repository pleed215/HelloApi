#include <windows.h>
#include <math.h>
#include <time.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ChildWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

HINSTANCE g_hInst;
HWND g_hMain=(HWND)NULL;
LPCTSTR lpszClass = L"HelloAPI";
LPCTSTR ChildClassName = L"ChildWin";


#define ID_BTN_START 1001
#define ID_BTN_HIGHSCORE 1002
#define IDT_TIMER1 1003

#define BOARD_SIZE 60
#define DIR_E 1
#define DIR_S 2
#define DIR_W 3
#define DIR_N 4
#define RESULT_OK 1
#define RESULT_FOOD 2
#define RESULT_WALL 3
#define RESULT_SELF 4
#define CELL_SIZE 10
#define INIT_LENGTH 3
#define GAME_STATE_STOP 0
#define GAME_STATE_PLAYING 1
#define GAME_STATE_END 2

typedef struct {
	int x, y;
} Position;

const int MAX_SCORE = BOARD_SIZE * 3;
int gameBoard[BOARD_SIZE][BOARD_SIZE];
Position snakeData[MAX_SCORE+3];
Position foodData;
int score = 0;
int length = 3;
int speed = 1;
int direction = DIR_E;
int gameState = GAME_STATE_STOP;
const int CONTENT_HEIGHT = 100;
const int WINDOW_WIDTH = CELL_SIZE * BOARD_SIZE + 40;
const int WINDOW_HEIGHT = CELL_SIZE * BOARD_SIZE + CONTENT_HEIGHT + 100;
const int START_X = 10;
const int START_Y = CONTENT_HEIGHT;

void initBoard();
void gameLoop();
void renderBoard(HDC);
int updateBoard();
void changeDir(int dir);
void generateFood();
void gainFood();
BOOL isFoodPositionOK();
void drawCell(HDC, Position&);

// Utils
int rangeRandom(int begin, int end); // end�� ���Ծ���.



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
		20, 20,							//�����찡 ���϶� X Y��ǥ
		WINDOW_WIDTH, WINDOW_HEIGHT,							//�������� ���� ����				
		(HWND)NULL,							//�θ������� �ڵ�
		(HMENU)NULL,						//�����찡 ������ �޴��ڵ�
		hInstance,							//�ν��Ͻ��ڵ�
		NULL);								//������ ������

	g_hMain = hWnd;
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
		TextOut(hdc, 10, 10, text, lstrlen(text));
		renderBoard(hdc);
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
			600,       		// ������ ��
			600,       		// ������ ����
			hWnd,         		// �θ� ������
			(HMENU)1000,        	// ���ϵ� ������ID 
			g_hInst,           		// �ν��Ͻ� �ڵ� 
			(LPVOID)NULL);      	// ������ ������

		if (!hChildWnd) 	return -1;
		CreateWindow(
			L"button",
			L"���� ����",
			WS_CHILD | WS_VISIBLE,
			210, 10, 100, 30,
			hWnd,
			(HMENU)ID_BTN_START,
			g_hInst,
			(LPVOID)NULL);
		CreateWindow(
			L"button",
			L"���̽��ھ�",
			WS_CHILD | WS_VISIBLE,
			320, 10, 100, 30,
			hWnd,
			(HMENU)ID_BTN_HIGHSCORE,
			g_hInst,
			(LPVOID)NULL);

		long seed = time(NULL);
		srand(seed);
		//ShowWindow(hChildWnd, SW_SHOW);
		initBoard();
		return 0;
	}
	case WM_COMMAND:
		if (LOWORD(wParam) == ID_BTN_START) {
			initBoard();
			gameState = GAME_STATE_PLAYING;
			SetFocus(hWnd);
			SetTimer(hWnd, IDT_TIMER1, 100, (TIMERPROC)NULL);
		}
		return 0;

	case WM_KEYDOWN:
		if (gameState == GAME_STATE_PLAYING) {
			switch (wParam) {
			case VK_UP:
				changeDir(DIR_N);
				break;
			case VK_DOWN:
				changeDir(DIR_S);
				break;
			case VK_LEFT:
				changeDir(DIR_W);
				break;
			case VK_RIGHT:
				changeDir(DIR_E);
				break;
			case VK_SPACE:
				gainFood();
				break;
			default:
				break;
			}
		}
		return 0;
	case WM_TIMER:
		switch (wParam) {
			case IDT_TIMER1:
				if (gameState == GAME_STATE_PLAYING) {
					int result = updateBoard();
					if (result == RESULT_WALL || result == RESULT_SELF) {
						KillTimer(hWnd, IDT_TIMER1);
						gameState = GAME_STATE_END;
						MessageBox(hWnd, L"�浹����", L"��������", MB_OK | MB_ICONERROR);
					}
					else if (result == RESULT_FOOD) {
						gainFood();
					}
				}
				
				break;

		}
		return 0;
	case WM_DESTROY:
		KillTimer(hWnd, IDT_TIMER1);
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
	score = 0;
	gameState = GAME_STATE_STOP;
	direction = DIR_E;
	int boardCenter = floor(BOARD_SIZE / 2);
	for (int i = 0; i < INIT_LENGTH; i++) {
		snakeData[i].x = boardCenter - i;
		snakeData[i].y = boardCenter;
	}
	generateFood();
}

void renderBoard(HDC hdc) {
	HPEN blackPen = (HPEN)GetStockObject(BLACK_PEN);
	HBRUSH blackBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
	HBRUSH redBrush = (HBRUSH)CreateSolidBrush(RGB(255, 0, 0));
	HBRUSH foodBrush = (HBRUSH)CreateSolidBrush(RGB(0, 255, 0));
	HGDIOBJ oldObj =  SelectObject(hdc, (HGDIOBJ)blackPen);

	int boardLength = BOARD_SIZE * CELL_SIZE;
	Rectangle(hdc, START_X, START_Y, START_X+boardLength, START_Y+boardLength);
	SelectObject(hdc, oldObj);
	
	for (int i = 0; i < score + INIT_LENGTH; i++) {
		if (i == 0) {
			oldObj = SelectObject(hdc, (HGDIOBJ)redBrush);
		}
		else {
			oldObj = SelectObject(hdc, (HGDIOBJ)blackBrush);
		}
		drawCell(hdc, snakeData[i]);
	}
	SelectObject(hdc, oldObj);

	oldObj = SelectObject(hdc, (HGDIOBJ)foodBrush);
	drawCell(hdc, foodData);
	SelectObject(hdc, oldObj);

	DeleteObject((HGDIOBJ)redBrush);
	DeleteObject((HGDIOBJ)foodBrush);
}

void drawCell(HDC hdc, Position& p) {
	int x = START_X + p.x * CELL_SIZE;
	int y = START_Y + p.y * CELL_SIZE;
	Rectangle(hdc, x, y, x + CELL_SIZE, y + CELL_SIZE);
}

int updateBoard() {
	RECT boardRect = {
		START_X,
		START_Y,
		START_X + CELL_SIZE * BOARD_SIZE,
		START_Y + CELL_SIZE * BOARD_SIZE,
	};

	for (int i = score+INIT_LENGTH; i > 0; i--) {
		snakeData[i] = snakeData[i - 1];
	}
	switch (direction) {
		case DIR_E:
			snakeData[0].x += 1;
			break;
		case DIR_S:
			snakeData[0].y += 1;
			break;
		case DIR_W:
			snakeData[0].x -= 1;
			break;
		case DIR_N:
			snakeData[0].y -= 1;
		default:
			break;
	}

	if (snakeData[0].x >= BOARD_SIZE || snakeData[0].x < 0 || snakeData[0].y >= BOARD_SIZE || snakeData[0].y < 0) {
		return RESULT_WALL;
	}

	for (int i = 0; i < score + INIT_LENGTH - 1; i++) {
		for (int j = i + 1; j < score + INIT_LENGTH; j++) {
			if (snakeData[i].x == snakeData[j].x && snakeData[i].y == snakeData[j].y) {
				return RESULT_SELF;
			}
		}
	}

	if (snakeData[0].x == foodData.x && snakeData[0].y == foodData.y) {
		return RESULT_FOOD;
	}

	InvalidateRect(g_hMain, &boardRect, TRUE);
	return RESULT_OK;
}

void generateFood() {

	do {
		foodData.x = rangeRandom(0, BOARD_SIZE);
		foodData.y = rangeRandom(0, BOARD_SIZE);
	} while (!isFoodPositionOK());
}

void gainFood() {
	generateFood();
	score += 1;
}

BOOL isFoodPositionOK() {
	for (int i = 0; i < score + INIT_LENGTH; i++) {
		if (snakeData[i].x == foodData.x && snakeData[i].y == foodData.y) {
			return FALSE;
		}
	}
	return TRUE;
}

void changeDir(int dir) {
	if ((direction == DIR_E && dir == DIR_W) ||
		(direction == DIR_W && dir == DIR_E) ||
		(direction == DIR_S && dir == DIR_N) ||
		(direction == DIR_N && dir == DIR_S)) {
		return;
	} 
	direction = dir;
}

int rangeRandom(int begin, int end) {
	return begin + (int)floor(rand() % (end-begin-1));
}
