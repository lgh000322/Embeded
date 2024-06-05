cmd_/home/lgh/embeded/kernelPratice/driver.mod := printf '%s\n'   driver.o | awk '!x[$$0]++ { print("/home/lgh/embeded/kernelPratice/"$$0) }' > /home/lgh/embeded/kernelPratice/driver.mod
