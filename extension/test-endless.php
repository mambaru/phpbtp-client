<?php

require __DIR__ . '/init.php';

btp_configure(__DIR__ . '/phpbtp-client.conf');

$i = 0;
while (1) {
    $i++;
    $btpMeter = btp_create_meter2(1);

    usleep(200);

    btp_release_meter2($btpMeter, 'test.php', 'my_service', 'server1', 'test1');

    if ($i % 100 == 0) {
        echo 'pushout' . PHP_EOL;
        btp_pushout();
        $i = 1;
    }
}
