savedcmd_/home/kernel/modules/char_driver.mod := printf '%s\n'   char_driver.o | awk '!x[$$0]++ { print("/home/kernel/modules/"$$0) }' > /home/kernel/modules/char_driver.mod
