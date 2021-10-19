/*
 * October 2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

char *in_filename_julia;
char *in_filename_mandelbrot;
char *out_filename_julia;
char *out_filename_mandelbrot;
int P; // numar de threaduri
pthread_barrier_t barrier;
pthread_mutex_t mutex;

// structura pentru un numar complex
typedef struct _complex {
	double a;
	double b;
} complex;

// structura pentru parametrii unei rulari
typedef struct _params {
	int is_julia, iterations;
	double x_min, x_max, y_min, y_max, resolution;
	complex c_julia;
} params;

params par_julia;
int width_julia, height_julia;
int **result_julia;

params par_man;
int width_man, height_man;
int **result_man;

// citeste argumentele programului
void get_args(int argc, char **argv)
{
	if (argc < 6) {
		printf("Numar insuficient de parametri:\n\t"
				"./tema1 fisier_intrare_julia fisier_iesire_julia "
				"fisier_intrare_mandelbrot fisier_iesire_mandelbrot\n");
		exit(1);
	}

	in_filename_julia = argv[1];
	out_filename_julia = argv[2];
	in_filename_mandelbrot = argv[3];
	out_filename_mandelbrot = argv[4];
	P = atoi(argv[5]);
	printf("nr de threaduri este %d\n", P);
}

// citeste fisierul de intrare
void read_input_file(char *in_filename, params* par)
{
	FILE *file = fopen(in_filename, "r");
	if (file == NULL) {
		printf("Eroare la deschiderea fisierului de intrare!\n");
		exit(1);
	}

	fscanf(file, "%d", &par->is_julia);
	fscanf(file, "%lf %lf %lf %lf",
			&par->x_min, &par->x_max, &par->y_min, &par->y_max);
	fscanf(file, "%lf", &par->resolution);
	fscanf(file, "%d", &par->iterations);

	if (par->is_julia) {
		fscanf(file, "%lf %lf", &par->c_julia.a, &par->c_julia.b);
	}

	fclose(file);
}

// scrie rezultatul in fisierul de iesire
void write_output_file(char *out_filename, int **result, int width, int height)
{
	int i, j;

	FILE *file = fopen(out_filename, "w");
	if (file == NULL) {
		printf("Eroare la deschiderea fisierului de iesire!\n");
		return;
	}

	fprintf(file, "P2\n%d %d\n255\n", width, height);
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			fprintf(file, "%d ", result[i][j]);
		}
		fprintf(file, "\n");
	}

	fclose(file);
}

// aloca memorie pentru rezultat
int **allocate_memory(int width, int height)
{
	int **result;
	int i;

	result = malloc(height * sizeof(int*));
	if (result == NULL) {
		printf("Eroare la malloc!\n");
		exit(1);
	}

	for (i = 0; i < height; i++) {
		result[i] = malloc(width * sizeof(int));
		if (result[i] == NULL) {
			printf("Eroare la malloc!\n");
			exit(1);
		}
	}

	return result;
}

// elibereaza memoria alocata
void free_memory(int **result, int height)
{
	int i;

	for (i = 0; i < height; i++) {
		free(result[i]);
	}
	free(result);
}

void *thread_function(void *arg){

	// preluare id pentru threadul curent
	int thread_id = *(int *)arg;

	// cativa indici pentru iterare
	int w, h, i;

	// Julia

	// lungimea portiunii din inaltime asignate threadului curent
	double arr = (double) height_julia / P;
	// capatul lungimii portiunii asignate threadului curent
	int last = ((thread_id + 1) * arr);

	// paramatrii de start si end ai portiunii de calcul curente (de pe thread)
	int start = thread_id * arr;
	int end = (height_julia < last ? height_julia : last);

	// calculul propriu-zis
	// iterare pe coloane (pe latime)
	for (w = 0; w < width_julia; w++) {
		// iterare pe linii (pe inaltime) conform parametrilor de start si end
		for (h = start; h < end; h++) {
			int step = 0;
			complex z = { .a = w * (&par_julia)->resolution + (&par_julia)->x_min,
							.b = h * (&par_julia)->resolution + (&par_julia)->y_min };

			while (sqrt(pow(z.a, 2.0) + pow(z.b, 2.0)) < 2.0 && step < (&par_julia)->iterations) {
				complex z_aux = { .a = z.a, .b = z.b };

				z.a = pow(z_aux.a, 2) - pow(z_aux.b, 2) + (&par_julia)->c_julia.a;
				z.b = 2 * z_aux.a * z_aux.b + (&par_julia)->c_julia.b;

				step++;
			}
			result_julia[h][w] = step % 256;
		}
	}

	// asteptarea completarii matricei rezultat
	pthread_barrier_wait(&barrier);

	//transforma rezultatul din coordonate matematice in coordonate ecran
	for (i = start / 2; i < end / 2; i++) {
		int *aux = result_julia[i];
		result_julia[i] = result_julia[height_julia - i - 1];
		result_julia[height_julia - i - 1] = aux;
	}

	// Mandelbrot

	// lungimea portiunii din inaltime asignate threadului curent
	arr = (double) height_man / P;
	// capatul lungimii portiunii asignate threadului curent
	last= ((thread_id + 1) * arr);

	// paramatrii de start si end ai portiunii de calcul curente (de pe thread)
	start = thread_id * arr;
	end = (height_man < last ? height_man : last);

	// calculul propriu-zis
	// iterare pe coloane (pe latime)
	for (w = 0; w < width_man; w++) {
		// iterare pe linii (pe inaltime) conform parametrilor de start si end
		for (h = start; h < end; h++) {
			complex c = { .a = w * (&par_man)->resolution + (&par_man)->x_min,
							.b = h * (&par_man)->resolution + (&par_man)->y_min };
			complex z = { .a = 0, .b = 0 };
			int step = 0;

			while (sqrt(pow(z.a, 2.0) + pow(z.b, 2.0)) < 2.0 && step < (&par_man)->iterations) {
				complex z_aux = { .a = z.a, .b = z.b };

				z.a = pow(z_aux.a, 2.0) - pow(z_aux.b, 2.0) + c.a;
				z.b = 2.0 * z_aux.a * z_aux.b + c.b;

				step++;
			}

			result_man[h][w] = step % 256;
		}
	}

	// asteptarea completarii matricei rezultat
	pthread_barrier_wait(&barrier);

	//transforma rezultatul din coordonate matematice in coordonate ecran
	for (i = start / 2; i < end / 2; i++) {
		int *aux = result_man[i];
		result_man[i] = result_man[height_man - i - 1];
		result_man[height_man - i - 1] = aux;
	}

	// parasire thread
	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	int i;

	N = argv[1];
	P = argv[2];
	// se citesc argumentele programului
	get_args(argc, argv);
	int thread_id[P];
	pthread_t tid[P];

	read_input_file(in_filename_julia, &par_julia);
	width_julia = (par_julia.x_max - par_julia.x_min) / par_julia.resolution;
	height_julia = (par_julia.y_max - par_julia.y_min) / par_julia.resolution;
	result_julia = allocate_memory(width_julia, height_julia);
	read_input_file(in_filename_mandelbrot, &par_man);
	width_man = (par_man.x_max - par_man.x_min) / par_man.resolution;
	height_man = (par_man.y_max - par_man.y_min) / par_man.resolution;
	result_man = allocate_memory(width_man, height_man);

	pthread_barrier_init(&barrier, NULL, P);
	// se creeaza thread-urile
	for (i = 0; i < P; i++) {
		thread_id[i] = i;
		pthread_create(&tid[i], NULL, thread_function, &thread_id[i]);
	}

	// se asteapta thread-urile
	for (i = 0; i < P; i++) {
		pthread_join(tid[i], NULL);
	}

	write_output_file(out_filename_julia, result_julia, width_julia, height_julia);
	write_output_file(out_filename_mandelbrot, result_man, width_man, height_man);

	return 0;
}
