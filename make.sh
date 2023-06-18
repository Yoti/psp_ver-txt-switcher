#!/bin/bash
rm -rf "v_switch_xxx-yyyn"

mkdir -p "v_switch_xxx-yyyn/eng_lang/ofw_mode"
mkdir -p "v_switch_xxx-yyyn/eng_lang/cfw_mode"
mkdir -p "v_switch_xxx-yyyn/rus_lang/ofw_mode"
mkdir -p "v_switch_xxx-yyyn/rus_lang/cfw_mode"
mkdir -p "v_switch_xxx-yyyn/src_gpl3"

rm -f "main.c" && cp "main_en.c" "main.c" && make clean && make OFW_MODE=1 && mv "EBOOT.PBP" "v_switch_xxx-yyyn/eng_lang/ofw_mode"
rm -f "main.c" && cp "main_en.c" "main.c" && make clean && make && mv "EBOOT.PBP" "v_switch_xxx-yyyn/eng_lang/cfw_mode"
rm -f "main.c" && cp "main_ru.c" "main.c" && make clean && make OFW_MODE=1 && mv "EBOOT.PBP" "v_switch_xxx-yyyn/rus_lang/ofw_mode"
rm -f "main.c" && cp "main_ru.c" "main.c" && make clean && make && mv "EBOOT.PBP" "v_switch_xxx-yyyn/rus_lang/cfw_mode"

rm -f "main.c" && make clean
cp -f *.c "v_switch_xxx-yyyn/src_gpl3" && cp -f *.TXT "v_switch_xxx-yyyn/src_gpl3" && cp -f makefile "v_switch_xxx-yyyn/src_gpl3" && cp -f *.PNG "v_switch_xxx-yyyn/src_gpl3" && cp -rf lib "v_switch_xxx-yyyn/src_gpl3/lib"
sleep 5s
