#include <stdio.h> 
int main(void) 
{ 
float x = 5.0; 
printf("x = %f, ", x); 
float y = 6.0; 
printf("y = %f\n", y); 
float *xp = &y; // Меняем указатель x на y чтобы получить 6 + 6 = 12 
float *yp = &y; 
printf("Результат: %f\n", *xp + *yp); 
} 