#include <iostream>
#include <Windows.h>
#include <assert.h>
#include <graphics.h>
#include <conio.h>
#include "ElevatorPanel.h"

int mButtonLength = 60, mPadding = 10;
int* indicatorRect;
int** buttonRect;

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
	int columns, row;
	setColumnsAndRow(columns, row, low, high);
	//生成窗口
	initgraph(mPadding + columns * (mPadding + mButtonLength), 2 * mPadding + mButtonLength * 2 + 11 * (mPadding + mButtonLength));
	for (int i = 0; i * 8 < 256; i++) {
		setbkcolor(RGB(i * 8, i * 8, i * 8));
		cleardevice();
		Sleep(1);
	}
	setbkcolor(WHITE);
}

void initView(int low, int high) {
	int columns, row;
	setColumnsAndRow(columns, row, low, high);

	//绘制楼层显示器
	indicatorRect = (int*)malloc(4 * sizeof(int));
	assert(indicatorRect);
	indicatorRect[0] = mPadding;
	indicatorRect[1] = mPadding;
	indicatorRect[2] = columns * (mPadding + mButtonLength);
	indicatorRect[3] = mPadding + 2 * mButtonLength;
	setlinecolor(BLACK);
	rectangle(indicatorRect[0], indicatorRect[1], indicatorRect[2], indicatorRect[3]);
	//绘制楼层按钮

	buttonRect = (int**)malloc(columns * row * sizeof(int*));//格子数数从右下角开始，从左往右，从下往上
	assert(buttonRect);
	for (int i = 0, j = 0, k = row - 1; i < columns * row; i++) {
		buttonRect[i] = (int*)malloc(4 * sizeof(int));
		assert(buttonRect[i]);
		buttonRect[i][0] = mPadding + (mPadding + mButtonLength) * j;
		buttonRect[i][1] = indicatorRect[3] + (mPadding + mButtonLength) * k + mPadding;
		buttonRect[i][2] = buttonRect[i][0] + mButtonLength;
		buttonRect[i][3] = buttonRect[i][1] + mButtonLength;
		if (++j % columns != 0) continue;
		j = 0;
		k--;
	}
	for (int i = 0; i < high + 3; i++) {
		if (i < columns && low != 0 && i > -1 * low - 1) continue;
		rectangle(buttonRect[i][0], buttonRect[i][1], buttonRect[i][2], buttonRect[i][3]);
	}
}
void initListener(int low, int high) {
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
	setrop2(R2_NOTXORPEN);//设置形状叠加时二元光栅：NOT（当前颜色 XOR 背景颜色）
	for (int i = 0; i < 11; i++) {
		setlinecolor(RGB(25 * i, 25 * i, 25 * i));
		circle(m.x, m.y, 2 * i);//参数为圆心x、y，和半径
		Sleep(8);
		circle(m.x, m.y, 2 * i);
	}
	FlushMouseMsgBuffer();//清空鼠标事件缓冲区，防止事件堆积
}