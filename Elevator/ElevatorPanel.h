#pragma once

void setButtonStyle(int buttonLength, int padding);
void setColumnsAndRow(int& columns, int& row, int low, int high);
void setWindowSize(int& width, int& height, int columns, int row);
void setPanelSize(int& width, int& height, int columns, int row);
void drawNumber(int number, RECT& r);
void drawStatus(int status, RECT& r);
int getClickButton(int x, int y);
void initWin(int low, int high);
void initView();
void drawView();
void addListener();
void* buttonDown(void* m);
void* elevatorMoving(void* arg);