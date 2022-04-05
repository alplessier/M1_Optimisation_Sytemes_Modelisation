#include "TP1Functions.h"
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include<stdio.h>
#include<ilcplex/cplex.h>



int read_TP1_instance(FILE*fin,dataSet* dsptr)
{
	int rval = 0;

	//Taille des boites
	int V;
	//Nombre d'objets
	int n;
	fscanf(fin,"%d,%d\n",&n,&V);

	dsptr->V 	= V;
	dsptr->n 	= n;
	dsptr->size = (int*)malloc(sizeof(int)*n);

	int i;
	for( i = 0 ; i < n ; i++)
		fscanf(fin,"%d\n",&(dsptr->size[i]));

	fprintf(stderr,"Instance file read, each bin is %d long and there is %d items of lengths:\n",
			V,n);
	for( i = 0 ; i < n ; i++)
		fprintf(stderr,"%d\t",dsptr->size[i]);
	fprintf(stderr,"\n");
    

	return rval;
}

//printf("\n\n =========== SOLUTION EXACTE =========== \n\n");


int TP1_solve_exact(dataSet* dsptr)
{
	int rval = 0;

	IP_problem* ip_prob_ptr = &(dsptr->master);
	ip_prob_ptr->env 			= NULL;
	ip_prob_ptr->lp 			= NULL;
	ip_prob_ptr->env 			= CPXopenCPLEX (&rval);

	if(rval) fprintf(stderr,"ERROR WHILE CALLING CPXopenCPLEX\n");
	if ( ip_prob_ptr->env == NULL ) 
	{
		char  errmsg[1024];
		fprintf (stderr, "Could not open CPLEX environment.\n");
		CPXgeterrorstring (ip_prob_ptr->env, rval, errmsg);
		fprintf (stderr, "%s", errmsg);
		exit(0);	
	}

	//We create the MIP problem
	ip_prob_ptr->lp = CPXcreateprob (ip_prob_ptr->env, &rval, "TP1");
	if(rval) fprintf(stderr,"ERROR WHILE CALLING CPXcreateprob\n");

	rval = CPXsetintparam (ip_prob_ptr->env, CPX_PARAM_DATACHECK, CPX_ON); 
	rval = CPXsetintparam (ip_prob_ptr->env, CPX_PARAM_SCRIND, CPX_ON);

	int n 	= dsptr->n;
	int*size = dsptr->size;
	int V 	= dsptr->V;
	int nv 	= n+ n*n;

	//We fill our arrays
	//Memory
	ip_prob_ptr->nv 			= nv;
   ip_prob_ptr->x 			= (double*)malloc(sizeof(double)*nv);
   ip_prob_ptr->cost		 	= (double*)malloc(sizeof(double)*nv);
   ip_prob_ptr->c_type 		= (char*)malloc(sizeof(char)*nv);
   ip_prob_ptr->up_bound 	= (double*)malloc(sizeof(double)*nv);
   ip_prob_ptr->low_bound 	= (double*)malloc(sizeof(double)*nv);
	ip_prob_ptr->var_name 	= (char**)malloc(sizeof(char*)*nv);

	int i,j,id = 0;
	//Structures keeping the index of each variable
	int*id_y_i 		= (int*)malloc(sizeof(int)*n);
	int**id_x_ij 	= (int**)malloc(sizeof(int*)*n);
	for( i = 0 ; i < n ; i++)
		id_x_ij[i] 	= (int*)malloc(sizeof(int)*n);

	//First the variables yi (bin #i used or not)
	for( i = 0 ; i < n ; i++)
	{
		//We keep the id
		id_y_i[i] 	= id;

		//We generate the variable attributes
		ip_prob_ptr->x[id] 			= 0;
		ip_prob_ptr->cost[id] 		= 1;
		ip_prob_ptr->c_type[id] 	= 'B';
		ip_prob_ptr->up_bound[id] 	= 1;
		ip_prob_ptr->low_bound[id] = 0;
		ip_prob_ptr->var_name[id] 	= (char*)malloc(sizeof(char)*1024);
	        snprintf(       ip_prob_ptr->var_name[id],
        	                1024,
                	        "y_i%d",
                        	i);
		id++;
	}


	//variables xij
	for( i = 0 ; i < n ; i++)
	for( j = 0 ; j < n ; j++)
	{
		//We keep the id
		id_x_ij[i][j]  = id;
        

		//We generate the variable attributes
		ip_prob_ptr->x[id] 			= 0;
		ip_prob_ptr->cost[id] 		= 0;
		ip_prob_ptr->c_type[id] 	= 'B';
		ip_prob_ptr->up_bound[id] 	= 1;
		ip_prob_ptr->low_bound[id] = 0;
		ip_prob_ptr->var_name[id] 	= (char*)malloc(sizeof(char)*1024);
	        snprintf(       ip_prob_ptr->var_name[id],
        	                1024,
                	        "x_i%d_j%d",
                        	i,j);
		id++;
	}



	rval = CPXnewcols( ip_prob_ptr->env, ip_prob_ptr->lp, 
			nv, 
			ip_prob_ptr->cost, 
			ip_prob_ptr->low_bound,
			ip_prob_ptr->up_bound,
			ip_prob_ptr->c_type,
			ip_prob_ptr->var_name);
	if(rval)
		fprintf(stderr,"CPXnewcols returned errcode %d\n",rval);



//Constraints part
   ip_prob_ptr->rhs 		= (double*)malloc(sizeof(double));
   ip_prob_ptr->sense 	= (char*)malloc(sizeof(char));
   ip_prob_ptr->rmatbeg = (int*)malloc(sizeof(int));
	ip_prob_ptr->nz 		= n+1;


   ip_prob_ptr->rmatind 		= (int*)malloc(sizeof(int)*nv);
   ip_prob_ptr->rmatval 		= (double*)malloc(sizeof(double)*nv);
	ip_prob_ptr->const_name 	= (char**)malloc(sizeof(char*));
	ip_prob_ptr->const_name[0] = (char*)malloc(sizeof(char)*1024);

	//We fill what we can 
	ip_prob_ptr->rmatbeg[0] = 0;

	//We generate and add each constraint to the model
	//Bin capacity constraints
	ip_prob_ptr->rhs[0]	 = 0;
	ip_prob_ptr->sense[0] = 'L';
	for( i = 0 ; i < n ; i++)
	{
		//Constraint name
	        snprintf(       ip_prob_ptr->const_name[0],
        	                1024,
                	        "capacity_bin_i%d",
                        	i);
		id=0;
		//variable y_i coefficient
	      ip_prob_ptr->rmatind[id] 	= id_y_i[i];
        	ip_prob_ptr->rmatval[id] 	=  -V;
		id++;
		//variables x_ij coefficients
		for( j = 0 ; j < n ; j++)
		{
		   ip_prob_ptr->rmatind[id] = id_x_ij[i][j];
        	ip_prob_ptr->rmatval[id] =  size[j];
			id++;
		}
		rval = CPXaddrows( ip_prob_ptr->env, ip_prob_ptr->lp, 
			0,//No new column
			1,//One new row
			n+1,//Number of nonzero coefficients
			ip_prob_ptr->rhs, 
			ip_prob_ptr->sense, 
			ip_prob_ptr->rmatbeg, 
			ip_prob_ptr->rmatind, 
			ip_prob_ptr->rmatval,
			NULL,//No new column
			ip_prob_ptr->const_name );
		if(rval)
			fprintf(stderr,"CPXaddrows returned errcode %d\n",rval);

	}

	//Objet unicity constraints
	ip_prob_ptr->rhs[0] 		= 1;
	ip_prob_ptr->sense[0] 	= 'E';
	for( j = 0 ; j < n ; j++)
	{
		//Constraint name
	        snprintf(       ip_prob_ptr->const_name[0],
        	                1024,
                	        "unicity_bin_j%d",
                        	j);
		id=0;
		//variable y_i coefficient
	      //  ip_prob_ptr->rmatind[id] = id_y_i[i];
        	//ip_prob_ptr->rmatval[id] =  -V;
		
		//variables x_ij coefficients
		for( i = 0 ; i < n ; i++)
		{
		   ip_prob_ptr->rmatind[id] = id_x_ij[i][j];
        	ip_prob_ptr->rmatval[id] =  1 ;
			id++;
		}
		rval = CPXaddrows( ip_prob_ptr->env, ip_prob_ptr->lp, 
			0,//No new column
			1,//One new row
			n,//Number of nonzero coefficients
			ip_prob_ptr->rhs, 
			ip_prob_ptr->sense, 
			ip_prob_ptr->rmatbeg, 
			ip_prob_ptr->rmatind, 
			ip_prob_ptr->rmatval,
			NULL,//No new column
			ip_prob_ptr->const_name );
		if(rval)
			fprintf(stderr,"CPXaddrows returned errcode %d\n",rval);

	}




	/****************FILL HERE*******************/
	/*****Each item must be in exactly one bin***/
	/********************************************/


	//We write the problem for debugging purposes, can be commented afterwards
	rval = CPXwriteprob (ip_prob_ptr->env, ip_prob_ptr->lp, "bin_packing.lp", NULL);
	if(rval)
		fprintf(stderr,"CPXwriteprob returned errcode %d\n",rval);

	//We solve the model
	rval = CPXmipopt (ip_prob_ptr->env, ip_prob_ptr->lp);
	if(rval)
		fprintf(stderr,"CPXmipopt returned errcode %d\n",rval);

	rval = CPXsolwrite( ip_prob_ptr->env, ip_prob_ptr->lp, "bin_packing.sol" );
	if(rval)
		fprintf(stderr,"CPXsolwrite returned errcode %d\n",rval);

	//We get the objective value
	rval = CPXgetobjval( ip_prob_ptr->env, ip_prob_ptr->lp, &(ip_prob_ptr->objval) );
	if(rval)
		fprintf(stderr,"CPXgetobjval returned errcode %d\n",rval);

	//We get the best solution found 
	rval = CPXgetobjval( ip_prob_ptr->env, ip_prob_ptr->lp, &(ip_prob_ptr->objval) );
	rval = CPXgetx( ip_prob_ptr->env, ip_prob_ptr->lp, ip_prob_ptr->x, 0, nv-1 );
	if(rval)
		fprintf(stderr,"CPXgetx returned errcode %d\n",rval);

	//We display the solution
	double tolerance = 0.0001;
	int remaining;
	for( i = 0 ; i < n ; i++) 
	{
		id = id_y_i[i];
		if(ip_prob_ptr->x[id] <= 1-tolerance)
			continue;
		remaining = V;
		fprintf(stderr,"Bin #%d is used with volume %d and contains:\n",i,V);
		for( j = 0 ; j < n ; j++)
		{
			id = id_x_ij[i][j];
			if(ip_prob_ptr->x[id] <= 1-tolerance)
				continue;
			remaining -= size[j];
			fprintf(stderr,"\tItem #%d of volume %d (remaining: %d)\n",j,size[j],remaining);
		}
	}

	return rval;
}



int ** initTab2D(int taille)
{
    
    int ** tab = malloc(taille*sizeof(int*));
        
    for(int i=0;i<taille;i++)
    {
            tab[i] = malloc(taille*sizeof(int));
            for(int j=0;j<taille;j++)
            {
                    tab[i][j] = 0;
            }
    }
    
    return tab;
    
}


int * initTab(int taille)
{
    
    int * tab = malloc(taille*sizeof(int*));
        
    for(int i=0;i<taille;i++)
    {
        tab[i] = 0;
    }
    
    return tab;
    
}


// Nombre de boites | Composition des boites | Ratio utilisation des boites
int TP1_solve_heuristic(dataSet* dsptr)
{
	int rval 	= 0;
   int cpt 		= 0;
    
   int n 		= dsptr->n;
   int V 		= dsptr->V;
    
   int col 		= 0;
   int ligne 	= 0;
    
   int ** sac;
   sac 			= initTab2D(n);
    
    
   int * cap;
   cap 			= initTab(n);
    

    
    
    // BEST-FIT DECREASING
    printf("\n=========== NEXT FIT ===========\n");
    
    int ** nextFit;
    nextFit = initTab2D(n);
    
    int * nextFitCap;
    nextFitCap = initTab(n);
    
    
    for(int i=0;i<n;i++)
    {
            
            while(dsptr->size[i]+cpt <= V)
            {
                cpt += dsptr->size[i];
                nextFit[ligne][col] = dsptr->size[i];
                
                col++;
                i++;
            }

            nextFitCap[ligne] = cpt;
            ligne++;
            i--;
            col = 0;
            cpt = 0;
                
                    
    }
    
    //AFFICHAGE
    printf("\n\nLa solution a ce problème : %d boites \n\n",ligne);
    
    for(int i =0;i<dsptr->n;i++)
    {   
        if(nextFit[i][0] != 0)
        {
            printf("Boite %d : ",i+1);
            printf("[");
        }
                
                for(int j=0;j<dsptr->n;j++)
                {   
                    
                    if(nextFit[i][j] != 0)
                    {
                        printf(" %d ",nextFit[i][j]);
                        
                    }
                }
                
            if(nextFitCap[i] != 0)
            {
                printf("]");
                printf(" pour une taille totale de %d \n",nextFitCap[i]);
            }
    }
    
    
    //FIRST-FIT
    printf("\n=========== FIRST FIT ===========\n");
    
    int ** firstFit;
    firstFit = initTab2D(n);
    
    int * firstFitCap;
    firstFitCap = initTab(n);
    
    int * nbObj;
    nbObj = initTab(n);
    
    cpt = 0;
    ligne = 0;
    col = 0;
    
    for(int i=0;i<n;i++)
    {
            for(int j=0;j<n;j++)
            {
                    if(dsptr->size[i] + firstFitCap[j] <= V)
                    {
                            firstFitCap[j] += dsptr->size[i];
                            firstFit[j][nbObj[j]] = dsptr->size[i];
                            
                            if(nbObj[j] == 0)
                                ligne++;
                            
                            nbObj[j] +=1;
                            j = n;
                    }
                    
            }
    }
    
    //AFFICHAGE
    printf("\n\nLa solution a ce problème : %d boites \n\n",ligne);
    
    for(int i =0;i<dsptr->n;i++)
    {   
        if(firstFit[i][0] != 0)
        {
            printf("Boite %d : ",i+1);
            printf("[");
        }
                
                for(int j=0;j<dsptr->n;j++)
                {   
                    
                    if(firstFit[i][j] != 0)
                    {
                        printf(" %d ",firstFit[i][j]);
                        
                    }
                }
                
            if(firstFitCap[i] != 0)
            {
                printf("]");
                printf(" pour une taille totale de %d \n",firstFitCap[i]);
            }
    }
    
    
    
    
    // BEST-FIT DECREASING
    printf("\n=========== BEST FIT DECREASING ===========");
    
    cpt = 0;
    ligne = 0;
    col = 0;
    
    
	// TRIER ORDRE DECROISSANT
	int i,j,tmp;
    
    for(i = 0;i < n; i++)
    {
        for(j=0;j<n-i-1;j++)
        {
            if(dsptr->size[j] < dsptr->size[j+1])
            {
                tmp = dsptr->size[j];
                dsptr->size[j] = dsptr->size[j+1];
                dsptr->size[j+1] = tmp;
            }
        }
            
    }
    
    
    // SOMMER AU PLUS PRES DE LA CAPACITE BOITE ET SUPPRIMER PUIS INCREMENTER BOITES
    
    for(int i=0;i<n;i++)
            {
                if((cpt + dsptr->size[i]) <= V)
                {
                        cpt += dsptr->size[i];
                        sac[ligne][col] = dsptr->size[i];
                        
                        for(int j=i;j<n-1;j++)
                        {
                                dsptr->size[j] = dsptr->size[j+1];
                        }
                        i--;
                        n--;
                        col++;
                }
                

                // Fermer la boite car maximum 
                if(cpt == V || i+1 == n)
                {
                        cap[ligne] = cpt;
                        cpt = 0;
                        
                        
                        //Changement de ligne dans le tableau resulat final
                        ligne++;
                        col = 0;
                        i=-1;
                }
                
                else
                    cpt+=0;                
                
            }
                
    
    //AFFICHAGE
    printf("\n\nLa solution a ce problème : %d boites \n\n",ligne);
    
    for(int i =0;i<dsptr->n;i++)
    {   
        if(sac[i][0] != 0)
        {
            printf("Boite %d : ",i+1);
            printf("[");
        }
                
                for(int j=0;j<dsptr->n;j++)
                {   
                    
                    if(sac[i][j] != 0)
                    {
                        printf(" %d ",sac[i][j]);
                        
                    }
                }
                
            if(cap[i] != 0)
            {
                printf("]");
                printf(" pour une taille totale de %d \n",cap[i]);
            }
    }
    
   
    
    

	return rval;
}


