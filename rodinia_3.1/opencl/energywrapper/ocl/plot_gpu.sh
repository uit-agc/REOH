#! /bin/bash

# FILE FORMAT:
#  ncpus freqind memctrl energy time

ROOT_DIR=$(echo $PWD)

function doplot {
gnuplot -persist <<EOF
set terminal png size 4096,2160
set termoption font ',20'
set output "gpu_plot.png"

set title "runtime / Configuration"

set xlabel "Configuration (WG size)"
set ylabel "runtime"

colors = "purple #00ed1f #ed5600 #3465b5 #ed0017"
set for [i=1:words(colors)] linetype i lw 1 pt 5 ps 0.5 lc rgb word(colors, i)

set yrange [1000000:4000000]
set xrange [0:256]

set grid
set style data lines

plot for [file in ${files}] file using 1:3 with linespoints title system('basename '.file) noenhanced

EOF
}

files="\""
for file in $ROOT_DIR/output/*/GPU/*
do
	files+="${file} "
done
files+="\""

doplot files
