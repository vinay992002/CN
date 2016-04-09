struct square_in{
	int num1;	
};

struct square_out{
	int res;
};
program SQUARE_PROG{
	version SQUARE_VERS {
		square_out SQUAREPROC(square_in) =1;
	} = 1;
} = 0x2fffffff;