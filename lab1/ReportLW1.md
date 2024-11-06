# Отчет по лабораторной работе № 1
## по курсу "Операционные системы"

Студент группы М8О-208Б-23 Федорова Екатерина Васильевна

Работа выполнена 

1. **Тема**: Первая программа
2. **Цель работы**:
Приобретение практических навыков в:
- Управление процессами в ОС
- Обеспечение обмена данных между процессами посредством каналов
3. **Задание (вариант №22)**:
Родительский процесс создает два дочерних процесса. Первой строкой пользователь в консоль
родительского процесса вводит имя файла, которое будет использовано для открытия File с таким
именем на запись для child1. Аналогично для второй строки и процесса child2. Родительский и
дочерний процесс должны быть представлены разными программами. Родительский процесс принимает от пользователя строки произвольной длины и пересылает их в
pipe1 или в pipe2 в зависимости от правила фильтрации. Процесс child1 и child2 производят работу над строками. Процессы пишут результаты своей работы в стандартный вывод. Правило фильтрации: с вероятностью 80% строки отправляются в pipe1, иначе в pipe2.
Дочерние процессы инвертируют строки.
4. **Идея, метод, алгоритм решения задачи**: На вход функции `ParentRoutine` подаются пути к исполняемым файлам дочерних профессов и поток ввода. Родительский процесс считывает из него названия файлов, создаёт два дочерних процесса и передаёт им по названию. Также создаются два канала для общения с соответствующими процессами: родительский только на записи, дочерние только на чтение. Родительский процесс продолжает считывать строки из потока и псевдорандомно записывать их в каналы `pipe1` или `pipe2`; дочерние процессы читают переданные по каналу строки, инверсируют их и записывают в выходной файл.

6. **Протокол**: 
Запуск тестов:
```
moscow@moscow:~/Рабочий стол/OS/OS_2k/lab1/build$ make test
Running tests...
Test project /home/moscow/Рабочий стол/OS/OS_2k/lab1/build
    Start 1: lab1_test
1/1 Test #1: lab1_test ........................   Passed    0.03 sec

100% tests passed, 0 tests failed out of 1

Total Test time (real) =   0.03 sec
```

Отдельные запуски вручную:
```
moscow@moscow:~/Рабочий стол/OS/OS_2k/lab1/build$ ./lab1/parent
t1.txt
t2.txt
asd fg
123456
!!oaoao!
Ich kenne nicht, dass mehr schreiben

moscow@moscow:~/Рабочий стол/OS/OS_2k/lab1/build$ cat t1.txt
654321
!oaoao!!
nebierhcs rhem ssad ,thcin ennek hcI

moscow@moscow:~/Рабочий стол/OS/OS_2k/lab1/build$ cat t2.txt
gf dsa
```

7. **Замечания автора** по существу работы: изначально хотела сделать рандом с помощью time, но пришлось отказаться от этой идеи, чтобы облегчить тестирование.
8. **Выводы**: Написать программы для процессов оказалось не так сложно, как затем составить тесты. Мне пришлось просить о помощи одногруппника, чтобы распутал клубок мыслей в моей голове. Писать тесты было одним из самых неприятных задач этой лабораторной работы. В целом поставленную цель считаю выполненной: пусть пока что не глубинно, но я изучила систему сборки CMake, базовые операторы и конструкции C++ и библиотеку для написания Unit-тестов Google Test, а также научилась писать простые программы с несколькими потоками. В будущем буду только совершенствоваться и изучать всё выше перечисленное подробнее.