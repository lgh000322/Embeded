cmd_/home/lgh/embeded/kernelPratice3/Module.symvers :=  sed 's/ko$$/o/'  /home/lgh/embeded/kernelPratice3/modules.order | scripts/mod/modpost -m -a    -o /home/lgh/embeded/kernelPratice3/Module.symvers -e -i Module.symvers -T - 
