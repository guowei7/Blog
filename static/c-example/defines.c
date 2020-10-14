#include <stdio.h>
#include <limits.h>
#include <float.h>

int main(void)
{
    printf("Some number limits for this system\n");
    printf("Biggest int:%d\n", INT_MAX);
    printf("Smallest long long %lld\n", LLONG_MIN);
    printf("One byte=%d bits on this system.\n", CHAR_BIT);
    printf("Largest double:%e\n", DBL_MAX);
    printf("Smallest normal float:%e\n", FLT_MIN);
    printf("float precision=%d digits\n", FLT_DIG);
    printf("float epsilon = %e\n", FLT_EPSILON);
    printf("double epsilon = %e\n",DBL_EPSILON);
    float n_e,m, n = 3.8f;
    m = n - (int)n;
    n_e = n + FLT_EPSILON;
    printf("n=%f\n", n);
    printf("m=%f\n", m);
    printf("n_e=%f\n",n_e);
    printf("n_e==n is %d",n_e==n);
    return 0;
}