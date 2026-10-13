#include <stdio.h>

void plotPoints(FILE *gp, int xc, int yc, int x, int y) {
    fprintf(gp,"%d %d\n",xc+x,yc+y);
    fprintf(gp,"%d %d\n",xc-x,yc+y);
    fprintf(gp,"%d %d\n",xc+x,yc-y);
    fprintf(gp,"%d %d\n",xc-x,yc-y);
    fprintf(gp,"%d %d\n",xc+y,yc+x);
    fprintf(gp,"%d %d\n",xc-y,yc+x);
    fprintf(gp,"%d %d\n",xc+y,yc-x);
    fprintf(gp,"%d %d\n",xc-y,yc-x);
}

int main() {
    int xc=15,yc=15,r=8;
    int x=0,y=r;
    int p=1-r;

    FILE *gp=popen("gnuplot -persistent","w");
    fprintf(gp,"set size square\n");
    fprintf(gp,"plot '-' with points pt 7\n");

    while(x<=y){
        plotPoints(gp,xc,yc,x,y);
        x++;
        if(p<0)
            p+=2*x+1;
        else{
            y--;
            p+=2*(x-y)+1;
        }
    }

    fprintf(gp,"e\n");
    pclose(gp);
    return 0;
}
