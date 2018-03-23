#include "stdafx.h"


//using namespace std;
//using namespace System;
//using namespace System::IO;

#pragma warning(disable : 4996)

#define ENTER 13
#define ESC 27
#define UP 72
#define DOWN 80

char dan[9][100] = {
	"Вуз с самой высокой стоимостью обучения?                    ",
	"Какой  основан позже всех?                                  ",
	"Список Московских Вузов с оплатой выше 200000               ",
	"Алфавитный список вузов.                                    ",
	"Количество Вузов с военной кафедрой.                        ",
	"Диаграмма.Процентное соотношение стоимости обучения в Вузах ",
	"Есть ли вузы из разных городов,основанные в один год        ",
	"Удаление                                                    ",
	"Выход                                                       "
};

char BlankLine[] = "                                                              ";
int NC;

struct z {
	char name[20];
	char vid[20];
	long summa;
	int data;
	char kaf[6];
};

struct sp {
	char fio[20];
	long summa;
	struct sp *sled;
	struct sp *pred;
} *spisok;

int menu(int);
void maxim(struct z*);
void maxim1(struct z*);
void first(struct z*);
void kolvo(struct z *);
void alfalist(struct z*);
void vstavka(struct z*, char*);
void listing(struct z*);
void diagram(struct z*);
void difficult(struct z*);
void ydalenie(struct z*);
char *gets(char*);

int NOmain()
{

	int i, n;
	FILE *in;
	struct z *clients;

	setlocale(LC_CTYPE, "Russian");
	System::Console::CursorVisible::set(false);
	System::Console::BufferHeight = System::Console::WindowHeight;
	System::Console::BufferWidth = System::Console::WindowWidth;

	if ((in = fopen("spisok.dat", "r")) == NULL)
	{
		printf("\nФайл spisok.dat не открыт !");
		getch(); exit(1);
	}

	fscanf(in, "%d", &NC);
	clients = (struct z*)malloc(NC * sizeof(struct z)); //Память под массив структур 

	for (i = 0; i < NC; i++) //чтение из файла 
		fscanf(in, "%s%s%ld%d%s", clients[i].name, clients[i].vid, &clients[i].summa, &clients[i].data, &clients[i].kaf);
	for (i = 0; i < NC; i++) //Печать структур
		printf("\n%-20s %-20s %7ld %7d %7s", clients[i].name, clients[i].vid, clients[i].summa, clients[i].data, clients[i].kaf);

	getch(); // Пауза

	while (1) //Отрисовка меню 
	{
		System::Console::ForegroundColor = System::ConsoleColor::Gray;
		System::Console::BackgroundColor = System::ConsoleColor::Black;
		System::Console::Clear();
		System::Console::ForegroundColor = System::ConsoleColor::Black;
		System::Console::BackgroundColor = System::ConsoleColor::Gray;
		System::Console::CursorLeft = 10;
		System::Console::CursorTop = 4;
		printf(BlankLine);

		for (i = 0; i < 9; i++)
		{
			System::Console::CursorLeft = 10;
			System::Console::CursorTop = i + 5;
			printf(" %s ", dan[i]);
		}
		System::Console::CursorLeft = 10;
		System::Console::CursorTop = 14;
		printf(BlankLine);

		n = menu(9);

		switch (n) {
		case 1: maxim(clients); break;
		case 2: maxim1(clients); break;
		case 3: listing(clients); break;
		case 4: alfalist(clients); break;
		case 5: kolvo(clients); break;
		case 6: diagram(clients); break;
		case 7: difficult(clients); break;
		case 8: ydalenie(clients); break;
		case 9: exit(0);
		}
	}
	return 0;
}

int menu(int n)
{
	int y1 = 0, y2 = n - 1;
	char c = 1;
	while (c != ESC)
	{
		switch (c) {
		case DOWN: y2 = y1; y1++; break;
		case UP: y2 = y1; y1--; break;
		case ENTER: return y1 + 1;
		}
		if (y1 > n - 1) { y2 = n - 1; y1 = 0; }
		if (y1 < 0) { y2 = 0; y1 = n - 1; }

		System::Console::ForegroundColor = System::ConsoleColor::Black;
		System::Console::BackgroundColor = System::ConsoleColor::DarkGray;
		System::Console::CursorLeft = 11;
		System::Console::CursorTop = y1 + 5;
		printf(dan[y1]);
		System::Console::ForegroundColor = System::ConsoleColor::Black;
		System::Console::BackgroundColor = System::ConsoleColor::Gray;
		System::Console::CursorLeft = 11;
		System::Console::CursorTop = y2 + 5;
		printf(dan[y2]);

		c = getch(); // Считывание действия пользователя

	}
	exit(0);
}

void maxim(struct z* client)
{
	int i = 0;
	struct z best;
	strcpy(best.name, client[0].name);
	best.summa = client[0].summa;
	for (i = 1; i < NC; i++)
		if (client[i].summa > best.summa)
		{
			strcpy(best.name, client[i].name);
			best.summa = client[i].summa;
		}
	System::Console::ForegroundColor = System::ConsoleColor::Green;
	System::Console::BackgroundColor = System::ConsoleColor::Black;
	System::Console::CursorLeft = 10;
	System::Console::CursorTop = 15;
	printf("Максимальную стоимость обучения %ld рублей", best.summa);
	System::Console::CursorLeft = 10;
	System::Console::CursorTop = 16;
	printf("имеет ВУЗ %s", best.name);
	getch();
}

void maxim1(struct z* client)
{
	int i = 0;
	struct z best;
	strcpy(best.name, client[0].name);
	best.data = client[0].data;
	for (i = 1; i < NC; i++)
		if (client[i].data < best.data)
		{
			strcpy(best.name, client[i].name);
			best.data = client[i].data;
		}
	System::Console::ForegroundColor = System::ConsoleColor::Green;
	System::Console::BackgroundColor = System::ConsoleColor::Black;
	System::Console::CursorLeft = 10;
	System::Console::CursorTop = 15;
	printf("Самый старый вуз");
	System::Console::CursorLeft = 10;
	System::Console::CursorTop = 16;
	printf("Основан в %d году", best.data);
	System::Console::CursorLeft = 10;
	System::Console::CursorTop = 17;
	printf("Название вуза:%s", best.name);
	getch();
}

void kolvo(struct z *client)
{
	int i, k = 0;
	for (i = 0; i < NC; i++)
		if (strcmp(client[i].kaf, "Есть") == 0) k++;
	System::Console::ForegroundColor = System::ConsoleColor::Green;
	System::Console::BackgroundColor = System::ConsoleColor::Black;
	System::Console::CursorLeft = 10;
	System::Console::CursorTop = 15;
	printf("Вузов с \"Военной кафедрой\"");
	System::Console::CursorLeft = 10;
	System::Console::CursorTop = 16;
	printf("Всего : %d", k);
	getch();
}

void alfalist(struct z *client)
{
	int i;
	struct sp* nt, *z;
	System::Console::ForegroundColor = System::ConsoleColor::Green;
	System::Console::BackgroundColor = System::ConsoleColor::Black;
	if (!spisok)
		for (i = 0; i < NC; i++)
			vstavka(client, client[i].name);
	System::Console::Clear();
	printf("\nАлфавитный список ВУЗОВ");
	printf("\n===============================\n");
	z = 0;
	for (nt = spisok; nt != 0; z = nt, nt = nt->sled)
		printf("\n%-20s %ld", nt->fio, nt->summa);
	getch();
	System::Console::Clear();
	printf("Алфавитный список от Я до А");
	printf("\n===============================\n");
	for (nt = z; nt; nt = nt->pred)
		printf("\n%-20s %ld", nt->fio, nt->summa);
	getch();
}

void vstavka(struct z* client, char *fio)
{
	int i;
	struct sp *New, *nt, *z = 0;
	for (nt = spisok; nt != 0 && strcmp(nt->fio, fio) < 0; z = nt, nt = nt->sled)
		if (nt && strcmp(nt->fio, fio) == 0) return;
	New = (struct sp *) malloc(sizeof(struct sp));
	strcpy(New->fio, fio);
	New->pred = z;
	New->sled = nt;
	New->summa = 0;
	for (i = 0; i < NC; i++)
		if (strcmp(client[i].name, fio) == 0)
			New->summa += client[i].summa;
	if (!z) spisok = New;
	else z->sled = New;
	if (nt) nt->pred = New;
	return;
}

void listing(struct z* client)
{
	int i;
	struct z* nt;
	System::Console::ForegroundColor = System::ConsoleColor::Green;
	System::Console::BackgroundColor = System::ConsoleColor::Black;
	System::Console::Clear();
	printf("\n\r Список Московских Вузов с оплатой выше 200000");
	printf("\n\r===============================================\n\r");
	for (i = 0, nt = client; i < NC; nt++, i++)
		if (nt->summa > 200000 && strcmp(nt->vid, "Москва") == 0)
			printf("\n\r %-20s %ld р.", nt->name, nt->summa);
	getch();
}

void diagram(struct z *client)
{
	struct sp *nt;
	int len, i, NColor;
	long sum = 0;
	char str1[20];
	char str2[20];
	System::ConsoleColor Color;

	System::Console::ForegroundColor = System::ConsoleColor::Black;
	System::Console::BackgroundColor = System::ConsoleColor::White;
	System::Console::Clear();

	for (i = 0; i < NC; i++) sum = sum + client[i].summa;
	if (!spisok)
		for (i = 0; i < NC; i++)
			vstavka(client, client[i].name);
	Color = System::ConsoleColor::Black; NColor = 0;

	for (nt = spisok, i = 0; nt != 0; nt = nt->sled, i++)
	{
		sprintf(str1, "%s", nt->fio);
		sprintf(str2, "%3.1f%%", (nt->summa*100. / sum));
		System::Console::ForegroundColor = System::ConsoleColor::Black;
		System::Console::BackgroundColor = System::ConsoleColor::White;
		System::Console::CursorLeft = 5;
		System::Console::CursorTop = i + 1; printf(str1);
		System::Console::CursorLeft = 20; printf(str2);
		System::Console::BackgroundColor = ++Color; NColor++;
		System::Console::CursorLeft = 30;
		for (len = 0; len < nt->summa * 50 / sum; len++)printf(" ");
		if (NColor == 14)
		{
			Color = System::ConsoleColor::Black; NColor = 0;
		}
	}
	getch();
	return;
}

void difficult(struct z *client)
{
	int i, j, k;
	System::Console::ForegroundColor = System::ConsoleColor::Green;
	System::Console::BackgroundColor = System::ConsoleColor::Black;
	System::Console::Clear();
	printf("Вузы из разных городов,основанные в один год");
	printf("\n\r==========================================");
	k = 0;
	for (i = 0; i < NC; i++)
	{
		if (k == 1) break;
		for (j = 0; j < NC; j++)
		{
			if (strcmp(client[i].vid, client[j].vid) != 0 && client[i].data == client[j].data)
			{
				printf("\n\r%s %s %d", client[i].name, client[i].vid, client[i].data);
				printf("\n\r%s %s %d", client[j].name, client[j].vid, client[j].data);
				k++;
				break;
			}
		}
	}
	if (k == 0) printf("\nТаких вузов в списке нет:");
	getch();
}

void ydalenie(struct z*client)
{
	int i, flag = 0;
	struct sp* nt, *z;
	char s[80];
	alfalist(client);
	printf("\n");
	printf("\nВедите Вуз,который нужно удалить из списка:");
	SetConsoleCP(1251);
	gets(s);
	SetConsoleCP(866);
	z = NULL;
	for (nt = spisok; nt != 0; z = nt, nt = nt->sled)
		if (nt && strcmp(nt->fio, s) == 0)
			if (z != 0)
			{
				if (nt->sled != 0)
				{
					z->sled = nt->sled;//Середина списка
					nt->sled->pred = z;
					flag = 1;
				}
				else
				{
					z->sled = NULL;//Конец списка
					flag = 1;
				}
			}
			else
			{
				spisok = nt->sled;
				nt->sled->pred = NULL;//Начало Списка
				flag = 1;
			}
	if (flag != 0)
	{
		free(nt);
		System::Console::ForegroundColor = System::ConsoleColor::Green;
		System::Console::BackgroundColor = System::ConsoleColor::Black;
		if (!spisok)
			for (i = 0; i < NC; i++)
				vstavka(client, client[i].name);
		System::Console::Clear();
		printf("\nАлфавитный список ВУЗОВ");
		printf("\n===============================\n");
		z = 0;
		for (nt = spisok; nt != 0; z = nt, nt = nt->sled)
			printf("\n%-20s %ld", nt->fio, nt->summa);
		getch();
		System::Console::Clear();
		printf("Алфавитный список от Я до А");
		printf("\n===============================\n");
		for (nt = z; nt; nt = nt->pred)
			printf("\n%-20s %ld", nt->fio, nt->summa);
	}
	else
		printf("Таких элементов нет");
	getch();
}

char *gets(char *s)
{
	/*очистка буфера ввода */
	fflush(stdin);

	int i, k = getchar();

	/* Возвращаем NULL если ничего не введено */
	if (k == EOF)
		return NULL;

	/* Считываем и копируем в буфер символы пока не достигнем конца строки или файла */
	for (i = 0; k != EOF && k != '\n'; ++i) {
		s[i] = k;
		k = getchar();

		/* При обнаружении ошибки результирующий буфер непригоден */
		if (k == EOF && !feof(stdin))
			return NULL;
	}

	/* Нуль-терминируем и возвращаем буфер в случае успеха.
	Символ перевода строки в буфере не хранится. */
	s[i] = '\0';

	return s;
}