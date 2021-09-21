#!/bin/bash

[ ! -d ../src ] && echo "FAIL: Run from build directory!" && exit 1

STACK_SRC="$(find ../src/eplssrc/ -name '*.c') $(find ../src/eplssrc/ -name '*.h') ../src/eplssrc/SN/*cin"
TST_SRC=" $(find ../src/unittest/eplssrc/TSTs* -name '*.c') $(find ../src/unittest/eplssrc/TSTs* -name '*.h') ../src/unittest/eplssrc/Target/*"

if [ ! -f tools/oscrccheck/oscrccheck ] ; then
  echo "No oscrccheck found"
  exit 1
fi

cl=$(which gcc)

[ -f outstack.log ] ; rm -f outstack.log
[ -f outunittest.log ] ; rm -f outunittest.log

echo "Calculating Stack crcs"
cp ../src/unittest/eplssrc/Target/EPLScfg_SL.h EPLScfg.h
for i in $STACK_SRC ; do 
    
    srcname=${i##*/}
    if [ ${srcname} = "eplssrc.h" ] ; then
        continue
    fi

    python ../contrib/scripts/rmcomments.py $i ./${srcname}
    if [ ! -e ${srcname} ] ; then
            exit 1
    fi

    if [ ${i##*.} == "c" ] ; then
        $cl -I. -I../src/unittest/eplssrc/Target -I../src/eplssrc/SCM -I../src/eplssrc/SN -S -o $srcname.o $srcname 2> ./compile.log
        $cl -I. -I../src/unittest/eplssrc/Target -I../src/eplssrc/SCM -I../src/eplssrc/SN -S -o $srcname.p $i 2>> ./compile.log
        diff $srcname.o $srcname.p
        if [ $? -ne 0 ] ; then
            echo " [FAIL] $i differs before and after spaces removal!"
            [ -f ./compile.log ] && echo "Compile Log: " && cat ./compile.log
            exit 1
        fi
    fi
    
    crc=$(tools/oscrccheck/oscrccheck -f ./$srcname | cut -f 2 -d ' ' | cut -f 1) ; 
    rm ${srcname}
    filepath=$(echo $i | sed -e 's/..\/src\/eplssrc\///g')
    echo "$filepath;$crc" >> outstack.log; 
done

rm *.o *.p

sort outstack.log > outstack2.log
mv outstack2.log outstack.log

for i in $(cat outstack.log); do
    file=$(echo $i | cut -f1 -d ';')
    crc=$(echo $i | cut -f2 -d ';')
    
    echo "$crc;$file" >> outstack2.log
done

mv outstack2.log outstack.log


# Src code check is not performed for untitests, as include directories would be enormous.
# Rather it is assumed, as it worked for the normal src up to this point, that it would
# be working as well for the unit tests
echo "Calculating Unittest crcs"
for i in $TST_SRC ; do 
    srcname=${i##*/}
    python ../contrib/scripts/rmcomments.py $i ./${srcname}
    if [ ! -e ${srcname} ] ; then
            exit 1
    fi
    
    crc=$(tools/oscrccheck/oscrccheck -f ./$srcname | cut -f 2 -d ' ' | cut -f 1) ; 

    rm ${srcname}
    filepath=$(echo $i | sed -e 's/..\/src\/unittest\/eplssrc\///g')
    echo "$filepath;$crc" >> outunittest.log; 
done

sort outunittest.log > outunittest2.log
mv outunittest2.log outunittest.log

for i in $(cat outunittest.log); do
    file=$(echo $i | cut -f1 -d ';')
    crc=$(echo $i | cut -f2 -d ';')
    
    echo "$crc;$file" >> outunittest2.log
done

mv outunittest2.log outunittest.log

mv outstack.log ../src/tools/oscrccheck/oschecksum.crc
git add ../src/tools/oscrccheck/oschecksum.crc
mv outunittest.log ../src/tools/oscrccheck/osunittest.crc
git add ../src/tools/oscrccheck/osunittest.crc
new_stackcrc=$(tools/oscrccheck/oscrccheck -f ../src/tools/oscrccheck/oschecksum.crc | cut -f 2 -d ' ' | cut -f 1)
new_unittestcrc=$(tools/oscrccheck/oscrccheck -f ../src/tools/oscrccheck/osunittest.crc | cut -f 2 -d ' ' | cut -f 1)

echo "New Stack CRC: ${new_stackcrc}"
echo "New Unittest CRC: ${new_unittestcrc}"
echo "Adding Checksums for file" 
stackcrc="s/.*STACK_CRC \".*\" /SET \( openSAFETY_STACK_CRC \"${new_stackcrc}\" /g"
unittestcrc="s/.*UNITTEST_CRC \".*\" /SET \( openSAFETY_UNITTEST_CRC \"${new_unittestcrc}\" /g"
cp -v ../cmake/CheckOSChecksum.cmake CheckOSChecksum.cmake.presed
cat ../cmake/CheckOSChecksum.cmake | sed -e "${stackcrc}" -e "${unittestcrc}" > CheckOSChecksum.cmake
cp -v CheckOSChecksum.cmake ../cmake/CheckOSChecksum.cmake
git add ../cmake/CheckOSChecksum.cmake
