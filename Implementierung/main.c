#include <stdio.h>  
#include <stdint.h>  
#include <time.h>

void encryptOptimized(unsigned int num_rounds, uint32_t v[2], uint32_t const key[4]);
void decryptOptimized(unsigned int num_rounds, uint32_t v[2], uint32_t const key[4]);

/* take 64 bits of data in v[0] and v[1] and 128 bits of key[0] - key[3] */  
// use "make" to run the code
// If you wanna test your assembly implementation, please use "make testassembly" instead.
void encrypt(unsigned int num_rounds, uint32_t v[2], uint32_t const key[4]) {  
    unsigned int i;  
    uint32_t v0=v[0], v1=v[1], sum=0, delta=0x9E3779B9;  
    for (i=0; i < num_rounds; i++) {  
        v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);  
        sum += delta;  
        v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum>>11) & 3]);  
    }  
    v[0]=v0; v[1]=v1;  
}  
void decrypt(unsigned int num_rounds, uint32_t v[2], uint32_t const key[4]) {  
    unsigned int i;  
    uint32_t v0=v[0], v1=v[1], delta=0x9E3779B9, sum=delta*num_rounds;  
    for (i=0; i < num_rounds; i++) {  
        v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum>>11) & 3]);  
        sum -= delta;  
        v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);  
    }  
    v[0]=v0; v[1]=v1;  
}  

void encryptOptimized(unsigned int num_rounds, uint32_t v[2], uint32_t const key[4]) {
    // TODO
    return;
}
void decryptOptimized(unsigned int num_rounds, uint32_t v[2], uint32_t const key[4]) {
    // TODO 
    return;
}
// encrypt the data n times in a row
float repeatedIterateTest(int n, void (*f)(unsigned int, uint32_t[], uint32_t const[]), unsigned int r, uint32_t v[2], uint32_t const k[4]){

    struct timespec start, end;
    long res = 0.0;
    for (int i = 0; i < n; i++){
        clock_gettime(CLOCK_MONOTONIC, &start);
        (*f)(r, v, k);  
        clock_gettime(CLOCK_MONOTONIC, &end);
        res += end.tv_nsec - start.tv_nsec;
    }
    return (float)res/n;
}
// ignore this yeah
// encrypt the same data n times in a row
float repeatedTest(int n, void (*f)(unsigned int, uint32_t[], uint32_t const[]), unsigned int r, uint32_t v[2], uint32_t const k[4]){
    unsigned int a = r;
    uint32_t b[2]={v[0],v[1]};  //{1,2}
    uint32_t const c[4]={k[0],k[1],k[2],k[3]};  
    struct timespec start, end;
    long res = 0.0;
    for (int i = 0; i < n; i++){
        clock_gettime(CLOCK_MONOTONIC, &start);
        (*f)(a, b, c);  
        clock_gettime(CLOCK_MONOTONIC, &end);
        res += end.tv_nsec - start.tv_nsec;
    }
    (*f)(r, v, k); 
    return (float)res/n;
}
int main()  
{  
    int testrounds = 30;
    uint32_t v[2]={1,2};  //{1,2}
    uint32_t const k[4]={2,2,3,4};  
    unsigned int r=32;              //num_rounds建议取值为32  
    // v为要加密的数据是两个32位无符号整数  
    // k为加密解密密钥，为4个32位无符号整数，即密钥长度为128位  
    uint32_t testflag = 0;  // change this to 1 if you wanna test your optimized implementation.

    float encryptTime, decryptTime;
    switch (testflag)
    {
    case 1:
        printf("Optimized version:\n");
        printf("Before encryption: %u %u\n",v[0],v[1]);
        printf ("Encryption done after average %f nanoseconds \n", 
            repeatedTest(testrounds, encryptOptimized, r, v, k));
        printf("After encryption: %u %u\n",v[0],v[1]);
        printf ("Decryption done after average %f nanoseconds \n", 
            repeatedTest(testrounds, decryptOptimized, r, v, k));
        printf("After decryption: %u %u\n",v[0],v[1]);
        break;
    
    default:
        printf("Original version:\n");
        printf("Before encryption: %u %u\n",v[0],v[1]);
        printf ("Encryption done after average %f nanoseconds \n", 
            repeatedTest(testrounds, encrypt, r, v, k));
        printf("After encryption: %u %u\n",v[0],v[1]);
        printf ("Decryption done after average %f nanoseconds \n", 
            repeatedTest(testrounds, decrypt, r, v, k));
        printf("After decryption: %u %u\n",v[0],v[1]);
        
        break;
    }

    return 0;  
}  