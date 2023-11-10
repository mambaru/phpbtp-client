[![tag](https://img.shields.io/github/v/tag/mambaru/phpbtp-client.svg?sort=semver)](https://github.com/mambaru/phpbtp-client/tree/master)

[![Build Status](https://github.com/mambaru/phpbtp-client/workflows/C++%20CI/badge.svg?branch=master)](https://github.com/mambaru/phpbtp-client/tree/master)
[![Build Status](https://github.com/mambaru/phpbtp-client/workflows/C++%20CI/badge.svg?branch=mambaru)](https://github.com/mambaru/phpbtp-client/tree/mambaru)
[![codecov](https://codecov.io/gh/mambaru/phpbtp-client/branch/master/graph/badge.svg)](https://codecov.io/gh/mambaru/phpbtp-client)

phpbtp-client
=========

Расширение для php7 и php8 которое позволяет более эффективно отправлять данные в демон [btpd](https://github.com/mambaru/btpd)

* [Build](docs-md/build.md)
* [API](docs-md/api.md)

## На github.com

* Репозитарий на [github.com](https://github.com/mambaru/phpbtp-client)
* Документация [doxygen](https://mambaru.github.io/phpbtp-client/index.html)
* Отчет [coverage](https://mambaru.github.io/phpbtp-client/cov-report/index.html)

### Обновление 11.11.2023
* Обновлен PHP-CPP до 2.4.1

### Обновление 14.02.2023
* В конфигурации **pushout_timer_s** переименован в **pushout_timer_ms**. Соответственно значение нужно указывать в миллисекундах
* Прерывание потока таймера при завершении работы
* Обновлен PHP-CPP до 2.3.5
