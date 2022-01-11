#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h> 
#include<unistd.h>

//global variables for sorting purposes
int N, num_of_phases;

//global variables for concurrency logic
pthread_mutex_t worker_count_mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t *semaphores;
int worker_count = 0;

//arguements for sorting worker threads
struct arg_struct{
	int *arr;
	int row;
};

//read N by N mesh array from input file, first line contains value of N 
int *read_mesh_array(const char *file_name);

//Function to calculate number of phases for shear sort: ceiling(log(n)) + 1
int number_of_phases(int n);

//sorting worker thread for shear sort
void *row_and_column_sort(void *args);


int main(){

	//read mesh array from input file and assign values to global variables N and num_of_phases
	int *test = read_mesh_array("lab3_array.txt");
	num_of_phases = number_of_phases(N * N);

	//print initial read array
	printf("\nInitial Mesh Array\n\n");
	for(int k = 0; k < N; k++){
		for(int j = 0; j < N; j++)
			printf("%d ", test[k * N + j]);
		printf("\n");
	}

	//Initialize N semaphores for sorting worker threads concurrency logic
	semaphores = (sem_t *) malloc(sizeof(sem_t) * N);
	for(int i = 0; i < N; i++){
		sem_init(semaphores + i, 0, 1);
	}

	//create N threads along with arguements
	pthread_t sorting_threads[N];

	for(int i = 0; i < N; i++){
		struct arg_struct *arguements = (struct arg_struct *) malloc(sizeof(struct arg_struct));;
		arguements->row = i + 1;
		arguements->arr = test; 
		pthread_create(sorting_threads + i, NULL, row_and_column_sort, arguements);
	}

	//join threads after they have finished sorting
	for(int i = 0; i < N; i++)
		pthread_join(sorting_threads[i], NULL);

	//free array and semaphore memory space
	free(semaphores);
	free(test);
}

int *read_mesh_array(const char *file_name){

	FILE *fp;

	//check if input file exists
	if((fp = fopen(file_name, "r")) == NULL){
		printf("File Not Found\n");
		exit(1);
	}


	fscanf(fp, "%d", &N);

	int *array = (int *) malloc(sizeof(int) * N * N);
	
	for(int i = 0; i < N * N; i++)
	{
		fscanf(fp, "%d", array + i);
	}	

	fclose(fp);

	return array;
}

int number_of_phases(int n){
	int i = 0;
	int val = 1;
	while(val < n){
		val *= 2;
		i++;
	}	
	return i + 1;
}

void *row_and_column_sort(void *args){
	//read and unpack arguements
	struct arg_struct *arguements = (struct arg_struct *) args; 
	int row_or_col_num = arguements->row;
	int *arr = (int *) arguements->arr;

	//extra helper variables for sorting 
	int j, k, temp;

	for(int i = 0; i < num_of_phases; i++){

		//concurrency logic: have to wait for all threads to finish a phase before starting the next one
		sem_wait(semaphores + row_or_col_num - 1);
		pthread_mutex_lock(&worker_count_mutex);
		worker_count++;
		pthread_mutex_unlock(&worker_count_mutex);
		
		//row sort, all sorting algorithms are just bubble sort for now
		if(i % 2 == 0){

			//sort in increasing order for odd rows
			if(row_or_col_num % 2 == 1){ 
				for (j = 0; j < N-1; j++){
					for (k = 0; k < N-j-1; k++) {
						if (arr[(row_or_col_num - 1) * N + k] > arr[(row_or_col_num - 1) * N + k+1]){
							temp = arr[(row_or_col_num - 1) * N + k];
							arr[(row_or_col_num - 1) * N + k] = arr[(row_or_col_num - 1) * N + k+1];
							arr[(row_or_col_num - 1) * N + k + 1] = temp;
						} 
					} 
				}
			}

			//sort in decrasing order for even rows
			else{

				for (j = 0; j < N-1; j++){
					for (k = 0; k < N-j-1; k++) {
						if (arr[(row_or_col_num - 1) * N + k] < arr[(row_or_col_num - 1) * N + k+1]){
							temp = arr[(row_or_col_num - 1) * N + k];
							arr[(row_or_col_num - 1) * N + k] = arr[(row_or_col_num - 1) * N + k+1];
							arr[(row_or_col_num - 1) * N + k + 1] = temp;
						} 
					} 
				}
			}
		}

		//column sort
		else {

			for (j = 0; j < N-1; j++){
				for (k = 0; k < N-j-1; k++) {
					if (arr[k * N + row_or_col_num - 1] > arr[(k+1) * N + row_or_col_num - 1]){
						temp = arr[k * N + row_or_col_num - 1];
						arr[k * N + row_or_col_num - 1] = arr[(k+1) * N + row_or_col_num - 1];
						arr[(k+1) * N + row_or_col_num - 1] = temp;
					} 
				} 
			}
		}

		//concurrency logic to sync all threads after each phase
		pthread_mutex_lock(&worker_count_mutex);
		if(worker_count == N){
			worker_count = 0;
			printf("\nAfter Phase %d\n\n", i + 1);
			for(k = 0; k < N; k++){
				for(j = 0; j < N; j++)
					printf("%d ", arr[k * N + j]);
				printf("\n");
			}
			for(int l = 0; l < N; l++)
				sem_post(semaphores + l);
		}
		pthread_mutex_unlock(&worker_count_mutex);
	}

	//free arguements to soritng working threads
	free(arguements);
}





