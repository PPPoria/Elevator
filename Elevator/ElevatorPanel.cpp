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
	FlushMouseMsgBuffer();//�������¼�������
	int rect[] = { 20, 40, 60, 60 };
	RECT r = { rect[0], rect[1], rect[2], rect[3] };//���ο�
	LOGFONT f;//����
	gettextstyle(&f);//��ȡ����
	f.lfQuality = ANTIALIASED_QUALITY;//�������忹���
	settextcolor(BLACK);//����������ɫΪ��ɫ
	drawtext(_T("����"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//�ھ����л�������
	setlinecolor(BLACK);//������״��������ɫ
	rectangle(rect[0], rect[1], rect[2], rect[3]);//������״
	MOUSEMSG m;//����¼�
	bool isIn = 0;
	while (true) {
		m = GetMouseMsg();//��õ�ǰ������¼�
		if (m.uMsg == WM_LBUTTONUP) {//����¼�Ϊ���̧��
			setrop2(R2_NOTXORPEN);//������״����ʱ��Ԫ��դ��NOT����ǰ��ɫ XOR ������ɫ��
			for (int i = 0; i < 11; i++) {
				setlinecolor(RGB(25 * i, 25 * i, 25 * i));
				circle(m.x, m.y, 2 * i);//����ΪԲ��x��y���Ͱ뾶
				Sleep(15);
				circle(m.x, m.y, 2 * i);
			}
			FlushMouseMsgBuffer();//�������¼�����������ֹ�¼��ѻ�
		}
		else if (m.uMsg == WM_MOUSEMOVE) {
			setrop2(R2_XORPEN);
			setlinecolor(LIGHTCYAN);//����ɫ
			setlinestyle(PS_SOLID, 3);//ʵ�ߣ����3
			setfillcolor(WHITE);//���ɫ��ɫ
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
	//���¥��߶ȴ���20����ťΪ����
	if (high > 20) columns = 3;
	//���㰴ť����
	if (high % columns == 0) row = high / columns;
	else row = row = high / columns + 1;
	if (low != 0) row++;
}

void initWin(int low, int high) {
	int columns, row;
	setColumnsAndRow(columns, row, low, high);
	//���ɴ���
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

	//����¥����ʾ��
	indicatorRect = (int*)malloc(4 * sizeof(int));
	assert(indicatorRect);
	indicatorRect[0] = mPadding;
	indicatorRect[1] = mPadding;
	indicatorRect[2] = columns * (mPadding + mButtonLength);
	indicatorRect[3] = mPadding + 2 * mButtonLength;
	setlinecolor(BLACK);
	rectangle(indicatorRect[0], indicatorRect[1], indicatorRect[2], indicatorRect[3]);
	//����¥�㰴ť

	buttonRect = (int**)malloc(columns * row * sizeof(int*));//�������������½ǿ�ʼ���������ң���������
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
		case WM_KEYDOWN://Esc�˳�
			if (m.vkcode == VK_ESCAPE) return;
		}
	}
}

void buttonDown(ExMessage m) {
	setrop2(R2_NOTXORPEN);//������״����ʱ��Ԫ��դ��NOT����ǰ��ɫ XOR ������ɫ��
	for (int i = 0; i < 11; i++) {
		setlinecolor(RGB(25 * i, 25 * i, 25 * i));
		circle(m.x, m.y, 2 * i);//����ΪԲ��x��y���Ͱ뾶
		Sleep(8);
		circle(m.x, m.y, 2 * i);
	}
	FlushMouseMsgBuffer();//�������¼�����������ֹ�¼��ѻ�
}