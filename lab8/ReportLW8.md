# Отчет по лабораторной работе № 8
## по курсу "Операционные системы"

Студент группы М8О-208Б-23 Федорова Екатерина Васильевна

Работа выполнена 

1. **Тема**: Диагностика ПО      
2. **Цель работы**:  
   - Приобретение практических навыков диагностики работы программного обеспечения   
   
3. **Задание**: При выполнении лабораторных работ по курсу ОС необходимо продемонстрировать ключевые системные вызовы, которые в них используются и то, что их использование соответствует варианту ЛР.  
По итогам выполнения всех лабораторных работ отчет по данной ЛР должен содержать краткую сводку по исследованию написанных программ.   
Средства диагностики:   
    Для ОС Windows:   
        ● Windbg   
            - http://windbg.info/doc/1-common-cmds.html   
        ● Sysinternals Suite   
            - Handle.exe   
            - Procmon.exe   
            - Procexp.exe   
    Для ОС *nix:   
        ● strace   

**Описание используемых утилит:**   
Strace — это утилита Linux, отслеживающая системные вызовы, которые представляют собой механизм трансляции, обеспечивающий интерфейс между процессом и операционной системой. Использование данной утилиты позволяет понять, что процесс пытается сделать в данное время. Strace может быть полезен при отладке программ.  
Для удобства работы с протоколом утилиты можно использовать следующие ключи:  
    -o file – Перенаправить протокол утилиты в файл file  
    -e trace=filters – Указать выражения, по которым будут фильтроваться системные вызовы. Например -e trace=write,%process задаёт фильтрацию по системным вызовам write и по группе системных вызовов, связанных с межпроцессорным взаимодействием.   
    -f – Отслеживать системные вызовы в дочерних процессах   
    -y – Заменить в протоколе все файловые дескрипторы на имена соответствующих им файлов (где возможно).   
    -p file – Отслеживать только обращения к файлу file   
    -k – Отображать стек вызовов    
Ltrace – это утилита Linux, отслеживающая системные вызовы, связанные с динамическими библиотеками. Использование данной утилиты очень похоже на использование strace.   

Рассмотрим на примере Лабораторной работы №1.

4. **Листинг диагностики**:
```
moscow@moscow:~/Рабочий стол/OS/OS_2k/build$ strace -f -e trace="%process,read,write,dup2,pipe" -o ../lab8/log_lab1.txt ./lab1/parent
asdfgrh
12345ng
asdptk
Rrraaaaadio, Oh, mein Radio!
```
```
moscow@moscow:~/Рабочий стол/OS/OS_2k/build$ strace -f -e trace="%process,read,write" -o ../lab8/log_lab2.txt ./lab2/monte_carlo 10 1000000
Total matches: 67906 of 1000000 rounds.
The probability that there are two identical cards on the top: 0.067906
Number of threads: 10
Number of rounds: 1000000
Execution time: 0.048193 seconds
```
```
moscow@moscow:~/Рабочий стол/OS/OS_2k/build$ strace -f -e trace="%process,read,write,dup2,mmap" -o ../lab8/log_lab3.txt ./lab3/parent
Eins! Hier kommt die Sonne,
Zwei! Hier kommt die Sonne,
1234...
Wie shön Du bist...
```
```
moscow@moscow:~/Рабочий стол/OS/OS_2k/build/lab4$ ltrace -o ../../lab8/log_lab4.txt ./prog_2
Library 1 is loaded by default.
1 15 3
GCF: 3
2 10
In the 2NS is 1010
0
Library 2 is loaded.
1 15 3
GCF: 3
2 10
In the 3NS is 101 
```
```
moscow@moscow:~/Рабочий стол/OS/OS_2k/build$ strace -o ../lab8/log_lab57.txt ./bin/control_node
> create 5
Ok: 19403
> create 6
> Ok: 19421
> create 3
> Ok: 19445
> tree
5
/--3
\--6
> exec 3
> asdfs
> s
> Ok:3: 1;4
> kill 19421
Ok: Process 19421 killed
> ping 5
> Ok: 1
> ping 3
> Ok: 1
> ping 6
> Ok: 0
> exit
```

5. **Результаты диагностики**: [lab1](log_lab1.txt), [lab2](log_lab2.txt), [lab3](log_lab3.txt), [lab4](log_lab4.txt), [lab57](log_lab57.txt)
6. **Анализ log_lab1.txt (основные моменты)**:   


7. **Вывод:** 