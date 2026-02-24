savedcmd_/home/kernel/modules/hello_module.mod := printf '%s\n'   hello_module.o | awk '!x[$$0]++ { print("/home/kernel/modules/"$$0) }' > /home/kernel/modules/hello_module.mod
