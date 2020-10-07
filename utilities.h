#ifndef SHOP_UTILITIES_H
#define SHOP_UTILITIES_H

#include "defines.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


void openFiles(const char *mode) {
    BooksFL = fopen("books.fl", mode);
    AuthorsFL = fopen("authors.fl", mode);
    AuthorsIND = fopen("authors.ind", mode);
}

char* getAuthor() {
    static char firstName[256];
    static char lastName[256];
    printf("Author's first and last names: \n");
    scanf("%s %s", firstName, lastName);
    strcat(firstName, " ");
    strcat(firstName, lastName);
    return firstName;
}

void closeFiles() {
    fclose(BooksFL);
    fclose(AuthorsFL);
    fclose(AuthorsIND);
}

int startDB() {
    openFiles("r+b");
    if (BooksFL == NULL ||
        AuthorsFL == NULL ||
        AuthorsIND == NULL) {
        closeFiles();
        openFiles("w+b");
    }
    return 0;
}

int finishDB() {
    closeFiles();
    return 0;
}



void setAuthor(struct Author *author, const char name[],const char address[], const char url[]) {
    strcpy(author->name, name);
    strcpy(author->address, address);
    strcpy(author->url, url);
    author->firstBookId = -1;
}

void setBook(struct Book *book, int bookId, int price, int year) {
    book->bookId = bookId;
    book->price = price;
    book->year = year;
    book->exists = true;
    book->nextBookPos = -1;
}


int authorsCount() {
    fseek(AuthorsIND, 0, SEEK_END);
    long authorsCount = ftell(AuthorsIND) / sizeof(struct AuthorIndex);
    rewind(AuthorsIND);

    int result = 0;
    struct AuthorIndex authorIndex;
    for (int i = 0; i < authorsCount; i++) {
        fread(&authorIndex, sizeof(struct AuthorIndex), 1, AuthorsIND);
        if (authorIndex.exists)
            result++;
    }

    return result;
}

int booksCount() {
    fseek(BooksFL, 0, SEEK_END);
    long booksCount = ftell(BooksFL) / sizeof(struct Book);
    rewind(BooksFL);
    int result = 0;
    struct Book book;
    for (int i = 0; i < booksCount; i++) {
        fread(&book, sizeof(struct Book), 1, BooksFL);
        if (book.exists)
            result++;
    }

    return result;
}

long get_authorIndexPosition(char *name) {
    struct AuthorIndex authorIndex;
    fseek(AuthorsIND, 0, SEEK_END);
    long authorsCount = ftell(AuthorsIND) / sizeof(struct AuthorIndex);
    rewind(AuthorsIND);
    fseek(AuthorsIND, 0, SEEK_SET);
    for (int i = 0; i < authorsCount; i++) {
        fread(&authorIndex, 1, sizeof(struct AuthorIndex), AuthorsIND);
        if (strcmp(authorIndex.name, name) == 0 && authorIndex.exists) {
            return (ftell(AuthorsIND) - sizeof(struct AuthorIndex));
        }
    }
    return -1;
}

bool bookExists(int bookId) {
    fseek(BooksFL, 0, SEEK_END);
    long booksCount = ftell(BooksFL) / sizeof(struct Book);
    rewind(BooksFL);

    struct Book book;

    for (int i = 0; i < booksCount; i++) {
        fread(&book, sizeof(struct Book), 1, BooksFL);
        if (book.bookId == bookId && book.exists)
            return true;
    }
    return false;
}

bool authorExists(char name[]) {
    long authorIndPos = get_authorIndexPosition(name);
    if (authorIndPos == -1) {
        return false;
    }
    return true;
}

#endif