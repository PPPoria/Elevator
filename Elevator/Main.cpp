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
	setButtonStyle(50, 10);//参数：正方形按钮的边长，以及按钮之间的空隙；修改其可修改界面大小（太小会有黑边）
	initWin(low, high);
	initView();
	initListener();//内有循环
	closegraph();//关闭
	printf("\n>电梯面板已关闭。\n");
	return 0;
}

void inputLevel(int& low, int& high)
{
	low = 1; high = 0;
	printf("\n>注意：地下层不小于负三、最高层不大于三十\n");
	while (low < -3 || low > 0) {
		printf(">请输入地下层（负数，且零代表没有地下层）：");
		if (scanf_s("%d", &low) == 0) while (getchar() != '\n');
	}
	if (low == 0) printf(">无地下层\n");
	else printf(">地下层：%d\n", low);
	while (high <= 0 || high <= low || high > 30) {
		printf(">请输入最高层：");
		if (scanf_s("%d", &high) == 0) while (getchar() != '\n');
	}
	printf(">最高层：%d\n\n>即将进入电梯面板界面，退出请按Esc（不要点右上角的叉）！\n>", high);
	system("pause");
}
