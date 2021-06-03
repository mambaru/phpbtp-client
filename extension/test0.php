<?php

btp_configure("./phpbtp-client.conf");
$id=btp_create_meter2( 1, 0);
btp_release_meter2($id, "phpscript2", "phpservice2", "phpserver2", "op2", 0);
$id=btp_create_meter2( 1, 0);
btp_release_meter2($id, "phpscript2", "phpservice2", "phpserver2", "op3", 0);
$id=btp_create_meter2( 1, 0);
btp_release_meter2($id, "phpscript2", "phpservice2", "phpserver2", "op4", 0);
$id=btp_create_meter2( 1, 0);
btp_release_meter2($id, "phpscript2", "phpservice2", "phpserver2", "op5", 0);

?>
