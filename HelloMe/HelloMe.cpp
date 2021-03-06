// https://github.com/pleed215/HelloApi

#include <windows.h>
#include <math.h>
#include <time.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ChildWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

HINSTANCE g_hInst;
HWND g_hMain=(HWND)NULL;
LPCTSTR lpszClass = L"HelloAPI";
LPCTSTR ChildClassName = L"ChildWin";


// 리소스 관련 ID
#define ID_BTN_START 1001 // 시작 버튼
#define ID_BTN_HELP 1002 // 도움말 버튼.
#define ID_BTN_HIGHSCORE 1003 // 최고 점수 확인.
#define IDT_TIMER1 1004 // 타이머 관련 id


// 게임 로직 관련된 선언들.
#define BOARD_SIZE 40
#define DIR_E 1
#define DIR_S 2
#define DIR_W 3
#define DIR_N 4
#define RESULT_OK 1
#define RESULT_FOOD 2
#define RESULT_WALL 3
#define RESULT_SELF 4
#define CELL_SIZE 14
#define INIT_LENGTH 3
#define GAME_STATE_STOP 0
#define GAME_STATE_PLAYING 1
#define GAME_STATE_END 2


// 게임 보드 index position.
typedef struct {
	int x, y;
} Position;

const int MAX_SCORE = BOARD_SIZE * 20; // max score. 하지만 max score 로직은 짜놓지 않음.
Position snakeData[MAX_SCORE+3];// 뱜 데이터
Position foodData; // 먹이 위치 저장
int score = 0;
int length = 3;
int speed = 1;
int direction = DIR_E;
int gameState = GAME_STATE_STOP;
int highscore = 0;
BOOL changingDirection = FALSE;
const int CONTENT_HEIGHT = 100;
const int WINDOW_WIDTH = CELL_SIZE * BOARD_SIZE + 40;
const int WINDOW_HEIGHT = CELL_SIZE * BOARD_SIZE + CONTENT_HEIGHT + 100;
const int START_X = 10;
const int START_Y = CONTENT_HEIGHT;

// 함수 프로토타입 선언.
void initBoard();
void renderBoard(HDC);
int updateBoard();
void changeDir(int dir);
void generateFood();
void gainFood();
BOOL isFoodPositionOK();
void drawCell(HDC, Position&);
void renderScore(HDC);
void invalidateScore();

// Utils
// 유틸함수. 선언... 은 랜덤밖에..
int rangeRandom(int begin, int end); // end는 포함안함.



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
	RegisterClass(&WndClass);     //메인윈도우 클래스 등록

	WndClass.lpfnWndProc = ChildWndProc;      //차일드 윈도우 프로시저
	WndClass.lpszClassName = ChildClassName; //차일드 윈도우 클래스이름
	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass,			//윈도우클래스 이름
		L"컴퓨터과학과 2학년",			    //윈도우타이틀
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,   //윈도우스타일
		20, 20,							//윈도우가 보일때 X Y좌표
		WINDOW_WIDTH, WINDOW_HEIGHT,							//윈도우의 폭과 높이				
		(HWND)NULL,							//부모윈도우 핸들
		(HMENU)NULL,						//윈도우가 가지는 메뉴핸들
		hInstance,							//인스턴스핸들
		NULL);								//여분의 데이터

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
	LPCTSTR text = L"이은덕 202234-243458";
	switch (iMessage) {
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		TextOut(hdc, 10, 10, text, lstrlen(text));
		renderScore(hdc);
		renderBoard(hdc);
		EndPaint(hWnd, &ps);
		return 0;
	}
	case WM_CREATE:
	{
		HWND hChildWnd = CreateWindow(
			ChildClassName,     		// 차일드 윈도우 클래스 이름 
			L"스네이크 게임",            	// 윈도우 타이틀 
			WS_CAPTION|WS_BORDER | WS_CHILD,  // 윈도우  스타일 
			320,       		// 윈도우 보일 때 x 좌표 
			10,       		// 윈도우 보일 때 y 좌표 
			250,       		// 윈도우 폭
			80,       		// 윈도우 높이
			hWnd,         		// 부모 윈도우
			(HMENU)1000,        	// 차일드 윈도우ID 
			g_hInst,           		// 인스턴스 핸들 
			(LPVOID)NULL);      	// 여분의 데이터

		if (!hChildWnd) 	return -1;
		CreateWindow(
			L"button",
			L"게임 시작",
			WS_CHILD | WS_VISIBLE,
			210, 10, 100, 30,
			hWnd,
			(HMENU)ID_BTN_START,
			g_hInst,
			(LPVOID)NULL);
			long seed = time(NULL);
		srand(seed);
		ShowWindow(hChildWnd, SW_SHOW);
		initBoard();
		return 0;
	}
	case WM_COMMAND:
		if (LOWORD(wParam) == ID_BTN_START) {
			initBoard();
			invalidateScore();
			gameState = GAME_STATE_PLAYING;
			SetFocus(hWnd);
			SetTimer(hWnd, IDT_TIMER1, 80, (TIMERPROC)NULL);
		}
		return 0;

	case WM_KEYDOWN:
		if (gameState == GAME_STATE_PLAYING&&!changingDirection) {
			changingDirection = TRUE;
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
						highscore = max(highscore, score);
						invalidateScore();
						MessageBox(hWnd, L"충돌감지", L"게임종료", MB_OK | MB_ICONERROR);
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
	switch (message)
	{
	case WM_CREATE:
		CreateWindow(L"button", L"도움말", WS_CHILD | WS_VISIBLE, 10, 1, 100, 35, hWnd, (HMENU)ID_BTN_HELP, g_hInst, NULL);
		CreateWindow(L"button", L"하이스코어", WS_CHILD | WS_VISIBLE, 120, 1, 100, 35, hWnd, (HMENU)ID_BTN_HIGHSCORE, g_hInst, NULL);
		return 0;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		return 0;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_BTN_HELP:
			MessageBox(hWnd, L"방향키 사용. (치트) space", L"스네이크 게임", MB_OK | MB_ICONASTERISK);
			break;
		case ID_BTN_HIGHSCORE:
			TCHAR s[100];
			wsprintf(s, L"하이스코어: %5d", highscore*100);
			MessageBox(hWnd, s, L"스네이크 게임", MB_OK | MB_ICONINFORMATION);
			break;
		}
		return 0;

	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

/*****************************************************************************/
/* 게임 코드                                                                 */
/*****************************************************************************/

// 보드 초기화
void initBoard() {
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
	changingDirection = FALSE;

	// 벽과 충돌 체크
	if (snakeData[0].x >= BOARD_SIZE || snakeData[0].x < 0 || snakeData[0].y >= BOARD_SIZE || snakeData[0].y < 0) {
		return RESULT_WALL;
	}

	// 자기 자신을 먹는지 체크
	for (int i = 0; i < score + INIT_LENGTH - 1; i++) {
		for (int j = i + 1; j < score + INIT_LENGTH; j++) {
			if (snakeData[i].x == snakeData[j].x && snakeData[i].y == snakeData[j].y) {
				return RESULT_SELF;
			}
		}
	}

	// 음식을 먹었는지 체크
	if (snakeData[0].x == foodData.x && snakeData[0].y == foodData.y) {
		return RESULT_FOOD;
	}

	// 보드 재렌더링.
	InvalidateRect(g_hMain, &boardRect, TRUE);
	// 아무일 없음
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
	invalidateScore();
}

void renderScore(HDC hdc) {
	TCHAR scoreStr[100];
	wsprintf(scoreStr, L"Score: %7d, highscore: %7d", score*100, highscore*100);
	TextOut(hdc, 10, 50, scoreStr, lstrlen(scoreStr));
}


// 혹시나 푸드가 스네이크 위에 있으면 안되니까.. 확인 차원에서..
BOOL isFoodPositionOK() {
	for (int i = 0; i < score + INIT_LENGTH; i++) {
		if (snakeData[i].x == foodData.x && snakeData[i].y == foodData.y) {
			return FALSE;
		}
	}
	return TRUE;
}


// 방향 전환. 반대 방향은 안댐.
void changeDir(int dir) {
	if ((direction == DIR_E && dir == DIR_W) ||
		(direction == DIR_W && dir == DIR_E) ||
		(direction == DIR_S && dir == DIR_N) ||
		(direction == DIR_N && dir == DIR_S)) {
		return;
	} 
	direction = dir;
}
void invalidateScore() {
	RECT scoreRect = {
		10, 50, 350, 70
	};
	InvalidateRect(g_hMain, &scoreRect, TRUE);
}
// 유틸함수.. 범위에 해당하는 난수 정수 발생.
int rangeRandom(int begin, int end) {
	return begin + (int)floor(rand() % (end-begin-1));
}
