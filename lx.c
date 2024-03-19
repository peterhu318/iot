#include <stdio.h>

int main() {
	int val_0xa5=0x11;
        int rtn_value;
	
	rtn_value=val_0xa5&0b11111101;
	printf("%02x",rtn_value);
	return rtn_value;
}



	
