@echo off

for %%f in (*.c;*.h) do (
	echo. 
	echo ****** FILE: %%f
	txtdump %%f 20
)