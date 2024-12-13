#include <iostream>
#include <Windows.h>
#include <assert.h>
#include <graphics.h>
#include <conio.h>
#include <pthread.h>
#include "ElevatorPanel.h"

constexpr auto CLOSED = -1;
constexpr auto MOVING_STOP = 0;
constexpr auto MOVING_UP = 1;
constexpr auto MOVING_DOWN = 2;


pthread_t elevatorThread;

//==========Global variables

int mLow, mHigh, mColumns, mRow, mPanelWidth, mPanelHeight, mWindowWidth, mWindowHeight, mFloorNumer, mButtonLength = 60, mPadding = 10;
int** indicatorRect, ** buttonRect, ** upAndDownRect; int* floorCardRect;
RECT* indicator, * button, * upAndDown; RECT floorCard;
bool event[33] = { 0 };//����е��²㣬��ǰ����Ϊ���²�{-3, -2, -1}��event[3]��ʼ����һ¥������event[0]����һ¥
int status = MOVING_STOP;
bool isOutDoor = 1, isOpenDoor = 0, clickedUp = 0, clickedDown = 0;

//==============================Utils

void setButtonStyle(int buttonLength, int padding) {
	mButtonLength = buttonLength;
	mPadding = padding;
}

void setColumnsAndRow(int& columns, int& row, int low, int high) {
	columns = 2;
	row = 0;
	//���¥��߶ȴ���20����ťΪ����
	if (high > 20) columns = 3;
	//���㰴ť����
	if (high % columns == 0) row = high / columns;
	else row = row = high / columns + 1;
	if (low != 0) row++;
}

//����򵥴��룬����Ч��
void drawNumber(int number, RECT& r) {
	switch (number) {
	case -3: drawtext(_T("-3"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case -2: drawtext(_T("-2"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case -1: drawtext(_T("-1"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case 1: drawtext(_T(" 1"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case 2: drawtext(_T(" 2"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case 3: drawtext(_T(" 3"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case 4: drawtext(_T(" 4"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case 5: drawtext(_T(" 5"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case 6: drawtext(_T(" 6"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case 7: drawtext(_T(" 7"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case 8: drawtext(_T(" 8"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case 9: drawtext(_T(" 9"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case 10: drawtext(_T("10"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case 11: drawtext(_T("11"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case 12: drawtext(_T("12"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case 13: drawtext(_T("13"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case 14: drawtext(_T("14"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case 15: drawtext(_T("15"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case 16: drawtext(_T("16"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case 17: drawtext(_T("17"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case 18: drawtext(_T("18"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case 19: drawtext(_T("19"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case 20: drawtext(_T("20"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case 21: drawtext(_T("21"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case 22: drawtext(_T("22"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case 23: drawtext(_T("23"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case 24: drawtext(_T("24"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case 25: drawtext(_T("25"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case 26: drawtext(_T("26"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case 27: drawtext(_T("27"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case 28: drawtext(_T("28"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case 29: drawtext(_T("29"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case 30: drawtext(_T("30"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	default: drawtext(_T("null"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
}

void drawStatus(int status, RECT& r) {
	switch (status) {
	case MOVING_STOP: drawtext(_T("��ǰ"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case MOVING_UP: drawtext(_T("����"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case MOVING_DOWN: drawtext(_T("�½�"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	default: drawtext(_T("����"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	}
}

int getClickButton(int x, int y) {
	for (int i = 0; i < mColumns * mRow; i++) {
		if (i < mColumns && mLow != 0 && 3 - i > -1 * mLow) continue;
		if (i >= mHigh + 3 && mLow != 0) break;
		if (i >= mHigh && mLow == 0) break;
		if (x > buttonRect[i][0] && y > buttonRect[i][1] && x < buttonRect[i][2] && y < buttonRect[i][3]) return i;
	}
	return -1;
}

void setWindowSize(int& width, int& height, int columns, int row) {
	width = 5 * (columns * (mPadding + mButtonLength) + mPadding);
	height = mButtonLength * 2 + 11 * (mPadding + mButtonLength) + 2 * mPadding;
}
void setPanelSize(int& width, int& height, int columns, int row) {
	width = columns * (mPadding + mButtonLength) + mPadding;
	height = mButtonLength * 2 + 11 * (mPadding + mButtonLength) + 2 * mPadding;
}

//==================Elevator

void initWin(int low, int high) {
	mLow = low;
	mHigh = high;
	setColumnsAndRow(mColumns, mRow, low, high);
	setWindowSize(mWindowWidth, mWindowHeight, mColumns, mRow);
	setPanelSize(mPanelWidth, mPanelHeight, mColumns, mRow);
	mFloorNumer = 1;;
	//���ɴ���
	initgraph(mWindowWidth, mWindowHeight);
	for (int i = 0; i * 8 < 256; i++) {
		setbkcolor(RGB(i * 8, i * 8, i * 8));
		cleardevice();
		Sleep(1);
	}
	setbkcolor(RGB(255, 255, 255));
}

void initView() {
	//��ʼ��¥����ʾ��
	indicatorRect = (int**)malloc(4 * sizeof(int*));
	indicatorRect[0] = (int*)malloc(4 * sizeof(int));
	indicatorRect[1] = (int*)malloc(4 * sizeof(int));
	assert(indicatorRect && indicatorRect[0] && indicatorRect[1]);
	indicator = (RECT*)malloc(2 * sizeof(RECT));//��һ��������ʾ���֣��ڶ���������ʾ����״̬
	assert(indicator);
	indicator[0].left = indicatorRect[0][0] = mPadding + 4 * mPanelWidth;
	indicator[0].top = indicatorRect[0][1] = mPadding;
	indicator[0].right = indicatorRect[0][2] = mColumns * (mPadding + mButtonLength) + 4 * mPanelWidth;
	indicator[0].bottom = indicatorRect[0][3] = mPadding + mButtonLength;
	indicator[1].left = indicatorRect[1][0] = mPadding + 4 * mPanelWidth;
	indicator[1].top = indicatorRect[1][1] = mPadding + mButtonLength;
	indicator[1].right = indicatorRect[1][2] = mColumns * (mPadding + mButtonLength) + 4 * mPanelWidth;
	indicator[1].bottom = indicatorRect[1][3] = mPadding + 2 * mButtonLength;

	//��ʼ��¥�㰴ť
	buttonRect = (int**)malloc(mColumns * mRow * sizeof(int*));//�������������½ǿ�ʼ���������ң���������
	button = (RECT*)malloc(mColumns * mRow * sizeof(RECT));
	assert(buttonRect && button);
	for (int i = 0, j = 0, k = mRow - 1; i < mColumns * mRow; i++) {
		buttonRect[i] = (int*)malloc(4 * sizeof(int));
		assert(buttonRect[i]);
		button[i].left = buttonRect[i][0] = mPadding + (mPadding + mButtonLength) * j + 4 * mPanelWidth;
		button[i].top = buttonRect[i][1] = indicatorRect[1][3] + (mPadding + mButtonLength) * k + mPadding;
		button[i].right = buttonRect[i][2] = buttonRect[i][0] + mButtonLength;
		button[i].bottom = buttonRect[i][3] = buttonRect[i][1] + mButtonLength;
		if (++j % mColumns != 0) continue;
		j = 0;
		k--;
	}

	//��ʼ�����°�ť
	upAndDownRect = (int**)malloc(2 * sizeof(int*));
	upAndDown = (RECT*)malloc(2 * sizeof(RECT));
	assert(upAndDownRect && upAndDown);
	upAndDownRect[0] = (int*)malloc(4 * sizeof(int));
	upAndDownRect[1] = (int*)malloc(4 * sizeof(int));
	assert(upAndDownRect[0] && upAndDownRect[1]);
	upAndDown[0].left = upAndDownRect[0][0] = mPanelWidth * 4 + mPanelWidth / 2 - mButtonLength / 2;
	upAndDown[0].top = upAndDownRect[0][1] = mPanelHeight / 2 - mButtonLength - mPadding;
	upAndDown[0].right = upAndDownRect[0][2] = mPanelWidth * 4 + mPanelWidth / 2 + mButtonLength / 2;
	upAndDown[0].bottom = upAndDownRect[0][3] = upAndDown[0].top + mButtonLength;
	upAndDown[1].left = upAndDownRect[1][0] = mPanelWidth * 4 + mPanelWidth / 2 - mButtonLength / 2;
	upAndDown[1].top = upAndDownRect[1][1] = mPanelHeight / 2;
	upAndDown[1].right = upAndDownRect[1][2] = mPanelWidth * 4 + mPanelWidth / 2 + mButtonLength / 2;
	upAndDown[1].bottom = upAndDownRect[1][3] = upAndDown[1].top + mButtonLength;

	//��ʼ����ǰ¥����ʾ��
	floorCardRect = (int*)malloc(4 * sizeof(int));
	assert(floorCardRect);
	floorCard.left = floorCardRect[0] = mPanelWidth / 2 - mButtonLength;
	floorCard.top = floorCardRect[1] = mPanelHeight / 2 - mButtonLength;
	floorCard.right = floorCardRect[2] = mPanelWidth / 2 + mButtonLength;
	floorCard.bottom = floorCardRect[3] = mPanelHeight / 2 + mButtonLength;
}

void drawView() {
	//�������
	setbkcolor(RGB(255, 255, 255));
	cleardevice();

	//����¥����ʾ��
	setlinecolor(BLACK);
	rectangle(indicatorRect[0][0], indicatorRect[0][1], indicatorRect[1][2], indicatorRect[1][3]);
	settextcolor(BLACK);
	drawNumber(1, indicator[0]);
	drawStatus(MOVING_STOP, indicator[1]);

	line(mPanelWidth, 0, mPanelWidth, mWindowHeight);
	line(mPanelWidth * 4, 0, mPanelWidth * 4, mWindowHeight);
	line(mPanelWidth, mButtonLength, mPanelWidth * 4, mButtonLength);
	line(mWindowWidth / 2, mButtonLength, mWindowWidth / 2, mWindowHeight);

	if (isOutDoor) {
		rectangle(upAndDownRect[0][0], upAndDownRect[0][1], upAndDownRect[0][2], upAndDownRect[0][3]);
		rectangle(upAndDownRect[1][0], upAndDownRect[1][1], upAndDownRect[1][2], upAndDownRect[1][3]);
		settextcolor(BLACK);
		drawtext(_T("UP"), &upAndDown[0], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		drawtext(_T("DOWN"), &upAndDown[1], DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		rectangle(floorCardRect[0], floorCardRect[1], floorCardRect[2], floorCardRect[3]);
		drawNumber(mFloorNumer, floorCard);
	}
	else {
		//����¥�㰴ť
		for (int i = 0, j = -3, k = 1, number; i < mColumns * mRow; i++) {
			if (i < mColumns && mLow != 0 && 3 - i > -1 * mLow) continue;
			if (i >= mHigh + 3 && mLow != 0) break;
			if (i >= mHigh && mLow == 0) break;
			rectangle(buttonRect[i][0], buttonRect[i][1], buttonRect[i][2], buttonRect[i][3]);
			if (i < mColumns && mLow != 0)
				drawNumber(-1 * (3 - i), button[i]);
			else
				drawNumber(k++, button[i]);
			if (!event[i]) continue;
			setrop2(R2_XORPEN);//XOR
			setlinecolor(LIGHTCYAN);
			setlinestyle(PS_SOLID, 3);
			setfillcolor(WHITE);
			fillrectangle(buttonRect[i][0], buttonRect[i][1], buttonRect[i][2], buttonRect[i][3]);
		}
	}
}

void addListener() {
	if (pthread_create(&elevatorThread, NULL, elevatorMoving, NULL) != 0) return;//���̣߳�������
	ExMessage m;
	while (true) {
		m = getmessage(EX_MOUSE | EX_KEY);
		switch (m.message) {
		case WM_LBUTTONDOWN:
			buttonDown(m);
			break;
		case WM_KEYDOWN://Esc�˳�
			if (m.vkcode == VK_ESCAPE) {
				status = CLOSED;
				pthread_join(elevatorThread, NULL);//�𼱣����ҽ��������ȴ��߳̽�����
				return;
			}
		}
	}
}

void buttonDown(ExMessage m) {
	//�����¼�
	setrop2(R2_XORPEN);//XOR
	setlinecolor(LIGHTCYAN);//����ɫ
	setlinestyle(PS_SOLID, 3);//ʵ�ߣ����3
	setfillcolor(WHITE);//���ɫ��ɫ
	if (isOutDoor) {
		if (m.x >= upAndDownRect[0][0] && m.x <= upAndDownRect[0][2] && m.y >= upAndDownRect[0][1] && m.y <= upAndDownRect[0][3] && !clickedUp) {
			clickedUp = 1;
			fillrectangle(upAndDownRect[0][0], upAndDownRect[0][1], upAndDownRect[0][2], upAndDownRect[0][3]);
		}
		else if (m.x >= upAndDownRect[1][0] && m.x <= upAndDownRect[1][2] && m.y >= upAndDownRect[1][1] && m.y <= upAndDownRect[1][3] && !clickedDown) {
			clickedDown = 1;
			fillrectangle(upAndDownRect[1][0], upAndDownRect[1][1], upAndDownRect[1][2], upAndDownRect[1][3]);
		}
		if (mLow != 0) {
			event[mFloorNumer + 2] = 1;
		}
		else {
			event[mFloorNumer] = 1;
		}
	}
	else {
		int click = getClickButton(m.x, m.y);
		if (click == -1) return;
		if (!event[click]) {
			event[click] = 1;
			fillrectangle(buttonRect[click][0], buttonRect[click][1], buttonRect[click][2], buttonRect[click][3]);
		}
	}

	//���Ƶ������
	setrop2(R2_NOTXORPEN);//NOT XOR
	for (int i = 0; i < 11; i++) {
		setlinecolor(RGB(25 * i, 25 * i, 25 * i));
		circle(m.x, m.y, 2 * i);
		Sleep(8);
		circle(m.x, m.y, 2 * i);
	}
	FlushMouseMsgBuffer();

}

void* elevatorMoving(void* arg) {
	int currentEvent = 0, i, indicateNumber = 1;
	if (mLow != 0) currentEvent = 3;
	while (true) {
		Sleep(100);
		//�ж��Ƿ�ر�
		if (status == CLOSED) return NULL;

		if (event[currentEvent]) {
			//���Ű���
			setrop2(R2_BLACK);
			setlinecolor(BLACK);
			setlinestyle(PS_SOLID, 1);
			setfillcolor(WHITE);
			for (i = 0; i * 4 < (mPanelWidth * 3) / 2; i++) {
				fillrectangle(mWindowWidth / 2 - i * 4, mButtonLength, mWindowWidth / 2 + i * 4, mWindowHeight);
				Sleep(1);
			}
			fillrectangle(mPanelWidth, mButtonLength, mPanelWidth * 4, mWindowHeight);
			if (isOutDoor) {

			}
			else {

			}
		}

		if (status == MOVING_STOP) {//ֹͣ
			//��ʾֹͣ
			drawStatus(MOVING_STOP, indicator[1]);
			if (event[currentEvent]) {
				Sleep(1000);
				if (!isOutDoor) {
					setrop2(R2_XORPEN);//XOR
					setlinecolor(LIGHTCYAN);
					setlinestyle(PS_SOLID, 3);
					setfillcolor(WHITE);
					fillrectangle(buttonRect[currentEvent][0], buttonRect[currentEvent][1], buttonRect[currentEvent][2], buttonRect[currentEvent][3]);
				}
				event[currentEvent] = 0;
				continue;
			}
			//�ж��Ƿ����˰��°�ť
			for (i = 0; i < 33; i++) if (event[i]) {
				if (i < currentEvent) status = MOVING_DOWN;
				else if (i > currentEvent) status = MOVING_UP;
				break;
			}
		}
		else if (status == MOVING_UP) {//����
			//��ʾ����
			drawStatus(MOVING_UP, indicator[1]);
			currentEvent++;
			if (mLow == 0) indicateNumber = currentEvent + 1;
			else if (currentEvent >= 3) indicateNumber = currentEvent - 2;
			else indicateNumber = currentEvent - 3;
			Sleep(1000);
			drawNumber(indicateNumber, indicator[0]);
			//�ж�״̬
			assert(currentEvent < 33);
			if (!event[currentEvent]) continue;
			if (!isOutDoor) {//������ڵ����⣬���ò�����ť
				setrop2(R2_XORPEN);
				setlinecolor(LIGHTCYAN);
				setlinestyle(PS_SOLID, 3);
				setfillcolor(WHITE);
				fillrectangle(buttonRect[currentEvent][0], buttonRect[currentEvent][1], buttonRect[currentEvent][2], buttonRect[currentEvent][3]);
			}
			event[currentEvent] = 0;
			for (i = currentEvent; i < 33; i++) if (event[i]) break;
			if (i != 33) continue;
			for (i = currentEvent; i >= 0; i--) if (event[i]) {
				status = MOVING_DOWN;
				break;
			}
			if (i == -1) status = MOVING_STOP;
		}
		else if (status == MOVING_DOWN) {//�½�
			//��ʾ�½�
			drawStatus(MOVING_DOWN, indicator[1]);
			currentEvent--;
			if (mLow == 0) indicateNumber = currentEvent + 1;
			else if (currentEvent >= 3) indicateNumber = currentEvent - 2;
			else indicateNumber = currentEvent - 3;
			Sleep(1000);
			drawNumber(indicateNumber, indicator[0]);
			//�ж�״̬
			assert(currentEvent >= 0);
			if (!event[currentEvent]) continue;
			if (!isOutDoor) {//������ڵ����⣬���ò�����ť
				setrop2(R2_XORPEN);
				setlinecolor(LIGHTCYAN);
				setlinestyle(PS_SOLID, 3);
				setfillcolor(WHITE);
				fillrectangle(buttonRect[currentEvent][0], buttonRect[currentEvent][1], buttonRect[currentEvent][2], buttonRect[currentEvent][3]);
			}
			event[currentEvent] = 0;
			for (i = currentEvent; i >= 0; i--) if (event[i]) break;
			if (i != -1) continue;
			for (i = currentEvent; i < 33; i++) if (event[i]) {
				status = MOVING_UP;
				break;
			}
			if (i == 33) status = MOVING_STOP;
		}
	}
	return NULL;
}