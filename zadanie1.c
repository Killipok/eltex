#include <stdio.h>

int main() {
    int choice;
    int i;

    printf("Выберите задание:\n");
    printf("1 — Двоичное представление положительного числа\n");
    printf("2 — Двоичное представление отрицательного числа\n");
    printf("3 — Количество единиц в двоичном представлении\n");
    printf("4 — Замена третьего байта числа\n");
    printf("Ваш выбор: ");
    scanf("%d", &choice);

    switch (choice) {

        /*ЗАДАНИЕ 1*/
        case 1: {
            unsigned int n;
            printf("Введите положительное число: ");
            scanf("%u", &n);

            printf("Двоичное представление:\n");
            for (i = 31; i >= 0; i--) {
                printf("%d", (n >> i) & 1);
                if (i % 4 == 0) printf(" ");
            }
            printf("\n");
            break;
        }

        /*  ЗАДАНИЕ 2  */
        case 2: {
            int n;
            printf("Введите отрицательное число: ");
            scanf("%d", &n);

            printf("Двоичное представление (доп. код):\n");
            for (i = 31; i >= 0; i--) {
                printf("%d", (n >> i) & 1);
                if (i % 4 == 0) printf(" ");
            }
            printf("\n");
            break;
        }

        /*  ЗАДАНИЕ 3  */
        case 3: {
            unsigned int n;
            int count = 0;

            printf("Введите положительное число: ");
            scanf("%u", &n);

            for (i = 0; i < 32; i++) {
                if ((n >> i) & 1)
                    count++;
            }

            printf("Количество единиц: %d\n", count);
            break;
        }

        /*  ЗАДАНИЕ 4  */
        case 4: {
            unsigned int n;
            unsigned char new_byte;

            printf("Введите исходное число: ");
            scanf("%u", &n);

            printf("Введите новое значение третьего байта (0–255): ");
            scanf("%hhu", &new_byte);

            /* Очистка третьего байта (байт №2) */
            n &= ~(0xFF << 16);

            /* Установка нового значения */
            n |= ((unsigned int)new_byte << 16);

            printf("Результат: %u\n", n);
            printf("Двоичное представление:\n");
            for (i = 31; i >= 0; i--) {
                printf("%d", (n >> i) & 1);
                if (i % 4 == 0) printf(" ");
            }
            printf("\n");
            break;
        }

        default:
            printf("Ошибка: неверный пункт меню\n");
    }

    return 0;
}
