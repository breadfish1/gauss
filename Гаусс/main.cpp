#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MIN 0.0001

double **create(int sz1, int sz2) { // создать двумерный массив
    double **t = (double**)malloc(sizeof(double*)*sz1);
    int i;
    
    for (i = 0; i < sz1; i++)
        t[i] = (double*)malloc(sizeof(double)*sz2);
    
    return t;
}

double *createAnswers(int sz) { // выделить память под массив для корней
    double *t = (double*)malloc(sizeof(double)*sz);
    
    return t;
}

void delAnswers(double *x) { // очисить память от массива корней
    free(x);
}

double **reader(FILE *f, int *sz) { //считывает уравнения
    double **x = NULL;
    
    if ((f = fopen("eq.txt", "r")) != NULL) { //открываем файл на чтение, если он не пустой
        fscanf(f, "%i", sz);
        int sz1 = *sz + 1;
        x = create(*sz, sz1);
        int i = 0, j = 0;
        
        while (!feof(f)) {
            for (i = 0; i < *sz; i++)
                for (j = 0; j < sz1; j++)
                    fscanf(f, "%lf", &(x[i][j]));
        }
        fclose(f); //закрываем
    }
    
    return x;
}

void del(double **x, int sz, int sz1) { // очистить память от матрицы
    for (int i = 0; i < sz; i++)
        free(x[i]);
    free(x);
}

void print(double **x, int sz1, int sz2) { // печать СЛАУ
    for (int i = 0; i < sz1; i++) {
        for (int j = 0; j < sz2; j++)
            printf("%13e ", x[i][j]);
        printf("\n");
    }
    printf("\n");
}

double **swap(double **x, int a, int b) { // поменять строки a и b местами
    double *temp;
    temp = x[a];
    x[a] = x[b];
    x[b] = temp;
    
    return x;
}

int find(double **x, int rows, int columns, int i, int j) { // найти ненулевой коэфициент в столбце j, начиная поиск со строки i, возвращает номер строки
    while (i < rows) {
        if (fabs(x[i][j]) < MIN)
            return i;
        i++;
    }
    
    return 0;
}

double **remove(double **x, int sz, int z, int y, double m) { // отнять ур-ие z умноженное на m из ур-ия y
    int a;
    
    x[y][z] = 0; // зануляем тот коэффициент, который стопроцентно должен стать нулем
    
    for (a = z + 1; a < sz; a++)
        if (fabs(x[y][a] -= x[z][a] * m) < MIN)
            x[y][a] = 0;
    
    
    return x;
}

int del_col(double **x, int rows, int columns, int i, int j) { // удалить столбец j начиная с iой + 1 строки, возвращает 1, если все хорошо, 0, если система вырождена
    if (fabs(x[i][j]) < MIN) {
        int s;
        s = find(x, rows, columns, i, j + 1);
        if (!s)
            return 0; // система вырождена
        
        x = swap(x, s, i);
    }
    
    int z = i + 1;
    while (z < rows) {
        double m = x[z][j] / x[i][j];
        x = remove(x, columns, i, z, m);
        z++;
    }
    
    return 1;
}

int gauss(double **x, int rows, int columns) { // функция возвращает целое число. 1 - бесконечно много решений, 2 - нет, 3 - вырождена, 4 - все хорошо
    int a = 0, result = 0;
    
    while (a < rows - 1) {
        result = del_col(x, rows, columns, a, a);
        
        if (!result) // если система вырождена
            return 3;
        a++;
    }
    
    if (!x[rows - 1][columns - 2]) {
        if (x[rows - 1][columns - 1])
            return 2; // решений нет
        else
            return 1; // решений бесконечно много
    }
    
    return 4; // все хорошо
}

void innerSubstitution(double **x, double *ans, int i, int rows, int columns) { // перенос всех членов, кроме искомого в правую часть, вычисление корня
    double temp = x[i][columns - 1];
    int k;
    
    for (k = columns - 2; k > i; k--)
        temp -= x[i][k] * ans[k];
    
    ans[i] = temp / x[i][k];
}

double *substitution(double **x, int rows, int columns) { // обратная подстановка
    int i = columns - 2;
    double *ans = createAnswers(rows);
    
    while (i >= 0)
        innerSubstitution(x, ans, i--, rows, columns);
    
    return ans;
}

void subPrint(double *ans, int sz) { // печать массива ответов
    int i;
    for (i = 0; i < sz - 1; i++)
        printf("X[%i] = %e, ", i, ans[i]);
    printf("X[%i] = %e\n", i, ans[i]);
}

double *output(double **x, int rows, int columns, int err) { // вывод конечного результата решения
    double *res;
    switch (err) {
        case 1:
            printf("Решений бесконечно много\n");
            return 0;
        case 2:
            printf("Решений нет\n");
            return 0;
        case 3:
            printf("Система вырождена\n");
            return 0;
        case 4:
            return res = substitution(x, rows, columns);
    }
    return NULL;
}

int main() {
    double **x, *end;
    int sz, sz1, res;
    
    FILE *f = NULL;
    
    x = reader(f, &sz);
    
    if (x) {
        sz1 = sz + 1;
        
        print(x, sz, sz1);
        
        res = gauss(x, sz, sz1);
        
        print(x, sz, sz1);
        
        end = output(x, sz, sz1, res);
        
        if (end) {
            subPrint(end, sz);
            delAnswers(end);
        }
        
        del(x, sz, sz1);
    }
    
    system("pause");
}
