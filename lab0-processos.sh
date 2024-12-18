# 1
ps -u
    # PID TTY          TIME CMD
    # 471 pts/10   00:00:00 sh
    # 492 pts/10   00:00:00 ps
ps -aux
# harmonia     471  0.0  0.0   7996  4096 pts/10   Ss   17:41   0:00 /usr/bin/sh
# harmonia     511  0.0  0.0   8580  3968 pts/10   R+   17:42   0:00 ps -aux
# harmonia     273  0.0  0.0   7996   140 pts/1    Ss+  dez12   0:00 /usr/bin/sh
# root       84791  0.0  0.0      0     0 ?        I    17:43   0:00 [kworker/u32:3]
# root       68943  0.0  0.0      0     0 ?        I    17:23   0:00 [kworker/5:0]
# root       69137  0.1  0.0      0     0 ?        I    17:23   0:02 [kworker/u32:0-ext4-rsv-conversion]

echo 'Observamos que na saída de ps -u, '