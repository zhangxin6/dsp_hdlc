copy /y .\Debug\*.out C:\Users\zhang\dsp_flash_write_easy
cd C:\Users\zhang\dsp_flash_write_easy
hex6x -order L helloworld_image.rmd *.out
Bttbl2Hfile helloworld.btbl helloworld.h helloworld.bin
del *.h *.btbl
dss.bat write_bin_2_flash.js