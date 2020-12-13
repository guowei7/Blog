#include <stdio.h>

int main(void)
{
	int a[3][4] = {0};
	printf("&(a[0])=%p,sizeof(&(a[0]))=%d\n",&(a[0]),sizeof(&(a[0])));
	printf("*&a=%p,sizeof(*&a)=%d\n",*&a,sizeof(*&a));
	printf("a=%p,sizeof(a)=%d,&a=%p,*a=%p,sizeof(&a)=%d,sizeof(*a)=%d\n",a,sizeof(a),&a,*a,sizeof(&a),sizeof(*a));
	printf("a[0]=%p,*(a+0)=%p,sizeof(a[0])=%d,sizeof(*(a+0))=%d\n",a[0],*(a+0),sizeof(a[0]),sizeof(*(a+0)));
	printf("a[1]=%p,*(a+1)=%p,sizeof(a[1])=%d,sizeof(*(a+1))=%d\n",a[1],*(a+1),sizeof(a[1]),sizeof(*(a+1)));
	printf("&a[0][0]=%p\n",&a[0][0]);
	return 0;
}