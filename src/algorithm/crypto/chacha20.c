#include "crypto.h"

#define ROTL32(v,n) ((v<<n)|(v>>(32-n)))
#define QR(a,b,c,d) a+=b; d^=a; d=ROTL32(d,16); c+=d; b^=c; b=ROTL32(b,12); a+=b; d^=a; d=ROTL32(d,8); c+=d; b^=c; b=ROTL32(b,7);


void    chacha20_block(uint32_t out[16], const uint32_t key[8], uint32_t counter, const uint32_t nonce[3])
{
    uint32_t x[16];

    x[0]=0x61707865;
    x[1]=0x3320646e;
    x[2]=0x79622d32;
    x[3]=0x6b206574;


    for(int i=0;i<8;i++)
        x[4+i]=key[i];


    x[12]=counter;
    x[13]=nonce[0];
    x[14]=nonce[1];
    x[15]=nonce[2];

    for(int i=0;i<16;i++)
        out[i]=x[i];

    for(int i=0;i<10;i++)
    {
        QR(out[0],out[4],out[8],out[12]);
        QR(out[1],out[5],out[9],out[13]);
        QR(out[2],out[6],out[10],out[14]);
        QR(out[3],out[7],out[11],out[15]);

        QR(out[0],out[5],out[10],out[15]);
        QR(out[1],out[6],out[11],out[12]);
        QR(out[2],out[7],out[8],out[13]);
        QR(out[3],out[4],out[9],out[14]);
    }


    for(int i=0;i<16;i++)
        out[i]+=x[i];
}