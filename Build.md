### Обновление 14.02.2023
* В конфигурации **pushout_timer_s** переименован в **pushout_timer_ms**. Соответственно значение нужно указывать в миллисекундах
* Прерывание потока таймера при завершении работы
* Обновлен PHP-CPP до 2.3.5

# Установка

Собранные расширения для Debian лежат в папке 

```bash 
repobuild4:/monamour/phpbtp-client/
```

|        |        |
| ------ | ------ |
| phpbtp-client-release.tar.gz | Текущий релиз |
| phpbtp-client-pre-release.tar.gz | Предрелизная версия с отладочной информацией | 
| phpbtp-client-devel.tar.gz | Рабочая версия для отладки | 
| phpbtp-client-wip.tar.gz | Текущая версия в разработке  | 

Скачиваем и устанавливаем:

```bash
scp repobuild4:/monamour/phpbtp-client/phpbtp-client-release.tar.gz .
tar -xvf phpbtp-client-release.tar.gz
sudo make install
```

В архиве уже собранная либа phpbtp-client.so, make-файл просто копирует ее в директорию расширений php. 
Также копирует phpbtp-client.ini файл для php и JSON-конфигурацию phpbtp-client.conf в директорию 
/etc/php7/conf.d. Полный путь к файлу конфигурации нужно будет передать при вызове btp_configure.
На данном этапе phpbtp-client.conf сконфигурирован на ```Серевер:devel``` (dd2: 192.168.2.13)

Чтобы проверить:
```bash 
php test.php
sleep 1 
php test.php
```
Чтобы данные появились на графиках нужно запустить пару раз (см. график phpservice1 на ```Серевер:devel``` в btp) и работает пока только 5-секундный график

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
