#include "stdafx.h"
#pragma warning(disable : 4996)
int nmain()
{
	setlocale(LC_CTYPE, "Russian");
	FILE * ptrFile = fopen("file.txt", "r");
	char mystring[100];
	System::Console::BufferHeight = System::Console::WindowHeight;
	System::Console::BufferWidth = System::Console::WindowWidth;
	System::Console::BackgroundColor = System::ConsoleColor::Red;
	System::Console::Clear();
	printf("TEST");
	//if (ptrFile == NULL) perror("Ошибка открытия файла");
	//else
	//{
	//	if (fgets(mystring, 100, ptrFile) != NULL) // считать символы из файла
	//		puts(mystring);  // вывод на экран
	//	fclose(ptrFile);
	//}
	system("pause");
	return 0;
}