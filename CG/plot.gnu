set title 'Cubic Bezier Curve'
set grid
plot 'control.dat' w lp lw 2 title 'Control Polygon',\
     'curve.dat' w l lw 3 title 'Bezier Curve'
pause -1
