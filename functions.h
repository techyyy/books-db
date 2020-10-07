#ifndef SHOP_FUNCTIONS_H
#define SHOP_FUNCTIONS_H

#include "defines.h"
#include "utilities.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


void get_m(struct Author *author, char name[]) {
    long authorIndPos = get_authorIndexPosition(name);
    if (authorIndPos == -1) {
        setAuthor(author, "", "", "");
        return;
    }

    fseek(AuthorsIND, authorIndPos, SEEK_SET);

    struct AuthorIndex authorIndex;
    fread(&authorIndex, sizeof(struct AuthorIndex), 1, AuthorsIND);
    fseek(AuthorsFL, authorIndex.index, SEEK_SET);
    fread(author, 1, sizeof(struct Author), AuthorsFL);
}

void get_s(struct Book *book, char name[], __int32 bookId) {
    struct Author author;
    get_m(&author, name);
    if (strcmp(author.name, name) != 0) {
        printf("Author %s doesn't exist\n", name);
        return;
    }
    if (!bookExists(bookId)) {
        printf("Book %i doesn't exist\n", bookId);
        return;
    }

    book->bookId = -1;
    fseek(BooksFL, 0, SEEK_SET);
    while (book->bookId != author.firstBookId) {
        fread(book, sizeof(struct Book), 1, BooksFL);
    }

    while (book->bookId != bookId || book->nextBookPos!= -1) {
        fseek(BooksFL, book->nextBookPos, SEEK_SET);
        fread(&book, sizeof(struct Book), 1, BooksFL);
    }
    if (book->bookId == bookId) {
        return;
    } else {
        printf("Found no book with id %i for %s", bookId, name);
        setBook(book, -1, -1, -1);
    }
}

void insert_m(struct Author *author) {
    struct Author authorChecker;
    get_m(&authorChecker, author->name);
    if (strcmp(authorChecker.name, author->name) == 0) {
        printf("Author %s already exists\n", author->name);
        return;
    }

    fseek(AuthorsFL, 0, SEEK_END);
    fwrite(author, sizeof(struct Author), 1, AuthorsFL);

    struct AuthorIndex authorIndex;
    strcpy(authorIndex.name, author->name);
    fseek(AuthorsFL, -sizeof(struct Author), SEEK_CUR);
    authorIndex.index = ftell(AuthorsFL);
    authorIndex.exists = true;

    fseek(AuthorsIND, 0, SEEK_END);
    fwrite(&authorIndex, sizeof(struct AuthorIndex), 1, AuthorsIND);
}

void insert_s(char name[], struct Book *book) {
    if (bookExists(book->bookId)) {
        printf("Book with Id %i already exists\n", book->bookId);
        return;
    }

    struct Author author;
    get_m(&author, name);
    if (strcmp(author.name, name) != 0) {
        printf("Author %s not found\n", name);
        return;
    }

    fseek(BooksFL, 0, SEEK_END);
    fwrite(book, sizeof(struct Book), 1, BooksFL);
    fseek(BooksFL, -sizeof(struct Book), SEEK_CUR);
    long newBookPos = ftell(BooksFL);

    if (author.firstBookId == -1) {
       author.firstBookId = book->bookId;

        long authorInd = get_authorIndexPosition(name);
        fseek(AuthorsIND, authorInd, SEEK_SET);
        struct AuthorIndex authorIndex;
        fread(&authorIndex, sizeof(struct AuthorIndex), 1, AuthorsIND);

        fseek(AuthorsFL, authorIndex.index, SEEK_SET);
        fwrite(&author, sizeof(struct Author), 1, AuthorsFL);
    } else {
        fseek(BooksFL, 0, SEEK_SET);
        struct Book tmpBook;
        tmpBook.bookId = -1;
        while (tmpBook.bookId != author.firstBookId) {
            fread(&tmpBook, sizeof(struct Book), 1, BooksFL);
        }

        while (tmpBook.nextBookPos != -1) {
            fseek(BooksFL, tmpBook.nextBookPos, SEEK_SET);
            fread(&tmpBook, sizeof(struct Book), 1, BooksFL);
        }

        tmpBook.nextBookPos = newBookPos;
        fseek(BooksFL, -sizeof(struct Book), SEEK_CUR);
        fwrite(&tmpBook, sizeof(struct Book), 1, BooksFL);
    }
}

void update_m(struct Author *updatedAuthor) {
    long authorIndPos = get_authorIndexPosition(updatedAuthor->name);
    if (authorIndPos == -1) {
        printf("Author %s not found\n", updatedAuthor->name);
        return;
    }

    struct Author author;
    fseek(AuthorsFL, authorIndPos, SEEK_SET);
    fread(&author, sizeof(struct Author), 1, AuthorsFL);
    updatedAuthor->firstBookId = author.firstBookId;
    fseek(AuthorsFL, authorIndPos, SEEK_SET);
    fwrite(updatedAuthor, sizeof(struct Author), 1, AuthorsFL);
}

void update_s(char name[], struct Book *updatedBook) {
    long authorIndPos = get_authorIndexPosition(name);
    if (authorIndPos == -1) {
        printf("Author %s not found\n", name);
        return;
    }
    if (!bookExists(updatedBook->bookId)) {
        printf("Book %i not found\n", updatedBook->bookId);
        return;
    }
    struct AuthorIndex authorIndex;
    fseek(AuthorsIND, authorIndPos, SEEK_SET);
    fread(&authorIndex, sizeof(struct AuthorIndex), 1, AuthorsIND);
    struct Author author;
    fseek(AuthorsFL, authorIndex.index, SEEK_SET);
    fread(&author, sizeof(struct Author), 1, AuthorsFL);

    struct Book book;
    book.bookId = -1;
    fseek(BooksFL, 0, SEEK_SET);
    while (book.bookId != author.firstBookId) {
        fread(&book, sizeof(struct Book), 1, BooksFL);
    }

    while (book.bookId != updatedBook->bookId || book.nextBookPos != -1) {
        fseek(BooksFL, book.nextBookPos, SEEK_SET);
        fread(&book, sizeof(struct Book), 1, BooksFL);
    }
    if (book.bookId == updatedBook->bookId) {
        fseek(BooksFL, -sizeof(struct Book), SEEK_CUR);
        updatedBook->nextBookPos = book.nextBookPos;
        fwrite(updatedBook, sizeof(struct Book), 1, BooksFL);
    } else {
        printf("Book object %i found for %s", updatedBook->bookId, name);
    }
}

void del_m(char name[]) {
    if (!authorExists(name)) {
        printf("Author %s not found\n", name);
        return;
    }
    struct AuthorIndex authorIndex;
    struct Author author;
    long authorIndPos = get_authorIndexPosition(name);
    fseek(AuthorsIND, authorIndPos, SEEK_SET);
    fread(&authorIndex, sizeof(struct AuthorIndex), 1, AuthorsIND);
    fseek(AuthorsIND, -sizeof(struct AuthorIndex), SEEK_CUR);
    authorIndex.exists = false;
    fwrite(&authorIndex, sizeof(struct AuthorIndex), 1, AuthorsIND);

    fseek(AuthorsFL, authorIndex.index, SEEK_SET);
    fread(&author, sizeof(struct Author), 1, AuthorsFL);

    if (author.firstBookId == -1)
        return;

    struct Book book;
    book.bookId = -1;
    fseek(BooksFL, 0, SEEK_SET);
    while (book.bookId != author.firstBookId) {
        fread(&book, sizeof(struct Book), 1, BooksFL);
    }

    book.exists = false;
    fseek(BooksFL, -sizeof(struct Book), SEEK_CUR);
    fwrite(&book, sizeof(struct Book), 1, BooksFL);

    while (book.nextBookPos != -1) {
        fseek(BooksFL, book.nextBookPos, SEEK_SET);
        fread(&book, sizeof(struct Book), 1, BooksFL);
        book.exists = false;
        fseek(BooksFL, -sizeof(struct Book), SEEK_CUR);
        fwrite(&book, sizeof(struct Book), 1, BooksFL);
    }
}

void del_s(char name[], __int32 bookId) {
    if (!authorExists(name)) {
        printf("Author %s not found\n", name);
        return;
    }
    struct AuthorIndex authorIndex;
    struct Author author;
    long authorIndPos = get_authorIndexPosition(name);
    fseek(AuthorsIND, authorIndPos, SEEK_SET);
    fread(&authorIndex, sizeof(struct AuthorIndex), 1, AuthorsIND);

    fseek(AuthorsFL, authorIndex.index, SEEK_SET);
    fread(&author, sizeof(struct Author), 1, AuthorsFL);

    if (author.firstBookId == -1)
        return;

    struct Book book;
    book.bookId = -1;
    fseek(BooksFL, 0, SEEK_SET);
    while (book.bookId != author.firstBookId) {
        fread(&book, sizeof(struct Book), 1, BooksFL);
    }


    while (book.bookId != bookId && book.nextBookPos != -1) {
        fseek(BooksFL, book.nextBookPos, SEEK_SET);
        fread(&book, sizeof(struct Book), 1, BooksFL);
    }
    if (book.bookId == bookId) {
        book.exists = false;
        fseek(BooksFL, -sizeof(struct Book), SEEK_CUR);
        fwrite(&book, sizeof(struct Book), 1, BooksFL);
    }
}

void show_sublist(__int32 firstBookId) {
    if (firstBookId < 0)
        return;
    struct Book book;
    book.bookId = -1;
    fseek(BooksFL, 0, SEEK_SET);
    while (book.bookId != firstBookId) {
        fread(&book, sizeof(struct Book), 1, BooksFL);
    }
    if (book.exists)
        printf("   %i %i %i\n", book.bookId, book.price, book.year);
    while (book.nextBookPos != -1) {
        fseek(BooksFL, book.nextBookPos, SEEK_SET);
        fread(&book, sizeof(struct Book), 1, BooksFL);
        if (book.exists)
            printf("   %i %i %i\n", book.bookId, book.price, book.year);
    }

}

void show_all() {
    struct AuthorIndex authorIndex;
    fseek(AuthorsIND, 0, SEEK_END);
    long authorsCount = ftell(AuthorsIND) / sizeof(struct AuthorIndex);
    rewind(AuthorsIND);

    struct Author author;

    for (int i = 0; i < authorsCount; i++) {
        fread(&authorIndex, 1, sizeof(struct AuthorIndex), AuthorsIND);
        if (authorIndex.exists) {
            fseek(AuthorsFL, authorIndex.index, SEEK_SET);
            fread(&author, 1, sizeof(struct Author), AuthorsFL);

            printf("%s %s %s\n", author.name, author.address, author.url);
            if (author.firstBookId != -1)
                show_sublist(author.firstBookId);
        }
    }
}

#endif