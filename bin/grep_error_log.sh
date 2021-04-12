
#!/bin/sh
grep '\[error\]\|\[fatal\]' log.txt | grep -v 'is ok'