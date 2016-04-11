@echo off
cd src
..\C_SimpleCompiler.exe
..\tools\masm /I..\include MyResult.asm  MyResult.obj MyResult.lst MyResult.crf
echo  MyResult | ..\tools\link MyResult.obj
copy MyResult.exe ..\bin
del *.asm *.CRF *.LST *.OBJ *.tmp *.stx *.exe
cd ..
echo on
