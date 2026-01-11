#include <stdio.h>
#include <string.h>

#define MAX_ABONENTS 100

struct abonent {
    char name[40];
    char second_name[40];
    char tel[20];
};


void clear_newline(char *s) {
    for (; *s; s++) {
        if (*s == '\n') {
            *s = '\0';
            break;
        }
    }
}


void add_abonent(struct abonent *book, int *count) {
    if (*count >= MAX_ABONENTS) {
        printf("Справочник переполнен!\n");
        return;
    }

    printf("Введите имя: ");
    fgets(book[*count].name, sizeof(book[*count].name), stdin);
    clear_newline(book[*count].name);

    printf("Введите фамилию: ");
    fgets(book[*count].second_name, sizeof(book[*count].second_name), stdin);
    clear_newline(book[*count].second_name);

    printf("Введите телефон: ");
    fgets(book[*count].tel, sizeof(book[*count].tel), stdin);
    clear_newline(book[*count].tel);

    (*count)++;
    printf("Абонент добавлен\n");
}


void delete_abonent(struct abonent *book, int *count) {
    if (*count == 0) {
        printf("Справочник пуст\n");
        return;
    }

    int index;
    printf("Введите номер абонента для удаления (1-%d): ", *count);
    scanf("%d", &index);
    getchar(); 

    if (index < 1 || index > *count) {
        printf("Неверный номер\n");
        return;
    }

    memset(&book[index - 1], 0, sizeof(struct abonent));
    printf("Абонент удалён\n");
}


void search_abonent(struct abonent *book, int count) {
    if (count == 0) {
        printf("Справочник пуст\n");
        return;
    }

    char search[40];
    printf("Введите имя для поиска: ");
    fgets(search, sizeof(search), stdin);
    clear_newline(search);

    int found = 0;
    for (int i = 0; i < count; i++) {
        if (book[i].name[0] != '\0' && strcmp(book[i].name, search) == 0) {
            printf("%d) %s %s, тел: %s\n",
                   i + 1,
                   book[i].name,
                   book[i].second_name,
                   book[i].tel);
            found = 1;
        }
    }

    if (!found) {
        printf("Абоненты с таким именем не найдены\n");
    }
}


void list_abonents(struct abonent *book, int count) {
    if (count == 0) {
        printf("Справочник пуст\n");
        return;
    }

    for (int i = 0; i < count; i++) {
        if (book[i].name[0] != '\0') {
            printf("%d) %s %s, тел: %s\n",
                   i + 1,
                   book[i].name,
                   book[i].second_name,
                   book[i].tel);
        }
    }
}


int main(void) {
    struct abonent book[MAX_ABONENTS] = {0};
    int count = 0;
    int choice;

    while (1) {
        printf("\nМеню:\n");
        printf("1) Добавить абонента\n");
        printf("2) Удалить абонента\n");
        printf("3) Поиск абонентов по имени\n");
        printf("4) Вывод всех записей\n");
        printf("5) Выход\n");
        printf("Выберите пункт: ");
        scanf("%d", &choice);
        getchar(); 

        switch (choice) {
            case 1: add_abonent(book, &count); break;
            case 2: delete_abonent(book, &count); break;
            case 3: search_abonent(book, count); break;
            case 4: list_abonents(book, count); break;
            case 5: printf("Выход из программы\n"); return 0;
            default: printf("Неверный пункт меню\n");
        }
    }
}
