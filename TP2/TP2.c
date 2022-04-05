#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include "TP2Functions.h"

int * randWeight(int n,int b);
int * randvalue(int n);
void write_inst(char* path, int* a, int* c, int n, int p);


int main(int argc, char **argv)
{
	int rval =0;	

	srand(time(NULL));

    	int n_rand;
    	int b_rand;
    	int* a_rand;
    	int* c_rand;

    	n_rand = rand()%101;
    	b_rand = rand()%1001;

    	while(b_rand < (int)(7/2*n_rand)) {
    		n_rand = rand()%101;
    		b_rand = rand()%1001;
    	}
    
    	
    	a_rand = randWeight(n_rand,b_rand);
    	c_rand = randvalue(n_rand);

    	write_inst("instance1.csv", a_rand, c_rand, n_rand, b_rand);

	//File instance name
	//-F option
        char instance_file[1024];
        snprintf(       instance_file,
                        1024,
                        "%s",
                        "instance1.csv");

	char c;
        while ((c=getopt (argc, argv,"F:h")) != EOF)
	{
		switch(c)
		{
                        case 'F':
				snprintf(       instance_file,
						1024,
						"%s",
						optarg);
				break;

			case 'h':
				fprintf(stderr,"Usage: ./TP2 [options]\nOptions:\n\n");
				fprintf(stderr,"******** INSTANCE DATA ********\n");
				fprintf(stderr,"\t-F Instance file name to load............................(default %s).\n",instance_file);
				break;
			default:
				exit(0);
		}
	}

	dataSet data;

	//Open the instance file
	FILE* fin = fopen(instance_file,"r");
	read_TP2_instance(fin,&data);
	fclose(fin);

	//execute your solution methods on the instance you just read
	//Exact solution
	TP2_solve_exact(&data);

	return rval;
}

