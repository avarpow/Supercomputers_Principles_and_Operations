#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <omp.h>
#include <sys/time.h>
int readData();
int calcNBody();
int checkAnswer();

#define N 30
#define POW(X) ((X) * (X))

#define GET_TIME(now)                           \
    {                                           \
        struct timeval t;                       \
        gettimeofday(&t, NULL);                 \
        now = t.tv_sec + t.tv_usec / 1000000.0; \
    }
void serial_TSP();
void Parallel_TSP();
void pthread_TSP();

void serial_dfs(int v, int step, int cost, int *road, int *vis);
void parallel_dfs(int v, int step, int cost, int *road, int *vis, int *localSerialMinRoad, int *localSerialMinCost_p);

double dis[N][N] = {0};
int road[N] = {0};
int Count = 0;
int secondLayer;
sem_t sem;
#define test_case 7
#define thread_case 8
long long thread_tests[thread_case] = {1, 2, 4, 8, 18, 36, 54, 72};
long long tests[test_case] = {8, 9, 10, 11, 12, 13, 14};
int thread_count;
pthread_t *thread_handles;
int globalSerialMinCost = 0x3f3f3f3f;
int globalSerialMinRoad[N] = {0};
int globalParallelMinCost = 0x3f3f3f3f;
int globalParallelMinRoad[N] = {0};
int globalPthreadMinCost = 0x3f3f3f3f;
int globalPthreadMinRoad[N] = {0};
void serial_dfs(int v, int step, int cost, int *road, int *vis)
{
    if (step == Count && cost + dis[v][road[0]] < globalSerialMinCost)
    {
        {
            globalSerialMinCost = cost + dis[v][road[0]];
            memcpy(globalSerialMinRoad, road, sizeof(int) * Count);
        }
        return;
    }
    //if(cost>globalSerialMinCost)return;
    for (int i = 0; i < Count; i++)
    {
        if (vis[i] == 0)
        {
            vis[i] = 1;
            road[step] = i;
            serial_dfs(i, step + 1, cost + dis[v][i], road, vis);
            vis[i] = 0;
        }
    }
}
void parallel_dfs(int v, int step, int cost, int *road, int *vis, int *localSerialMinRoad, int *localSerialMinCost_p)
{
    if (step == Count && cost + dis[v][road[0]] < *localSerialMinCost_p)
    {
        *localSerialMinCost_p = cost + dis[v][road[0]];
        memcpy(localSerialMinRoad, road, sizeof(int) * Count);

        return;
    }
    //if(cost>*localSerialMinCost_p)return;
    for (int i = 1; i < Count; i++)
    {
        if (vis[i] == 0)
        {
            vis[i] = 1;
            road[step] = i;
            parallel_dfs(i, step + 1, cost + dis[v][i], road, vis, localSerialMinRoad, localSerialMinCost_p);
            vis[i] = 0;
        }
    }
}
void pthread_dfs(int v, int step, int cost, int *road, int *vis, int *localSerialMinRoad, int *localSerialMinCost_p)
{
    if (step == Count && cost + dis[v][road[0]] < *localSerialMinCost_p)
    {
        *localSerialMinCost_p = cost + dis[v][road[0]];
        memcpy(localSerialMinRoad, road, sizeof(int) * Count);

        return;
    }
    //if(cost>*localSerialMinCost_p)return;
    for (int i = 1; i < Count; i++)
    {
        if (vis[i] == 0)
        {
            vis[i] = 1;
            road[step] = i;
            pthread_dfs(i, step + 1, cost + dis[v][i], road, vis, localSerialMinRoad, localSerialMinCost_p);
            vis[i] = 0;
        }
    }
}
int main()
{
    //readData
    FILE *init = fopen("tsp2.txt", "r");
    if (init == NULL)
    {
        printf("read file failed\n  EXIT!!\n");
        return 0;
    }
    //alloc buffer
    int lastCount = 0;
    char *temp = NULL;
    size_t len = 999;
    getline(&temp, &len, init);
    //printf("len %d %s\n",len,temp);
    for (int i = 0; temp[i]; i++)
    {
        if (temp[i] >= '0' && temp[i] <= '9')
        {
            Count++;
            while (temp[i] >= '0' && temp[i] <= '9')
                i++;
        }
    }
    rewind(init);
    for (int i = 0; i < Count; i++)
    {
        for (int j = 0; j < Count; j++)
        {
            fscanf(init, "%lf", &dis[i][j]);
        }
    }

    double start, finish, elapsed;
    printf("Count %d\n", Count);

    //Count=11;
    for (int test = 0; test < test_case; test++)
    {
        Count = tests[test];
        secondLayer = (Count - 1) * (Count - 1);

        printf("<<<<<<<<<<<<< start new Count: %d >>>>>>>>>>\n", Count);
        fprintf(stderr, "<<<<<<<<<<<<< start new Count: %d >>>>>>>>>>\n", Count);
        globalSerialMinCost = 0x3f3f3f3f;
        GET_TIME(start);
        serial_TSP(dis, Count, globalSerialMinRoad, &globalSerialMinCost);
        GET_TIME(finish);
        elapsed = finish - start;

        printf(" ===========  Serial time  =============\n ");
        printf("The elapsed time is %e seconds\n", elapsed);
        printf("get New best cost%d\n", globalSerialMinCost);

        for (int i = 0; i < Count; i++)
        {
            printf("%d ", globalSerialMinRoad[i]);
        }
        printf("\n");
        for (int i = 1; i < Count; i++)
        {
            printf("%.0lf ", dis[globalSerialMinRoad[i - 1]][globalSerialMinRoad[i]]);
        }
        printf("%.0lf ", dis[globalSerialMinRoad[Count - 1]][globalSerialMinRoad[0]]);
        printf("\n\n");
        for (int th = 0; th < thread_case; th++)
        {
            globalParallelMinCost = 0x3f3f3f3f;
            globalPthreadMinCost = 0x3f3f3f3f;
            thread_count = thread_tests[th];
            printf("################## start new thread_nums: %d #################\n", thread_count);
            fprintf(stderr, "################## start new thread_nums: %d #################\n", thread_count);

            GET_TIME(start);
            Parallel_TSP(dis, Count, globalParallelMinRoad, &globalParallelMinCost);
            GET_TIME(finish);
            elapsed = finish - start;
            printf(" ====  OpenMP time  thread_nums:%d ====\n ", thread_count);
            printf("The elapsed time is %e seconds\n", elapsed);
            printf("get New best cost%d\n", globalParallelMinCost);

            for (int i = 0; i < Count; i++)
            {
                printf("%d ", globalParallelMinRoad[i]);
            }
            printf("\n");
            for (int i = 1; i < Count; i++)
            {
                printf("%.0lf ", dis[globalParallelMinRoad[i - 1]][globalParallelMinRoad[i]]);
            }
            printf("%.0lf ", dis[globalParallelMinRoad[Count - 1]][globalParallelMinRoad[0]]);
            printf("\n\n");

            GET_TIME(start);
            pthread_TSP();
            GET_TIME(finish);
            elapsed = finish - start;
            printf(" ====  pthread time  thread_nums:%d ====\n ", thread_count);
            printf("The elapsed time is %e seconds\n", elapsed);
            printf("get New best cost%d\n", globalParallelMinCost);

            for (int i = 0; i < Count; i++)
            {
                printf("%d ", globalParallelMinRoad[i]);
            }
            printf("\n");
            for (int i = 1; i < Count; i++)
            {
                printf("%.0lf ", dis[globalParallelMinRoad[i - 1]][globalParallelMinRoad[i]]);
            }
            printf("%.0lf ", dis[globalParallelMinRoad[Count - 1]][globalParallelMinRoad[0]]);
            printf("\n");
        }
    }

    return 0;
}
void serial_TSP()
{
    int vis[N] = {1};
    road[0] = 0;
    for (int i = 1; i < Count; i++)
    {
        vis[i] = 1;
        road[1] = i;
        serial_dfs(i, 2, dis[0][i], road, vis);
        vis[i] = 0;
    }
}
void Parallel_TSP()
{

#pragma omp parallel for shared(dis, Count) num_threads(thread_count)
    for (int i = 0; i < secondLayer; i++)
    {
        int first = i / (Count - 1) + 1;
        int second = i % (Count - 1) + 1;
        if (first == second)
            continue;
        int localvis[N] = {1};
        int localroad[N] = {0};
        int localParallelMinCost = 0x3f3f3f3f;
        int localParallelMinRoad[N] = {0};
        localvis[first] = 1;
        localvis[second] = 1;
        localroad[1] = first;
        localroad[2] = second;
        parallel_dfs(second, 3, dis[0][first] + dis[first][second], localroad, localvis, localParallelMinRoad, &localParallelMinCost);
        localvis[first] = 0;
        localvis[second] = 0;

#pragma omp critial
        {
            if (globalParallelMinCost > localParallelMinCost)
            {
                globalParallelMinCost = localParallelMinCost;
                memcpy(globalParallelMinRoad, localParallelMinRoad, sizeof(int) * Count);
            }
        }
    }
}
void pthread_TSP_func(void *rank)
{
    long my_rank = (long)rank;
    long long i;
    long long my_n = secondLayer / thread_count;
    long long my_first_i = my_n * my_rank;
    long long my_last_i = my_first_i + my_n;
    for (i = my_first_i; i < my_last_i; i++)
    {
        int first = i / (Count - 1) + 1;
        int second = i % (Count - 1) + 1;
        if (first == second)
            continue;
        int localvis[N] = {1};
        int localroad[N] = {0};
        int localPthreadMinCost = 0x3f3f3f3f;
        int localPthreadMinRoad[N] = {0};
        localvis[first] = 1;
        localvis[second] = 1;
        localroad[1] = first;
        localroad[2] = second;
        parallel_dfs(second, 3, dis[0][first] + dis[first][second], localroad, localvis, localPthreadMinRoad, &localPthreadMinCost);
        localvis[first] = 0;
        localvis[second] = 0;

        sem_wait(&sem);
        if (globalPthreadMinCost > localPthreadMinCost)
        {
            globalPthreadMinCost = localPthreadMinCost;
            memcpy(globalPthreadMinRoad, localPthreadMinRoad, sizeof(int) * Count);
        }
        sem_post(&sem);
    }
}
void pthread_TSP()
{
    thread_handles = (pthread_t *)malloc(thread_count * sizeof(pthread_t));
    long thread;
    sem_init(&sem, 0, 1);
    for (thread = 0; thread < thread_count; thread++)
        pthread_create(&thread_handles[thread], NULL,
                       (void *)pthread_TSP_func, (void *)thread);

    for (thread = 0; thread < thread_count; thread++)
        pthread_join(thread_handles[thread], NULL);
}