reset
load "~/.gnuplot"
unset key

file="../data/run145/tvse-lifetime-200-1600.dat"

set xlabel "Time (ms)"
set ylabel "Counts (arb)"

low=2700.
high=12000.

a=5.0e4
b=1.0e4
c=100.
t1=1155.59872775205
t2=2000.

f(x) = a*exp(-x/t1) + b*exp(-x/t2) + c
fit [low:high] f(x) file via a,b,t2,c

#f(x) = exp(-(x-a)/t1) + c
#fit [low:high] f(x) file via a,c,t1

set logscale y

plot [low:high] file w steps lw 3 lc rgb prim3,\
     f(x) lw 3 lc rgb comp2

#set terminal postscript eps enhanced color solid "Helvetica,18"
#set output "../pics/fits/tvse-lifetime-200-1600-fit-a.eps"
#replot