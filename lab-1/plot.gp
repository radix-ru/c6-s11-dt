set terminal svg size 1440,900 background "white"
set output "result.svg"

set xlabel "n_{doors}"
set ylabel "n_{cars}"
set zlabel "prob., %"

set xtics 1
set ytics 1
set ztics 5

set zrange [0:100]

set grid
set ticslevel 0

n_rounds=3200

set samples (20-3+1)
set isosamples (18-0+1)

set key off

set lmargin 0
set rmargin 0
set bmargin 0
set tmargin 0

splot \
	"data.txt" using 1:2:($3/n_rounds*100) with lines lc "#404040", \
	"data.txt" using 1:2:($4/n_rounds*100) with lines lc "#ff0000", \
	"data2.txt" using 1:2:($3/n_rounds*100) with lines lc "#404040", \
	"data2.txt" using 1:2:($4/n_rounds*100) with lines lc "#ff0000", \
	"data3.txt" using 1:2:($3*100) with points lc "#404040" pt 6 ps 0.75, \
	"data3.txt" using 1:2:($4*100) with points lc "#ff0000" pt 6 ps 0.75, \

#	y/x*100 with points lc 1 lw 0.5, \
#	y*(x - 1)/(x*(x - 2))*100 with points lc 2 lw 0.5, \

#	"data.bin" binary format="%u%u%u%u" using 1:2:($3/n_rounds)*100 with lines, \
#	"data.bin" binary format="%u%u%u%u" using 1:2:($4/n_rounds)*100 with lines
