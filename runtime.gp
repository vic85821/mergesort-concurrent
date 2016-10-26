reset
set xlabel 'test'
set ylabel 'time(sec)'
set title 'performance comparison'
set term png enhanced font 'Verdana 10'
set output 'runtime.png'

plot [:][0:0.6] "output.txt" using 1:2 title 'thread: 1' with points pointtype 3, \
    ""  using 1:3 title 'thread: 2' with points pointtype 7, \
    ""  using 1:4 title 'thread: 4' with points pointtype 10, \
    ""  using 1:5 title 'thread: 8' with points pointtype 18, \
    ""  using 1:6 title 'thread: 16' with points pointtype 26
