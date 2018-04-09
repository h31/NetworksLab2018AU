# Protocol
Protocol based on http 1.1 with only GET queries.

## inner protocol
##### 1. Получение от сервера и вывод последних N рассчитанных простых чисел

_клиент - запрос:_
```
GET_LAST_N_QUERY
<N : int>
```

_сервер - ответ:_
```
GET_LAST_N_RESP
<N : int>
[<prime : int> : N] # i.e. N чисел ч\з пробел
```


##### 2. запрос макс. простого числа (клиент, запрос)
п.1 с параметром N = 1


##### 3. Получение от сервера диапазона расчета простых чисел
_сервер - запрос:_
```
COMP_RANGE_QUERY
<bottom (including) : int> <upper (excluding) : int>
```


_клиент - ответ:_
```
COMP_RANGE_RESP
<bottom> <upper>
<N - кол-во простых>
[<prime> : N] # N простых чисел
```
