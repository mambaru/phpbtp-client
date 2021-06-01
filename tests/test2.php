<?php
// while :; do php ./test.php; done
btp_configure('../extension/phpbtp-client.conf');
$btpMeter = btp_create_meter2(1);
print "$btpMeter\n";
usleep(2000000);
$res1=btp_release_meter2($btpMeter, 'test.php', 'my_service_single_run', 'server134', 'test13434');
print "$res1\n";
$res2=btp_pushout();
print "btp_pushout $res2\n";
usleep(2000000);
?>
