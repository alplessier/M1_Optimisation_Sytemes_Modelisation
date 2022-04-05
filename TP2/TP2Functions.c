#include "TP2Functions.h"
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include<stdio.h>

/* instance1
	4,11
	12,5
	8,4
	2,1
	5,3
	x=(1,1,1,0)

	instance2
	3,50
	70,10
	100,20
	120,30
	x=(0,1,1)
*/

int read_TP2_instance(FILE*fin,dataSet* dsptr)
{
	int rval = 0;

	//capacite b
	int b;
	//Nombre d'objets
	int n;
	rval = fscanf(fin,"%d,%d\n",&n,&b);
	dsptr->b = b;
	dsptr->n = n;
	dsptr->c = (int*)malloc(sizeof(int)*n);
	dsptr->a = (int*)malloc(sizeof(int)*n);


	int i;
	for( i = 0 ; i < n ; i++)
		rval = fscanf(fin,"%d,%d\n",&(dsptr->c[i]),&(dsptr->a[i]));

	/*fprintf(stderr,"\nInstance file read, we have capacity %d and there is %d items of values/weights:\n",
			b,n);
	for( i = 0 ; i < n ; i++)
		fprintf(stderr,"%d,%d\n",dsptr->c[i],dsptr->a[i]);
	fprintf(stderr,"\n");*/


	return rval;
}

int * randWeight(int n, int b)
{
    int temp;
    int moy = (int)(b/n);
    srand(time(NULL));

    
    int* a_rand = (int*)malloc(sizeof(int)*(n));
    //Remplissage des poids
    for(int i=0; i<n; i++) {       
            //Meilleure répartition des poids
            temp = rand()%(2*moy)+(moy-((int)moy/2));
            a_rand[i] = temp;
    }

    return a_rand;
}

int * randvalue(int n)
{
    int * c_rand = (int*)malloc(sizeof(int)*(n));
    srand(time(NULL));
    
    //Remplissage des valeurs
    for(int i=0; i<n; i++) {
        c_rand[i] = rand()%10+1;
    }

    return c_rand;
}

void write_inst(char* path, int* a, int* c, int n, int p)
{
    FILE * fichier = NULL;
    fichier = fopen(path,"w");
    
    if(fichier != NULL)
    {
        fprintf(fichier,"%d,%d\n", n, p);
        
        for(int i = 0; i<n;i++)
        {
            fprintf(fichier,"%d,%d\n" ,c[i], a[i]);
        }
        
    }
    
    fclose(fichier);
}

int TP2_solve_exact(dataSet* dsptr)
{
	printf("\n");

	int rval = 0;

	//capacite b
	int b = dsptr->b;
	//Nombre d'objets
	int n = dsptr->n;

	// poids a
	int* a = (int*)malloc(sizeof(int)*n);

	// valeurs c
	int* c = (int*)malloc(sizeof(int)*n);

	for (int i=0; i<n; i++) {
		c[i] = dsptr->c[i];
		a[i] = dsptr->a[i];
	}

	printf("capacité: %d\n", b);
	printf("nb objets: %d\n", n);

	/*for (int i=0; i<n; i++) {
		printf("poids: %d, valeur %d\n", a[i], c[i]);
	}*/

	// tri des variables

	// tableau tempon c/a, indice i
	float** sortArray = (float**)malloc(sizeof(float)*(n*2));

	for (int i=0; i<n; i++) {
	    sortArray[i] = (float*)malloc(2 * sizeof(float));
	}

	for (int i=0; i<n; i++) {
		int cs = c[i];
		int as = a[i];
		float result = (float) cs / as;

		sortArray[i][0] = result;
		sortArray[i][1] = i;
	}

	/*printf("\n");

	printf("avant le tri\n");

	for (int i=0; i<n; i++) {
		for (int j=0; j<2; j++) {
			printf("%f, ", sortArray[i][j]);
		}
		printf("\n");
	}*/

	// fonction pour qsort (a-b: croissant, b-a: décroissant)
	int compare (const void * pa, const void * pb)
	{
		const int *a = *(const int **)pa;
	    const int *b = *(const int **)pb;

	    return b[0] - a[0];
	}

	// tri le tableau dans l'ordre décroissant c1/a1 ≥c2/a2 ≥…≥ cn/an.
	qsort (sortArray, n, sizeof(sortArray[0]), compare);

	/*printf("\n");
	printf("après le tri\n");

	for (int i=0; i<n; i++) {
		for (int j=0; j<2; j++) {
			printf("%f, ", sortArray[i][j]);
		}
		printf("\n");
	}*/

	// tempons tab a et c
	int* buffA = (int*)malloc(sizeof(int)*n);
	int* buffC = (int*)malloc(sizeof(int)*n);

	// recopie des tableaux
	for (int i=0; i<n; i++) {
		buffA[i] = a[i];
		buffC[i] = c[i];
	}

	for (int i=0; i<n; i++) {
		int index = sortArray[i][1];
		a[i] = buffA[index];
		c[i] = buffC[index];
	}

	printf("\n");
	for (int i=0; i<n; i++) {
		printf("poids: %d, valeur %d\n", a[i], c[i]);
	}


	// Algo de résolution

	// (a) initialisation
	int* Z = (int*)malloc(sizeof(int)*(b+1));
	int* Zs = (int*)malloc(sizeof(int)*(b+1));
	int* D = (int*)malloc(sizeof(int)*(b+1));
	int* x = (int*)malloc(sizeof(int)*n);
	int k=0;

	for (int y=0; y<=b; y++) {
		Z[y] = 0;
		D[y] = 0;
	}

	// (c) 
	while (k != n) {

		// (b) Etape k
		for (int y=0; y<=b; y++) {
			Zs[y] = Z[y];
		}

		for (int y=a[k]; y<=b; y++) {
			
			if (Zs[y-a[k]] + c[k] > Zs[y]) {
				D[y] = k+1;
				
				if (Zs[y] > c[k] + Zs[y-a[k]]) {
					Z[y] = Zs[y];
				} else {
					Z[y] = c[k] + Zs[y-a[k]];
				}
			}
		}

		k++;
	}

	// (d)
	for (int j=0; j<n; j++) {
		int y=b;
		x[j] = 0;

		while (y>0) {
			while (Z[y] == Z[y-1]) {
				y--;
			}
			x[D[y]-1] = 1;
			y = y - a[D[y]-1];
		}
	}




	// affichage résultat

	// Z
	printf("\n");
	printf("affichage de Z\n");
	for (int i=0; i<b+1; i++) {
		printf("%d, ", Z[i]);
	}
	printf("\n");
	printf("\n");

	// D
	printf("affichage de D\n");
	for (int i=0; i<b+1; i++) {
		printf("%d, ", D[i]);
	}
	printf("\n");
	printf("\n");

	// x
	printf("affichage de x\n");
	for (int i=0; i<n; i++) {
		printf("%d", x[i]);
	}

	printf("\n");

	return rval;
}

