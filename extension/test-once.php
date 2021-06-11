<?php

require __DIR__ . '/init.php';

btp_configure(__DIR__ . '/phpbtp-client.conf');

$btpMeter = btp_create_meter2(1);

usleep(200);

btp_release_meter2($btpMeter, 'test.php', 'my_service_single_run', 'server134', 'test13434');
btp_pushout();


echo PHP_EOL;
echo 'Done' . PHP_EOL;
