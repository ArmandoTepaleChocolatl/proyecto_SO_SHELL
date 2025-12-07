#include <unistd.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

void w64(unsigned char *b, unsigned long long v){
    for(int i=0;i<8;i++) b[i]=v>>(i*8);
}
unsigned long long r64(unsigned char *b){
    unsigned long long v=0;
    for(int i=7;i>=0;i--) v=(v<<8)|b[i];
    return v;
}
void w16(unsigned char *b,unsigned short v){
    b[0]=v; b[1]=v>>8;
}
unsigned short r16(unsigned char *b){
    return b[0]|(b[1]<<8);
}

int main(int c,char *v[]){
    if(c<2){
        write(2,"Uso: echo 'ruta R|W bytes' | ./bit-log record\n     ./bit-log view < system.bitlog\n",83);
        return 1;
    }
    
    if(!strcmp(v[1],"record")){
        char line[2048];
        int n=read(0,line,2047);
        if(n<=0){ write(2,"Sin entrada\n",12); return 1; }
        line[n]=0;
        
        char path[1024],op[8],bytes[32];
        if(sscanf(line,"%s %s %s",path,op,bytes)!=3){
            write(2,"Formato: ruta R|W bytes\n",24);
            return 1;
        }
        
        unsigned char b[19];
        w64(b,time(NULL));
        b[8]=op[0];
        unsigned short L=strlen(path);
        w16(b+9,L);
        w64(b+11,strtoull(bytes,0,10));
        
        write(1,b,19);
        write(1,path,L);
        return 0;
    }
    
    if(!strcmp(v[1],"view")){
        unsigned char b[19];
        char path[1024],line[1200];
        
        write(1,"\n=== Registros ===\n\n",21);
        while(read(0,b,19)==19){
            unsigned short L=r16(b+9);
            if(read(0,path,L)!=L)break;
            path[L]=0;
            
            time_t ts=r64(b);
            struct tm *t=localtime(&ts);
            int n=snprintf(line,1200,"%04d-%02d-%02d %02d:%02d:%02d  %c  %llu bytes  %s\n",
                t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec,
                b[8],r64(b+11),path);
            write(1,line,n);
        }
        return 0;
    }
    
    write(2,"Opcion invalida\n",16);
    return 1;
}
