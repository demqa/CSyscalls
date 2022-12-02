/*

SIGNALS:

1, 2, 3 - номера исходов для поступившего сигнала.

1. SIGNAL IGNORE      // просто игнорируется
2. SIGNAL TERMINATE   // завершение
SIGNAL INTERUPTION
SIGNAL SEGMENTATION FAULT -> ABORT

SIG_DFL
SIG_IGN

действие по умолчанию нельзя поставить на TERMINATE

core - главная память

core - дамп главной памяти

подсистема abrt
процесс
subsystem abrt

3. обработчик (Action)
просто функция.

он не является обработчиком по умолчанию

syscall - signal(1, function); // n - номер сигнала, function - функция
возвращает старое значение обработчика

в результате

int previous_proceeder = signal(number, function);

// ... code ...

signal(number, previous_proceeder);

syscall может быть прерван с помощью SIG_INT например.
Будет вызван обработчик сигнала. Либо кастомный, либо обычный.

Можно программно отправить процессу сигнал. kill. чтобы убить себя: kill(getpid());
kill - не только терминировать процесс. Такие сигналы называются ненадёжными.

Сигналы не выстраиваются в очередь, а образуют какую-то битовую маску. Pending signals.
Поэтому если упало больше 1 сигнала, то они просто схлопнутся в один.

пронумерованы по номерам signal'a. маска какая-то - 64 или 128 бит в зависимости от реализации.
сигналы могут исполняться в любом порядке, но зачастую приоритет по младшему биту.

Сигнал может быть проигнорирован, если что-то блокирующее в цикле может быть.

Сигнал написали двумя разными способами и там не совсем понятно, что там есть...




Есть mycat

алгоритм который читает и пишет

разбиваем на два процесса.
поток передачи данных хотим организовать с помощью сигналов.
по битам можно...

USRD1
USRD2

но сигналы не буфферизуются, следовательно нужно как-то это контролировать.

нужно получить бит, и после этого с помощью сигнала сказать другому процессу, что он получен и т.д.

pause - ждём сигнала, или блокируется.

race condition

следовательно весь алгоритм должен получится в обработчике сигнала

группы сигналов

1. клавиатурные
SIGINT
SIGQUIT - Ctrl + \. Скидывает core.
SIGKILL - только программно (есть команда kill). Нельзя поставить кастомный обработчик на kill. Всегда терминирует.
SIGSTOP - не может быть проигнорирован и не может быть кастомный. Приостанавливает работу процесса.
SIGCONT - не может быть проигнорирован и не может быть кастомный. Продолжает       работу процесса.
2. cpu signal
SIGSEGV - когда есть регион памяти связанный с процессом, но смещение некорректно. Когда отсутствует страница.
SIGBUS  - когда отсутствует даже корректный сегмент.
SIGFPE  - floating point exception
SIGZERO
3. user signal
SIGUSR1
SIGUSR2
4. kernel signal
SIGCLD - посылается всегда, когда завершается дочерний
будет зомби, если самому не обработать. Альтернативный вариант обработки убийства ребенка
SIGPIPE - default: terminate. Можно в программах типа shell в нашей
конвеерной понять, что когда один процесс, который читает из пайпа, умер например.
SIGALRM

SIGSUSPEND. можем забанить обработку сигналов.

sigemptyset
sigfillset
sigdelset
sigaddset

API для генерации масок

SIGACTION

надёжные флаги: указатель на функцию, маски и т.д.

флаг системные вызовы прерывать или нет
имеет смысл для тех сигналов, которые постоянно приходят (SIGCLD или SIGPIPE)
с флагом SA_RESTART системного вызова.

SIGWAIT

*/
