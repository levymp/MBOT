#include <stdio.h>

enum motor {
    RIGHT = 2,
    LEFT = 1,
};

enum direction {
	FWD = 1, 
	BWD = -1
};

enum speed {
	FST = 100, 
	MEDF = 75,
	MED = 50,
	SLOW = 25,
	STP = 0
};


double DC(enum direction d, enum speed s);

int main() 
{
    double tst1 = DC(FWD, SLOW);
    double tst2 = DC(BWD, SLOW);
    if(tst1 != 0.25)
    {
        printf("FAILED: %f\n", tst1);
    }
    if(tst2 != -0.25)
    {
        printf("FAILED: %f\n", tst2);
    }
}


double DC(enum direction d, enum speed s)
{
	printf("d: %d, s:%d\n", d, s);
    double result =(double) d * s * 0.01;
    return result;
}