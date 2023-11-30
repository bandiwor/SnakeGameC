#include <Windows.h>
#include <stdio.h>
#include <time.h>
#include <math.h>


HINSTANCE g_hInstance;

#define WM_SPEED_CHANGE (WM_USER + 1)
#define WM_MAP_SIZE_CHANGE (WM_USER + 2)

#define UPDATE_TIMER_ID 3001

#define LOW_SPEED_INTERVAL 1000
#define MEDIUM_SPEED_INTERVAL 600
#define NORMAL_SPEED_INTERVAL 400
#define GOOD_SPEED_INTERVAL 300
#define FAST_SPEED_INTERVAL 200
#define CRAZY_SPEED_INTERVAL 100
#define ZXC_SPEED_INTERVAL 50
#define UNREAL_SPEED_INTERVAL 25

#define SMALL_MAP_SIZE 5
#define MEDIUM_MAP_SIZE 10
#define BIG_MAP_SIZE 20
#define LARGE_MAP_SIZE 30
#define TENXL_MAP_SIZE 75

#define LOW_SPEED_NAME L"Низкая"
#define MEDIUM_SPEED_NAME L"Средняя"
#define NORMAL_SPEED_NAME L"Нормальная"
#define GOOD_SPEED_NAME L"Хорошая"
#define FAST_SPEED_NAME L"Быстрая"
#define CRAZY_SPEED_NAME L"Сумасшедшая"
#define ZXC_SPEED_NAME L"ZXC"
#define UNREAL_SPEED_NAME L"НЕВОЗМОЖНО"

#define SMALL_MAP_SIZE_NAME L"Маленькая"
#define MEDIUM_MAP_SIZE_NAME L"Средняя"
#define BIG_MAP_SIZE_NAME L"Большая"
#define LARGE_MAP_SIZE_NAME L"Огромная"
#define TENXL_MAP_SIZE_NAME L"10XL!"

typedef enum {
	ID_SPEED_CHANGE = 5100,
	ID_SPEED_LOW,
	ID_SPEED_MEDIUM,
	ID_SPEED_NORMAL,
	ID_SPEED_GOOD,
	ID_SPEED_FAST,
	ID_SPEED_CRAZY,
	ID_SPEED_ZXC,
	ID_SPEED_UNREAL

} GAME_SPEED_MENU_ID;

typedef enum {
	ID_MAP_SIZE_CHANGE = 5200,
	ID_MAP_SIZE_SMALL,
	ID_MAP_SIZE_MEDIUM,
	ID_MAP_SIZE_BIG,
	ID_MAP_SIZE_LARGE,
	ID_MAP_SIZE_10XL,

} MAP_SIZE_MENU_ID;

#define ID_COMMAND_HELP 6000

#define MSG(format, value) { WCHAR buffer[512]; wsprintf(buffer, format, value); MessageBox(NULL, buffer, buffer, NULL); }
#define MSG2(format, v1, v2) { WCHAR buffer[512]; wsprintf(buffer, format, v1, v2); MessageBox(NULL, buffer, buffer, NULL); }
#define GET_BODY_CEIL_X(value) ((value) >> 8)
#define GET_BODY_CEIL_Y(value) ((value) % 256)
#define MAKE_BODY_CEIL_COORD(X, Y) ((X << 8) | (Y))
#define MAKE_MAP_SIZE_DWORD(WIDTH, HEIGHT) (((WIDTH) << 16) | (HEIGHT))

#define GAME_OVER_TEXT_TEMPLATE (L"Game over! Скорость: %s. Карта: %s. Счёт: %d. Рекорд: %d. Нажмите ПРОБЕЛ для повтора. Escape чтобы выйти.")
#define GAME_PAUSE_TEXT_TEMPLATE (L"Пауза. Нажмите пробел, чтобы продолджить. (Скорость: %s, Счёт: %d, Рекорд: %d, Карта: %s)")
#define GAME_PLAYING_TEXT_TEMPLATE (L"%s. Очки: %d")




typedef enum {
	GS_PLAYING,
	GS_PAUSE,
	GS_GAME_OVER,

} GAME_STATE;

typedef enum {
	SR_UP,
	SR_DOWN,
	SR_LEFT,
	SR_RIGHT,

} SNAKE_ROTATION;


void appendGameMenu(HWND hwnd) {
	HMENU hMenu = CreateMenu();
	HMENU hSubSpeedMenu = CreatePopupMenu();
	HMENU hSubMapSizeMenu = CreatePopupMenu();

	AppendMenu(hSubSpeedMenu, MF_STRING, ID_SPEED_LOW, LOW_SPEED_NAME);
	AppendMenu(hSubSpeedMenu, MF_STRING, ID_SPEED_MEDIUM, MEDIUM_SPEED_NAME);
	AppendMenu(hSubSpeedMenu, MF_STRING, ID_SPEED_NORMAL, NORMAL_SPEED_NAME);
	AppendMenu(hSubSpeedMenu, MF_STRING, ID_SPEED_GOOD, GOOD_SPEED_NAME);
	AppendMenu(hSubSpeedMenu, MF_STRING, ID_SPEED_FAST, FAST_SPEED_NAME);
	AppendMenu(hSubSpeedMenu, MF_STRING, ID_SPEED_CRAZY, CRAZY_SPEED_NAME);
	AppendMenu(hSubSpeedMenu, MF_STRING, ID_SPEED_ZXC, ZXC_SPEED_NAME);
	AppendMenu(hSubSpeedMenu, MF_STRING, ID_SPEED_UNREAL, UNREAL_SPEED_NAME);

	AppendMenu(hSubMapSizeMenu, MF_STRING, ID_MAP_SIZE_SMALL, SMALL_MAP_SIZE_NAME);
	AppendMenu(hSubMapSizeMenu, MF_STRING, ID_MAP_SIZE_MEDIUM, MEDIUM_MAP_SIZE_NAME);
	AppendMenu(hSubMapSizeMenu, MF_STRING, ID_MAP_SIZE_BIG, BIG_MAP_SIZE_NAME);
	AppendMenu(hSubMapSizeMenu, MF_STRING, ID_MAP_SIZE_LARGE, LARGE_MAP_SIZE_NAME);
	AppendMenu(hSubMapSizeMenu, MF_STRING, ID_MAP_SIZE_10XL, TENXL_MAP_SIZE_NAME);

	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hSubSpeedMenu, L"Скорость");
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hSubMapSizeMenu, L"Размер карты");
	AppendMenu(hMenu, MF_STRING, (UINT_PTR)ID_COMMAND_HELP, L"Помощь");
	SetMenu(hwnd, hMenu);
}

unsigned short getNewAppleCoord(unsigned short* body, unsigned int bodyLength, unsigned short countCellX, unsigned short countCellY, unsigned short headCoord) {
	if (bodyLength == 0) {
		while (TRUE) {
			unsigned short newCoord = MAKE_BODY_CEIL_COORD(rand() % countCellX, rand() % countCellY);
			if (headCoord != newCoord) return newCoord;
		}
	}

	while (TRUE)
	{
		unsigned short newCoord = MAKE_BODY_CEIL_COORD(rand() % countCellX, rand() % countCellY);
		//MSG2(L"%d %d", GET_BODY_CEIL_X(newCoord), GET_BODY_CEIL_Y(newCoord));
		
		unsigned char newAppleInSnake = 0;
		for (unsigned int i = 0; i < bodyLength; i++)
		{
			if (newCoord == body[i]) {
				newAppleInSnake = 1;
			}
		}

		if (!newAppleInSnake && headCoord != newCoord)
			return newCoord;
	}
}

void moveBody(unsigned short* body, unsigned int bodyLength, unsigned short oldHeadCoord) {
	if (bodyLength == 0) return;

	for (unsigned int i = bodyLength - 1; i > 0; --i)
		body[i] = body[i - 1];

	body[0] = oldHeadCoord;
}

void appendBodyCeil(unsigned short* body, unsigned int bodyLengthBeforeAppend, unsigned short headCoords, SNAKE_ROTATION snakeRotation) {
	if (bodyLengthBeforeAppend == 0) {
		switch (snakeRotation)
		{
		case SR_UP:
			body[0] = MAKE_BODY_CEIL_COORD(GET_BODY_CEIL_X(headCoords), GET_BODY_CEIL_Y(headCoords) + 1);
			break;
		case SR_DOWN:
			body[0] = MAKE_BODY_CEIL_COORD(GET_BODY_CEIL_X(headCoords), GET_BODY_CEIL_Y(headCoords) - 1);
			break;
		case SR_LEFT:
			body[0] = MAKE_BODY_CEIL_COORD(GET_BODY_CEIL_X(headCoords) + 1, GET_BODY_CEIL_Y(headCoords));
			break;
		case SR_RIGHT:
			body[0] = MAKE_BODY_CEIL_COORD(GET_BODY_CEIL_X(headCoords) - 1, GET_BODY_CEIL_Y(headCoords));
			break;
		}
		return;
	}

	unsigned short lastElementCoord, previousLastElementCoord;
	if (bodyLengthBeforeAppend == 1)
	{
		lastElementCoord = body[0];
		previousLastElementCoord = headCoords;
	}
	else {
		lastElementCoord = body[bodyLengthBeforeAppend - 1];
		previousLastElementCoord = body[bodyLengthBeforeAppend - 2];
	}

	unsigned short dx = GET_BODY_CEIL_X(lastElementCoord) - GET_BODY_CEIL_X(previousLastElementCoord); // Изменение положения по x между последним и предпоследним элементом тела
	unsigned short dy = GET_BODY_CEIL_Y(lastElementCoord) - GET_BODY_CEIL_Y(previousLastElementCoord); // Изменение положения по y между последним и предпоследним элементом тела

	body[bodyLengthBeforeAppend] = MAKE_BODY_CEIL_COORD(
		dx + GET_BODY_CEIL_X(lastElementCoord),
		dy + GET_BODY_CEIL_Y(lastElementCoord)
	);
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	static unsigned short headCoord = 0; // 8bit - x, 8bit - y
	static unsigned short* body = NULL; // forEach => 8bit - x, 8bit - y
	static unsigned int bodyLength = 0;
	static GAME_STATE gameState = GS_PLAYING;
	static SNAKE_ROTATION snakeRotation = SR_RIGHT;

	static unsigned short appleCoord = 0; // 8bit - x, 8bit - y

	static unsigned short countCellX = MEDIUM_MAP_SIZE;
	static unsigned short countCellY = MEDIUM_MAP_SIZE;
	static float sizeCellX = 0;
	static float sizeCellY = 0;
	
	static RECT clientRect = { 0, 0, 800, 600 };

	static HBRUSH headBrush = NULL;
	static HBRUSH bodyBrush = NULL;
	static HBRUSH appleBrush = NULL;
	static HBRUSH backgroundBrush = NULL;

	static unsigned char rotationNotApply = 0;
	static unsigned int score = 0;
	static unsigned int highScore = 0;

	static int timerInterval = NORMAL_SPEED_INTERVAL;
	static UINT_PTR timerId = NULL;

	static WCHAR textBuffer[128] = L"";

	static WCHAR* currentSpeedName = NULL;
	static WCHAR* currentMapSizeName = NULL;

	switch (msg) {
		
	case WM_CREATE:
	{
		// Init body
		body = (unsigned short*) malloc((countCellX * countCellY - 1) * sizeof(unsigned short));
		if (body == NULL)
		{
			MessageBox(hwnd, L"Error with allocate memory for \"body\".", L"Error", MB_ICONERROR | MB_OK);
			DestroyWindow(hwnd);
			return;
		}

		for (unsigned int i = 0; i < countCellX * countCellY - 1; i++)
			body[i] = 0;
		// End init body
		
		GetClientRect(hwnd, &clientRect);

		//GetClientRect(hwnd, &clientRect);
		sizeCellX = (float)(clientRect.right - clientRect.left) / countCellX;
		sizeCellY = (float)(clientRect.bottom - clientRect.top) / countCellY;

		appleCoord = MAKE_BODY_CEIL_COORD(countCellX / 2, countCellY / 2);

		// Create brushes
		headBrush = CreateSolidBrush(RGB(0, 200, 0));
		bodyBrush = CreateSolidBrush(RGB(20, 180, 0));
		appleBrush = CreateSolidBrush(RGB(220, 10, 5));
		backgroundBrush = CreateSolidBrush(RGB(0, 0, 0));

		timerId = SetTimer(hwnd, UPDATE_TIMER_ID, timerInterval, NULL);
		currentSpeedName = NORMAL_SPEED_NAME;
		currentMapSizeName = MEDIUM_MAP_SIZE_NAME;
 	}
		break;
		
		
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		FillRect(hdc, &clientRect, backgroundBrush);
		
		switch (gameState)
		{
		case GS_PLAYING:
			// Draw head
			SelectObject(hdc, headBrush);
			Rectangle(hdc, roundf(sizeCellX * GET_BODY_CEIL_X(headCoord)), roundf(sizeCellY * GET_BODY_CEIL_Y(headCoord)), roundf(sizeCellX * GET_BODY_CEIL_X(headCoord) + sizeCellX), roundf(sizeCellY * GET_BODY_CEIL_Y(headCoord) + sizeCellY));

			// Draw body
			SelectObject(hdc, bodyBrush);
			for (unsigned int i = 0; i < bodyLength; ++i)
				Rectangle(hdc, roundf(sizeCellX * GET_BODY_CEIL_X(body[i])), roundf(sizeCellY * GET_BODY_CEIL_Y(body[i])), roundf(sizeCellX * GET_BODY_CEIL_X(body[i]) + sizeCellX), roundf(sizeCellY * GET_BODY_CEIL_Y(body[i]) + sizeCellY));

			// Draw apple
			SelectObject(hdc, appleBrush);
			Ellipse(hdc, roundf(sizeCellX * GET_BODY_CEIL_X(appleCoord)), roundf(sizeCellY * GET_BODY_CEIL_Y(appleCoord)), roundf(sizeCellX * GET_BODY_CEIL_X(appleCoord) + sizeCellX), roundf(sizeCellY * GET_BODY_CEIL_Y(appleCoord) + sizeCellY));

			SetBkColor(hdc, RGB(0, 0, 0));
			SetTextColor(hdc, RGB(255, 255, 255));

			wsprintf(textBuffer, GAME_PLAYING_TEXT_TEMPLATE, currentSpeedName, score);
			DrawText(hdc, textBuffer, -1, &clientRect, DT_CENTER | DT_VCENTER | DT_WORDBREAK);
			break;
		case GS_GAME_OVER:
			SetBkColor(hdc, RGB(0,0,0));
			SetTextColor(hdc, RGB(255,255,255));
			wsprintf(textBuffer, GAME_OVER_TEXT_TEMPLATE, currentSpeedName, currentMapSizeName, score, highScore);
			DrawText(hdc, textBuffer, -1, &clientRect, DT_CENTER | DT_VCENTER | DT_WORDBREAK);
			break;

		case GS_PAUSE:
			SetBkColor(hdc, RGB(0, 0, 0));
			SetTextColor(hdc, RGB(255, 255, 255));
			wsprintf(textBuffer, GAME_PAUSE_TEXT_TEMPLATE, currentSpeedName, score, highScore, currentMapSizeName);
			DrawText(hdc, textBuffer, -1, &clientRect, DT_CENTER | DT_VCENTER | DT_WORDBREAK);
			break;
		}
		
		EndPaint(hwnd, &ps);
	}
		break;
		
	case WM_COMMAND:
		switch (LOWORD(wparam))
		{
			// CHANGE SPEED
		case ID_SPEED_LOW:
			timerInterval = LOW_SPEED_INTERVAL;
			PostMessage(hwnd, WM_SPEED_CHANGE, LOW_SPEED_INTERVAL, LOW_SPEED_NAME);
			break;
		case ID_SPEED_MEDIUM:
			timerInterval = MEDIUM_SPEED_INTERVAL;
			PostMessage(hwnd, WM_SPEED_CHANGE, MEDIUM_SPEED_INTERVAL, MEDIUM_SPEED_NAME);
			break;
		case ID_SPEED_NORMAL:
			timerInterval = NORMAL_SPEED_INTERVAL;
			PostMessage(hwnd, WM_SPEED_CHANGE, NORMAL_SPEED_INTERVAL, NORMAL_SPEED_NAME);
			break;
		case ID_SPEED_GOOD:
			timerInterval = GOOD_SPEED_INTERVAL;
			PostMessage(hwnd, WM_SPEED_CHANGE, GOOD_SPEED_INTERVAL, GOOD_SPEED_NAME);
			break;
		case ID_SPEED_FAST:
			timerInterval = FAST_SPEED_INTERVAL;
			PostMessage(hwnd, WM_SPEED_CHANGE, FAST_SPEED_INTERVAL, FAST_SPEED_NAME);
			break;
		case ID_SPEED_CRAZY:
			timerInterval = CRAZY_SPEED_INTERVAL;
			PostMessage(hwnd, WM_SPEED_CHANGE, CRAZY_SPEED_INTERVAL, CRAZY_SPEED_NAME);
			break;
		case ID_SPEED_ZXC:
			timerInterval = ZXC_SPEED_INTERVAL;
			PostMessage(hwnd, WM_SPEED_CHANGE, ZXC_SPEED_INTERVAL, ZXC_SPEED_NAME);
			break;
		case ID_SPEED_UNREAL:
			timerInterval = UNREAL_SPEED_INTERVAL;
			PostMessage(hwnd, WM_SPEED_CHANGE, UNREAL_SPEED_INTERVAL, UNREAL_SPEED_NAME);
			break;
		// END CHANGE SPEED

		// CHANGE MAP SIZE
		case ID_MAP_SIZE_SMALL:
			PostMessage(hwnd, WM_MAP_SIZE_CHANGE, MAKE_MAP_SIZE_DWORD(SMALL_MAP_SIZE, SMALL_MAP_SIZE), SMALL_MAP_SIZE_NAME);
			break;
		case ID_MAP_SIZE_MEDIUM:
			PostMessage(hwnd, WM_MAP_SIZE_CHANGE, MAKE_MAP_SIZE_DWORD(MEDIUM_MAP_SIZE, MEDIUM_MAP_SIZE), MEDIUM_MAP_SIZE_NAME);
			break;
		case ID_MAP_SIZE_BIG:
			PostMessage(hwnd, WM_MAP_SIZE_CHANGE, MAKE_MAP_SIZE_DWORD(BIG_MAP_SIZE, BIG_MAP_SIZE), BIG_MAP_SIZE_NAME);
			break;
		case ID_MAP_SIZE_LARGE:
			PostMessage(hwnd, WM_MAP_SIZE_CHANGE, MAKE_MAP_SIZE_DWORD(LARGE_MAP_SIZE, LARGE_MAP_SIZE), LARGE_MAP_SIZE_NAME);
			break;
		case ID_MAP_SIZE_10XL:
			PostMessage(hwnd, WM_MAP_SIZE_CHANGE, MAKE_MAP_SIZE_DWORD(TENXL_MAP_SIZE, TENXL_MAP_SIZE), TENXL_MAP_SIZE_NAME);
			break;
		
		case ID_COMMAND_HELP:
			MessageBox(hwnd, L"Управление:\nВверх - [Стрелочка вверх, W]\nВниз - [Стрелочка вниз, S]\nВлево - [Стрелочка влево, A]\nВправо - [Стрелочка вправо, D]\nИзменить скорость: 0-9, NUMPAD 0-9.\nИзменить размер карты: Y, U, I, O, P - от меньшего к большему.\n", L"Помощь", MB_OK);
			break;

		// END CHANGE MAP SIZE
			
		}
		break;
	case WM_SPEED_CHANGE:
		KillTimer(hwnd, timerId);
		timerId = SetTimer(hwnd, UPDATE_TIMER_ID, wparam, NULL);
		currentSpeedName = lparam;

		InvalidateRect(hwnd, NULL, TRUE);
		break;

	case WM_MAP_SIZE_CHANGE:
		countCellX = HIWORD(wparam);
		countCellY = LOWORD(wparam);
		appleCoord = MAKE_BODY_CEIL_COORD(countCellX / 2, countCellY / 2);
		headCoord = 0;
		sizeCellX = (float)(clientRect.right - clientRect.left) / countCellX;
		sizeCellY = (float)(clientRect.bottom - clientRect.top) / countCellY;
		currentMapSizeName = lparam;
		score = 0;
		snakeRotation = SR_RIGHT;
		bodyLength = 0;
		if (body)
			free(body);
		body = (unsigned short*)malloc((countCellX * countCellY - 1) * sizeof(unsigned short));
		if (body == NULL)
		{
			MessageBox(hwnd, L"Error with allocate memory for \"body\".", L"Error", MB_ICONERROR | MB_OK);
			DestroyWindow(hwnd);
			return;
		}

		for (unsigned int i = 0; i < countCellX * countCellY - 1; ++i)
			body[i] = 0;

		InvalidateRect(hwnd, NULL, TRUE);

		break;
	
		
	case WM_TIMER:
	{
		switch (wparam)
		{
		case UPDATE_TIMER_ID:
		{
			if (gameState != GS_PLAYING)
				break;
			
			rotationNotApply = 0;
			unsigned short oldAppleCoord = headCoord;

			switch (snakeRotation)
			{
			case SR_UP:
				headCoord = MAKE_BODY_CEIL_COORD(GET_BODY_CEIL_X(headCoord), GET_BODY_CEIL_Y(headCoord) - 1);
				break;
			case SR_DOWN:
				headCoord = MAKE_BODY_CEIL_COORD(GET_BODY_CEIL_X(headCoord), GET_BODY_CEIL_Y(headCoord) + 1);
				break;
			case SR_LEFT:
				headCoord = MAKE_BODY_CEIL_COORD(GET_BODY_CEIL_X(headCoord) - 1, GET_BODY_CEIL_Y(headCoord));
				break;
			case SR_RIGHT:
				headCoord = MAKE_BODY_CEIL_COORD(GET_BODY_CEIL_X(headCoord) + 1, GET_BODY_CEIL_Y(headCoord));
				break;
			}
			moveBody(body, bodyLength, oldAppleCoord);

			if (GET_BODY_CEIL_X(headCoord) < 0 || GET_BODY_CEIL_X(headCoord) >= countCellX || GET_BODY_CEIL_Y(headCoord) < 0 || GET_BODY_CEIL_Y(headCoord) >= countCellY)
				gameState = GS_GAME_OVER;

			else if (headCoord == appleCoord) {
				appleCoord = getNewAppleCoord(body, bodyLength, countCellX, countCellY, headCoord);
				appendBodyCeil(body, bodyLength, headCoord, snakeRotation);
				++bodyLength;

				score += bodyLength * 1.5;
			}
			else {
				for (unsigned int i = 0; i < bodyLength; i++) {
					if (headCoord == body[i])
						gameState = GS_GAME_OVER;
				}
			}
			

			InvalidateRect(hwnd, NULL, TRUE);
		}
			break;
		}
	}
	break;

	case WM_KEYDOWN:
		switch (wparam)
		{
		case VK_NUMPAD1:
		case '1':
			PostMessage(hwnd, WM_SPEED_CHANGE, LOW_SPEED_INTERVAL, LOW_SPEED_NAME);
			break;
		case VK_NUMPAD2:
		case '2':
			PostMessage(hwnd, WM_SPEED_CHANGE, MEDIUM_SPEED_INTERVAL, MEDIUM_SPEED_NAME);
			break;
		case VK_NUMPAD3:
		case '3':
			PostMessage(hwnd, WM_SPEED_CHANGE, NORMAL_SPEED_INTERVAL, NORMAL_SPEED_NAME);
			break;
		case VK_NUMPAD4:
		case '4':
			PostMessage(hwnd, WM_SPEED_CHANGE, GOOD_SPEED_INTERVAL, GOOD_SPEED_NAME);
			break;
		case VK_NUMPAD5:
		case '5':
			PostMessage(hwnd, WM_SPEED_CHANGE, FAST_SPEED_INTERVAL, FAST_SPEED_NAME);
			break;
		case VK_NUMPAD6:
		case '6':
			PostMessage(hwnd, WM_SPEED_CHANGE, CRAZY_SPEED_INTERVAL, CRAZY_SPEED_NAME);
			break;
		case VK_NUMPAD7:
		case '7':
			PostMessage(hwnd, WM_SPEED_CHANGE, ZXC_SPEED_INTERVAL, ZXC_SPEED_NAME);
			break;
		case VK_NUMPAD8:
		case '8':
			PostMessage(hwnd, WM_SPEED_CHANGE, UNREAL_SPEED_INTERVAL, UNREAL_SPEED_NAME);
			break;
		
		case 'Y':
			PostMessage(hwnd, WM_MAP_SIZE_CHANGE, MAKE_MAP_SIZE_DWORD(SMALL_MAP_SIZE, SMALL_MAP_SIZE), SMALL_MAP_SIZE_NAME);
			break;
		case 'U':
			PostMessage(hwnd, WM_MAP_SIZE_CHANGE, MAKE_MAP_SIZE_DWORD(MEDIUM_MAP_SIZE, MEDIUM_MAP_SIZE), MEDIUM_MAP_SIZE_NAME);
			break;
		case 'I':
			PostMessage(hwnd, WM_MAP_SIZE_CHANGE, MAKE_MAP_SIZE_DWORD(BIG_MAP_SIZE, BIG_MAP_SIZE), BIG_MAP_SIZE_NAME);
			break;
		case 'O':
			PostMessage(hwnd, WM_MAP_SIZE_CHANGE, MAKE_MAP_SIZE_DWORD(LARGE_MAP_SIZE, LARGE_MAP_SIZE), LARGE_MAP_SIZE_NAME);
			break;
		case 'P':
			PostMessage(hwnd, WM_MAP_SIZE_CHANGE, MAKE_MAP_SIZE_DWORD(TENXL_MAP_SIZE, TENXL_MAP_SIZE), TENXL_MAP_SIZE_NAME);
			break;



		case VK_ESCAPE:
			if (gameState == GS_GAME_OVER)
				DestroyWindow(hwnd);

			break;
		case 'T':
		{
			KillTimer(hwnd, timerId);
			timerInterval -= 50;

			timerId = SetTimer(hwnd, UPDATE_TIMER_ID, timerInterval, NULL);
		}
			break;
		case VK_SPACE:
			switch (gameState)
			{
			case GS_PLAYING:
				gameState = GS_PAUSE;
				break;
			case GS_PAUSE:
				gameState = GS_PLAYING;
				break;
			case GS_GAME_OVER:
				gameState = GS_PLAYING;
				bodyLength = 0;
				for (unsigned int i = 0; i < countCellX * countCellY - 1; ++i)
					body[i] = 0;
				appleCoord = MAKE_BODY_CEIL_COORD(countCellX / 2, countCellY / 2);
				snakeRotation = SR_RIGHT;
				headCoord = 0;

				if (highScore < score)
					highScore = score;
				score = 0;
				break;
			}
			InvalidateRect(hwnd, NULL, TRUE);
			break;

		case VK_UP:
		case 'W':
			if (!rotationNotApply && snakeRotation != SR_DOWN) {
				snakeRotation = SR_UP;
				rotationNotApply = 1;
			}
			break;
		case VK_DOWN:
		case 'S':
			if (!rotationNotApply && snakeRotation != SR_UP)
			{
				snakeRotation = SR_DOWN;
				rotationNotApply = 1;
			}
			break;
		case VK_LEFT:
		case 'A':
			if (!rotationNotApply && snakeRotation != SR_RIGHT)
			{
				snakeRotation = SR_LEFT;
				rotationNotApply = 1;
			}
			break;
		case VK_RIGHT:
		case 'D':
			if (!rotationNotApply && snakeRotation != SR_LEFT)
			{
				snakeRotation = SR_RIGHT;
				rotationNotApply = 1;
			}
			break;
		}
		break;
		

	case WM_SIZE:
		sizeCellX = (float)LOWORD(lparam) / countCellX;
		sizeCellY = (float)HIWORD(lparam) / countCellY;
		clientRect.right = LOWORD(lparam);
		clientRect.bottom = HIWORD(lparam);

		break;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);

		DeleteObject(headBrush);
		DeleteObject(bodyBrush);
		DeleteObject(appleBrush);
		DeleteObject(backgroundBrush);
		free(body);

		break;
		
	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
		break;
	}
	return 0;
}


int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
	g_hInstance = hInstance;
	srand(time(NULL));
	
	WNDCLASSEX wnd = { 0 };
	wnd.lpfnWndProc = WndProc;
	wnd.lpszClassName = L"MainWindow";
	wnd.cbClsExtra = sizeof(wnd);
	wnd.hbrBackground = (COLOR_WINDOW + 1);
	wnd.style = CS_HREDRAW | CS_VREDRAW;
	wnd.cbSize = sizeof(wnd);
	
	if (!RegisterClassEx(&wnd)) {
		MessageBox(NULL, L"Cann't register wnd class.", L"Error", MB_ICONERROR);
		return 1;
	}

	HWND window = CreateWindowEx(WS_EX_CLIENTEDGE, L"MainWindow", L"Snake game", WS_OVERLAPPEDWINDOW, 100, 100, 800, 600, NULL, NULL, g_hInstance, NULL);
	if (window == NULL) {
		MessageBox(NULL, L"Cann't create window.", L"Error", MB_ICONERROR);
		return 1;
	}

	appendGameMenu(window);

	MSG msg = { 0 };
	ShowWindow(window, nCmdShow);
	UpdateWindow(window);

	while (GetMessage(&msg, window, NULL, NULL) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.message;
}

