#include <stdio.h>
#include <stdlib.h>

int main()
{
    int n = 0;                                              // счетчик прочитанных байтов
    int NumOfByte = 0;                                      // колличество прочитанных байтов при определении Jpeg файла
    unsigned char Buff[4] = {0, 0, 0, 0};                   // приемный буфер для поиска совпадений в файле
    unsigned char MarkEOJPG[2] = {0xff, 0xd9};              // маркер конца jpg файла
    unsigned char SignLFH[4] = {0x50, 0x4b, 0x03, 0x04};    // Сигнатура Local file header


    // Вводим имя файла из директории программы
    printf("Please enter a filename in the following format: \"NAME.TYPE\" and press ENTER\n");
    char FileNameForScan[100];
    scanf("%s", FileNameForScan);
    //printf("\n%s", FileNameForScan);

    FILE *f;

    f = fopen(FileNameForScan, "rb");

    if(f == NULL)
    {
        printf("Error opening file\n");
        printf("\n\nPress 'e' to exit\n");
        while(getch() != 'e');
        exit(1);
    }

    //Сканируем файл на предмет совпадения маркера конца JPEG файла и если после него больше ничего нет, значит это он. Сдвигаем пару маркерных байтов по файлу
    while(1) // Как мне защититься от зацикливания при непредвиденной ошибке?
    {
        n += fread(Buff, sizeof(unsigned char), 2, f); // читаем очередные 2 байта
        //printf("n=%d\tBuff = %x\t%x\t%x\t%x\n", n, Buff[0], Buff[1], Buff[2], Buff[3]);

        if(!memcmp(Buff, MarkEOJPG, 2) && !NumOfByte) // сравниваем с маркером конца jpg файла, при первом совпадении делаем отметку в NumOfByte
            NumOfByte = n;

        if(feof(f)) // если дошли до конца файла завершаем цикл
            break;

        fseek(f, -1, SEEK_CUR); // смещаем положение указателя ч/з назад на один байт
        n -= 1;
    }
    fclose(f);

    if(NumOfByte == n)
        printf("\nThis is the non-zipjpeg file\n\n");
    else printf("\nThis is the zipjpeg file\n\n");
    //printf("%d\t%d\n", NumOfByte, n);


    f = fopen(FileNameForScan, "rb");
    if(f == NULL)
    {
        printf("Error opening file\n");
        printf("\n\nPress 'e' to exit\n");
        while(getch() != 'e');
        exit(1);
    }

    //Сканируем файл на предмет совпадения сигнатуры Local file header. Если нашли совпадение, выводим на экран содержимое zip файла.
    while(1)
    {
        n += fread(Buff, sizeof(unsigned char), 4, f);
        //printf("n=%d\tBuff = %x\t%x\t%x\t%x\n", n, Buff[0], Buff[1], Buff[2], Buff[3]);

        if(memcmp(Buff, SignLFH, 4) == 0)
        {
            fseek(f, (0x1a - 0x04), SEEK_CUR);                  // смещаем положение указателя ч/з в положение File Name Len
            unsigned char fileNameLen[2];                       // переменная для чтения длины имени файла (поле File Name Len)
            fread(fileNameLen, sizeof(unsigned char), 2, f);    // читаем поле File Name Len
            //printf("fileNameLen = %x\t%x\n", fileNameLen[0], fileNameLen[1]);
            unsigned char FileName[fileNameLen[0] + 1];         // переменная для вывода на экран, допускаем, что длина
                                                                // файла не превышает 256 символов, чтобы не склеивать число из двух байт.
            FileName[fileNameLen[0]] = '\0';                    // положим в последний байт массива символ конца строки
            fseek(f, (0x1e - 0x1c), SEEK_CUR);                  // смещаем положение указателя ч/з в положение File Name
            fread(FileName, sizeof(unsigned char), fileNameLen[0], f); // читаем имя очередного файла и
            printf("%s\n", FileName);                                  // выводим на экран
        }

        if(feof(f))
            break;

        fseek(f, -3, SEEK_CUR);                                 // смещаем положение указателя ч/з назад на один байт
        n -= 3;                                                 // и также уменьшаем количество прочитанных байтов
    }
    fclose(f);

    printf("\nPress 'e' to exit\n");
    while(getch() != 'e');

    return 0;
}

