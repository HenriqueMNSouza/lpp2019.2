#include"mpi.h"
#include<stdio.h>
#include<stdlib.h>
#include<time.h> 

#define SIZE 1024 

//-------------------------


void logg(int line,const char * f)
{
	fprintf(stderr,"LINE: %d - FUNCTION: %s\n",line,f);
}

void print_vet(int * v, int tamanho)
{
int i;
	fprintf(stderr,"\nPrintando vetor:\n");
	for(i=0;i<tamanho;i++){
		fprintf(stderr," %d ",v[i]);
	}
	fprintf(stderr,"\n-----------------------\n");
}

int * create_random_vet(int r)
{
int *v;
int i;

	v = (int*)malloc(sizeof(int)*SIZE);
	//srand(time(0));

	for(i=0;i<SIZE;i++){
		v[i] = rand()/10000000;
	}
	if(r==0){
		fprintf(stderr,"Contruindo vetor:\n");
		print_vet(v,SIZE);
		fprintf(stderr,"----------------\n");
	}
	return v;
}

int *merge(int *v1, int *v2, int tamanho)
{
int 	*v = (int*)malloc(sizeof(int)*tamanho*2);//conferir se todo lugar que chama o merge o tamanho ta certo
int 	t1 = 0,
	t2 = 0,
	t = 0;
	while( t1 != tamanho && t2 !=tamanho ){  
		if(v1[t1]>v2[t2]) 	v[t++] = v2[t2++];
		else			v[t++] = v1[t1++];
	}
	
	while(t1 != tamanho){
		v[t++] = v1[t1++];
	}
	while(t2 != tamanho){
		v[t++] = v2[t2++];
	}

//	free(v1);
//	free(v2);
	/*logg(__LINE__,__func__);
	fprintf(stderr,"v1:\n");
	print_vet(v1,tamanho);
	fprintf(stderr,"v2:\n");
        print_vet(v2,tamanho);
        fprintf(stderr,"v:\n");
	print_vet(v,tamanho*2);*/
	return v;
}

int * merge_sort(int *v, int tamanho)
{
	//logg(__LINE__,__func__);
	if( tamanho <= 1 ) return v;
	
	//logg(__LINE__,__func__);
	return merge(merge_sort(v,tamanho/2),merge_sort(&v[tamanho/2],tamanho/2),tamanho/2);
}

int * final_merge(int * v, int n, int tamanho)
{

	if(n<=2) return merge(v,&v[tamanho/2],tamanho/2);

	else return merge(final_merge(v,n/2,tamanho/2),final_merge(&v[tamanho/2],n/2,tamanho/2),tamanho/2);  
}

int main(int argc, char ** argv)
{
int meu_rank,np,
	n=4,
	local_n,
	source,
	dest=0,
	tag=200,
	len_fatia_vetor;


int 	*v,
	*local_vet,
	*new_vet,
	*final_vet,
	*temp;
double ts_start;
MPI_Status status;
MPI_Init(&argc, &argv);
MPI_Comm_rank(MPI_COMM_WORLD, &meu_rank);
MPI_Comm_size(MPI_COMM_WORLD,&np);
	
	if(meu_rank == 0i){
		ts_start = MPI_Wtime();
	}
	//if(meu_rank != 1) return 0;
	v = create_random_vet(meu_rank);
	len_fatia_vetor = SIZE/n;
	//printf("fatia_vetor: %d - np: %d\n",len_fatia_vetor,np);
	local_vet = &v[len_fatia_vetor*meu_rank];
		
	if(meu_rank ==0){
		new_vet = (int*)malloc( SIZE*sizeof(int));
		int t =0;
		temp = merge_sort(local_vet,len_fatia_vetor);
		for(source = 0; source<len_fatia_vetor;source++){
			new_vet[source]=temp[source];
		}
		
		for(source = 1; source<np; source++){
			t = len_fatia_vetor*source;
			MPI_Recv(&new_vet[t],len_fatia_vetor,MPI_INT,source,tag,MPI_COMM_WORLD, &status);
		}
		
	/*	fprintf(stderr,"Meu_rank %d:\n",meu_rank);
		print_vet(new_vet,SIZE);
	*/	final_vet = final_merge(new_vet,n,SIZE);
		
	}else{
	/*	fprintf(stderr,"Meu_rank %d:\n",meu_rank);
		print_vet(local_vet,len_fatia_vetor);
	*/	local_vet = merge_sort(local_vet,len_fatia_vetor);
	//	print_vet(local_vet,len_fatia_vetor);
		MPI_Send(local_vet, len_fatia_vetor, MPI_INT, dest, tag, MPI_COMM_WORLD);
	}

	if(meu_rank == 0){
		fprintf(stderr,"Printando vetor ordenado: ");
		print_vet(final_vet,SIZE);
		printf("Tempo de exeção: %f\n",MPI_Wtime()-ts_start);
	}
	
MPI_Finalize();
	return 0;
}
