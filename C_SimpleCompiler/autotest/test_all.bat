@echo off
set /A MY_COUNT=0
FOR /D %%j IN (MK23T_%1*) DO CALL tools\test_one.bat %%j
echo "========= %MY_COUNT%  TESTS END ============
