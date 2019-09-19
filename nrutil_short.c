/* This file contains utilities for the Numerical Recipes in C algorithms. */

#include <malloc.h>
#include <stdio.h>

#define NR_END 1
#define FREE_ARG char*

void nrerror(char error_text[])
{
	void exit();

	fprintf(stderr,"Numerical Recipes run-time error...\n");
	fprintf(stderr,"%s\n",error_text);
	fprintf(stderr,"...now exiting to system...\n");
	exit(1);
}

double *dvector(long nl,long nh)
/* allocate a double vector with subscript range v[nl..nh] */
{
        double *v;
 
        v=(double *)malloc((unsigned int) ((nh-nl+1+NR_END)*sizeof(double)));
        if (!v) nrerror("allocation failure in dvector()");
        return v-nl+NR_END;
}

void free_dvector(double *v,long nl)
/* free a double vector allocated with dvector() */
{
        free((FREE_ARG) (v+nl-NR_END));
}


