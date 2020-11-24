#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
/*
 При создании необходимо предусмотреть ключи, которые позволяли бы задать максимальное количество потоков, используемое программой. При возможности необходимо использовать максимальное количество возможных потоков. Ограничение потоков может быть задано или ключом запуска вашей программы, или алгоритмом.
 3.    Отсортировать массив строк при помощи параллельной сортировки слиянием.
 
 */

int MAX = 256;  // Количество элементов в массиве
int THREAD_MAX; //количество потоков

int part=0; //переменная, которая будет хранить количество потоков
int* a ;    //указатель на массив для хранения элементов



// функция для слияния двух частей
void merge(int low, int mid, int high)
{
    int n1 = mid - low + 1, nr = high - mid, i, j;
    
    int* left = calloc(n1,sizeof(int));//выделение памяти под левую половину
    int* right = calloc(nr,sizeof(int));//выделение памяти под правую половину
    
    for(i = 0; i < n1; i++)//заполняем левую половину
        left[i] = a[i + low];
    
    for(i = 0; i < nr; i++)// заполняем правую половину
        right[i] = a[i + mid + 1];
    
    int k = low;
    i = j = 0;
    
    // объединяем левую и правую половины в порядке возрастания
    while(i < n1 && j < nr)
    {
        if(left[i] <= right[j])
            a[k++] = left[i++];
        else
            a[k++] = right[j++];
    }
    
    // вставляем оставшиеся значения из левой половины
    while(i < n1) {
        a[k++] = left[i++];
    }
    
    // вставляем оставшиеся значения из правой половины
    while(j < nr) {
        a[k++] = right[j++];
    }
    
    
    
}

// функция сортировки слиянием
void merge_sort(int low, int high)
{
    // вычисление средней точки массива
    int mid = low + ((high - low) / 2);
    if(low < high) {
        
        //сортировка первой половины
        merge_sort(low, mid);
        
        // сортировка второй половины
        merge_sort(mid + 1, high);
        
        // объединение половинок
        merge(low, mid, high);
    }
}

//функция сортировки с использованием потоков
void* merge_sort_tread()
{
    //получает идентификатор потока
    int thread_part = part;
    part+=1;
    
    // вычисляем нижнюю и верхнюю границу массива
    int low = thread_part * (MAX / THREAD_MAX);
    int high = (thread_part + 1) * (MAX / THREAD_MAX) - 1;
    
    //printf("+%d %d %d\n",part,low,high);
    
    //   обновление средней точки
    int mid = low + (high - low) / 2;
    
    
    if(low < high)
    {
        merge_sort(low, mid);
        merge_sort(mid + 1, high);
        merge(low, mid, high);
    }
    
    
}



void marge_rec(int tread_m)//рекурсивная функция сбора данных после отработки потоков
{
    if(tread_m>THREAD_MAX)
        return;
    
    marge_rec(tread_m*2);
    
    int minsize = MAX/tread_m;
    
    for(int i=0;i<tread_m;i++)
    {
        
        int low = i * minsize;
        int high = (i + 1) * (minsize) - 1;
        int mid = low + (high - low) / 2;
        
        // printf("$%d %d\n",low,high);
        // обновление средней точки
        
        merge(low, mid, high);
        
        
        
    }
    
}
// главная функция
int main()
{
    
    //находим максимальное количество потоков
//    int pow_ = 0;
//    while (1)
//    {
//        int p = MAX >> pow_;
//        if(p==0||p==1)
//            break;
//
//        pow_++;
//    }
//
//    MAX=pow(2,pow_);
    THREAD_MAX = MAX/16;
    
    
    a = calloc(MAX,sizeof(int));//выделение памяти под левую половину
    printf("Входной массив: ");
    //------------------------генерация массива чисел
    int i;
    srand(time(NULL)); // устанавливаем начальное значение генератора случайных чисел
    for( i=0; i < MAX; i++)
    {
        a[i] = rand() % MAX;
        printf("%d ",a[i]);
    }
    printf("\n");
    

    pthread_t threads[THREAD_MAX];//создаем массив идентификаторов потока
    
    
    int status;
    
    // создаем потоки
    for(int i = 0; i < THREAD_MAX; i++)
    {
        printf("part- %d\n",part);
        
        status = pthread_create(&threads[i], NULL, merge_sort_tread, NULL);
        // &threads[i] является адресом для хранения идентификатора создаваемого потока типа pthread_t
       //вызываемая функция не требует аргументов поэтому NULL
        if (status != 0)
            printf("main error: can't create thread, status = %d\n", status);
        
        
    }
    
    //ожидание завершения всех потоков
    for(int i = 0; i < THREAD_MAX; i++)
        pthread_join(threads[i], NULL);
    
    
    //объединяем результаты работы потоков
    marge_rec(2);
    merge(0, (MAX - 1)/2, MAX - 1);
    
   
    

    
    // вывод отсортированного массива
    printf("Выходной массив: ");
    for(int i = 0; i < MAX; i++)
        printf("%d\n",a[i]);
    
    printf("Количество потоков: %d",THREAD_MAX);
    return 0;
}
