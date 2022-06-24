#include <stdio.h>  
#include <stdint.h>  
#include <time.h>
void encipherOptimized(unsigned int num_rounds, uint32_t v[2], uint32_t const key[4]);
void decipherOptimized(unsigned int num_rounds, uint32_t v[2], uint32_t const key[4]);

/* take 64 bits of data in v[0] and v[1] and 128 bits of key[0] - key[3] */  
// use "make" to run the code
// If you wanna test your assembly implementation, please use "make testassembly" instead.
void encipher(unsigned int num_rounds, uint32_t v[2], uint32_t const key[4]) {  
    unsigned int i;  
    uint32_t v0=v[0], v1=v[1], sum=0, delta=0x9E3779B9;  
    for (i=0; i < num_rounds; i++) {  
        v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);  
        sum += delta;  
        v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum>>11) & 3]);  
    }  
    v[0]=v0; v[1]=v1;  
}  
void decipher(unsigned int num_rounds, uint32_t v[2], uint32_t const key[4]) {  
    unsigned int i;  
    uint32_t v0=v[0], v1=v[1], delta=0x9E3779B9, sum=delta*num_rounds;  
    for (i=0; i < num_rounds; i++) {  
        v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum>>11) & 3]);  
        sum -= delta;  
        v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);  
    }  
    v[0]=v0; v[1]=v1;  
}  

void encipherOptimized(unsigned int num_rounds, uint32_t v[2], uint32_t const key[4]) {
    // TODO
    return;
}
void decipherOptimized(unsigned int num_rounds, uint32_t v[2], uint32_t const key[4]) {
    // TODO 
    return;
}    
int main()  
{  
    uint32_t v[2]={1,2};  //{1,2}
    uint32_t const k[4]={2,2,3,4};  
    unsigned int r=32;              //num_rounds建议取值为32  
    // v为要加密的数据是两个32位无符号整数  
    // k为加密解密密钥，为4个32位无符号整数，即密钥长度为128位  
    uint32_t testflag = 0;  // change this to 1 if you wanna test your optimized implementation.
    struct timespec encryptStart, encryptEnd, decryptStart, decryptEnd;
    switch (testflag)
    {
    case 1:
        printf("Optimized version:\n");
        printf("Before encryption: %u %u\n",v[0],v[1]);
        clock_gettime(CLOCK_MONOTONIC, &encryptStart);
        encipherOptimized(r, v, k);  
        clock_gettime(CLOCK_MONOTONIC, &encryptEnd);
        printf("After encryption: %u %u\n",v[0],v[1]);
        clock_gettime(CLOCK_MONOTONIC, &decryptStart);
        decipherOptimized(r, v, k);
        clock_gettime(CLOCK_MONOTONIC, &decryptEnd);
        printf("After decryption: %u %u\n",v[0],v[1]);
        break;
    
    default:
        printf("Original version:\n");
        printf("Before encryption: %u %u\n",v[0],v[1]);
        clock_gettime(CLOCK_MONOTONIC, &encryptStart);
        encipher(r, v, k);  
        clock_gettime(CLOCK_MONOTONIC, &encryptEnd);
        printf("After encryption: %u %u\n",v[0],v[1]);
        clock_gettime(CLOCK_MONOTONIC, &decryptStart);
        decipher(r, v, k);
        clock_gettime(CLOCK_MONOTONIC, &decryptEnd);
        printf("After decryption: %u %u\n",v[0],v[1]);
        
        break;
    }
    // double encryptTime = encryptEnd.tv_sec - encryptStart.tv_sec +
    //     1e-9*(encryptEnd.tv_nsec - encryptStart.tv_nsec);
    long encryptTime = encryptEnd.tv_nsec - encryptStart.tv_nsec;
    long decryptTime = decryptEnd.tv_nsec - decryptStart.tv_nsec;
    // double decryptTime = decryptEnd.tv_sec - decryptStart.tv_sec +
    //     1e-9*(decryptEnd.tv_nsec - decryptStart.tv_nsec);
    printf ("Encryption done after %ld nanoseconds \n", encryptTime);
    printf ("Decryption done after %ld nanoseconds \n", decryptTime);

    return 0;  
}  