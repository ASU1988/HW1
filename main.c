#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int n = 0;                                              // ������� ����������� ������
    int NumOfByte = 0;                                      // ����������� ����������� ������ ��� ����������� Jpeg �����
    unsigned char Buff[4] = {0};                            // �������� ����� ��� ������ ���������� � �����
    unsigned char MarkEOJPG[2] = {0xff, 0xd9};              // ������ ����� jpg �����
    unsigned char SignLFH[4] = {0x50, 0x4b, 0x03, 0x04};    // ��������� Local file header

    FILE *f = fopen(argv[1], "rb");;

    if(f == NULL)
    {
        printf("Error opening file\n");
        exit(1);
    }

    //��������� ���� �� ������� ���������� ������� ����� JPEG ����� � ���� ����� ���� ������ ������ ���, ������ ��� ��. �������� ���� ��������� ������ �� �����
    while(1) // ��� ��� ���������� �� ������������ ��� �������������� ������?
    {
        n += fread(Buff, sizeof(unsigned char), 2, f); // ������ ��������� 2 �����
        //printf("n=%d\tBuff = %x\t%x\t%x\t%x\n", n, Buff[0], Buff[1], Buff[2], Buff[3]);

        if(!memcmp(Buff, MarkEOJPG, 2) && !NumOfByte) // ���������� � �������� ����� jpg �����, ��� ������ ���������� ������ ������� � NumOfByte
            NumOfByte = n;

        if(feof(f)) // ���� ����� �� ����� ����� ��������� ����
            break;

        fseek(f, -1, SEEK_CUR); // ������� ��������� ��������� �/� ����� �� ���� ����
        n -= 1;
    }

    if(NumOfByte == n)
        printf("\nThis is the non-zipjpeg file\n\n");
    else printf("\nThis is the zipjpeg file\n\n");
    //printf("%d\t%d\n", NumOfByte, n);


    fseek(f, 0, SEEK_SET); // ������� ��������� ��������� �/� � ������

    //��������� ���� �� ������� ���������� ��������� Local file header. ���� ����� ����������, ������� �� ����� ���������� zip �����.
    while(1)
    {
        n += fread(Buff, sizeof(unsigned char), 4, f);
        //printf("n=%d\tBuff = %x\t%x\t%x\t%x\n", n, Buff[0], Buff[1], Buff[2], Buff[3]);

        if(memcmp(Buff, SignLFH, 4) == 0)
        {
            fseek(f, (0x1a - 0x04), SEEK_CUR);                  // ������� ��������� ��������� �/� � ��������� File Name Len
            unsigned char fileNameLen[2];                       // ���������� ��� ������ ����� ����� ����� (���� File Name Len)
            fread(fileNameLen, sizeof(unsigned char), 2, f);    // ������ ���� File Name Len
            //printf("fileNameLen = %x\t%x\n", fileNameLen[0], fileNameLen[1]);
            unsigned char FileName[fileNameLen[0] + 1];         // ���������� ��� ������ �� �����, ���������, ��� �����
                                                                // ����� �� ��������� 256 ��������, ����� �� ��������� ����� �� ���� ����.
            FileName[fileNameLen[0]] = '\0';                    // ������� � ��������� ���� ������� ������ ����� ������
            fseek(f, (0x1e - 0x1c), SEEK_CUR);                  // ������� ��������� ��������� �/� � ��������� File Name
            fread(FileName, sizeof(unsigned char), fileNameLen[0], f); // ������ ��� ���������� ����� �
            printf("%s\n", FileName);                                  // ������� �� �����
        }

        if(feof(f))
            break;

        fseek(f, -3, SEEK_CUR);                                 // ������� ��������� ��������� �/� ����� �� ���� ����
        n -= 3;                                                 // � ����� ��������� ���������� ����������� ������
    }
    fclose(f);

    return 0;
}

