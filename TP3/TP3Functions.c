#include "TP3Functions.h"
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include <stdio.h>
#include <time.h>

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


	return rval;
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


	// fonction pour qsort (a-b: croissant, b-a: décroissant)
	int compare (const void * pa, const void * pb)
	{
		const int *a = *(const int **)pa;
	    const int *b = *(const int **)pb;

	    return b[0] - a[0];
	}

	// tri le tableau dans l'ordre décroissant c1/a1 ≥c2/a2 ≥…≥ cn/an.
	qsort (sortArray, n, sizeof(sortArray[0]), compare);

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

	/**

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
**/
    
    float temps1;
    float temps2;
    float temps3;
    clock_t t1,t2,t3,t4,t5;
    
    
	// x
    int cptPoids = 0;
    int cptValG = 0;
    
    t1 = clock();
    // Algo Glouton
	printf("\nAffichage de x Glouton \n");
	for (int i=0; i<n; i++) {
        if((cptPoids + a[i]) <= b)
        {
            cptPoids+= a[i];
            cptValG+= c[i];
            printf("1 ");
        }
        else
            printf("0 ");
	}
	t2 = clock();
	
	printf("\nSolution = %d\n",cptValG);

	printf("\n");
        
    
    // Algo Relaxation Lineaire
    cptPoids = 0;
    int cptVal = 0;
    int i = -1;
    
    while(cptPoids<b)
    {
        i++;
        cptPoids+=a[i];
        cptVal+=c[i];

    }

    cptPoids-= a[i];
    cptVal-= c[i];
    
    int pivot = i;
    
    float fraction = (float)(b-cptPoids)/(float)a[pivot];
    float solution = (float)cptVal+(fraction*c[pivot]);    

    
        printf("\nAffichage de Zr avec relaxation lineaire\n");
        printf("Pivot : %d\n",pivot);
        printf("Fraction : %f\n",fraction);

            for(int i=0;i<n;i++)
    {
            if(i<pivot)
                printf("1 ");
            else if(i == pivot)
                printf("%f ",fraction);
            else
                printf("0 ");
    }
    
    printf("\nSolution : %f\n",solution);
    
    t3 = clock();
    //Reduction du problème - Pre-processing
    printf("\nPre-Processing");
    int* xR = (int*)malloc(sizeof(int)*n);
    
    for(int j =0;j<n;j++)
    {
        
        
        if(fabs(((double)c[j]-((double)c[pivot]/a[pivot]*a[j]))) >= (solution-(double)cptValG))
        {
            if(j<=pivot-1)
            {
                xR[j] = 1;
                b -= a[j];
            }
            else
                xR[j] = 0;
        }
        else
            xR[j] = 2;
    }
    
    printf("\n");
    for(int i =0;i<n;i++)
    {
        printf("%d ",xR[i]);
    }
    
    printf("\nValeur de b restante : %d\n",b);
    
    
    //Nouveau tableau
    int n2 = 0;
    
    for(int i = 0;i<n;i++)
    {
        if(xR[i] == 2)
            n2++;
    }
    
    
    int* newTabPoids = (int*)malloc(sizeof(int)*n2);
    int* newTabVal = (int*)malloc(sizeof(int)*n2);
    int cpt = 0;
    
    for(int i=0;i<n;i++)
    {
        if(xR[i] == 2)
        {
            newTabPoids[cpt] = a[i];
            newTabVal[cpt] = c[i];
            cpt++;
        }
    }
    
        
    t5 = clock();
    // Algo de résolution
    
	// (a) initialisation
	int* Z = (int*)malloc(sizeof(int)*(b+1));
	int* Zs = (int*)malloc(sizeof(int)*(b+1));
	int* D = (int*)malloc(sizeof(int)*(b+1));
	int* xN = (int*)malloc(sizeof(int)*n2);
	int k=0;

	for (int y=0; y<=b; y++) {
		Z[y] = 0;
		D[y] = 0;
	}

	// (c) 
	while (k != n2) {

		// (b) Etape k
		for (int y=0; y<=b; y++) {
			Zs[y] = Z[y];
		}

		for (int y=newTabPoids[k]; y<=b; y++) {
			
			if (Zs[y-newTabPoids[k]] + newTabVal[k] > Zs[y]) {
				D[y] = k+1;
				
				if (Zs[y] > newTabVal[k] + Zs[y-newTabPoids[k]]) {
					Z[y] = Zs[y];
				} else {
					Z[y] = newTabVal[k] + Zs[y-newTabPoids[k]];
				}
			}
		}

		k++;
	}

	// (d)
	for (int j=0; j<n2; j++) {
		int y=b;
		xN[j] = 0;

		while (y>0) {
			while (Z[y] == Z[y-1]) {
				y--;
			}
			xN[D[y]-1] = 1;
			y = y - newTabPoids[D[y]-1];
		}
	}
    t4 = clock();



	// affichage résultat
    /**
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
    **/
    
    
	// x
    cptPoids = 0;
    cptValG = 0;
    
    for(int i =0; i<n2;i++)
    {
        printf("%d ",xN[i]);
    }

    
    //Reaffichage solution
    cpt = 0;
    int sum = 0;
    
    for(int i = 0;i<n;i++)
    {
        if(xR[i] == 2)
        {
            xR[i] = xN[cpt];
            cpt++;
        }

    }
    
    for(int i=0;i<n;i++)
    {
        if(xR[i] == 1)
            sum += c[i];
    }
    
    printf("\nSolution Prog dynamique (après pre-processing) : %d\n",sum);
    
    temps1 = (((float)t2-t1)/CLOCKS_PER_SEC)*1000;
    temps2 = (((float)t4-t3)/CLOCKS_PER_SEC)*1000;
    temps3 = (((float)t4-t5)/CLOCKS_PER_SEC)*1000;
    
    printf("\n\nTemps Algo glouton : %f secondes\n",temps1);
    printf("Temps Prog Dynamique Avec Pre-Processing : %f secondes\n",temps2);
    printf("Temps Prog Dynamique Sans Pre-Processing : %f secondes\n",temps3);
    
    
	return rval;
}

