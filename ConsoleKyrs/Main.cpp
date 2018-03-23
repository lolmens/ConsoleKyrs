#include "stdafx.h"
#include <crtdbg.h>
#pragma warning(disable : 4996) //Разрешение использования устаревших функций

const char settingsFile[29] = "ConfigAndData/settings.conf"; //Файл настроек
char RUmainMenuFile[29] = "ConfigAndData/ruMainMenu.dat"; //Файл Главного меню
char RUsettingsMenuFile[33] = "ConfigAndData/ruSettingsMenu.dat"; //Файл меню Настроек
char ENmainMenuFile[29] = "ConfigAndData/enMainMenu.dat"; //Файл Главного меню
char ENsettingsMenuFile[33] = "ConfigAndData/enSettingsMenu.dat"; //Файл меню Настроек
const char lvlFile[24] = "ConfigAndData/lvls.list"; //Файл настроек
char RUstatisticFile[30] = "ConfigAndData/ruStatistic.dat"; //Файл пункта статистики в меню настроек
char ENstatisticFile[30] = "ConfigAndData/enStatistic.dat"; //Файл пункта статистики в меню настроек
const int MAXMENULEN = 300; //Максимальная длина строки меню
const byte amountItemMainMenu = 6;//Колличество пунктов в MainMenu
const byte amountItemSettingsMenu = 5;//Колличество пунктов в SettingsMenu
const byte offsetSizes = 15;// "offset on the sides" Сколько процентов от размера экрана занимают отступы (вводится отступ с одной стороны, например если ввести 15%, отступ справа и слева будет по 15%, полезная площадь в таком случае 70%)
const byte offsetParties = 15;// Аналогично, только используется для отступов сверху и снизу
boolean DEBAG = false;

#define ENTER 13
#define ESC 27
#define UP 72
#define DOWN 80

enum Lang {
	RU = 0,
	EN = 1
} lang;

enum ConsoleColor
{
	Black = 0,
	Blue = 1,
	Green = 2,
	Cyan = 3,
	Red = 4,
	Magenta = 5,
	Brown = 6,
	LightGray = 7,
	DarkGray = 8,
	LightBlue = 9,
	LightGreen = 10,
	LightCyan = 11,
	LightRed = 12,
	LightMagenta = 13,
	Yellow = 14,
	White = 15
};

struct SETTINGS { //Структура хранящая настройки
	boolean sizeAuto = true;
	byte h = 0, w = 0, rt = 0, rr = 0, rl = 0, rb = 0; // h - высота окна, w - ширина окна, rt - отступ сверху, rr - отступ справа, rl - отступ слева rb - отступ снизу.
	byte
		colorBackground = ConsoleColor::Black,
		colorBackgroundMenu = ConsoleColor::LightGray,
		colorTextMenu = ConsoleColor::Black,
		colorSelectionBackgroundMenu = ConsoleColor::DarkGray,
		colorSelectionTextMenu = ConsoleColor::Green,
		colorDefaultText = ConsoleColor::LightGray;
} setting;

struct MENU { //Структура хранящая настройки
	char** strings;
	byte n = 0;//Кол-во пунктов
	int *w; //Массив хранящий размер строки пунктов
} *menus;

enum MENUToken {//Хранит id, в массиве, экранов меню 
	MAIN = 0,         //0
	SETTING = 1         //1       
};

enum variableTypes {
	___INT = 0,
	___STRING = 1
};

struct meta {// Хранит Мета(о) информацию о "динамической переменной"
	char *name;// Имя переменной, например "Цена"
	byte key;//Ключ для поиска переменной внутри объекта 3-го lvl
	variableTypes variableType;// Тип Данной переменной
	byte idOnType;// Место в массиве переменных данного типа
	boolean vizible = false;// У объекта 3-го lvl есть 1 поле которое показывает краткую информацию о нём, этот флаг указывает на принадлежность этому полю.
};

struct lvl1 { //lvl 1
	//Первый уровень объектов, хранит категории оъбектов (Например, Техника, Мебель, Предметы утвари, Животные.)
	char *name; //Имя категории, например "Мебель"
	int idType; //Собственный ID, по которому во время загрузки его находят объекты 2-го и 3-го уровня.
	bool haveLvl2; //Если false зачит ссылка на lvl3 иначе lvl2
	int size = 0;//Кол-во подэлементов.
	union {
		struct lvl2 *dataLvl2; //Ссылка на lvl2 и lvl3 одна  Массивы хранят сами данные в структурированном виде.
		struct lvl3 *dataLvl3; //Один из данных масивов = NULL;
	};

};

struct lvl2 { //lvl 2
	//Второй уровень объектов, хранит подкатегории оъбектов (Например, если категория техника, то подкатегориями могут быть кухонная техника, компьютерная техника, портативные средства.)
	char *name; //Имя подкатегории, например "компьютерная техника"
	int idTypeParrent;//ID родителя (1 уровня) 
	int idType; //Собственный ID, по которому во время загрузки его находят объекты 3-го уровня. 
	int size = 0;//Кол-во подэлементов.
	struct lvl3 *data;
};

struct lvl3 {  //lvl 3
	//Третий уровень объектов, хранит непосредственно объекты (PC, тумба, стул)
	char *name; //Имя объекта, например "Чашка Бугалтера"
	int idTypeParrent; //ID родителя (2 или 1 уровня)
	byte size = 0;
	struct meta *metadata; //Хранит данные объекта (тип, имя переменной, место в массиве со значениями).
	int *IntegerMass; //Физически хранит числовые переменные, НА ПРЯМУЮ НЕ ОБРАЩАТЬСЯ!
	char **StringMass; //Физически хранит текстовые переменные, НА ПРЯМУЮ НЕ ОБРАЩАТЬСЯ!
};

SETTINGS loadSettings(struct SETTINGS &setting);
MENU loadMenus(char *nameFile, int maxMenuItemLen, int amountItemMenu);
void printmenu(MENU menu, int secection);
void menu(enum MENUToken selectedMenu, lvl1 *root, int countlvl1, int countlvl2, int countlvl3);
lvl1* load_lvl(int maxStringLen, lvl1*, int &lv1, int &lv2, int &lv3);

int main() {
	printf("Loadig...\n");
	setlocale(LC_CTYPE, "Russian");
	loadSettings(setting);
	if (setting.sizeAuto) { //Доводим до ума стандартные настройки
		setting.rl = setting.rr = ((byte)(setting.w * 0.01 * offsetSizes)); // offsetSizes - задаётся в %, поэтому домнажается на 0.01.
		setting.rt = setting.rb = ((byte)(setting.h * 0.01 * offsetSizes));
	}
	printf("Settings Load!\n");
	MENU mainMenu, setttingMenu;

	if (lang == Lang::RU)
		mainMenu = loadMenus(RUmainMenuFile, MAXMENULEN, amountItemMainMenu),
		setttingMenu = loadMenus(RUsettingsMenuFile, MAXMENULEN, amountItemSettingsMenu);
	else
		mainMenu = loadMenus(ENmainMenuFile, MAXMENULEN, amountItemMainMenu),
		setttingMenu = loadMenus(ENsettingsMenuFile, MAXMENULEN, amountItemSettingsMenu);
	menus = (MENU*)malloc(sizeof(mainMenu) + sizeof(setttingMenu));
	menus[MENUToken::MAIN] = mainMenu;
	menus[MENUToken::SETTING] = setttingMenu;
	printf("Menus Load!\n");
	printf("Load lvls.list...\n");
	lvl1 *rootLvl = NULL;
	int countlvl1 = 0, countlvl2 = 0, countlvl3 = 0;
	rootLvl = load_lvl(1000, rootLvl, countlvl1, countlvl2, countlvl3);

	printf("У нас %s проблемы с повреждением памяти.\n", (_CrtCheckMemory() ? "присутствуют" : "отсутствуют"));
	printf("Load!");
	if (DEBAG) getchar();
	enum MENUToken selectedMenu = MENUToken::MAIN;
	System::Console::CursorVisible::set(false);
	menu(selectedMenu, rootLvl, countlvl1, countlvl2, countlvl3);

	return 0;
}

void printTree(lvl1 *root, int countlvl1, bool full);
int selector();
void printStatistic(int countlvl1, int countlvl2, int countlvl3);
void menu(enum MENUToken selectedMenu, lvl1 *root, int countlvl1, int countlvl2, int countlvl3) {
	byte select = 0;
	while (true) {
		printmenu(menus[selectedMenu], select);
		int enter = selector();
		switch (enter) {
		case -1: if (select + 1 < menus[selectedMenu].n) select++; continue; //Не разрешаем выйти ниже меню.
		case 1: if (select - 1 >= 0) select--; continue; //Не разрешаем выйти выше меню.
		case 0:
			if (selectedMenu == MENUToken::MAIN)
				switch (select)
				{
				case 0:	//Список всей техники
					System::Console::BackgroundColor = (System::ConsoleColor) setting.colorBackground;
					System::Console::Clear();//Очистка
					System::Console::ForegroundColor = (System::ConsoleColor) setting.colorDefaultText;
					//root[0]; //Элементы из категории техника
					for (int j = 0; j < root[0].size; j++) {//Количество элементов на 2-х уровнях
						if (root[0].haveLvl2) {//Для безопасности
							for (int h = 0; h < root[0].dataLvl2[j].size; h++) {//Количество элементов на 3-ем уровне
								printf("|-*- %s\n", root[0].dataLvl2[j].data[h].name);
								for (int r = 0; r < root[0].dataLvl2[j].data[h].size; r++) {
									if (root[0].dataLvl2[j].data[h].metadata[r].vizible) {//Печать только видимых
										if (root[0].dataLvl2[j].data[h].metadata[r].variableType == variableTypes::___INT)
											printf("	|- %s:%3d \n", root[0].dataLvl2[j].data[h].metadata[r].name, root[0].dataLvl2[j].data[h].IntegerMass[root[0].dataLvl2[j].data[h].metadata[r].idOnType]);
										else
											printf("	|- %s - %s \n", root[0].dataLvl2[j].data[h].metadata[r].name, root[0].dataLvl2[j].data[h].StringMass[root[0].dataLvl2[j].data[h].metadata[r].idOnType]);
									}
								}
							}
						}
					}
					getchar();
					continue;
				case 1:	//Просмотр техники по категорям
					System::Console::BackgroundColor = (System::ConsoleColor) setting.colorBackground;//Фон
					System::Console::Clear();//Очистка
					System::Console::ForegroundColor = (System::ConsoleColor) setting.colorDefaultText;//Текст
					for (int i = 0; i < countlvl1; i++) {//Количество элементов на первом уровне 
						printf("|-***- %s \n", root[i].name);
						for (int j = 0; j < root[i].size; j++) {//Количество элементов на 2-х уровнях
							if (root[i].haveLvl2) {
								printf("	|-**- %s \n", root[i].dataLvl2[j].name);
							}
						}
					}
					getchar();
					continue;
				case 2:	//Укороченная печать дерева инвентаризации
					printTree(root, countlvl1, false);
					continue;
				case 3:	//Печать полного дерева инвентаризации
					printTree(root, countlvl1, true);
					continue;
				case 4:	//Настройки
					selectedMenu = MENUToken::SETTING;
					select--;//Передвижение курсора на "выход" в меню Settings
					continue;
				case 5: //Выход
					return;
				default:
					continue;
				}
			if (selectedMenu == MENUToken::SETTING)
				switch (select)
				{
				case 0: //Statistic
					printStatistic(countlvl1, countlvl2, countlvl3);
					getchar();
					continue;
				case 2: //Lang
					if (lang == Lang::RU)
						menus[MENUToken::MAIN] = loadMenus(ENmainMenuFile, MAXMENULEN, amountItemMainMenu),
						menus[MENUToken::SETTING] = loadMenus(ENsettingsMenuFile, MAXMENULEN, amountItemSettingsMenu),
						lang = Lang::EN;
					else
						menus[MENUToken::MAIN] = loadMenus(RUmainMenuFile, MAXMENULEN, amountItemMainMenu),
						menus[MENUToken::SETTING] = loadMenus(RUsettingsMenuFile, MAXMENULEN, amountItemSettingsMenu),
						lang = Lang::RU;
					continue;
				case 3: //Выход в главное меню
					selectedMenu = MENUToken::MAIN;
					continue;
				case 4: //Debag
					DEBAG = !DEBAG;
					continue;
				default:
					continue;
				}
		}
	}

}

int selector() {
	char c = getch();
	switch (c) {
	case DOWN: return -1;
	case UP: return 1;
	case ENTER: return 0;
	default: return c;
	}
}

void setsettings(struct SETTINGS* settings, char* name, char* value);
SETTINGS loadSettings(struct SETTINGS &setting) { //Загрузка настроек из файла в структуру
	FILE *in;
	char s[80];
	if ((in = fopen(settingsFile, "r")) == NULL)
	{
		printf("\nFile %s not open!", settingsFile);
		system("pause"); exit(1);
	}

	while (true)
		if (fgets(s, 80, in) != NULL) {
			if (s[0] == '#') {//если строка комментарий
				//puts(s);
				continue;
			}
			else {//если строка не комментарий
				char *nameSetEnd = strchr(s, '=');
				if (nameSetEnd) { // if nameSetEnd != NULL //Защита от не полного считывания строки fgets
					int nameSetLeght = strlen(s) - strlen(nameSetEnd);
					*nameSetEnd = '\0';
					++nameSetEnd;// ++nameSetEnd тк strchr() возвращает указатель на =, котой уже \0
					char *variableSetEnd = strchr(nameSetEnd, ';');
					*variableSetEnd = '\0'; //Удаление лишнего в конце строки
					//printf("len = %d Variable '%s' value '%s'\n", nameSetLeght, s, nameSetEnd);
					setsettings(&setting, s, nameSetEnd);
				}
				else //Защита от не полного считывания строки fgets, ругается в случае ошибок
				{
					printf("null");
				}
			}
		}
		else //Выход из цикла, когда файл закончился.
			break;
	fclose(in);
	return setting;
}

void setsettings(struct SETTINGS* settings, char* name, char* value) {
	if (strcmp(name, "size") == 0) {
		if (strcmp(value, "auto") == 0) {
			(*settings).sizeAuto = true;
			(*settings).h = 50;//System::Console::WindowHeight;//Установка параметров по умолчанию
			(*settings).w = 100;//System::Console::WindowWidth;
		}
		else
			(*settings).sizeAuto = false;
		/*printf(" %s %d", value, (*settings).sizeAuto);*/
	}
	else if (strcmp(name, "language") == 0) {
		if (strcmp(value, "RU") == 0)
			lang = Lang::RU;
		else
			lang = Lang::EN;
	}
	else if (strcmp(name, "h") == 0) { if (!(*settings).sizeAuto) (*settings).h = (byte)atoi(value); } // !(*settings).sizeAuto <- отключается если выставлено true. Перенести в первый if нельзя тк будет описываться ошибка Argument '%s' isn`t defined!
	else if (strcmp(name, "w") == 0) { if (!(*settings).sizeAuto) (*settings).w = (byte)atoi(value); }
	else if (strcmp(name, "rt") == 0) { (*settings).rt = (byte)atoi(value); }
	else if (strcmp(name, "rl") == 0) { (*settings).rl = (byte)atoi(value); }
	else if (strcmp(name, "rr") == 0) { (*settings).rr = (byte)atoi(value); }
	else if (strcmp(name, "rb") == 0) { (*settings).rb = (byte)atoi(value); }
	else if (strcmp(name, "colorBackground") == 0) { (*settings).colorBackground = (byte)atoi(value); }//TODO:Переписать под парсинг строк
	else if (strcmp(name, "colorBackgroundMenu") == 0) { (*settings).colorBackgroundMenu = (byte)atoi(value); }
	else if (strcmp(name, "colorTextMenu") == 0) { (*settings).colorTextMenu = (byte)atoi(value); }
	else if (strcmp(name, "colorSelectionBackgroundMenu") == 0) { (*settings).colorSelectionBackgroundMenu = (byte)atoi(value); }
	else if (strcmp(name, "colorSelectionTextMenu") == 0) { (*settings).colorSelectionTextMenu = (byte)atoi(value); }
	else if (strcmp(name, "colorDefaultText") == 0) { (*settings).colorDefaultText = (byte)atoi(value); }
	else printf("Argument '%s' isn`t defined!", name); //Если аргумент не найден.
}

MENU loadMenus(char *nameFile, int maxMenuItemLen, int amountItemMenu) {
	FILE *fileMenu;
	struct MENU menu;
	byte strings = 0;
	if ((fileMenu = fopen(nameFile, "r")) == NULL)
	{
		printf("\nFile %s not open!", nameFile);
		system("pause"); exit(1);
	}

	char *readString = (char *)malloc(maxMenuItemLen * sizeof(char));//Память под считанную строку из файла
	menu.strings = (char **)malloc(amountItemMenu * sizeof(char*));//Память под массив строк
	menu.n = amountItemMenu;
	menu.w = (int*)malloc(amountItemMenu * sizeof(int));
	//if (DEBAG) printf("\nDEBAG function loadMenus: menu.n=%d\n", menu.n);
	while (fgets(readString, maxMenuItemLen, fileMenu) != NULL) {
		menu.strings[strings] = (char *)malloc(strlen(readString) * sizeof(char));//Память под строку
		strcpy(menu.strings[strings], readString);//Копирование считаной строки в память которую только что выделили
		menu.w[strings++] = strlen(readString);
		//if (DEBAG) printf("\nDEBAG function loadMenus: len = %d strings = %d, %s", menu.w[strings - 1], strings - 1, menu.strings[strings - 1]);
	}
	fclose(fileMenu);
	return menu;
}

char* formateMenuString(char *menuString);
void printmenu(MENU menu, int selection) { //selection - нумерация с 0.
	int i;
	if (setting.w && setting.h) // По умолчанию размер 0,0 это защита
		System::Console::SetWindowSize(setting.w, setting.h);
	//System::Console::BufferHeight = System::Console::WindowHeight;//FIXME: Боится когда изменяются размеры окна, выпадает в ошибку.
	//1System::Console::BufferWidth = System::Console::WindowWidth;
	System::Console::BackgroundColor = (System::ConsoleColor) setting.colorBackground;
	System::Console::Clear();//Очистка
	//System::Console::SetCursorPosition(setting.rl, setting.rt);
	System::Console::BackgroundColor = (System::ConsoleColor) setting.colorBackgroundMenu;
	System::Console::ForegroundColor = (System::ConsoleColor) setting.colorTextMenu;
	if (DEBAG) printf("DEBAG function formateMenuString: menu.n=%d", menu.n);
	byte add = 0;
	System::Console::SetCursorPosition(setting.rl, setting.rt + add++);
	printf("%s", formateMenuString(""));
	for (i = 0; menu.n > i; i++) { // + 2 <- Добавление пробелов в начале и конце.
		System::Console::SetCursorPosition(setting.rl, setting.rt + add++);//отрисовка со следующей строки
		if (i == selection) {
			System::Console::BackgroundColor = (System::ConsoleColor) setting.colorSelectionBackgroundMenu;
			System::Console::ForegroundColor = (System::ConsoleColor) setting.colorSelectionTextMenu;
			printf("%s", formateMenuString(menu.strings[i]));
			System::Console::BackgroundColor = (System::ConsoleColor) setting.colorBackgroundMenu;
			System::Console::ForegroundColor = (System::ConsoleColor) setting.colorTextMenu;
		}
		else {
			printf("%s", formateMenuString(menu.strings[i]));
		}

	}
	System::Console::SetCursorPosition(setting.rl, setting.rt + add++);
	printf("%s", formateMenuString(""));


	//System::Console::ForegroundColor = (System::ConsoleColor) setting.colorBackground;
}

void printStatistic(int countlvl1, int countlvl2, int countlvl3) {
	FILE *in;
	char s[80];
	if ((in = fopen((lang == Lang::RU ? RUstatisticFile : ENstatisticFile), "r")) == NULL)
	{
		printf("\nFile %s not open!", settingsFile);
		system("pause"); exit(1);
	}
	System::Console::BackgroundColor = (System::ConsoleColor) setting.colorBackground;
	System::Console::Clear();//Очистка
	System::Console::BackgroundColor = (System::ConsoleColor) setting.colorBackgroundMenu;
	System::Console::ForegroundColor = (System::ConsoleColor) setting.colorTextMenu;
	byte add = 0;
	System::Console::SetCursorPosition(setting.rl, setting.rt + add++);
	printf("%s", formateMenuString(""));
	for (int i = 0; 4 > i; i++) {
		System::Console::SetCursorPosition(setting.rl, setting.rt + add++);//отрисовка со следующей строки
		while (true)
			if (fgets(s, 80, in) != NULL) {
				if (s[0] == '#') {//если строка комментарий
					continue;
				}
				else {//если строка не комментарий
					int toprint = 0;
					switch (i) {
					case 0:
						toprint = countlvl1 + countlvl2 + countlvl3;
						break;
					case 1:
						toprint = countlvl1;
						break;
					case 2:
						toprint = countlvl2;
						break;
					case 3:
						toprint = countlvl3;
						break;
					}
					char temp[4];
					itoa(toprint, temp, 10);
					if (strchr(s, '\n') != NULL)//Нахождение символа переноса строки и подстановка символа - туда.
						*(strchr(s, '\n')) = '-';
					strcat(s, ": ");
					strcat(s, temp);
					strcat(s, "  ");//Костыль, чтоб последний символ не пропадал после formateMenuString(s)
					printf("%s", formateMenuString(s));
					break;
				}
			}
			else //Выход из цикла, когда файл закончился.
				break;
	}
	System::Console::SetCursorPosition(setting.rl, setting.rt + add);
	printf("%s", formateMenuString(""));
	fclose(in);
}


char* formateMenuString(char *menuString) {
	//Существует 3 случая: 
	//1- Строка меньше чем надо 
	//2- Cтрока подходит в притык
	//3- Cтрока больше чем надо
	char* string;
	int menuStringLen = setting.w - setting.rl - setting.rr;
	int i;
	byte add = 2;// 2 пробела - один в начале и в конце
	int len = strlen(menuString);//Убираем тормаза 
	if (len + add < menuStringLen) {
		string = (char*)malloc(menuStringLen * sizeof(char));//menuStringLen+1 место под символ тереноса строки 
		string[0] = ' ';//добавление пробела в начале
		string[1] = '\0';//добавление \0 после пробела в начале, для работы strcat
		strcat(string, menuString); // Строка имеет вид " Текст\0" нужно добавить пробелы в конец.
		int len = strlen(string);//Убираем тормаза 
		for (i = len - 1; i < menuStringLen; i++) // menuStringLen-strlen(string) Кол-во пробелов которое надо добавить. strlen(string)-1 нумерация с нуля значит надо сместиться на один символ левее.
			string[i] = ' ';
		//string[i-1] = '\n';//Предпоследний символ - символ переноса строки
		string[i] = '\0';//Последний символ в любой строке должен быть \0;
		//printf("%s", string);
		return string;
	}
	//FIXME:Поскольку случая 2 и 3 не встречается, я напишу их на досуге
	//Андрей!!! Не забудь что когда строка слишком длинная её нужно перенести на другую, это значит что она будет занимать 2 строки, 
	//добавь дополнительную переменную в объявление функции char* formateMenuString(char *menuString,byte *add)
	//Такая ситуация может произойти если в ручной настройке окна сильно ужать размер окна.
	return NULL;
}

boolean haveEnd(char *string);
void loadlvl1(char* str, lvl1 &lvl);
void loadlvl2(char* str, lvl2 &lvl);
void loadlvl3(char* str, lvl3 &lvl, bool needMeta);
lvl1* load_lvl(int maxStringLen, lvl1 *root, int &countlvl1, int &countlvl2, int &countlvl3) {
	/*
	*Тут потребуется краткое описание дальнейших действий программы
	*Во первых Нужно отделить полезные для программы данные от мусора на подобии комментрариев, лишних переносов строк, отступов
	*
	*/
	FILE *file_lvls;
	if ((file_lvls = fopen(lvlFile, "r")) == NULL)
	{
		printf("\nFile %s not open!", lvlFile);
		system("pause"); exit(1);
	}
	int stringsCount = 0;//Переменная для подсчёта кол-ва строк с данными, далее требуется для выделении памяти.
	char *readString = (char *)malloc(maxStringLen * sizeof(char));//Память под считанную строку из файла
	while (fgets(readString, maxStringLen, file_lvls) != NULL) {
		//if (DEBAG) printf("\nDEBAG function load_lvl String \"%s\" len:%d", readString, strlen(readString));
		switch (readString[0])
		{
		case '#': continue;//Пропуск комментариев
		case '-'://Выбираем что-то полезное 
			stringsCount++;
			continue;
		}
	}
	rewind(file_lvls);//В начало файла, подсчёт кол-ва полезных данных закончен.

	//if (DEBAG) printf("\nDEBAG function load_lvl Total strings:%d\n", stringsCount);

	char **stringsWithData = (char**)malloc(stringsCount * sizeof(char*));//Массив указателей на строки с уже подготовленными данными.

	stringsCount = 0;
	char *tempString = (char *)malloc(maxStringLen * sizeof(char));//Место под динамическую подгрузку частей строк.
	char *copyTempString = tempString;
	while (fgets(readString, maxStringLen, file_lvls) != NULL) {
		//if (DEBAG) printf("\nDEBAG function load_lvl Read String \"%s\" len:%d", readString, strlen(readString));
		switch (readString[0])
		{
		case '#': continue;//Пропуск комментариев
		case '-':
			while (!haveEnd(readString)) {//Если в строке нет ";" значит в этой строке структура не закончилась и требуется дополнить строку.
				//if (DEBAG) printf("\nDEBAG function load_lvl String \"%s\" len:%d", readString, strlen(readString));
				if (fgets(tempString, maxStringLen, file_lvls) != NULL) {
					strcat(readString, tempString); //Склеиваем начало строки с только что подгруженной частью.
				}
				else {
					//FIXME: Короче, в этом случае файл скорее всего неожиданно кончился.
				}
			}
			break;
		default:
			continue;
		}
		//if (DEBAG) printf("\nDEBAG function load_lvl Full String \"%s\" len:%d", readString, strlen(readString));
		stringsWithData[stringsCount] = (char*)malloc(strlen(readString) * sizeof(char)); //Выделение достаточного кол-ва памяти под "чистую строку"
		strcpy(stringsWithData[stringsCount], readString);//Сохранение полученных даных.
		//if (DEBAG) printf("\nDEBAG function load_lvl Full String \"%s\" len:%d ", stringsWithData[stringsCount], strlen(stringsWithData[stringsCount]));
		stringsCount++;
		//printf("%d", stringsCount);
	}
	for (int i = 0; i < stringsCount; i++) {//считываем 5-й символ, символ отвечающий за номер уровня
		tempString[0] = stringsWithData[i][5];
		tempString[1] = '\0';
		switch (atoi(tempString))
		{
		case 1:
			countlvl1++;
			break;
		case 2:
			countlvl2++;
			break;
		case 3:
			countlvl3++;
			break;
		}
	}
	//if (DEBAG) printf("DEBAG function loadlvl.\n Lvl 1 count cells: %d\n Lvl 2 count cells: %d\n Lvl 3 count cells: %d\n", countlvl1, countlvl2, countlvl3);
	root = (struct lvl1*)malloc(countlvl1 * sizeof(struct lvl1));
	for (int i = 0; i < countlvl1; i++) {
		root[i].size = 0;
		root[i].haveLvl2 = false;
	}
	countlvl1 = 0; //lv2 = 0, lv3 = 0;
	//Заполняем 1 уровень и считаем кол-во элементов на 2-м уровне
	for (int i = 0; i < stringsCount; i++) {//считываем 5-й символ, символ отвечающий за номер уровня
		tempString[0] = stringsWithData[i][5];
		tempString[1] = '\0';
		switch (atoi(tempString))
		{
		case 1:
			loadlvl1(&stringsWithData[i][6], root[countlvl1]);
			//if (DEBAG) printf("DEBAG function loadlvl Id:%d Name:%s\n", root[lv1].idType, root[lv1].name);
			countlvl1++;
			break;
		case 2: //Подсчет количесва элементов lvl 2 в каждом lvl 1
			lvl2 temp;
			loadlvl2(&stringsWithData[i][6], temp);
			//if (DEBAG) printf("DEBAG function loadlvl IdType:%d IdTypeParrent:%d Name:%s\n", temp.idType, temp.idTypeParrent, temp.name);
			for (int i = 0; i < countlvl1; i++) {
				if (root[i].idType == temp.idTypeParrent) {
					//printf("DEBAG function loadlvl Id:%d Name:%s Size:%d\n", root[i].idType, root[i].name, root[i].size);
					root[i].haveLvl2 = true;
					root[i].size++;
					break;
				}
			}
			break;
		}
	}

	//Выделяем память под структуры 2-го lvl
	for (int i = 0; i < countlvl1; i++) {
		if (root[i].haveLvl2 == true) {
			root[i].dataLvl2 = (struct lvl2*)malloc(root[i].size * sizeof(struct lvl2));
			root[i].size = 0; //Будет использоваться как счётчик
		}
	}

	//Заполняем 2 уровень 
	for (int i = 0; i < stringsCount; i++) {//считываем 5-й символ, символ отвечающий за номер уровня
		tempString[0] = stringsWithData[i][5];
		tempString[1] = '\0';
		switch (atoi(tempString))
		{
		case 2:
			lvl2 temp;
			loadlvl2(&stringsWithData[i][6], temp);
			//if (DEBAG) printf("DEBAG function loadlvl IdType:%d IdTypeParrent:%d Name:%s\n", temp.idType, temp.idTypeParrent, temp.name);
			for (int j = 0; j < countlvl1; j++) {
				if (root[j].idType == temp.idTypeParrent) {
					loadlvl2(&stringsWithData[i][6], root[j].dataLvl2[root[j].size]); //root[j].size - Используется в качестве счётчика.
					root[j].dataLvl2[root[j].size].size = 0;
					root[j].size++;
					break;
				}
			}
			break;
		}
	}

	//При заполнении 3-го уровня будут так-же использоваться размеры как счётчики для элементов, предварительно требуется из очистить от мусора и прировнять к 0.
	for (int i = 0; i < stringsCount; i++) {//считываем 5-й символ, символ отвечающий за номер уровня
		tempString[0] = stringsWithData[i][5];
		tempString[1] = '\0';
		switch (atoi(tempString)) {
		case 3:
			lvl3 temp;
			loadlvl3(&stringsWithData[i][6], temp, false);
			for (int i = 0; i < countlvl1; i++) {//Проход по 1-му уровню.
				if (root[i].haveLvl2 == true) {
					for (int j = 0; j < root[i].size; j++) {//Проход по 2-му уровню если таковой есть.
						if (root[i].dataLvl2[j].idType == temp.idTypeParrent) {
							root[i].dataLvl2[j].size = 0;
							goto contini1;
						}
					}
				}
				else //Проход по 3-му уровню если нет прослойки в виде 2-го.
					if (root[i].idType == temp.idTypeParrent) {
						root[i].size = 0;
						goto contini1;
					}
			}
		}
	contini1:
		;
	}

	//Считаем кол-во элементов на 3-м уровне
	for (int i = 0; i < stringsCount; i++) {//считываем 5-й символ, символ отвечающий за номер уровня
		tempString[0] = stringsWithData[i][5];
		tempString[1] = '\0';
		switch (atoi(tempString)) {
		case 3:
			lvl3 temp;
			loadlvl3(&stringsWithData[i][6], temp, false);
			for (int h = 0; h < countlvl1; h++) {//Проход по 1-му уровню.
				if (root[h].haveLvl2 == true) {
					for (int j = 0; j < root[h].size; j++)//Проход по 2-му уровню если таковой есть.
						if (root[h].dataLvl2[j].idType == temp.idTypeParrent) {
							root[h].dataLvl2[j].size++;
							goto contini2;
						}
				}
				else { //Проход по 3-му уровню если нет прослойки в виде 2-го.
					if (root[h].idType == temp.idTypeParrent) {
						root[h].size++;
						goto contini2;
					}

				}

			}
		}
	contini2:
		;
	}

	//printf("Перед выделением памяти\n");
	if (false)	for (int i = 0; i < countlvl1; i++) {//Количество элементов на первом уровне 
		printf("|-lvl1- IdType:%d Name:%s Size:%d Type child %s\n", root[i].idType, root[i].name, root[i].size, (root[i].haveLvl2 ? "2" : "3"));
		for (int j = 0; j < root[i].size; j++) {//Количество элементов на 2-х уровнях
			if (root[i].haveLvl2) {
				printf("	|-lvl2- IdType:%d IdTypeParrent:%d Size:%d Name:%s\n", root[i].dataLvl2[j].idType, root[i].dataLvl2[j].idTypeParrent, root[i].dataLvl2[j].size, root[i].dataLvl2[j].name);
				for (int h = 0; h < root[i].dataLvl2[j].size; h++)//Количество элементов на 3-х уровнях
					;// printf("		|-lvl3- IdTypeParrent:%d Name:%s\n", root[i].dataLvl3[h].idTypeParrent, root[i].dataLvl3[h].name);
				//root[i].dataLvl2[j]
			}
			else
				;//printf("	|-lvl3- IdTypeParrent:%d Name:%s\n", (*root[i].dataLvl3).idTypeParrent, (*root[i].dataLvl3).name);
		}
	}

	//Выделяем память под элементы 3-го уровня
	for (int i = 0; i < stringsCount; i++) {//считываем 5-й символ, символ отвечающий за номер уровня
		tempString[0] = stringsWithData[i][5];
		tempString[1] = '\0';
		switch (atoi(tempString))
		{
		case 3:
			lvl3 temp;
			loadlvl3(&stringsWithData[i][6], temp, false);
			for (int h = 0; h < countlvl1; h++) {//Проход по 1-му уровню.
				if (root[h].haveLvl2 == true) {
					for (int j = 0; j < root[h].size; j++)//Проход по 2-му уровню если таковой есть.
						if (root[h].dataLvl2[j].idType == temp.idTypeParrent) {
							root[h].dataLvl2[j].data = (struct lvl3*)malloc(root[h].dataLvl2[j].size * sizeof(lvl3));
							goto contini3;
						}
				}
				else //Проход по 3-му уровню если нет прослойки в виде 2-го.
					if (root[h].idType == temp.idTypeParrent) {
						root[h].dataLvl3 = (struct lvl3*)malloc(root[h].size * sizeof(lvl3));
						goto contini3;
					}
			}
		}
	contini3:
		;
	}

	//Скидываем размеры массивов 3-го уровня чтобы использовать их как счётчики.
	for (int i = 0; i < stringsCount; i++) {//считываем 5-й символ, символ отвечающий за номер уровня
		tempString[0] = stringsWithData[i][5];
		tempString[1] = '\0';
		switch (atoi(tempString))
		{
		case 3:
			lvl3 temp;
			loadlvl3(&stringsWithData[i][6], temp, false);
			for (int h = 0; h < countlvl1; h++) {//Проход по 1-му уровню.
				if (root[h].haveLvl2 == true) {
					for (int j = 0; j < root[h].size; j++)//Проход по 2-му уровню если таковой есть.
						if (root[h].dataLvl2[j].idType == temp.idTypeParrent) {
							root[h].dataLvl2[j].size = 0;
							goto contini4;
						}
				}
				else //Проход по 3-му уровню если нет прослойки в виде 2-го.
					if (root[h].idType == temp.idTypeParrent) {
						root[h].size = 0;
						goto contini4;
					}
			}
		}
	contini4:
		;
	}
	//printf("Перед заполнением\n");
	if (false)	for (int i = 0; i < countlvl1; i++) {//Количество элементов на первом уровне 
		printf("|-lvl1- IdType:%d Name:%s Size:%d Type child %s\n", root[i].idType, root[i].name, root[i].size, (root[i].haveLvl2 ? "2" : "3"));
		for (int j = 0; j < root[i].size; j++) {//Количество элементов на 2-х уровнях
			if (root[i].haveLvl2) {
				printf("	|-lvl2- IdType:%d IdTypeParrent:%d Size:%d Name:%s\n", root[i].dataLvl2[j].idType, root[i].dataLvl2[j].idTypeParrent, root[i].dataLvl2[j].size, root[i].dataLvl2[j].name);
				for (int h = 0; h < root[i].dataLvl2[j].size; h++)//Количество элементов на 3-х уровнях
					;// printf("		|-lvl3- IdTypeParrent:%d Name:%s\n", root[i].dataLvl3[h].idTypeParrent, root[i].dataLvl3[h].name);
				//root[i].dataLvl2[j]
			}
			else
				;// printf("	|-lvl3- IdTypeParrent:%d Name:%s\n", (*root[i].dataLvl3).idTypeParrent, (*root[i].dataLvl3).name);
		}
	}

	//Заполняем элементами 3-го уровня
	for (int i = 0; i < stringsCount; i++) {//считываем 5-й символ, символ отвечающий за номер уровня
		tempString[0] = stringsWithData[i][5];
		tempString[1] = '\0';
		switch (atoi(tempString))
		{
		case 3:
			lvl3 temp;
			loadlvl3(&stringsWithData[i][6], temp, false);
			for (int h = 0; h < countlvl1; h++) {//Проход по 1-му уровню.
				if (root[h].haveLvl2 == true) {
					for (int j = 0; j < root[h].size; j++)//Проход по 2-му уровню если таковой есть.
						if (root[h].dataLvl2[j].idType == temp.idTypeParrent) {
							loadlvl3(&stringsWithData[i][6], root[h].dataLvl2[j].data[root[h].dataLvl2[j].size], true);//Осторожно, можно сломать мозг..Или запутаться в i,h & j //FIXME: Тут с памятью точно какой-то косяк!!
							//printf("Debag:temp  |-lvl3- IdTypeParrent:%d Name:%s\n", temp.idTypeParrent, temp.name);
							root[h].dataLvl2[j].size++;
							goto contini5;
						}

				}
				else //Проход по 3-му уровню если нет прослойки в виде 2-го.
					if (root[h].idType == temp.idTypeParrent) { //FIXME: Тут с памятью точно какой-то косяк!!
						loadlvl3(&stringsWithData[i][6], root[h].dataLvl3[root[h].size], true);
						//printf("Debag:root  |-lvl3- root[h].size:%d IdTypeParrent:%d Name:%s\n", root[h].size, root[h].dataLvl3[root[h].size].idTypeParrent, root[h].dataLvl3[root[h].size].name);
						root[h].size++;
						goto contini5;
					}
			}
		}
	contini5:
		;
	}

	printf("Финальный вывод\n");
	if (DEBAG)	for (int i = 0; i < countlvl1; i++) {//Количество элементов на первом уровне 
		printf("|-lvl1- IdType:%d Name:%s Size:%d Type child %s\n", root[i].idType, root[i].name, root[i].size, (root[i].haveLvl2 ? "2" : "3"));
		for (int j = 0; j < root[i].size; j++) {//Количество элементов на 2-х уровнях
			if (root[i].haveLvl2) {
				printf("	|-lvl2- J:%d IdType:%d IdTypeParrent:%d Size:%d Name:%s\n", j, root[i].dataLvl2[j].idType, root[i].dataLvl2[j].idTypeParrent, root[i].dataLvl2[j].size, root[i].dataLvl2[j].name);
				for (int h = 0; h < root[i].dataLvl2[j].size; h++) {//Количество элементов на 3-х уровнях
					printf("		|-lvl3- H:%d IdTypeParrent:%d, Size:%d, Name:%s\n", h, root[i].dataLvl2[j].data[h].idTypeParrent, root[i].dataLvl2[j].data[h].size, root[i].dataLvl2[j].data[h].name);
					for (int r = 0; r < root[i].dataLvl2[j].data[h].size; r++) {
						if (/*root[i].dataLvl2[j].data[h].metadata[r].vizible */true) {
							if (root[i].dataLvl2[j].data[h].metadata[r].variableType == variableTypes::___INT)
								printf("			|-%s:%d\n", root[i].dataLvl2[j].data[h].metadata[r].name, root[i].dataLvl2[j].data[h].IntegerMass[root[i].dataLvl2[j].data[h].metadata[r].idOnType]);
							else
								printf("			|-%s - %s\n", root[i].dataLvl2[j].data[h].metadata[r].name, root[i].dataLvl2[j].data[h].StringMass[root[i].dataLvl2[j].data[h].metadata[r].idOnType]);
							//break;
						}
						else
							continue;
					}
				}

			}
			else {
				printf("		|-lvl3- J:%d IdTypeParrent:%d, Size:%d, Name:%s\n", j, root[i].dataLvl3[j].idTypeParrent, root[i].dataLvl3[j].size, root[i].dataLvl3[j].name);
				for (int r = 0; r < root[i].dataLvl3[j].size; r++) {
					if (/*root[i].dataLvl3[j].metadata[r].vizible*/true) {
						if (root[i].dataLvl3[j].metadata[r].variableType == variableTypes::___INT)
							printf("			|-%s:%d\n", root[i].dataLvl3[j].metadata[r].name, root[i].dataLvl3[j].IntegerMass[root[i].dataLvl3[j].metadata[r].idOnType]);
						else
							printf("			|-%s - %s\n", root[i].dataLvl3[j].metadata[r].name, root[i].dataLvl3[j].StringMass[root[i].dataLvl3[j].metadata[r].idOnType]);
						//break;
					}
					else
						continue;
				}
			}

		}
	}
	return root;
	fclose(file_lvls);
	//free(tempString);//Память надо не забывать освобождать, иначе утечка..! Или не надо..
}

boolean haveEnd(char *string) {//Убирает лишние переносы строк и Tab (все что попадутся).
	int i = 0;
	int len = strlen(string);//Убираем тормаза
	for (i = 0; i < len; i++)
		switch (string[i])
		{
		case ';'://Если ; значит, в этой строке закрылась структура.
			if (i + 1 < len&&string[i + 1] == '\n')
				string[i] = '\0';
			return true;
		case '	'://Замена Tab на Пробел
			string[i] = ' ';
		case '\n'://Замена Enter на Пробел
			string[i] = ' ';
		}
	return false;
}
void loadlvl1(char* str, lvl1 &lvl) {//FIXME:Переписать под поиск * а не статическое полежение
	//str =  *idType*name* 
	char c = '*';
	char *tempString = (char*)malloc(strlen(str) * sizeof(char));
	strcpy(tempString, str);
	tempString = strchr(tempString, c), tempString++;//Нахождение первого символа * и удаление его
	*(strchr(tempString, c)) = '\0';//Нахождение второго символа * и постановка конца строки туда.
	lvl.idType = atoi(tempString);
	tempString = strchr(tempString, '\0'), tempString++;//Нахождение конца и перемецение на один символ дальше (А ты можешь выйти за рамки??)
	*(strchr(tempString, c)) = '\0';//Нахождение символа * и постановка конца строки туда.
	lvl.name = (char *)malloc(strlen(tempString) * sizeof(char));
	strcpy(lvl.name, tempString);
	//free(tempString);//Хз почему неработает
	//if (DEBAG) printf("DEBAG function loadlvl1 Id:%d Name:%s\n", lvl.idType, lvl.name);
}

void loadlvl2(char* str, lvl2 &lvl) {
	char c = '*';
	char *tempString = (char*)malloc(strlen(str) * sizeof(char));
	strcpy(tempString, str);
	tempString = strchr(tempString, c), tempString++;//Нахождение первого символа * и удаление его
	*(strchr(tempString, c)) = '\0';//Нахождение второго символа * и постановка конца строки туда.
	lvl.idType = atoi(tempString);
	tempString = strchr(tempString, '\0'), tempString++;//Нахождение конца и перемецение на один символ дальше (А ты можешь выйти за рамки??)
	*(strchr(tempString, c)) = '\0';//Нахождение символа * и постановка конца строки туда.
	lvl.idTypeParrent = atoi(tempString);
	tempString = strchr(tempString, '\0'), tempString++;//Нахождение конца и перемецение на один символ дальше
	*(strchr(tempString, c)) = '\0';//Нахождение символа * и постановка конца строки туда.
	lvl.name = (char *)malloc(strlen(tempString) * sizeof(char));
	strcpy(lvl.name, tempString);
	//if (DEBAG) printf("DEBAG function loadlvl2 IdType:%d IdTypeParrent:%d Name:%s\n", lvl.idType, lvl.idTypeParrent, lvl.name);
}

void loadlvlMeta(char* str, meta &met, char **text, int &integer);
void loadlvl3(char* str, lvl3 &lvl, bool needMeta) {
	//printf("У нас %s проблемы.\n", (_CrtCheckMemory()? "присутствуют" : "отсутствуют"));
	//if (DEBAG) printf("DBAG function loadlvl3 String:%s\n", str);
	char c = '*';
	char *tempString = (char*)malloc(strlen(str) * sizeof(char));
	char *tempMetaString = (char*)malloc(strlen(str) * sizeof(char));
	//printf("sizeof tempString %d\n", strlen(tempString));
	strcpy(tempString, str);
	tempString = strchr(tempString, c), tempString++;//Нахождение первого символа * и удаление его
	*(strchr(tempString, c)) = '\0';//Нахождение второго символа * и постановка конца строки туда.
	lvl.idTypeParrent = atoi(tempString);
	tempString = strchr(tempString, '\0'), tempString++;//Нахождение конца и перемецение на один символ дальше
	*(strchr(tempString, c)) = '\0';//Нахождение символа * и постановка конца строки туда.
	lvl.name = (char *)malloc((strlen(tempString) + 1) * sizeof(char));
	strcpy(lvl.name, tempString);
	tempString = strchr(tempString, '\0'), tempString++;//Нахождение конца и перемецение на один символ дальше чтобы была возможность искать дальше 
	//if (DEBAG) printf("DEBAG function loadlvl3 String:%s\n", tempString);
	tempString = strchr(tempString, '{'), tempString++;//Нахождение первого символа { и удаление его
	*(strchr(tempString, '}')) = '\0';//Нахождение второго символа } и постановка конца строки туда.
	//if (DEBAG) printf("DEBAG function loadlvl3 IdTypeParrent:%d Name:%s meta:%s\n", lvl.idTypeParrent, lvl.name, tempString);
	//printf("У нас %s проблемы.\n", (_CrtCheckMemory() ? "присутствуют" : "отсутствуют"));
	if (!needMeta)
		return;
	char* metString = 0;
	int countIntMeta = 0, countStringMeta = 0, metInt = 0;
	meta met;
	met.idOnType = 0, met.name = "", met.variableType = variableTypes::___INT, met.vizible = false;
	char *copyTempMetaString = tempMetaString;//Делаем копию ссылки на начало строки
	strcpy(tempMetaString, tempString);
	while (true) {
		bool needBreak = false;
		if (strchr(tempMetaString, ',') == NULL)//Если true значит мы нашли последнее значение
			needBreak = true;
		else
			*(strchr(tempMetaString, ',')) = '\0';//Нахождение символа ',' и постановка конца строки туда.
		tempMetaString = strchr(tempMetaString, c);//Нахождение первого символа * и перемещение туда
		loadlvlMeta(tempMetaString, met, &metString, metInt);
		if (met.variableType == variableTypes::___INT)
			countIntMeta++;
		else
			countStringMeta++;
		tempMetaString = strchr(tempMetaString, '\0'), tempMetaString++; //Нахождение конца и перемецение на один символ дальше
		if (needBreak)
			break;
	}
	//printf("countIntMeta: %d countStringMeta: %d\n", countIntMeta, countStringMeta);
	lvl.metadata = (meta*)malloc((countIntMeta + countStringMeta) * sizeof(meta));
	lvl.IntegerMass = (int*)malloc(countIntMeta * sizeof(int));
	lvl.StringMass = (char**)malloc(countStringMeta * sizeof(char*));
	lvl.size = countIntMeta + countStringMeta;
	countIntMeta = 0, countStringMeta = 0;
	tempMetaString = copyTempMetaString;//Возвращаем указатель на начато выделенного куска памяти и уже туда копируем что-либо
	strcpy(tempMetaString, tempString);

	while (true) {
		bool needBreak = false;
		if (strchr(tempMetaString, ',') == NULL)//Если true значит мы нашли последнее значение
			needBreak = true;
		else
			*(strchr(tempMetaString, ',')) = '\0';//Нахождение символа ',' и постановка конца строки туда.
		tempMetaString = strchr(tempMetaString, c);//Нахождение первого символа * и перемещение туда
		loadlvlMeta(tempMetaString, lvl.metadata[(countIntMeta + countStringMeta)], &metString, metInt);
		if (!(countIntMeta + countStringMeta))
			lvl.metadata[(countIntMeta + countStringMeta)].vizible = true;
		else
			lvl.metadata[(countIntMeta + countStringMeta)].vizible = false;
		if (lvl.metadata[(countIntMeta + countStringMeta)].variableType == variableTypes::___INT) {
			lvl.IntegerMass[countIntMeta] = metInt;
			lvl.metadata[(countIntMeta + countStringMeta)].idOnType = countIntMeta;
			//printf("Lvl:%20s Metdata: idOnType =%2d, variableType = INT, vizible =%2d, met.name = %30s, metInt = %3d\n", lvl.name, lvl.metadata[(countIntMeta + countStringMeta)].idOnType, lvl.metadata[(countIntMeta + countStringMeta)].vizible, lvl.metadata[(countIntMeta + countStringMeta)].name, metInt);
			countIntMeta++;
		}
		else {
			lvl.StringMass[countStringMeta] = (char*)malloc(strlen(metString) * sizeof(char));
			strcpy(lvl.StringMass[countStringMeta], metString);
			lvl.metadata[(countIntMeta + countStringMeta)].idOnType = countStringMeta;
			//printf("Lvl:%20s Metdata: idOnType =%2d, variableType = STR, vizible =%2d, met.name = %30s, metString = %s\n", lvl.name, lvl.metadata[(countIntMeta + countStringMeta)].idOnType, lvl.metadata[(countIntMeta + countStringMeta)].vizible, lvl.metadata[(countIntMeta + countStringMeta)].name, metString);
			countStringMeta++;
		}
		tempMetaString = strchr(tempMetaString, '\0'), tempMetaString++; //Нахождение конца и перемецение на один символ дальше
		//printf("Lvl:%20s Metdata: idOnType =%2d, key=%2d, variableType =%2d, vizible =%2d, met.name = %30s, metInt = %3d, metString = %s\n", lvl.name, lvl.metadata[(countIntMeta + countStringMeta)-1].idOnType, lvl.metadata[(countIntMeta + countStringMeta) - 1].key, lvl.metadata[(countIntMeta + countStringMeta)-1].variableType, lvl.metadata[(countIntMeta + countStringMeta)-1].vizible, lvl.metadata[(countIntMeta + countStringMeta) - 1].name, metInt, metString);
		if (needBreak)
			break;
	}
	//printf("У нас %s проблемы с повреждением памяти.\n", (_CrtCheckMemory() ? "присутствуют" : "отсутствуют"));
	//printf("metdata: met.idOnType = %d, met.idOnType = %d, met.name = %s, met.variableType = %d, met.vizible = %d, metInt = %d, metString = %s\n", met.idOnType, met.idOnType, met.name, met.variableType, met.vizible, metInt, metString);
}

void loadlvlMeta(char* str, meta &met, char **text, int &integer) {
	char c = '*';
	char *tempString = (char*)malloc(strlen(str) * sizeof(char));
	strcpy(tempString, str);
	//printf("//*key*name*:i:123 OR *key*name*:S:*Text* \n %s \n", tempString);
	tempString = strchr(tempString, c), tempString++;//Нахождение первого символа * и удаление его
	//key*name*:i:123 OR key*name*:S:*Text*
	//printf("//key*name*:i:123 OR key*name*:S:*Text* \n %s \n", tempString);
	*(strchr(tempString, c)) = '\0';//Нахождение второго символа * и постановка конца строки туда.
	//key'\0'name*:i:123 OR key'\0'name*:S:*Text*
	met.key = atoi(tempString);
	tempString = strchr(tempString, '\0'), tempString++;//Нахождение конца и перемецение на один символ дальше
	//name*:i:123 OR name*:S:*Text*
	*(strchr(tempString, c)) = '\0';//Нахождение символа * и постановка конца строки туда.
	//name'\0':i:123 OR name'\0':S:*Text*
	met.name = (char *)malloc(strlen(tempString) * sizeof(char));
	strcpy(met.name, tempString);
	tempString = strchr(tempString, '\0'), tempString++;//Нахождение конца и перемецение на один символ дальше
	//:i:123 OR :S:*Text*
	*(strchr(tempString, ':')) = '\0';
	//'\0'i:123 OR '\0'S:*Text*
	tempString = strchr(tempString, '\0'), tempString++;
	//i:123 OR S:*Text*
	*(strchr(tempString, ':')) = '\0';
	//i'\0'123 OR S'\0'*Text*
	char type = tempString[0];
	tempString = strchr(tempString, '\0'), tempString++;
	//123 OR *Text*
	switch (type)
	{
	case 'i'://integer
		met.variableType = variableTypes::___INT;
		integer = atoi(tempString);
		break;
	case 'S'://Stiring
		//*Text*
		met.variableType = variableTypes::___STRING;
		tempString = strchr(tempString, c), tempString++;
		//Text*
		//printf("%s, strlen: %d \n", tempString, strlen(tempString));
		*(strchr(tempString, c)) = '\0';
		//Text
		*text = ((char*)malloc(strlen(tempString) * sizeof(char)));
		strcpy(*text, tempString);
		break;
	}
	//if (DEBAG) printf("DEBAG function loadlvlMeta key:%d Name:%s Type:%s, data string:%s data integer:%d \n", met.key, met.name, (met.variableType == variableTypes::___INT ? "INT" : "STRING"), text, integer);
}

void printTree(lvl1 *root, int countlvl1, bool full) {
	System::Console::BackgroundColor = (System::ConsoleColor) setting.colorBackground;
	System::Console::Clear();//Очистка
	System::Console::ForegroundColor = (System::ConsoleColor) setting.colorDefaultText;
	for (int i = 0; i < countlvl1; i++) {//Количество элементов на первом уровне 
		printf("|-***- %s \n", root[i].name);
		for (int j = 0; j < root[i].size; j++) {//Количество элементов на 2-х уровнях
			if (root[i].haveLvl2) {
				printf("	|-**- %s \n", root[i].dataLvl2[j].name);
				for (int h = 0; h < root[i].dataLvl2[j].size; h++) {//Количество элементов на 3-х уровнях
					printf("		|-*- %s\n", root[i].dataLvl2[j].data[h].name);
					for (int r = 0; r < root[i].dataLvl2[j].data[h].size; r++) {
						if (root[i].dataLvl2[j].data[h].metadata[r].vizible || full) {
							if (root[i].dataLvl2[j].data[h].metadata[r].variableType == variableTypes::___INT)
								printf("			|- %s:%3d \n", root[i].dataLvl2[j].data[h].metadata[r].name, root[i].dataLvl2[j].data[h].IntegerMass[root[i].dataLvl2[j].data[h].metadata[r].idOnType]);
							else
								printf("			|- %s - %s \n", root[i].dataLvl2[j].data[h].metadata[r].name, root[i].dataLvl2[j].data[h].StringMass[root[i].dataLvl2[j].data[h].metadata[r].idOnType]);
						}
					}
				}

			}
			else {
				printf("		|-*- %s \n", root[i].dataLvl3[j].name);
				for (int r = 0; r < root[i].dataLvl3[j].size; r++) {
					if (root[i].dataLvl3[j].metadata[r].vizible || full) {
						if (root[i].dataLvl3[j].metadata[r].variableType == variableTypes::___INT)
							printf("			|- %s:%3d \n", root[i].dataLvl3[j].metadata[r].name, root[i].dataLvl3[j].IntegerMass[root[i].dataLvl3[j].metadata[r].idOnType]);
						else
							printf("			|- %s - %s \n", root[i].dataLvl3[j].metadata[r].name, root[i].dataLvl3[j].StringMass[root[i].dataLvl3[j].metadata[r].idOnType]);
					}
				}
			}

		}
	}
	getchar();
}
