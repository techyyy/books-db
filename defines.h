#ifndef SHOP_DEFINES_H
#define SHOP_DEFINES_H

#include <stdio.h>
#include <stdbool.h>

FILE *BooksFL;
FILE *AuthorsFL;
FILE *AuthorsIND;

struct Book{
    int bookId; 
    int price;
    int year;
    int nextBookPos;
    bool exists;
};

struct Author{
    char name[256];
    char address[256];
    char url[256];
    int firstBookId;

};

struct AuthorIndex {
    char name[256];
    size_t index;
    bool exists;
};

#endif