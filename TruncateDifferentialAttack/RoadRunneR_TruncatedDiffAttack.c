#include <stdio.h>
#include <stdlib.h>

#define u32        unsigned
#define L(x)    ((((x)^((x<<1)^(x>>7))^((x<<2)^(x>>6))))&0xff)

#include "SLK_PASS.h"

u32 HW(u32 inp){
    unsigned out = 0;
    while(inp!=0){
        out += inp&1;
        inp >>= 1;
    }
    return out;
}

int main(){
    unsigned in[20], te[20], we[20], i, j, k, m, n, w, **SLK_PASS_LIST, HW8[256] = {0};
    unsigned F_PASS[256][512] = {0}, **F_PASS_LIST, min, **XOR_PASS, base, ones;
    SLK_PASS_LIST = (u32**)calloc(256,sizeof(u32*));
    F_PASS_LIST = (u32**)calloc(256,sizeof(u32*));//(total_output,min_weight),(out0,weight0),...
    XOR_PASS = (u32**)calloc(0x10000,sizeof(u32*));
    for(j=1;j<256;j++) HW8[j] = HW(j);
    for(i=0;i<256;i++) for(j=0;j<256;j++) F_PASS[i][2*j+1] = 100; 
    for(i=0;i<256;i++){
        k = 0;
        for(j=0;j<256;j++) if(SLK_PASS[i][j]==1) k++;
        SLK_PASS_LIST[i] = (u32*)calloc(k+1,sizeof(u32));
        SLK_PASS_LIST[i][0] = k;
        k = 1;
        for(j=0;j<256;j++){
            if(SLK_PASS[i][j]==1){
                SLK_PASS_LIST[i][k] = j;
                k++;
            }
        }
    }
    for(in[0]=1;in[0]<256;in[0]++){
        we[0] = HW8[in[0]];
        for(te[0]=0;te[0]<SLK_PASS_LIST[in[0]][0];te[0]++){
            in[1] = SLK_PASS_LIST[in[0]][te[0]+1];
            we[1] = we[0] + HW8[in[1]];
            for(te[1]=0;te[1]<SLK_PASS_LIST[in[1]][0];te[1]++){
                in[2] = SLK_PASS_LIST[in[1]][te[1]+1];
                we[2] = we[1] + HW8[in[2]];
                for(te[2]=0;te[2]<SLK_PASS_LIST[in[2]][0];te[2]++){
                    in[3] = SLK_PASS_LIST[in[2]][te[2]+1];
                    we[3] = we[2] + HW8[in[3]];
                    F_PASS[in[0]][2*in[3]] = 1;
                    if(we[3]<F_PASS[in[0]][2*in[3]+1]) F_PASS[in[0]][2*in[3]+1] = we[3];
                }
            }
        }
    }
    F_PASS_LIST[0] = (u32*)calloc(4,sizeof(u32));
    F_PASS_LIST[0][0] = 1;
    for(i=1;i<256;i++){
        k = 0;
        min = 100;
        for(j=0;j<256;j++){
            if(F_PASS[i][2*j]==1){
                if(F_PASS[i][2*j+1]<min) min = F_PASS[i][2*j+1];
                k++;
            }
        }
        F_PASS_LIST[i] = (u32*)calloc(2*(k+1),sizeof(u32));
        F_PASS_LIST[i][0] = k;
        F_PASS_LIST[i][1] = min;
        k = 1;
        for(j=1;j<256;j++){
            if(F_PASS[i][2*j]==1){
                F_PASS_LIST[i][2*k] = j;
                F_PASS_LIST[i][2*k+1] = F_PASS[i][2*j+1];
                k++;
            }
        }
    }
    for(i=0;i<256;i++){
        for(j=i;j<256;j++){
            base = i^j;
            ones = i&j;
            u32 ctr = 0, list[8] = {0};
            for(k=0;k<8;k++){
                if((ones>>(7-k))&1){
                    list[ctr] = k;
                    ctr++;
                }
            }
            w = HW8[ones];
            XOR_PASS[(i<<8)+j] = (u32*)calloc(1+(1<<w),sizeof(u32));
            XOR_PASS[(j<<8)+i] = (u32*)calloc(1+(1<<w),sizeof(u32));
            XOR_PASS[(i<<8)+j][0] = 1<<w;
            XOR_PASS[(j<<8)+i][0] = 1<<w;
            for(k=0;k<(1<<w);k++){
                n = 0;
                for(m=0;m<w;m++) n ^= ((k>>(w-1-m))&1)<<(7-list[m]);
                XOR_PASS[(i<<8)+j][1+k] = base^n;
                XOR_PASS[(j<<8)+i][1+k] = base^n;
            }
        }
    }
    min = 100;
    for(i=1;i<0x10000;i++){
        in[0] = i>>8;
        in[1] = i&0xff;
        we[0] = F_PASS_LIST[in[0]][1];
        if(we[0]<min){
            for(te[0]=0;te[0]<F_PASS_LIST[in[1]][0];te[0]++){
                in[2] = F_PASS_LIST[in[1]][2*(te[0]+1)];
                we[1] = we[0] + F_PASS_LIST[in[1]][2*(te[0]+1)+1];
                if(we[1]<min){
                    for(te[1]=0;te[1]<XOR_PASS[(in[2]<<8)+in[0]][0];te[1]++){
                        in[3] = XOR_PASS[(in[2]<<8)+in[0]][te[1]+1];
                        for(te[2]=0;te[2]<F_PASS_LIST[in[3]][0];te[2]++){
                            in[4] = F_PASS_LIST[in[3]][2*(te[2]+1)];
                            we[2] = we[1] + F_PASS_LIST[in[3]][2*(te[2]+1)+1];
                            if(we[2]<min){
                                for(te[3]=0;te[3]<XOR_PASS[(in[4]<<8)+in[1]][0];te[3]++){
                                    in[5] = XOR_PASS[(in[4]<<8)+in[1]][te[3]+1];
                                    for(te[4]=0;te[4]<F_PASS_LIST[in[5]][0];te[4]++){
                                        in[6] = F_PASS_LIST[in[5]][2*(te[4]+1)];
                                        we[3] = we[2] + F_PASS_LIST[in[5]][2*(te[4]+1)+1];
                                        if(we[3]<min){
                                            for(te[5]=0;te[5]<XOR_PASS[(in[6]<<8)+in[3]][0];te[5]++){
                                                in[7] = XOR_PASS[(in[6]<<8)+in[3]][te[5]+1];
                                                for(te[6]=0;te[6]<F_PASS_LIST[in[7]][0];te[6]++){
                                                    in[8] = F_PASS_LIST[in[7]][2*(te[6]+1)];
                                                    we[4] = we[3] + F_PASS_LIST[in[7]][2*(te[6]+1)+1];
                                                    if(we[4]<=min){
                                                        for(te[7]=0;te[7]<XOR_PASS[(in[8]<<8)+in[5]][0];te[7]++){
                                                            in[9] = XOR_PASS[(in[8]<<8)+in[5]][te[7]+1];
                                                            we[5] = we[4] + F_PASS_LIST[in[9]][1];
                                                            if(we[5]<min){
                                                                min = we[5];
                                                                printf("MIN = %2d : ",min);
                                                                for(j=0;j<10;j++) printf("%02X ",in[j]);
                                                                printf("\n");
                                                            }
                                                            else if(we[5]==min){
                                                                printf("MIN = %2d : ",min);
                                                                for(j=0;j<10;j++) printf("%02X ",in[j]);
                                                                printf("\n");
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}
