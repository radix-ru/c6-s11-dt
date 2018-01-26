#w =  960; h = 600
w = 1200; h = 800

set terminal svg size w,h background "black"
set output "path.svg"

set xlabel	tc "white"
set ylabel	tc "white"
set border	lc "white"
set key		tc "white"
set grid	lc "white"

unset key

h1 = 180/360.0
h2 =   0/360.0
d = h2 - h1
set palette model HSV functions h1+gray*d,1,1

set ticslevel 0
set view 45,135

set xrange [0:1]
set yrange [0:1]
set zrange [0:1]

set xlabel "x"
set ylabel "y"
set zlabel "z"

set label at 1.0/3.0,1.0/3.0,1.0/3.0 "Answer" point lc "white" tc "white" front

#plot \
#	"path.bin" binary format="%lf%lf%lf%lf%lf%lf" using 1:2:($4-$1):($5-$2):0 with vectors linecolor palette, \
#	"path.bin" binary format="%lf%lf%lf%lf%lf%lf" using 1:2:0 with points linecolor palette \

plot "path.bin" binary format="%lf%lf%lf" using 1:2:0 with lines linecolor palette
#plot "path.bin" binary format="%lf%lf%lf%lf%lf%lf" using 1:2:0 with lines linecolor palette
#splot "path.bin" binary format="%lf%lf%lf%lf%lf%lf" using 1:2:3:0 with lines linecolor palette
#splot "path.bin" binary format="%lf%lf%lf" using 1:2:3:0 with lines linecolor palette

#plot "path.bin" binary format="%lf%lf%lf%lf" using 1:2:($3-$1):($4-$2):0 with vectors linecolor palette
#plot "path.bin" binary format="%lf%lf%lf%lf" using 1:2:0 with lines linecolor palette
