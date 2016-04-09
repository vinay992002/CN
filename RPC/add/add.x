struct input{
	int num[100];
	int n;	
};
program ADD_PROG{
	version ADD_VERS {
		int ADDPROC(input) =1;
	} = 1;
} = 0x2ff0ffff;