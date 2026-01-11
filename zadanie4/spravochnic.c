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

int main(void) {
    struct abonent book[MAX_ABONENTS] = {0};
    int choice;
    int count = 0;

    while (1) {
        printf("\nМеню:\n");
        printf("1) Добавить абонента\n");
        printf("2) Удалить абонента\n");
        printf("3) Поиск абонентов по имени\n");
        printf("4) Вывод всех записей\n");
        printf("5) Выход\n");
        printf("Выберите пункт: ");
        scanf("%d", &choice);
        getchar();  // убрать '\n' после scanf

        switch (choice) {

        /* ===== 1. Добавить абонента ===== */
        case 1:
            if (count >= MAX_ABONENTS) {
                printf("Справочник переполнен!\n");
                break;
            }

            printf("Введите имя: ");
            fgets(book[count].name, sizeof(book[count].name), stdin);
            clear_newline(book[count].name);

            printf("Введите фамилию: ");
            fgets(book[count].second_name, sizeof(book[count].second_name), stdin);
            clear_newline(book[count].second_name);

            printf("Введите телефон: ");
            fgets(book[count].tel, sizeof(book[count].tel), stdin);
            clear_newline(book[count].tel);

            count++;
            printf("Абонент добавлен\n");
            break;

        /* ===== 2. Удалить абонента ===== */
        case 2: {
            int index;
            printf("Введите номер абонента для удаления (1-%d): ", count);
            scanf("%d", &index);
            getchar();

            if (index < 1 || index > count) {
                printf("Неверный номер\n");
                break;
            }

            memset(&book[index - 1], 0, sizeof(struct abonent));
            printf("Абонент удалён\n");
            break;
        }

        /* ===== 3. Поиск по имени ===== */
        case 3: {
            char search[10];
            int found = 0;

            printf("Введите имя для поиска: ");
            fgets(search, sizeof(search), stdin);
            clear_newline(search);

            for (int i = 0; i < count; i++) {
                if (strcmp(book[i].name, search) == 0) {
                    printf("%d) %s %s, тел: %s\n",
                           i + 1,
                           book[i].name,
                           book[i].second_name,
                           book[i].tel);
                    found = 1;
                }
            }

            if (!found)
                printf("Абоненты с таким именем не найдены\n");

            break;
        }

        /* ===== 4. Вывод всех записей ===== */
        case 4:
            if (count == 0) {
                printf("Справочник пуст\n");
                break;
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
            break;

        /* ===== 5. Выход ===== */
        case 5:
            printf("Выход из программы\n");
            return 0;

        default:
            printf("Неверный пункт меню\n");
        }
    }
}
