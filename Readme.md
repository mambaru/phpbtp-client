# php-btp-pusher

Extension для PHP на C++, которое отправляет метрики в BTP по UDP оптимизируя исходящий трафик.

Перед компиляцией нужно установить PHP-CPP
```bash
 git clone https://github.com/CopernicaMarketingSoftware/PHP-CPP.git
 cd PHP-CPP
 make
 sudo make install
```

Компиляцию и установку как расширения PHP нужно производить из папки deploy проекта: 
```bash
 cd deploy
 make
 sudo make install
```

