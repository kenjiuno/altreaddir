# altreaddir

Simulate EIO error on [readdir](https://linux.die.net/man/3/readdir) by overriding API call.

```sh
admin@ip-172-30-0-216:~/altreaddir$ export LD_PRELOAD=`realpath alt.so`

admin@ip-172-30-0-216:~/altreaddir$ export BROKEN=/tmp

admin@ip-172-30-0-216:~/altreaddir$ ls /tmp
ls: reading directory '/tmp': Input/output error
```
