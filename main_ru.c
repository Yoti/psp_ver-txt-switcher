/**
 * VERSiON.TXT SWiTCHER v1.1 by Yoti @ 25.10.2011
 * Licenced under GPLv3 (http://www.gnu.org/licenses/gpl-3.0.html)
 * Special thanks to Team PRO and PSPx.RU Team (ErikPshat & Frostgater)
 * You may contact me via PM on lan.st or wololo.net/talk
 * ...or donate via PayPal:
 * http://procfw.wen.ru/donate.html
 *
 * VERSiON.TXT SWiTCHER v1.1 от Yoti @ 25.10.2011
 * Лицензированно под GPLv3 (http://code.google.com/p/gpl3rus)
 * Особая благодарность PSPx.RU Team (ErikPshat & Frostgater) и Team PRO
 * При необходимости мне можно написать на pspx.ru или pspfaqs.ru
 * ...или сделать небольшое пожертвование через WebMoney:
 * R100422509682, U323272178662, Z328879459191, E156367214886
 **/

#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <string.h> // str???();

#define printf pspDebugScreenPrintf
#define MIN_VALID_FILE_SIZE 135 // оригинальный version.txt из оф.прошивки 1.00
#define MAX_VALID_FILE_SIZE 164 // отредактированный файл из retail прошивки, win-переносы и пустая строка в конце
#define MAJOR_VER 1
#define MINOR_VER 1
#define DEFAULT_TEXT_COLOR 0xFF0080FF

#ifdef OFW_MODE
	PSP_MODULE_INFO("VTSWiTCH_YOTi_OFW", 0x0000, MAJOR_VER, MINOR_VER);
#else
	PSP_MODULE_INFO("VTSWiTCH_YOTi_CFW", 0x0800, MAJOR_VER, MINOR_VER);
#endif
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
PSP_HEAP_SIZE_KB(4*1024);

int exitCallback(int arg1, int arg2, void *common)
{
	sceKernelExitGame();

	return 0;
}

int callbackThread(SceSize args, void *argp)
{
	int cbid;

	cbid = sceKernelCreateCallback("Exit Callback", (void*)exitCallback, NULL);
	sceKernelRegisterExitCallback(cbid);
	sceKernelSleepThreadCB();

	return 0;
}

int setupCallbacks(void)
{
	int thid = 0;

	thid = sceKernelCreateThread("update_thread", callbackThread, 0x11, 0xFA0, 0, 0);
	if (thid >= 0)
		sceKernelStartThread(thid, 0, 0);

	return thid;
}

int pspgo;
char buf[0xFF];
SceCtrlData Key;
int file_exists[4] = {0, 0, 0, 0};
char filename[16] = "version#.txt";
char versions[16] = "000000000000";
char*dest_name_all[2] = {"version.txt", "usbversion.txt"};
u32 version_colors[4] = {0xFF00FF00, 0xFF0000FF, 0xFFFF0000, 0xFFFF00FF}; // формат: aaBBggRR
#ifdef OFW_MODE
char*dest_path_ms0[3] = {"ms0:/seplugins", "ms0:/plugins", "ms0:"};
char*dest_path_ef0[6] = {"ms0:/seplugins", "ms0:/plugins", "ms0:", "ef0:/seplugins", "ef0:/plugins", "ef0:"};
#else
char*dest_path_ms0[4] = {"ms0:/seplugins", "ms0:/plugins", "ms0:", "flash0:/vsh/etc"};
char*dest_path_ef0[7] = {"ms0:/seplugins", "ms0:/plugins", "ms0:", "ef0:/seplugins", "ef0:/plugins", "ef0:", "flash0:/vsh/etc"};
#endif

#ifndef OFW_MODE
int remount(void)
{
	SceUID ret;

	ret = sceIoUnassign("flash0:");
	if (ret < 0)
		return -1;

	ret = sceIoAssign("flash0:", "lflash0:0,0", "flashfat0:", IOASSIGN_RDWR, NULL, 0);
	if (ret < 0)
		return -2;

	ret = sceIoOpen("flash0:/ignore.me", PSP_O_WRONLY | PSP_O_CREAT, 0777);
	if (ret < 0)
		return -3;

	sceIoClose(ret);
	sceIoRemove("flash0:/ignore.me");
	return 0;
}
#endif

/// FROM PROCFW.GOOGLECODE.COM
int is_pspgo(void)
{
	// This call will always fail, but with a different error code depending on the model
	SceUID result = sceIoOpen("ef0:/", 1, 0777);

	// Check for "No such device" error
	return ((result == (int)0x80020321) ? 0 : 1);
}
/// THANKS TO PROCFW TEAM! =)

int read_vtxt(char*path)
{
	int i;
	int ret;
	SceUID file;

	for (i = 0; i < 0xFF; i++)
		buf[i] = 0; // чистим буфер

	file = sceIoOpen(path, PSP_O_RDONLY, 0777); 
	if (file >= 0)
	{
		ret = sceIoLseek(file, 0, PSP_SEEK_END); // получаем размер файла
		sceIoLseek(file, 0, PSP_SEEK_SET); // и возвращаемся к началу

		sceIoRead(file, buf, ret);
	}
	else
		ret = -1;
	sceIoClose(file);

	return ret;
}

int flash(char*path, char*name, int file_num)
{
	int size;
	SceUID file;

	pspDebugScreenSetTextColor(0xFF0000FF); // красный
	filename[7] = file_num+0x30+1; // 0x30 это код символа "ноль"; +1 т.к. с отсчёт с ноля
	size = read_vtxt(filename);
	if ((size > MIN_VALID_FILE_SIZE-1) && (size < MAX_VALID_FILE_SIZE+1))
	{
		char filepath[64] = "";
		strcpy(filepath, path);
		strcat(filepath, "/");
		strcat(filepath, name);

		sceIoMkdir(path, 0777);

		sceIoRemove(filepath);
		file = sceIoOpen(filepath, PSP_O_WRONLY | PSP_O_CREAT, 0777);
		if (file >= 0)
		{
			sceIoWrite(file, buf, size);
			pspDebugScreenSetTextColor(0xFF00FF00); // зелёный
			printf(">> Файл успешно записан как %s!\n", filepath);
		}
		else
			printf(">> Ошибка %08x при записи файла!\n", file);
		sceIoClose(file);
	}
	else
		printf(">> Ошибка %08x при чтении файла!\n", size);

	pspDebugScreenSetTextColor(DEFAULT_TEXT_COLOR);
	sceKernelDelayThread(2*1000*1000);

	return 0;
}

int show_vtxt(char*path)
{
	int i;
	int ret;
	SceUID file;

	for (i = 0; i < 0xFF; i++)
		buf[i] = 0; // чистим буфер

	file = sceIoOpen(path, PSP_O_RDONLY, 0777); 
	if (file >= 0)
	{
		ret = sceIoLseek(file, 0, PSP_SEEK_END); // получаем размер файла
		sceIoLseek(file, 0, PSP_SEEK_SET); // и возвращаемся к началу

		sceIoRead(file, buf, ret);
		printf("%s", buf); // вывод файла на экран
	}
	else
		ret = -1;
	sceIoClose(file);

	return ret;
}

void scan(int k)
{
	int i, size;
	char filepath[64] = "";

	for (i = 0; i < k+1; i++) // вывод существующих файлов; +1 - хитрый хак
	{
		if (pspgo == 0)
		{
			strcpy(filepath, dest_path_ms0[i]);
			strcat(filepath, "/");
			strcat(filepath, dest_name_all[0]);

			size = read_vtxt(filepath);

			if ((size > MIN_VALID_FILE_SIZE-1) && (size < MAX_VALID_FILE_SIZE+1))
				printf("%s - v%c.%c%c     ", filepath, buf[0x8], buf[0xA], buf[0xB]);
			else
				printf("%s - нет файла ", filepath);
			printf("\n");
		}
		else if (pspgo == 1)
		{
			strcpy(filepath, dest_path_ef0[i]);
			strcat(filepath, "/");
			strcat(filepath, dest_name_all[0]);

			size = read_vtxt(filepath);

			if ((size > MIN_VALID_FILE_SIZE-1) && (size < MAX_VALID_FILE_SIZE+1))
				printf("%s - v%c.%c%c     ", filepath, buf[0x8], buf[0xA], buf[0xB]);
			else
				printf("%s - нет файла ", filepath);
			printf("\n");
		}
	} // for...

	if (pspgo == 0)
	{
		strcpy(filepath, dest_path_ms0[0]); // ms0:/seplugins
		strcat(filepath, "/");
		strcat(filepath, dest_name_all[1]); // usbversion.txt

		size = read_vtxt(filepath);

		if ((size > MIN_VALID_FILE_SIZE-1) && (size < MAX_VALID_FILE_SIZE+1))
			printf("%s - v%c.%c%c     ", filepath, buf[0x8], buf[0xA], buf[0xB]);
		else
			printf("%s - нет файла ", filepath);
		printf("\n");
	}
	else if (pspgo == 1)
	{
		strcpy(filepath, dest_path_ef0[0]); // ms0:/seplugins
		strcat(filepath, "/");
		strcat(filepath, dest_name_all[1]); // usbversion.txt

		size = read_vtxt(filepath);

		if ((size > MIN_VALID_FILE_SIZE-1) && (size < MAX_VALID_FILE_SIZE+1))
			printf("%s - v%c.%c%c     ", filepath, buf[0x8], buf[0xA], buf[0xB]);
		else
			printf("%s - нет файла ", filepath);
		printf("\n");

		// ------------------------------------------------------- //

		strcpy(filepath, dest_path_ef0[2]); // ef0:/seplugins
		strcat(filepath, "/");
		strcat(filepath, dest_name_all[1]); // usbversion.txt

		size = read_vtxt(filepath);

		if ((size > MIN_VALID_FILE_SIZE-1) && (size < MAX_VALID_FILE_SIZE+1))
			printf("%s - v%c.%c%c     ", filepath, buf[0x8], buf[0xA], buf[0xB]);
		else
			printf("%s - нет файла ", filepath);
		printf("\n");
	} // if...
}

void menu(void)
{
	int x, y, k = 0, y_result;
	int i1 = 0, i2 = 0, i3 = 0;

	if (pspgo == 0)
		k = (sizeof(dest_path_ms0)/sizeof*(dest_path_ms0)) - 1; // количество элементов в массиве
	else if (pspgo == 1)
		k = (sizeof(dest_path_ef0)/sizeof*(dest_path_ef0)) - 1; // количество элементов в массиве

	pspDebugScreenClear();
	pspDebugScreenSetTextColor(DEFAULT_TEXT_COLOR);
	printf("VERSiON.TXT SWiTCHER v%01i.%01i", MAJOR_VER, MINOR_VER);
	#ifdef OFW_MODE
	printf(" (РЕЖИМ OFW)");
	#else
	printf(" (РЕЖИМ CFW)");
	#endif
	printf("\n\n");
	pspDebugScreenSetTextColor(0xFFFF0000); // синий
	y = pspDebugScreenGetY();
	printf(" %c & %c - выбор ячейки, текущая: ", '<', '>');
	x = pspDebugScreenGetX();
	pspDebugScreenSetTextColor(0xFFFF00FF); // сиреневый
	printf("ячейка #%i (v%c.%c%c)\n", 1, versions[0], versions[1], versions[2]);
	pspDebugScreenSetTextColor(0xFFFF0000); // синий
	printf(" %c & %c - выбор пути, текущий:   ", 'v', '^');
	pspDebugScreenSetTextColor(0xFFFF00FF); // сиреневый
	printf("%s\n", dest_path_ms0[0]);
	pspDebugScreenSetTextColor(0xFFFF0000); // синий
	printf(" \6 & \5 - выбор имени, текущее:  ");
	pspDebugScreenSetTextColor(0xFFFF00FF); // сиреневый
	printf("%s\n\n", dest_name_all[0]);
	pspDebugScreenSetTextColor(0xFF00FF00); // зелёный
	printf(" \3 - записать файл");
	pspDebugScreenSetTextColor(DEFAULT_TEXT_COLOR); // сиреневый
	printf(" или");
	pspDebugScreenSetTextColor(0xFF0000FF); // красный
	printf(" \4 - выйти из программы\n\n");

	pspDebugScreenSetTextColor(DEFAULT_TEXT_COLOR); // стандарт
	scan(k);
	y_result = pspDebugScreenGetY();

	pspDebugScreenSetXY(0, y_result+1);
	printf("Подождите...");
	sceKernelDelayThread(3*1000*1000);
	pspDebugScreenSetXY(0, y_result+1);
	printf("                                                            ");
	while (1)
	{
		sceCtrlReadBufferPositive(&Key, 1);

		if (Key.Buttons & PSP_CTRL_CROSS)
		{
			pspDebugScreenSetXY(0, y_result+1);
			printf("                                                            ");

			pspDebugScreenSetXY(0, y_result+1);
			if (pspgo == 0)
				flash(dest_path_ms0[i2], dest_name_all[i3], i1);
			else if (pspgo == 1)
				flash(dest_path_ef0[i2], dest_name_all[i3], i1);

			sceKernelDelayThread(2*1000*1000);

			pspDebugScreenSetXY(0, y_result-(k+1+1)); // кол-во путей + 1 строка с usbversion.txt
			scan(k);

			pspDebugScreenSetXY(0, y_result+1);
			printf("                                                            ");
		}
		else if (Key.Buttons & PSP_CTRL_CIRCLE)
			sceKernelExitGame();

		else if (Key.Buttons & PSP_CTRL_LEFT)
		{
			if (i1 > 0)
				i1--;

			pspDebugScreenSetXY(x + 0, y + 0);
			printf("                ");

			pspDebugScreenSetXY(x + 0, y + 0);
			pspDebugScreenSetTextColor(0xFFFF00FF); // сиреневый
			printf("ячейка #%i (v%c.%c%c)", i1+1, versions[0+3*i1], versions[1+3*i1], versions[2+3*i1]);
		}
		else if (Key.Buttons & PSP_CTRL_RIGHT)
		{
			if (i1 < 3)
				i1++;

			pspDebugScreenSetXY(x + 0, y + 0);
			printf("                ");

			pspDebugScreenSetXY(x + 0, y + 0);
			pspDebugScreenSetTextColor(0xFFFF00FF); // сиреневый
			printf("ячейка #%i (v%c.%c%c)", i1+1, versions[0+3*i1], versions[1+3*i1], versions[2+3*i1]);
		}

		else if (Key.Buttons & PSP_CTRL_UP)
		{
			if (i2 > 0)
				i2--;

			pspDebugScreenSetXY(x + 0, y + 1);
			printf("                ");

			pspDebugScreenSetXY(x + 0, y + 1);
			pspDebugScreenSetTextColor(0xFFFF00FF); // сиреневый
			if (pspgo == 0)
				printf("%s", dest_path_ms0[i2]);
			else if (pspgo == 1)
				printf("%s", dest_path_ef0[i2]);

		}
		else if (Key.Buttons & PSP_CTRL_DOWN)
		{
			if (i2 < k)
				i2++;

			pspDebugScreenSetXY(x + 0, y + 1);
			printf("                ");

			pspDebugScreenSetXY(x + 0, y + 1);
			pspDebugScreenSetTextColor(0xFFFF00FF); // сиреневый
			if (pspgo == 0)
				printf("%s", dest_path_ms0[i2]);
			else if (pspgo == 1)
				printf("%s", dest_path_ef0[i2]);
		}

		else if (Key.Buttons & PSP_CTRL_SQUARE)
		{
			if (i3 == 1)
				i3 = 0;

			pspDebugScreenSetXY(x + 0, y + 2);
			printf("                ");

			pspDebugScreenSetXY(x + 0, y + 2);
			pspDebugScreenSetTextColor(0xFFFF00FF); // сиреневый
			printf("%s", dest_name_all[i3]);
		}
		else if (Key.Buttons & PSP_CTRL_TRIANGLE)
		{
			if (i3 == 0)
				i3 = 1;

			pspDebugScreenSetXY(x + 0, y + 2);
			printf("                ");

			pspDebugScreenSetXY(x + 0, y + 2);
			pspDebugScreenSetTextColor(0xFFFF00FF); // сиреневый
			printf("%s", dest_name_all[i3]);
		}

		sceKernelDelayThread(0.1*1000*1000);
	}
}

void print(void)
{
	pspDebugScreenClear();
	pspDebugScreenSetTextColor(DEFAULT_TEXT_COLOR);
	printf("VERSiON.TXT SWiTCHER\n\n");

	int i, size;
	for (i = 0; i < 4; i++)
	{
		pspDebugScreenSetTextColor(version_colors[i]);
		printf(">> Ячейка: %i\n", i+1);
		filename[7] = i+0x30+1; // 0x30 это код символа "ноль"; +1 т.к. с отсчёт с ноля
		size = show_vtxt(filename);
		if ((size > MIN_VALID_FILE_SIZE-1) && (size < MAX_VALID_FILE_SIZE+1))
		{
			file_exists[i] = 1;
			if (pspDebugScreenGetX() > 0)
				printf("\n");
			versions[0+3*i] = buf[0x8];
			versions[1+3*i] = buf[0xA];
			versions[2+3*i] = buf[0xB];
			printf(">> Версия: %c.%c%c\n", versions[0+3*i], versions[1+3*i], versions[2+3*i]);
		}
		else
			printf(">> Файл version%i.txt не найден =(\n", i+1);
		printf("\n");
	}

	pspDebugScreenSetXY(0, 0);
	pspDebugScreenSetTextColor(DEFAULT_TEXT_COLOR);
	printf("VERSiON.TXT SWiTCHER v%i.%i [", MAJOR_VER, MINOR_VER);
	if !(file_exists[0] == 0 && file_exists[1] == 0 && file_exists[2] == 0 && file_exists[3] == 0)
	{
		pspDebugScreenSetTextColor(0xFF00FF00); // зелёный
		printf("X - ДАЛЕЕ");
		pspDebugScreenSetTextColor(DEFAULT_TEXT_COLOR);
		printf(" / ");
	}
	pspDebugScreenSetTextColor(0xFF0000FF); // красный
	printf("O - ВЫХОД");
	pspDebugScreenSetTextColor(DEFAULT_TEXT_COLOR);
	printf("]");

	while (1)
	{
		sceCtrlReadBufferPositive(&Key, 1);

		if ((Key.Buttons & PSP_CTRL_CROSS) && !(file_exists[0] == 0 && file_exists[1] == 0 && file_exists[2] == 0 && file_exists[3] == 0))
			menu();
		else if (Key.Buttons & PSP_CTRL_CIRCLE)
			sceKernelExitGame();

		sceKernelDelayThread(0.1*1000*1000);
	}
}

#ifndef OFW_MODE
void check_gen_hack(void)
{
	if (sceKernelDevkitVersion() == 0x05050010)
	{
		int size = read_vtxt("flash0:/vsh/etc/version.txt");
		if ((buf[0x8] == '9') && (buf[0xA] == '9'))
		{
			pspDebugScreenClear();
			pspDebugScreenSetTextColor(DEFAULT_TEXT_COLOR);
			printf("VERSiON.TXT SWiTCHER v%01i.%01i от Yoti\n", MAJOR_VER, MINOR_VER);
			printf("\n");
			pspDebugScreenSetTextColor(0xFF0000FF); // красный
			printf(" ВНИМАНИЕ!!!\n");
			pspDebugScreenSetTextColor(DEFAULT_TEXT_COLOR);
			printf("В результате проверки был найден патч файла version.txt от GEN.\n");
			printf("Этот патч будет мешать обновлению на новые прошивки. Исправить?\n");
			printf("\n");
			printf(" \3 - восстановить (рекомендуется)\n");
			printf(" \4 - пропустить только в этот раз\n");
			printf(" \5 - пропустить и больше не спрашивать\n");

			while (1)
			{
				sceCtrlReadBufferPositive(&Key, 1);

				if (Key.Buttons & PSP_CTRL_CROSS)
				{
					buf[0x8] = '5';
					buf[0xA] = '5';
					buf[0xB] = '0';

					sceIoRemove("flash0:/vsh/etc/version.txt");
					SceUID file = sceIoOpen("flash0:/vsh/etc/version.txt", PSP_O_WRONLY | PSP_O_CREAT, 0777);
					if (file >= 0)
					{
						sceIoWrite(file, buf, size);
						pspDebugScreenSetTextColor(0xFF00FF00); // зелёный
						printf("\n>> Всё готово! =)\n");
					}
					else
					{
						pspDebugScreenSetTextColor(0xFF0000FF); // красный
						printf("\n>> Ошибка %08x!\n", file);
					}
					sceIoClose(file);

					break;
				}
				else if (Key.Buttons & PSP_CTRL_CIRCLE)
				{
					printf("\nПодождите...\n");

					break;
				}
				else if (Key.Buttons & PSP_CTRL_TRIANGLE)
				{
					SceUID file;
					file = sceIoOpen("neverask.me", PSP_O_WRONLY | PSP_O_CREAT, 0777);
					sceIoClose(file);

					printf("\nПодождите...\n");

					break;
				}

				sceKernelDelayThread(0.1*1000*1000);
			}
			sceKernelDelayThread(2*1000*1000);
		}
	}
}
#endif

int main(int argc, char *argv[])
{
	setupCallbacks();
	pspDebugScreenInit();

	pspgo = is_pspgo();
	#ifndef OFW_MODE
	if (remount() < 0)
		sceKernelExitGame();

	SceUID file;
	file = sceIoOpen("neverask.me", PSP_O_RDONLY, 0777);
	if (file < 0)
		check_gen_hack();
	sceIoClose(file);
	#endif

	print();

	return 0;
}
