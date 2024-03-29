Сборка
======

Требования для сборки:
* g++ или clang++ (с поддержкой 17 стандарта)
* cmake (3.10)
* git
* php7 или php8

```sh
sudo apt install g++ cmake git
git clone https://github.com/mambaru/phpbtp-client.git
cd phpbtp-client
make release
```

В процессе сборки будут автоматически подгружены необходимые модули Boost и PHP-CPP, а также подключены необходимые git-субмодули. Сам `phpbtp-client.so` будет собран в статике и не будет зависеть он сторонних `.so`. В случае успешной сборки в директории ./extension:

```sh
Makefile
phpbtp-client.conf
phpbtp-client.ini
phpbtp-client.so
test.php
```

## Установка

```sh
cd ./extension
sudo make install
```

Это сработает только если PHP установлен по стандартным путям, в противном случае нужно в ручную скопировать `phpbtp-client.so` и `phpbtp-client.ini` в необходимые директории для расширений PHP.

Для проверки работоспособности можно запустить:

```sh
php test.php
```

Если не будет ошибок, значит расширенее установлено успешно. Данные отправляются по UDP, поэтому ошибок подключения к серверу btpd не будет. В конфигурации по умолчанию `phpbtp-client.conf` указан адресс `127.0.0.1`, а в примерах конфигурации `btpd` скорее всего `0.0.0.0`. Чтобы  получить данные с тестовго скрипта в конфигурации расширения нужно изменить поля `addr` (или в конфигурации демона `addr` установить в `127.0.0.1`, если предполагается, что он будет работать локально).

## Конфигурация

Подробное описание опций смотрите в комметариях файла конфигурации `phpbtp-client.conf`. Здесь отмечу пару не очевидных моментов.

Демон `btpd` может работать по двум протоколам на разных портах. В примерах конфигураций которые с ним идут это порт `38001` для класического `JSON-RPC` и порт `38000` для так называемого `копактного JSON-RPC`, они не совместимы между собой. Расширение `phpbtp-client.so` может работать с тем или иным, в зависимости от конфигурации. Предоставляемый файл конфигурации работает с компактным вариантом. Опция `"json_compact": true` указывает то, что сообщения нужно отпаравлять в компактном варианте. Если нужен класический `JSON-RCP` то помимо установки `"json_compact": false` нужно также сменить порты на `38001`. В случе если ошибетесь с портами в логах `phpbtp-client.so` (по умолчанию отключены) ошибок вы не увидите, но вы эти ошибки увидите в логах демона `btpd`

Компактный `JSON-RPC` сериализует все структуры `params` в массивы `JSON` где каждый элемент массива соответствует полю структуры, уменьшая таким образом размер сериализованного `JSON`. Это очень не удобно если вы пытаетесь записать или анализировать компактный `JSON` в ручную, но на програмном уровне это очень похоже на бинарные протоколы, с той лишь разницей, что сохраняеться какая-никая читабельность. Библиотека [wjson](https://github.com/mambaru/wjson) которая исползуеться для сериализации как в расширении `phpbtp-client`так и на демоне `btpd` не сильно уступает тому же `protobuf` в производительности по сериализации даже для классического `JSON`, но компактный вариант `JSON` может не уступать `protobuf`по размеру сериализованного сообщения и даже первосходить его частных случаях (например когда передаётся много `0`).
















## Конфигурация

Вся конфигурация в JSON-файле 

## Сборка 

Для ручной сборки на repobuild4 или repobuild3:
```bash
git clone git@gitlab.mamba.ru:cpp/phpbtp-client.git
cd phpbtp-client
BOOST_ROOT=/usr/monamour/boost/ make static
cd build/bin
ls -l
```
Для сборки под CentOS нужно установлить cmake3 и задать переменную CMAKE:
```bash
CMAKE=cmake3 CXX_STANDARD=11 make static
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
* cmake 3.1 и выше
* boost-1.76 (самый свежий) собраный с ключами `./b2 link=static runtime-link=static`
* php7
