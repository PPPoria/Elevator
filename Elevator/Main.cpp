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
	FlushMouseMsgBuffer();//�������¼�������
	int rect[] = { 20, 40, 60, 60 };
	RECT r = {rect[0], rect[1], rect[2], rect[3]};//���ο�
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