<?php

btp_configure("./phpbtp-client.conf");
$id=btp_create_meter("phpscript1","phpservice1","phpserver1","op1", 1, 0);
print "id=$id\n";
sleep(5);
btp_release_meter($id, 0);
#sleep(1);
#$count=btp_pushout();
#print "count=$count\n";
?>

