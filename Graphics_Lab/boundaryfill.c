#include <stdio.h>

#define SIZE 40

int screen[SIZE][SIZE] = {0};

void boundaryFill(int x, int y) {
    if(x < 0 || y < 0 || x >= SIZE || y >= SIZE)
        return;

    if(screen[x][y] == 0) {
        screen[x][y] = 2; 
        boundaryFill(x+1,y);
        boundaryFill(x-1,y);
        boundaryFill(x,y+1);
        boundaryFill(x,y-1);
    }
}

int main() {

    FILE *gp = popen("gnuplot -persistent", "w");
    for(int i=10; i<=30; i++) {
        screen[10][i] = 1;
        screen[30][i] = 1;
        screen[i][10] = 1;
        screen[i][30] = 1;
    }
    boundaryFill(20,20);

    fprintf(gp,"set xrange [0:40]\n");
    fprintf(gp,"set yrange [0:40]\n");
    fprintf(gp,"set size square\n");

    fprintf(gp,"plot '-' with points pt 7 lc rgb 'red' title 'Boundary', \
                      '-' with points pt 7 lc rgb 'blue' title 'Filled'\n");
    for(int i=0;i<SIZE;i++)
        for(int j=0;j<SIZE;j++)
            if(screen[i][j] == 1)
                fprintf(gp,"%d %d\n",i,j);
    fprintf(gp,"e\n");
    for(int i=0;i<SIZE;i++)
        for(int j=0;j<SIZE;j++)
            if(screen[i][j] == 2)
                fprintf(gp,"%d %d\n",i,j);
    fprintf(gp,"e\n");

    pclose(gp);
    return 0;
}
