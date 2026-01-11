#include <stdio.h>

int main() {
    int x;
    unsigned char new_byte;

    printf("Введите целое положительное число: ");
    scanf("%d", &x);

    printf("Введите новое значение третьего байта (0–255): ");
    scanf("%hhu", &new_byte);

    unsigned char *p = (unsigned char *)&x;
    p[2] = new_byte;

    printf("Результат: %d\n", x);
    return 0;
}
