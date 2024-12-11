#include <iostream>
#include <Windows.h>
#include <graphics.h>
#include <conio.h>
#include "ElevatorPanel.h"

void test();

int main() {
	test();
	return 0;
}

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
	RECT r = {rect[0], rect[1], rect[2], rect[3]};//矩形框
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