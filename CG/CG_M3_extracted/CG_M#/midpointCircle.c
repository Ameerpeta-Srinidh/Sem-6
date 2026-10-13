#include<stdio.h>
#include<stdlib.h>

int r=4;
int h=0;
int k=0;

FILE* set_plot()
{
    FILE* gp=popen("gnuplot -p","w");
    fprintf(gp,"set title 'midpoint algo'\n");
    fprintf(gp,"set size ratio -1\n");
    fprintf(gp,"set xrange[%d:%d]\n",-r-2,r+2);
    fprintf(gp,"set yrange[%d:%d]\n",-r-2,r+2);
    fprintf(gp,"set object 2 circle at %d,%d size %d fillcolor rgb 'black' fillstyle empty linewidth 2\n",h,k,r);
    fprintf(gp,"set grid\n");
    fprintf(gp,"plot '-' with points pointtype 7 pointsize 1.5 linecolor rgb 'red' title 'points'\n");
    return gp;
}

void plotOctant(FILE* gp,int h, int k,int x,int y)
{
    fprintf(gp,"%d %d\n",h+x,k+y);
    fprintf(gp,"%d %d\n",h-x,k+y);
    fprintf(gp,"%d %d\n",h+x,k-y);
    fprintf(gp,"%d %d\n",h-x,k-y);
    fprintf(gp,"%d %d\n",h+y,k+x);
    fprintf(gp,"%d %d\n",h+y,k-x);
    fprintf(gp,"%d %d\n",h-y,k+x);
    fprintf(gp,"%d %d\n",h-y,k-x);
}


int main()
{   
    FILE* gp = set_plot();
    if (!gp) {
        perror("gnuplot");
        return EXIT_FAILURE;
    }

    int x = 0;
    int y = r;
    int p = 1 - r;

    plotOctant(gp, h, k, x, y);

    while (x < y) {
        x++;
        if (p < 0) {
            p = p + 2 * x + 1;
        } else {
            y--;
            p = p + 2 * (x - y) + 1;
        }
        plotOctant(gp, h, k, x, y);
    }

    fprintf(gp, "e\n");
    pclose(gp);
    return EXIT_SUCCESS;
}
