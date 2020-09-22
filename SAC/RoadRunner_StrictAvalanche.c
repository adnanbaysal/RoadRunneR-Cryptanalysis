//Adnan Baysal
//RoadRunneR Strict Avalanche Criteria (SAC) test, graphical

#include <stdio.h>
#include <stdlib.h>

#define EXPERIMENTCNT  16384
#define u32          unsigned
#define u8 	         unsigned char
#define ROTL(x,y)	   ((((x)<<(y))^((x)>>(8-(y))))&255)
#define F3(x)	      (((x)^(ROTL(x,1)))^(ROTL(x,7))&255)

u32 Rounds = 12;

void draw_PNG(unsigned *table,u32 rows,u32 cols,char *filename){
   u32 i, j;
   FILE *file;
   file = fopen(filename,"wb");
   fprintf(file,"P5 %d %d 255\n",cols,rows);
   for(i=0;i<rows;i++) for(j=0;j<cols;j++) fprintf(file,"%c",(char)table[i*cols+j]);
   fclose(file);
}

void S(u8 *data){
   data[2] ^= (data[0]|data[1])^0xff;
   data[3] ^= (data[1]^0xff)&data[2];
   data[0] ^= (data[2]&data[3])^0xff;
   data[1] ^= (data[0]^0xff)|data[3];
}

void S2(u8 *data){
   data[2] ^= data[0]&data[1];
   data[3] ^= 0XFF;
   data[3] ^= data[1]&data[2];
   data[0] ^= 0XFF;
   data[0] ^= (data[3]^0xff)&data[2];
   data[1] ^= 0XFF;
   data[1] ^= (data[0]^0xff)&data[3];
   data[2] ^= 0XFF;
}


void M(u8 *data){
   u8 i;
   for(i=0;i<4;i++) data[i] = F3(data[i]);
}

void F(u8 *key,u8 *data){
   u8 i, j;
   for(i=0;i<3;i++){
      S(data);
      M(data);
      for(j=0;j<4;j++) data[j] ^= key[4*i+j];
   }
   S(data);
}

void RRR(u8 *key,u8 *data){
   u8 i, j, temp[4];
   for(i=0;i<Rounds;i++){
      for(j=0;j<4;j++) temp[j] = data[j];
      F(key+4*i,data);
      for(j=0;j<4;j++) data[j] ^= data[j+4];
      for(j=0;j<4;j++) data[j+4] = temp[j];
   }
}

#define MULTI  1

void SACerEnc(char *filename){
   unsigned table[64*64] = {0};
   u32 i, j, k, m;
   for(m=0;m<MULTI;m++){
      u8 key[12*12] = {0}, cipher1[8], cipher2[8];
      for(i=0;i<12*12;i++) key[i] = rand()&255;
      for(i=0;i<64;i++){
         for(j=0;j<EXPERIMENTCNT;j++){
            for(k=0;k<8;k++) cipher1[k] = rand()&255;
            for(k=0;k<8;k++) cipher2[k] = cipher1[k];
            cipher2[i/8] ^= 1<<(7-(i&7));
            RRR(key,cipher1);
            RRR(key,cipher2);
            for(k=0;k<8;k++) cipher1[k] ^= cipher2[k];
            for(k=0;k<64;k++) table[64*i+k] += (cipher1[k/8]>>(7-(k&7)))&1;
         }
      }
   }
   for(i=0;i<64;i++){
      for(j=0;j<64;j++){
         table[64*i+j] *= 255;
         table[64*i+j] /= EXPERIMENTCNT*MULTI;
      }
   }
   draw_PNG(table,64,64,filename);
}

int main(int argc,char **argv){//Usage : a.exe SEED filename Rounds
   srand(atoi(argv[1]));
   Rounds = atoi(argv[3]);
   SACerEnc(argv[2]);
   return 0;
}
