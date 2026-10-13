#include <stdio.h>
#include <stdlib.h>

#define MAX 50
#define HEIGHT 500

typedef struct Edge{
    int ymax;
    float x;
    float invSlope;
    struct Edge *next;
} Edge;

Edge *ET[HEIGHT];
Edge *AET = NULL;

void insertEdge(Edge **list, Edge *edge) {
    if (*list == NULL || edge->x < (*list)->x) {
        edge->next = *list;
        *list = edge;
    } else {
        Edge *temp = *list;
        while (temp->next && temp->next->x < edge->x)
            temp = temp->next;
        edge->next = temp->next;
        temp->next = edge;
    }
}

void removeEdges(int y) {
    while (AET && AET->ymax == y) {
        Edge *temp = AET;
        AET = AET->next;
        free(temp);
    }

    Edge *curr = AET;
    while (curr && curr->next) {
        if (curr->next->ymax == y) {
            Edge *temp = curr->next;
            curr->next = temp->next;
            free(temp);
        } else {
            curr = curr->next;
        }
    }
}

void updateX() {
    Edge *temp = AET;
    while (temp) {
        temp->x += temp->invSlope;
        temp = temp->next;
    }
}

void sortAET() {
    Edge *sorted = NULL;
    Edge *curr = AET;
    while (curr) {
        Edge *next = curr->next;
        insertEdge(&sorted, curr);
        curr = next;
    }
    AET = sorted;
}

int main() {

    FILE *gp = popen("gnuplot -persistent", "w");

    int n;
    int poly[MAX][2];

    printf("Enter number of vertices: ");
    scanf("%d", &n);

    printf("Enter coordinates (x y):\n");
    for(int i = 0; i < n; i++)
        scanf("%d %d", &poly[i][0], &poly[i][1]);

    /* Auto compute plot range */
    int xmin = poly[0][0], xmax = poly[0][0];
    int ymin_input = poly[0][1], ymax_input = poly[0][1];

    for(int i = 1; i < n; i++) {
        if(poly[i][0] < xmin) xmin = poly[i][0];
        if(poly[i][0] > xmax) xmax = poly[i][0];
        if(poly[i][1] < ymin_input) ymin_input = poly[i][1];
        if(poly[i][1] > ymax_input) ymax_input = poly[i][1];
    }

    for(int i = 0; i < HEIGHT; i++)
        ET[i] = NULL;

    int ymin = HEIGHT, ymax = 0;

    /* Build Edge Table */
    for(int i = 0; i < n; i++) {

        int x1 = poly[i][0];
        int y1 = poly[i][1];
        int x2 = poly[(i+1)%n][0];
        int y2 = poly[(i+1)%n][1];

        if(y1 == y2) continue;

        int yMinEdge = y1 < y2 ? y1 : y2;
        int yMaxEdge = y1 > y2 ? y1 : y2;
        float x = (y1 < y2) ? x1 : x2;
        float invSlope = (float)(x2 - x1)/(y2 - y1);

        Edge *edge = (Edge*)malloc(sizeof(Edge));
        edge->ymax = yMaxEdge;
        edge->x = x;
        edge->invSlope = invSlope;
        edge->next = NULL;

        insertEdge(&ET[yMinEdge], edge);

        if(yMinEdge < ymin) ymin = yMinEdge;
        if(yMaxEdge > ymax) ymax = yMaxEdge;
    }

    /* Clean plot settings */
    fprintf(gp,"set xrange [%d:%d]\n", xmin-5, xmax+5);
    fprintf(gp,"set yrange [%d:%d]\n", ymin_input-5, ymax_input+5);
    fprintf(gp,"set size ratio -1\n");

    fprintf(gp,"unset key\n");
    fprintf(gp,"unset tics\n");
    fprintf(gp,"unset border\n");
    fprintf(gp,"unset grid\n");

    fprintf(gp,"plot '-' with points pt 7 ps 1.5 lc rgb 'blue'\n");

    int y = ymin;

    while(AET != NULL || y <= ymax) {

        Edge *temp = ET[y];
        while(temp) {
            Edge *next = temp->next;
            insertEdge(&AET, temp);
            temp = next;
        }

        removeEdges(y);
        sortAET();

        Edge *curr = AET;

        while(curr && curr->next) {
            int xStart = (int)(curr->x);
            int xEnd   = (int)(curr->next->x);

            for(int x = xStart; x <= xEnd; x++)
                fprintf(gp,"%d %d\n",x,y);

            curr = curr->next->next;
        }

        y++;
        updateX();
    }

    fprintf(gp,"e\n");
    pclose(gp);

    return 0;
}
