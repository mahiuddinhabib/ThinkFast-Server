#include <stdio.h>
#include <stdlib.h>
#include <print_center.h>

extern yCoord;

char buffer[100];
char *quiz_select()
{
    int choice, category, level;
    do
    {
        printCenter("+==================================+", &yCoord);
        printCenter("|       Select Quiz Category       |", &yCoord);
        printCenter("|==================================|", &yCoord);
        printCenter("| 1. Primary                       |", &yCoord);
        printCenter("| 2. Secondary                     |", &yCoord);
        printCenter("| 3. Higher Secondary              |", &yCoord);
        printCenter("|==================================|", &yCoord);
        printCenter("| Enter your choice (1-3):         |", &yCoord);
        printCenter("|==================================|", &yCoord);
        printCenter("| Enter level (1-10):              |", &yCoord);
        printCenter("+==================================+", &yCoord);

        //  fflush(stdout);  // make sure the prompt is displayed before input is read
        gotoxy(20 + 27, yCoord - 4);
        if (scanf("%d", &choice) != 1)
        {
            system("cls");
            gotoxy(20, 2);
            printf("\x1b[31mInvalid choice\x1b[0m\n");
            while (getchar() != '\n')
                ; // clear input buffer
            continue;
        }
        switch (choice)
        {
        case 1:
            category = 0;
            gotoxy(20 + 22, yCoord - 2);
            scanf("%d", &level);
            break;
        case 2:
            category = 10;
            gotoxy(20 + 22, yCoord - 2);
            scanf("%d", &level);
            break;
        case 3:
            category = 20;
            gotoxy(20 + 22, yCoord - 2);
            scanf("%d", &level);
            break;
        default:
            system("cls");
            printf("\x1b[31mInvalid choice. Please enter a number between 1 and 3\x1b[0m\n");
            break;
        }
    } while (choice < 1 || choice > 3);

    while (getchar() != '\n')
        ;

    if (category == 20)
        sprintf(buffer, "database\\question_bank\\higher_secondary\\higher_secondary_level_%d.txt", category + level);
    else if (category == 10)
        sprintf(buffer, "database\\question_bank\\secondary\\secondary_level_%d.txt", category + level);
    else
        sprintf(buffer, "database\\question_bank\\primary\\primary_level_%d.txt", category + level);

    return buffer;
}