#ifndef SHOP_MENU_H
#define SHOP_MENU_H

#include "defines.h"
#include "functions.h"
#include "utilities.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int workCycle() {
    char command[10] = "";

    char name[256];
    char address[256];
    char url[256];

    int bookId;
    int price;
    int year;

    char buff[8][64];

    printf("To end program use [end] command\n");

    while (true) {
        printf("Enter command "
               "[get-m, get-s, del-m, del-s, update-m, update-s, insert-m, insert-s, count, show-all]\n");
        scanf("%s", command);
        if (strcmp(command, "end") == 0) {
            printf("Ending work with DB..\n");
            finishDB();
            printf("Finished!");
            return 0;
        }
        printf("Enter argument for [%s]\n", command);
        if (strcmp(command, "get-m") == 0) {
            strcpy(name, getAuthor());
            struct Author author;
            get_m(&author, name);

            printf("%s %s %s\n", author.name, author.address, author.url);
            if (author.firstBookId != -1)
                show_sublist(author.firstBookId);
        }

        if (strcmp(command, "get-s") == 0) {
            strcpy(name, getAuthor());
            printf("Book id: \n");
            scanf("%s", buff[0]);
            bookId = strtol(buff[0], NULL, 10);
            struct Book book;
            get_s(&book, name, bookId);
            if (book.bookId != -1){
                printf("%i %i %i\n", book.bookId, book.price, book.year);
            }
        }

        if (strcmp(command, "update-m") == 0) {
            strcpy(name, getAuthor());
            printf("E-mail address and url: \n");
            scanf("%s %s", address, url);
            struct Author author;
            setAuthor(&author, name, address, url);
            update_m(&author);
        }

        if (strcmp(command, "update-s") == 0) {
            strcpy(name, getAuthor());
            printf("Id to update , price, year \n");
            scanf("%s %s %s", buff[0], buff[1], buff[2]);
            struct Book book;
            bookId = strtol(buff[0], NULL, 10);
            price = strtol(buff[1], NULL, 10);
            year = strtol(buff[2], NULL, 10);
            setBook(&book, bookId, price, year);

            update_s(name, &book);
        }

        if (strcmp(command, "insert-m") == 0) {
            strcpy(name, getAuthor());
            printf("E-mail address, url: \n");
            scanf("%s %s", address, url);
            struct Author author;
            setAuthor(&author, name, address, url);

            insert_m(&author);
        }

        if (strcmp(command, "insert-s") == 0) {
            strcpy(name, getAuthor());
            printf("Book id, price, year\n");
            scanf("%s %s %s", buff[0], buff[1], buff[2]);
            struct Book book;
            bookId = strtol(buff[0], NULL, 10);
            price = strtol(buff[1], NULL, 10);
            year = strtol(buff[2], NULL, 10);
            setBook(&book, bookId, price, year);

            insert_s(name, &book);
        }

        if (strcmp(command, "del-m") == 0) {
            strcpy(name, getAuthor());
            del_m(name);
        }

        if (strcmp(command, "del-s") == 0) {
            strcpy(name, getAuthor());
            printf("Book id: \n");
            scanf("%s", buff[0]);
            bookId = strtol(buff[0], NULL, 10);
            del_s(name, bookId);
        }

        if (strcmp(command, "count") == 0) {
            printf("Authors count: %i\nBooks count: %i\n", authorsCount(), booksCount());
        }

        if (strcmp(command, "show-all") == 0) {
            show_all();
        }
    }
}

#endif 