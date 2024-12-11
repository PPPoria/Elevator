#pragma once

void test();
void setButtonStyle(int buttonLength, int padding);
void setColumnsAndRow(int& columns, int& row, int low, int high);
void initWin(int low, int high);
void initView(int low, int high);
void initListener(int low, int high);
void buttonDown(ExMessage m);