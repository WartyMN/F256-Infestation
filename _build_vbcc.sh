#!/bin/zsh

DEV=~/dev/bbedit-workspace-foenix
PROJECT=$DEV/F256-Infestation
CONFIG_DIR=$PROJECT/config_cc65

# name that will be used in files
VERSION_STRING="1.0b1"

# debug logging levels: 1=error, 2=warn, 3=info, 4=debug general, 5=allocations
#DEBUG_DEF_1="-DLOG_LEVEL_1"
#DEBUG_DEF_2="-DLOG_LEVEL_2"
#DEBUG_DEF_3="-DLOG_LEVEL_3"
#DEBUG_DEF_4="-DLOG_LEVEL_4"
#DEBUG_DEF_5="-DLOG_LEVEL_5"
DEBUG_DEF_1=
DEBUG_DEF_2=
DEBUG_DEF_3=
DEBUG_DEF_4=
DEBUG_DEF_5=

# whether disk or serial debug will be used, IF debug is actually on
# defining serial debug means serial will be used, not defining it means disk will be used. 
DEBUG_VIA_SERIAL="-DUSE_SERIAL_LOGGING"
#DEBUG_VIA_SERIAL=

#STACK_CHECK="--check-stack"
STACK_CHECK=

#optimization
#OPTI=-Oirs
OPTI=-Os

BUILD_DIR=$PROJECT/build_cc65
TARGET_DEFS="-D_TRY_TO_WRITE_TO_DISK"
#PLATFORM_DEFS="-D_SIMULATOR_" #do not define simulator if running on real hardware
PLATFORM_DEFS= #do not define simulator if running on real hardware
CC65TGT=none
CC65LIB=$CONFIG_DIR/lib/f256_lichking_only.lib
CC65CPU=65C02
OVERLAY_CONFIG=infest_overlay_f256.cfg
DATADIR=data

cd $PROJECT

echo "\n**************************\nCC65 compile start...\n**************************\n"
which cc65

mkdir -p $BUILD_DIR
mkdir -p $BUILD_DIR/infest_install/
mkdir -p $BUILD_DIR/infest_install/disk

rm -r $BUILD_DIR/*.s
rm -r $BUILD_DIR/*.o

# compile
cc65 -g --cpu $CC65CPU -t $CC65TGT $OPTI -I $CONFIG_DIR $TARGET_DEFS $PLATFORM_DEFS $DEBUG_DEF_1 $DEBUG_DEF_2 $DEBUG_DEF_3 $DEBUG_DEF_4 $DEBUG_DEF_5 $DEBUG_VIA_SERIAL $STACK_CHECK -T app.c -o $BUILD_DIR/app.s
cc65 -g --cpu $CC65CPU -t $CC65TGT $OPTI -I $CONFIG_DIR $TARGET_DEFS $PLATFORM_DEFS $DEBUG_DEF_1 $DEBUG_DEF_2 $DEBUG_DEF_3 $DEBUG_DEF_4 $DEBUG_DEF_5 $DEBUG_VIA_SERIAL $STACK_CHECK -T comm_buffer.c -o $BUILD_DIR/comm_buffer.s
cc65 -g --cpu $CC65CPU -t $CC65TGT $OPTI -I $CONFIG_DIR $TARGET_DEFS $PLATFORM_DEFS $DEBUG_DEF_1 $DEBUG_DEF_2 $DEBUG_DEF_3 $DEBUG_DEF_4 $DEBUG_DEF_5 $DEBUG_VIA_SERIAL $STACK_CHECK -T general.c -o $BUILD_DIR/general.s
cc65 -g --cpu $CC65CPU -t $CC65TGT $OPTI -I $CONFIG_DIR $TARGET_DEFS $PLATFORM_DEFS $DEBUG_DEF_1 $DEBUG_DEF_2 $DEBUG_DEF_3 $DEBUG_DEF_4 $DEBUG_DEF_5 $DEBUG_VIA_SERIAL $STACK_CHECK -T keyboard.c -o $BUILD_DIR/keyboard.s
cc65 -g --cpu $CC65CPU -t $CC65TGT $OPTI -I $CONFIG_DIR $TARGET_DEFS $PLATFORM_DEFS $DEBUG_DEF_1 $DEBUG_DEF_2 $DEBUG_DEF_3 $DEBUG_DEF_4 $DEBUG_DEF_5 $DEBUG_VIA_SERIAL $STACK_CHECK -T level.c -o $BUILD_DIR/level.s
cc65 -g --cpu $CC65CPU -t $CC65TGT $OPTI -I $CONFIG_DIR $TARGET_DEFS $PLATFORM_DEFS $DEBUG_DEF_1 $DEBUG_DEF_2 $DEBUG_DEF_3 $DEBUG_DEF_4 $DEBUG_DEF_5 $DEBUG_VIA_SERIAL $STACK_CHECK -T object.c -o $BUILD_DIR/object.s
cc65 -g --cpu $CC65CPU -t $CC65TGT --code-name OVERLAY_STARTUP $OPTI -I $CONFIG_DIR $TARGET_DEFS $PLATFORM_DEFS $DEBUG_DEF_1 $DEBUG_DEF_2 $DEBUG_DEF_3 $DEBUG_DEF_4 $DEBUG_DEF_5 $DEBUG_VIA_SERIAL $STACK_CHECK -T overlay_startup.c -o $BUILD_DIR/overlay_startup.s
cc65 -g --cpu $CC65CPU -t $CC65TGT $OPTI -I $CONFIG_DIR $TARGET_DEFS $PLATFORM_DEFS $DEBUG_DEF_1 $DEBUG_DEF_2 $DEBUG_DEF_3 $DEBUG_DEF_4 $DEBUG_DEF_5 $DEBUG_VIA_SERIAL $STACK_CHECK -T player.c -o $BUILD_DIR/player.s
cc65 -g --cpu $CC65CPU -t $CC65TGT --code-name OVERLAY_SCREEN $OPTI -I $CONFIG_DIR $TARGET_DEFS $PLATFORM_DEFS $DEBUG_DEF_1 $DEBUG_DEF_2 $DEBUG_DEF_3 $DEBUG_DEF_4 $DEBUG_DEF_5 $DEBUG_VIA_SERIAL $STACK_CHECK -T screen.c -o $BUILD_DIR/screen.s
cc65 -g --cpu $CC65CPU -t $CC65TGT $OPTI -I $CONFIG_DIR $TARGET_DEFS $PLATFORM_DEFS $DEBUG_DEF_1 $DEBUG_DEF_2 $DEBUG_DEF_3 $DEBUG_DEF_4 $DEBUG_DEF_5 $DEBUG_VIA_SERIAL $STACK_CHECK -T strings.c -o $BUILD_DIR/strings.s
cc65 -g --cpu $CC65CPU -t $CC65TGT $OPTI -I $CONFIG_DIR $TARGET_DEFS $PLATFORM_DEFS $DEBUG_DEF_1 $DEBUG_DEF_2 $DEBUG_DEF_3 $DEBUG_DEF_4 $DEBUG_DEF_5 $DEBUG_VIA_SERIAL $STACK_CHECK -T sys.c -o $BUILD_DIR/sys.s
cc65 -g --cpu $CC65CPU -t $CC65TGT $OPTI -I $CONFIG_DIR $TARGET_DEFS $PLATFORM_DEFS $DEBUG_DEF_1 $DEBUG_DEF_2 $DEBUG_DEF_3 $DEBUG_DEF_4 $DEBUG_DEF_5 $DEBUG_VIA_SERIAL $STACK_CHECK -T text.c -o $BUILD_DIR/text.s

# Kernel access
cc65 -g --cpu 65C02 -t $CC65TGT $OPTI -I $CONFIG_DIR $TARGET_DEFS $PLATFORM_DEFS -T kernel.c -o $BUILD_DIR/kernel.s


echo "\n**************************\nCA65 assemble start...\n**************************\n"

# assemble into object files
cd $BUILD_DIR
ca65 -t $CC65TGT app.s
ca65 -t $CC65TGT comm_buffer.s
ca65 -t $CC65TGT general.s
ca65 -t $CC65TGT keyboard.s
ca65 -t $CC65TGT level.s
ca65 -t $CC65TGT object.s
ca65 -t $CC65TGT overlay_startup.s
ca65 -t $CC65TGT player.s
ca65 -t $CC65TGT screen.s
ca65 -t $CC65TGT strings.s
ca65 -t $CC65TGT sys.s
ca65 -t $CC65TGT text.s

# Kernel access
ca65 -t $CC65TGT kernel.s -o kernel.o

# name 'header'
#ca65 -t $CC65TGT ../name.s -o name.o
ca65 -t $CC65TGT ../memory.asm -o memory.o


echo "\n**************************\nLD65 link start...\n**************************\n"

# link files into an executable
ld65 -C $CONFIG_DIR/$OVERLAY_CONFIG -o infest.rom kernel.o app.o comm_buffer.o general.o keyboard.o level.o memory.o object.o player.o overlay_startup.o screen.o strings.o sys.o text.o $CC65LIB -m infest_$CC65TGT.map -Ln labels.lbl


echo "\n**************************\nCC65 tasks complete\n**************************\n"

# get copy of sprite data
cp $PROJECT/data/robot.bin $BUILD_DIR/
cp $PROJECT/data/human1.bin $BUILD_DIR/
cp $PROJECT/data/bullets_s.bin $BUILD_DIR/
cp $PROJECT/data/bullets_l.bin $BUILD_DIR/
cp $PROJECT/data/tiles.bin $BUILD_DIR/
cp $PROJECT/data/tilemap.bin $BUILD_DIR/

#build pgZ for disk
fname=("infest.rom" "infest.rom.1" "infest.rom.2" "robot.bin" "human1.bin" "bullets_s.bin" "bullets_l.bin" "tilemap.bin" "tiles.bin")
addr=("990700" "000001" "002001" "004002" "005002" "005802" "005A02" "A85D02" "006002")

for ((i = 1; i <= $#fname; i++)); do
v1=$(stat -f%z $fname[$i]); v2=$(printf '%04x\n' $v1); v3='00'$v2; v4=$(echo -n $v3 | tac -rs ..); v5=$addr[$i]$v4;v6=$(sed -Ee 's/([A-Za-z0-9]{2})/\\\x\1/g' <<< "$v5"); echo -n $v6 > $fname[$i]'.hdr'
done

echo -n 'Z' >> pgZ_start.hdr
echo -n '\x99\x07\x00\x00\x00\x00' >> pgZ_end.hdr

cat pgZ_start.hdr infest.rom.hdr infest.rom infest.rom.1.hdr infest.rom.1 infest.rom.2.hdr infest.rom.2 robot.bin.hdr robot.bin human1.bin.hdr human1.bin bullets_s.bin.hdr bullets_s.bin bullets_l.bin.hdr bullets_l.bin tilemap.bin.hdr tilemap.bin tiles.bin.hdr tiles.bin pgZ_end.hdr > infest.pgZ 

rm *.hdr

cp infest.pgZ infest_install/disk/

# copy pgz binary to SD Card on F256 via fnxmanager
python3 $FOENIXMGR/FoenixMgr/fnxmgr.py --copy infest.pgZ


# clear temp files
rm infest.ro*



echo "\n**************************\nCC65 build script complete\n**************************\n"
