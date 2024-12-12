#pragma once

void test();
void setButtonStyle(int buttonLength, int padding);
void setColumnsAndRow(int& columns, int& row, int low, int high);
void initWin(int low, int high);
void initView();
void initListener();
void buttonDown(ExMessage m);
void drawNumber(int number, RECT& r);
void drawStatus(int status, RECT& r);
int getClickButton(int x, int y);
void* elevatorMoving(void* arg);