reset
load '~/.gnuplot'


file='../data/fitstudy/nocal-noF.dat'

set pm3d map

set xlabel "Fit Low"
set ylabel "Fit High"
set zlabel "Chi-Sq"

splot file u 1:2:13

set terminal postscript eps enhanced color "Helvetica,18"
set output '../pics/fitstudy/nocal-noF-chisq.eps'
replot