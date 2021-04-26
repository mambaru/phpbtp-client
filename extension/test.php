<?php

btp_configure("/etc/php7/conf.d/phpbtp-client.conf");
$id=btp_create_meter2( 1, 0);
print "id=$id\n";
btp_release_meter2($id, "phpscript1", "phpservice1", "phpserver1", "op1", 0);
sleep(1);
$id=btp_create_meter("phpscript1","phpservice1","phpserver1","op1", 1, 0);
print "id=$id\n";
sleep(1);
btp_release_meter($id, 0);

btp_add_size("phpscript1","phpservice1","phpserver1","add_time1", 100, 1);
btp_add_time("phpscript1","phpservice1","phpserver1","add_time1", 101, 1);
btp_add_time("phpscript1","phpservice1","phpserver1","add_time1", 102, 1);
sleep(1);
btp_add_time("phpscript1","phpservice1","phpserver1","add_time1", 120, 1);
sleep(1);
btp_add_time("phpscript1","phpservice1","phpserver1","add_time1", 140, 1);
sleep(1);
btp_add_time("phpscript1","phpservice1","phpserver1","add_time1", 150, 1);
#$count=btp_pushout();
#print "count=$count\n";

?>
