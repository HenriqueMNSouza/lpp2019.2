#include<stdio.h>
#include<stdlib.h>
#define cima 0
#define baixo 1
#include <time.h>
#include <sys/time.h>
#include <omp.h>

int m;
int numThreads;

int* criaVetorAleatorio(int chaves){
    int* vetor;
    vetor = (int *) malloc (chaves * sizeof(int));
    for (int i = 0; i < chaves; i++){
        vetor[i] = (rand() % 1024);
    }
    return vetor;

}

void printaVetor(int* vetor){
    for (int i = 0; i < 1024 ; i++){
        printf("%d ", vetor[i]);
    }
}


void bitonic_sort_seq(int start, int length, int *seq, int flag)
{
    int i;
    int split_length;

    if (length == 1)
        return;

    split_length = length / 2;

    // bitonic split
    for (i = start; i < start + split_length; i++)
    {
        if (flag == cima)
        {
            if (seq[i] > seq[i + split_length])
                troca(&seq[i], &seq[i + split_length]);
        }
        else
        {
            if (seq[i] < seq[i + split_length])
                troca(&seq[i], &seq[i + split_length]);
        }
    }

    bitonic_sort_seq(start, split_length, seq, flag);
    bitonic_sort_seq(start + split_length, split_length, seq, flag);
}

void bitonic_sort_par(int start, int length, int *seq, int flag)
{
    int i;
    int split_length;

    if (length == 1)
        return;

    split_length = length / 2;

    // bitonic split
#pragma omp parallel for shared(seq, flag, start, split_length) private(i)
    for (i = start; i < start + split_length; i++)
    {
        if (flag == cima)
        {
            if (seq[i] > seq[i + split_length])
                troca(&seq[i], &seq[i + split_length]);
        }
        else
        {
            if (seq[i] < seq[i + split_length])
                troca(&seq[i], &seq[i + split_length]);
        }
    }

    if (split_length > m)
    {
        // m = n/numThreads
        bitonic_sort_par(start, split_length, seq, flag);
        bitonic_sort_par(start + split_length, split_length, seq, flag);
    }

    return;
}

void troca(int *x1, int *x2)
{
    int aux;
    aux = *x1;
    *x1 = *x2;
    *x2 = aux;
}

int main()
{
    int i, j;
    int n;
    int flag;
    int *seq;
    int* vetorteste;
    vetorteste = criaVetorAleatorio(1024);
    printaVetor(vetorteste);
    printf("_________________________SEQUENCIA ORDENADA___________________________");
    printf("\n");


    int numThreads,id;

    n = 1024;
    seq = (int *) malloc (n * sizeof(int));
    int *byte_array_dec;
    byte_array_dec = (int *) malloc (1024 * sizeof(int));
    byte_array_dec = criaVetorAleatorio(1024);
    seq = byte_array_dec;
    // começo do  algoritmo
    double tempoInicial = omp_get_wtime();

    numThreads =  4;
    // separar as partes nas threads
    m = n / numThreads;

    // transformar sequencia bitonica 1 parte
    for (i = 2; i <= m; i = 2 * i)
    {
#pragma omp parallel for shared(i, seq) private(j, flag)
        for (j = 0; j < n; j += i)
        {
            if ((j / i) % 2 == 0)
                flag = cima;
            else
                flag = baixo;
            bitonic_sort_seq(j, i, seq, flag);
        }
    }

    // transformar sequencia bitonica 2 parte
    for (i = 2; i <= numThreads; i = 2 * i)
    {
        for (j = 0; j < numThreads; j += i)
        {
            if ((j / i) % 2 == 0)
                flag = cima;
            else
                flag = baixo;
            bitonic_sort_par(j*m, i*m, seq, flag);
        }

#pragma omp parallel for shared(j)
        for (j = 0; j < numThreads; j++)
        {
            if (j < i)
                flag = cima;
            else
                flag = baixo;
            bitonic_sort_seq(j*m, m, seq, flag);
        }
    }


    double tempoFinal = omp_get_wtime() - tempoInicial;
    printaVetor(seq);
    printf("\n TEMPO DE EXECUCAO = %f \n", tempoFinal);

    free(seq);
    return 0;
}


