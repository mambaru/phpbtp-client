### Обновление 14.02.2023
* В конфигурации **pushout_timer_s** переименован в **pushout_timer_ms**. Соответственно значение нужно указывать в миллисекундах
* Прерывание потока таймера при завершении работы
* Обновлен PHP-CPP до 2.3.5

## Сборка 

```bash
git clone git@github.com:mambaru/phpbtp-client.git
cd phpbtp-client
BOOST_ROOT=/path/to/boost/ make static
cd build/bin
ls -l
```

В случае успешной сборки в директории build/bin:
```bash
Makefile
phpbtp-client.conf
phpbtp-client.ini
phpbtp-client.so
test.php
```

Эти файлы нужно перенести на целевую машину и выполнить `make install`

Требования:
* g++ 4.9 и выше (до g++-9, на g++-10 есть проблемы (январь 2021))
* git 
* cmake 3.10 и выше
* boost-1.76 и выше собраный с ключами `./b2 link=static runtime-link=static`
* php7
