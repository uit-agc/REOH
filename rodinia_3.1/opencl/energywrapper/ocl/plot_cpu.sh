#! /bin/bash

# FILE FORMAT:
#  ncpus freqind memctrl energy time

ROOT_DIR=$(echo $PWD)

function doplot {
gnuplot -persist <<EOF
set terminal png size 4096,2160
set termoption font ',20'
set output "cpu_plot.png"

set title "Runtime / Configuration"

set xlabel "Configuration (ncpus|freqind|memctrl)"
set ylabel "Runtime"

colors = "purple #00ed1f #ed5600 #3465b5 #ed0017"
set for [i=1:words(colors)] linetype i lw 1 pt 5 ps 0.5 lc rgb word(colors, i)

set autoscale y
#set yrange [0:6000000]
set xtics rotate by -90
set xtics font ",1"

set autoscale xmax

set xtics autofreq

set grid
set style data lines

plot for [file in ${files}] file using 5:xticlabels(sprintf("%d|%d|%d",1,2,3)) with linespoints title system('basename '.file) noenhanced

EOF
}

files="\""
for file in $ROOT_DIR/output/*/CPU/*
do
	files+="${file} "
done
files+="\""

doplot files
