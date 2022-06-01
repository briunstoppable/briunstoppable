#include 'stdio.h'

int main()
{
    int a[] = {1, 2, 3, 8, 3, 5, 9, 0, 12, 56};  // 10 elements
    int i, *p;
    
    for(i=0; i<(&a)[1]-a; i++)
    	printf("%d ", a[i]);
    
    printf("\n\n");
    // OR, Another approach
 
    for(p=a; p<(&a)[1]; p++)   // even more efficient
    	printf("%d ", *p);
 
    return 0;    
}