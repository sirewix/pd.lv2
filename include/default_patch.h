#define DEFAULT_PATCH "\
#N canvas 1153 26 475 1136 12;\n\
#X obj 135 299 dac~;\n\
#X obj 160 135 adc~;\n\
#X obj 157 227 *~;\n\
#X obj 224 182 osc~ 5;\n\
#X obj 31 24 notein;\n\
#X obj 32 136 mtof;\n\
#X obj 102 168 osc~;\n\
#X floatatom 22 102 5 0 0 0 - - - 0;\n\
#X floatatom 50 71 5 0 0 0 - - - 0;\n\
#X floatatom 99 52 5 0 0 0 - - - 0;\n\
#X connect 1 0 2 0;\n\
#X connect 1 1 2 0;\n\
#X connect 2 0 0 1;\n\
#X connect 2 0 0 0;\n\
#X connect 3 0 2 1;\n\
#X connect 4 0 7 0;\n\
#X connect 4 1 8 0;\n\
#X connect 4 2 9 0;\n\
#X connect 5 0 6 0;\n\
#X connect 6 0 2 0;\n\
#X connect 7 0 5 0;\n\
"
