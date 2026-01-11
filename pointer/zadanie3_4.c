#include <stdio.h>

int main(void) {
    char str[100];
    char sub[50];
    char *result = NULL;

    printf("Введите строку: ");
    fgets(str, sizeof(str), stdin);

    printf("Введите подстроку: ");
    fgets(sub, sizeof(sub), stdin);

    for (char *p = str; *p != '\0'; p++) {
        if (*p == '\n') {
            *p = '\0';
            break;
        }
    }

    for (char *p = sub; *p != '\0'; p++) {
        if (*p == '\n') {
            *p = '\0';
            break;
        }
    }

    for (char *p = str; *p != '\0'; p++) {
        char *p1 = p;
        char *p2 = sub;

        while (*p1 && *p2 && (*p1 == *p2)) {
            p1++;
            p2++;
        }

        if (*p2 == '\0') {
            result = p;
            break;
        }
    }

    if (result) {
        printf("Подстрока найдена\n");
        printf("Адрес начала: %p\n", (void *)result);
        printf("Строка с этого адреса: %s\n", result);
    } else {
        printf("Подстрока не найдена\n");
    }

    return 0;
}
