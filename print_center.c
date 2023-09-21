#include <windows.h>
#include <conio.h>
#include <wincon.h>

int yCoord = 3;

COORD coord = {0, 0};
void gotoxy(int x, int y)
{
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
void printCenter(const char ch[], int *y)
{
    gotoxy(20, *y);
    printf("%s", ch);
    (*y)++;
}