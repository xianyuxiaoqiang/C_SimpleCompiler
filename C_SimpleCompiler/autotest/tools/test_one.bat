@echo "------------------------------"
@echo " @@@@  Testing  %1 Begin @@@@"
@set /A MY_COUNT=%MY_COUNT% + 1
@cd %1
@..\C_SimpleCompiler.exe
@cd ..
@echo " @@@@  Testing  %1 End @@@@"
