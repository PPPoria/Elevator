#include <iostream>
#include <Windows.h>
#include <assert.h>
#include <graphics.h>
#include <conio.h>
#include "ElevatorPanel.h"


void inputLevel(int& low, int& high);


int main() {
	int low, high;
	inputLevel(low, high);
	setButtonStyle(50, 10);//�����������ΰ�ť�ı߳����Լ���ť֮��Ŀ�϶���޸�����޸Ľ����С��̫С���кڱߣ�
	initWin(low, high);
	initView();
	initListener();//����ѭ��
	closegraph();//�ر�
	printf("\n>��������ѹرա�\n");
	return 0;
}

void inputLevel(int& low, int& high)
{
	low = 1; high = 0;
	printf("\n>ע�⣺���²㲻С�ڸ�������߲㲻������ʮ\n");
	while (low < -3 || low > 0) {
		printf(">��������²㣨�������������û�е��²㣩��");
		if (scanf_s("%d", &low) == 0) while (getchar() != '\n');
	}
	if (low == 0) printf(">�޵��²�\n");
	else printf(">���²㣺%d\n", low);
	while (high <= 0 || high <= low || high > 30) {
		printf(">��������߲㣺");
		if (scanf_s("%d", &high) == 0) while (getchar() != '\n');
	}
	printf(">��߲㣺%d\n\n>����������������棬�˳��밴Esc����Ҫ�����ϽǵĲ棩��\n>", high);
	system("pause");
}
