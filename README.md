task1 - character segments

В приложении к заданию находится дамп базы Redis, содержащей два набора данных:

t:* - список текстовых обозначений (на латинице, заглавными). Ключ имеет формат t:ID, значение - ID|XXX, где XXX - само обозначение, а ID - его идентификатор.

    t:1 = 1|ABRADABRAX
    t:2 = 2|ABRAXANE
    t:3 = 3|AXANE
    ...

c:* - словарь четырёхсимвольных отрезков, которые встречаются в обозначениях. Запросив ключ c:ABCD, мы получим ID всех строк, содержащих в себе подстроку ABCD.

    c:ABRA = set(1,2)
    c:BRAD = set(1)
    c:RADA = set(1)
    c:ADAB = set(1)
    c:BRAX = set(1,2)
    c:RAXA = set(2)
    c:AXAN = set(2,3)
    c:XANE = set(2,3)
    ...

Необходимо написать консольную программу на c++, которая сможет:

1) Принять произвольное слово (образец) из командной строки.
2) Разбить это слово на все возможные четырёхсимвольные отрезки (ABRACADABRA => ABRA, BRAC, RACA, ACAD, CADA, ADAB, DABR).
3) Загрузить из базы все обозначения, где эти отрезки встречаются.
4) Сравнить каждое обозначение с образцом по следующему алгоритму: разбиваем обе строки на трёхбуквенные отрезки (аналогично п.2) и считаем, сколько из них совпало. Например,

    Образец "ABRACADABRA" разбивается на ABR, BRA, RAC, ACA, CAD, ADA, DAB
    Обозначение "ABRADABRAX" разбивается на ABR, BRA, RAD, ADA, DAB, RAX
    Совпало 4 отрезка (ABR, BRA, ADA, DAB)

5) Построчно вывести в консоль ID загруженных знаков и количество совпадений:

    1: 4
    2: 2

