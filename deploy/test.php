<?php

btp_configure("/monamour/btpclient.conf");
$id=btp_create_meter("phpscript1","phpservice1","phpserver1","op1", 1, 0);
print "id=$id\n";
btp_release_meter($id, 0);
sleep(1);
$id=btp_create_meter("phpscript1","phpservice1","phpserver1","op1", 1, 0);
print "id=$id\n";
sleep(1);
btp_release_meter($id, 0);
$count=btp_pushout();
print "count=$count\n"

?>
