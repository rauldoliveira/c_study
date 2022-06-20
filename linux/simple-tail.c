#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#define BUFSIZE 8192
int main(void)
{
    char filename[] = "/var/lib/pgsql/9.4/data/pg_log/postgresql-Thu.log";
    int n_lines = 5;                   /* кол-во строк для вывода */
    int fd;                             /* номер дескриптора для файла */
    struct stat stats;                  /* структура для хранения статы о файле*/
    off_t start_pos = -1;               /* позиция начала файла */
    off_t end_pos;                      /* позиция конца файла */
    off_t pos;                          /* позиция откуда начнем читать файл для копирования в буфер */
    size_t bytes_read;                  /* кол-во прочитанных байт, меньше либо равно размеру буффера */
    char buffer[BUFSIZE];               /* размер данных в буфере, позиция чтения в буффере  */
    int i, nl_count = 0, len, scan_pos;                  /* указатель на найденный символ конца строки */
    char *p;

    /* 
     * открываем файл и копируем в буффер последние 8KB данных
     */
    fd = open(filename, O_RDONLY);       /* handle error if file can't open */
    fstat(fd, &stats);                                                              /* handle error ? */
    if (S_ISREG (stats.st_mode)                                                     /* это должен быть файл и ничто иное */
            && (start_pos = lseek (fd, 0, SEEK_CUR)) != -1                          /* начало файла должно быть успешно определено */
            && start_pos < (end_pos = lseek (fd, 0, SEEK_END))) {                   /* значение начала файла должно быть меньше значения конца файла */
        pos = end_pos;                                                              /* уст. позицию-начало чтения равная концу файла */
        bytes_read = BUFSIZE;                                                       /* читаем блоками по 8KБ */
        if (end_pos < BUFSIZE)
            pos = 0;
        else
            pos = pos - bytes_read;                                                     /* определяем позицию начала чтения = конец файла - 128Б */
        lseek(fd, pos, SEEK_SET);                                                   /* устанавливаем позицию в файле = pos */
        bytes_read = read(fd, buffer, bytes_read);                                  /* читаем 8KБ c позиции. err handling ? */

        len = strlen(buffer);                                       /* определяем актуальный размер буфера */
        scan_pos = len;                                             /* устанавливаем позицию чтения в конец буфера */

        for (i = 0; i < sizeof(buffer); i++)                        /* считаем количество \n символов в буфере */
            if (buffer[i] == '\n')
                nl_count++;
        if (n_lines > nl_count) {                                   /* если \n символов в буфере меньше чем запрошено n_lines */
            write(1, buffer, len);                                  /* то печатаем содержимое буфера */
            return;                                                 /* и завершаем работу */
        }

        /* итак в буфере достаточно данных теперь нам нужно найти позицию \n с которой начнем вывод строк */
        do {
            p = memrchr(buffer, '\n', scan_pos);                    /* ищем \n с позиции scan_pos */
            if (p != NULL) {                                        /* если найдено */
                scan_pos = (p - buffer);                            /* то запоминаем где нашли */
            } else {                                                /* если не нашли */
                break;                                              /* завершаем работу */
            }
            n_lines--;                                              /* после итерации уменьшаем искомое число строк */
        } while ((n_lines + 1) != 0);                               /* завершаем цикл когда кол-во строк стало отрицательным */

        write(1, buffer + scan_pos + 1, len - scan_pos);            /* выводим содержимое строк с позиции последней найденой \n + 1 и до конца буфера */
    } else {
        printf("not regular or empty file\n");                      /* если файл оказался корявым то ничего с ним не делаем */
    }

    return 0;
}