#include <stdio.h>

int main() {
    int arr[10];
    int *p = arr;

    for (int i = 0; i < 10; i++)
        *(p + i) = i + 1;

    printf("Элементы массива:\n");
    for (int i = 0; i < 10; i++)
        printf("%d ", *(p + i));

    printf("\n");
    return 0;
}
