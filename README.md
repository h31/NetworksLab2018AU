# NetworksLab2017

## Elegram

#### Запуск сервера

Аргументы командной строки:

* Номер порта

#### Запуск клиента

Аргументы командной строки:

* Адрес сервера

* Номер порта

* Логин

#### Прикладной протокол

####  Клиент

Формат сообщения клиента:

* Четыре байта - длина логина(в байтах) + '\0'. Максимальная длина логина 2^32.

* Далее - логин

* Четыре байта - длина сообщения(в байтах) + '\0'. Максимальная длина сообщения 2^32.

* Далее - сообщение

Логин передается в каждом сообщении к серверу.

#### Сервер

Формат ответа сервера:

* Четыре байта - часы

* Четыре байта - минуты

* Четыре байта - длина логина(в байтах) + '\0'. Максимальная длина сообщения 2^32.

* Далее - логин

* Четыре байта - длина сообщения(в байтах) + '\0'. Максимальная длина сообщения 2^32.

* Далее - сообщение
