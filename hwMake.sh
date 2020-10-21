#Change Student ID
NAME="b05901011_fraig"

TNAME="$NAME.tgz"
INDIR1="src/cir/"
FPATH1="./src/cir/"
FILES1="cirDef.h cirCmd.h cirCmd.cpp cirMgr.h cirMgr.cpp cirGate.h cirGate.cpp cirFraig.cpp cirSim.cpp cirOpt.cpp make.cir"
INDIR2="src/util/"
FPATH2="./src/util/"
FILES2="myHashMap.h myHashSet.h"

INDIR3="/"
FPATH3="./"
FILES3="b05901011.pdf"

rm -f $TNAME
rm -r -f ./$NAME
mkdir -p ./$NAME/$INDIR1
mkdir ./$NAME/$INDIR2
mkdir ./$NAME/include


for F in $FILES1
do
   cp $FPATH1$F ./$NAME/$INDIR1
done

for F in $FILES2
do
   cp $FPATH2$F ./$NAME/$INDIR2
done

for F in $FILES3
do
   cp $FPATH3$F ./$NAME/$INDIR3
done

cp ./Makefile ./$NAME

tar zcvf $TNAME $NAME

./SelfCheck $TNAME

rm -r -f $NAME