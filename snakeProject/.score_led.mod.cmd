cmd_/home/lgh/embeded/snakeProject/score_led.mod := printf '%s\n'   score_led.o | awk '!x[$$0]++ { print("/home/lgh/embeded/snakeProject/"$$0) }' > /home/lgh/embeded/snakeProject/score_led.mod
