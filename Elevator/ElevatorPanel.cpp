#include <iostream>
#include <Windows.h>
#include <assert.h>
#include <graphics.h>
#include <conio.h>
#include "ElevatorPanel.h"

int mLow, mHigh, mColumns, mRow;
int mButtonLength = 60, mPadding = 10;
int* indicatorRect;
int** buttonRect;
RECT indicator;
RECT* button;
bool event[33] = { 0 };//如果有地下室，则前三个为地下室，event[3]开始才是一楼；否则event[0]即是一楼

void test() {
	int winWidth = 480;
	int winHeight = 360;
	initgraph(winWidth, winHeight);
	//====
	for (int i = 0; i < 256; i++) {
		setbkcolor(RGB(i, i, i));
		cleardevice();
		Sleep(1);
	}
	FlushMouseMsgBuffer();//清空鼠标事件缓冲区
	int rect[] = { 20, 40, 60, 60 };
	RECT r = { rect[0], rect[1], rect[2], rect[3] };//矩形框
	LOGFONT f;//字体
	gettextstyle(&f);//获取字体
	f.lfQuality = ANTIALIASED_QUALITY;//设置字体抗锯齿
	settextcolor(BLACK);//设置字体颜色为黑色
	drawtext(_T("测试"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//在矩形中绘制字体
	setlinecolor(BLACK);//设置形状线条的颜色
	rectangle(rect[0], rect[1], rect[2], rect[3]);//绘制形状
	MOUSEMSG m;//鼠标事件
	bool isIn = 0;
	while (true) {
		m = GetMouseMsg();//获得当前的鼠标事件
		if (m.uMsg == WM_LBUTTONUP) {//如果事件为左键抬起
			setrop2(R2_NOTXORPEN);//设置形状叠加时二元光栅：NOT（当前颜色 XOR 背景颜色）
			for (int i = 0; i < 11; i++) {
				setlinecolor(RGB(25 * i, 25 * i, 25 * i));
				circle(m.x, m.y, 2 * i);//参数为圆心x、y，和半径
				Sleep(15);
				circle(m.x, m.y, 2 * i);
			}
			FlushMouseMsgBuffer();//清空鼠标事件缓冲区，防止事件堆积
		}
		else if (m.uMsg == WM_MOUSEMOVE) {
			setrop2(R2_XORPEN);
			setlinecolor(LIGHTCYAN);//亮青色
			setlinestyle(PS_SOLID, 3);//实线，宽度3
			setfillcolor(WHITE);//填充色白色
			if (m.x > rect[0] && m.x < rect[2] && m.y > rect[1] && m.y < rect[3] && !isIn) {
				fillrectangle(rect[0], rect[1], rect[2], rect[3]);
				isIn = 1;
			}
			else if (!(m.x > rect[0] && m.x < rect[2] && m.y > rect[1] && m.y < rect[3]) && isIn) {
				fillrectangle(rect[0], rect[1], rect[2], rect[3]);
				isIn = 0;
			}
		}
	}
	//====
	system("pause");
	closegraph();
}

void setButtonStyle(int buttonLength, int padding) {
	mButtonLength = buttonLength;
	mPadding = padding;
}

void setColumnsAndRow(int& columns, int& row, int low, int high) {
	columns = 2;
	row = 0;
	//如果楼层高度大于20，则按钮为三列
	if (high > 20) columns = 3;
	//计算按钮行数
	if (high % columns == 0) row = high / columns;
	else row = row = high / columns + 1;
	if (low != 0) row++;
}

void initWin(int low, int high) {
	mLow = low;
	mHigh = high;
	setColumnsAndRow(mColumns, mRow, low, high);
	//生成窗口
	initgraph(mPadding + mColumns * (mPadding + mButtonLength), 2 * mPadding + mButtonLength * 2 + 11 * (mPadding + mButtonLength));
	for (int i = 0; i * 8 < 256; i++) {
		setbkcolor(RGB(i * 8, i * 8, i * 8));
		cleardevice();
		Sleep(1);
	}
	setbkcolor(RGB(255, 255, 255));
}

void initView() {
	//绘制楼层显示器
	indicatorRect = (int*)malloc(4 * sizeof(int));
	assert(indicatorRect);
	indicator.left = indicatorRect[0] = mPadding;
	indicator.top = indicatorRect[1] = mPadding;
	indicator.right = indicatorRect[2] = mColumns * (mPadding + mButtonLength);
	indicator.bottom = indicatorRect[3] = mPadding + 2 * mButtonLength;
	setlinecolor(BLACK);
	rectangle(indicatorRect[0], indicatorRect[1], indicatorRect[2], indicatorRect[3]);
	settextcolor(BLACK);
	drawNumber(1, indicator);

	//绘制楼层按钮
	buttonRect = (int**)malloc(mColumns * mRow * sizeof(int*));//格子数数从右下角开始，从左往右，从下往上
	button = (RECT*)malloc(mColumns * mRow * sizeof(RECT));
	assert(buttonRect && button);
	for (int i = 0, j = 0, k = mRow - 1; i < mColumns * mRow; i++) {
		buttonRect[i] = (int*)malloc(4 * sizeof(int));
		assert(buttonRect[i]);
		button[i].left = buttonRect[i][0] = mPadding + (mPadding + mButtonLength) * j;
		button[i].top = buttonRect[i][1] = indicatorRect[3] + (mPadding + mButtonLength) * k + mPadding;
		button[i].right = buttonRect[i][2] = buttonRect[i][0] + mButtonLength;
		button[i].bottom = buttonRect[i][3] = buttonRect[i][1] + mButtonLength;
		if (++j % mColumns != 0) continue;
		j = 0;
		k--;
	}

	for (int i = 0, j = -1, k = 1, number; i < mColumns * mRow; i++) {
		if (i < mColumns && mLow != 0 && i > -1 * mLow - 1) continue;
		if (i >= mHigh + 3 && mLow != 0) break;
		if (i >= mHigh && mLow == 0) break;
		rectangle(buttonRect[i][0], buttonRect[i][1], buttonRect[i][2], buttonRect[i][3]);

		if (i < mColumns && mLow != 0)
			drawNumber(j--, button[i]);
		else
			drawNumber(k++, button[i]);
	}
}
void initListener() {
	ExMessage m;
	while (true) {
		m = getmessage(EX_MOUSE | EX_KEY);
		switch (m.message) {
		case WM_LBUTTONDOWN:
			buttonDown(m);
			break;
		case WM_KEYDOWN://Esc退出
			if (m.vkcode == VK_ESCAPE) return;
		}
	}
}

void buttonDown(ExMessage m) {
	//处理事件
	setrop2(R2_XORPEN);//XOR
	setlinecolor(LIGHTCYAN);//亮青色
	setlinestyle(PS_SOLID, 3);//实线，宽度3
	setfillcolor(WHITE);//填充色白色
	int click = getClickButton(m.x, m.y);
	if (click == -1) return;
	if (!event[click]) {
		event[click] = 1;
		fillrectangle(buttonRect[click][0], buttonRect[click][1], buttonRect[click][2], buttonRect[click][3]);
	}
	
	//绘制点击波纹
	setrop2(R2_NOTXORPEN);//NOT XOR
	for (int i = 0; i < 11; i++) {
		setlinecolor(RGB(25 * i, 25 * i, 25 * i));
		circle(m.x, m.y, 2 * i);
		Sleep(8);
		circle(m.x, m.y, 2 * i);
	}
	FlushMouseMsgBuffer();
}

void drawNumber(int number, RECT& r) {
	switch (number) {
	case -3: drawtext(_T("-3"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case -2: drawtext(_T("-2"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case -1: drawtext(_T("-1"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case 1: drawtext(_T("1"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case 2: drawtext(_T("2"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case 3: drawtext(_T("3"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case 4: drawtext(_T("4"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case 5: drawtext(_T("5"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case 6: drawtext(_T("6"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case 7: drawtext(_T("7"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case 8: drawtext(_T("8"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
	case 9: drawtext(_T("9"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
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

int getClickButton(int x, int y) {
	for (int i = 0; i < mColumns * mRow; i++) {
		if (i < mColumns && mLow != 0 && i > -1 * mLow - 1) continue;
		if (i >= mHigh + 3 && mLow != 0) break;
		if (i >= mHigh && mLow == 0) break;
		if (x > buttonRect[i][0] && y > buttonRect[i][1] && x < buttonRect[i][2] && y < buttonRect[i][3]) return i;
	}
	return -1;
}