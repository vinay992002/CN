/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "square.h"

square_out *
squareproc_1_svc(square_in *argp, struct svc_req *rqstp)
{
	static square_out  result;
	result.res = argp->num1 * argp->num1;
	/*
	 * insert server code here
	 */

	return &result;
}