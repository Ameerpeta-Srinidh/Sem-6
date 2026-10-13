#include <stdio.h>

#define SIZE 60

int screen[SIZE][SIZE] = {0};


void floodFill(int x, int y, int oldColor, int newColor){

    if(x < 0 || y < 0 || x >= SIZE || y >= SIZE)
        return;

    if(screen[x][y] == oldColor){
        screen[x][y] = newColor;

        floodFill(x+1,y,oldColor,newColor);
        floodFill(x-1,y,oldColor,newColor);
        floodFill(x,y+1,oldColor,newColor);
        floodFill(x,y-1,oldColor,newColor);
    }
}

void drawCircle(int xc, int yc, int r) {

    int x = 0;
    int y = r;
    int p = 1 - r;

    while(x <= y) {

        screen[xc+x][yc+y] = 1;
        screen[xc-x][yc+y] = 1;
        screen[xc+x][yc-y] = 1;
        screen[xc-x][yc-y] = 1;

        screen[xc+y][yc+x] = 1;
        screen[xc-y][yc+x] = 1;
        screen[xc+y][yc-x] = 1;
        screen[xc-y][yc-x] = 1;

        x++;

        if(p < 0)
            p += 2*x + 1;
        else {
            y--;
            p += 2*(x - y) + 1;
        }
    }
}

int main(){

    FILE *gp = popen("gnuplot -persistent", "w");

    int xc = 30, yc = 30, r = 15;

    drawCircle(xc, yc, r);
    floodFill(xc, yc, 0, 2);

    fprintf(gp,"set xrange [0:60]\n");
    fprintf(gp,"set yrange [0:60]\n");
    fprintf(gp,"set size square\n");

    fprintf(gp,"plot '-' with points pt 7 lc rgb 'red' title 'Circle Boundary', \
                      '-' with points pt 7 lc rgb 'blue' title 'Flood Filled'\n");

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
