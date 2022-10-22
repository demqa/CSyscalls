/*

INTERPROCESS COMMUNICATION

SYSTEM V IPC

message queue
semaphores
shared memory

They are not in filesystem.
We can get it by key. It is not linked with filesystem.

Some mechanism of generation random keys.
i_node -> key

ftok(path, num); // file to key.
     ^-----^----- some operation that gives us a key.

PIPE (FIFO)
PIPE doesnt have a name,
so somebody third cannot read or write from PIPE.

MSGQ
id = msgget(key, flags)
// FLAGS: IPC_CREAT, IPC_EXCL (exclusive message queue).
//        IPC_RWX,
//        for user, group, and others.

Can construct new object if it not exists,
other calls can only get id. // like fd
// id наследуется when fork process
// также можно просто получить

semget
shmget

msgget(key, IPC_CREAT | 0666);

open  (path, O_CREAT  | 0666); // O_READ | O_WRITE

key = IPC_PRIVATE // will create new unique key

then we can get ket from id with some syscall

msgoff(id, op, value);

OPERATIONS:
IPC_RMID - remove identifier. Value parameter will be ignored.

MESSAGE QUEUE:
FIFO OBJECT.
We have some slots for data. Size can be different.
Can get in FIFO or by index.

Every struct contains:

struct Some {
    long type; // type MUST be > 0
 // other data
}

send, receive.

msgsnd(id, buf, size,       flags);
msgrcv(id, buf, size, type, flags); // type == 0 - any type
We do buffer ourselves. And writes to buffer long type;

Описать эстафету бегунов которые передают друг другу палочку.

Стадион. Изначально пустой. Судья.
Бегуны: runners - [1, 2, 3, 4];
Каждый должен сделать полный круг и отдать палочку следующему.
Судья мерит время каждого и общее время.

отдельные процессы:
судья, бегун1, бегун2

judge()
{
  //
  send_brick();
}

runner(int id)
{
  // Сначала они говорят, что они пришли на стадион.
  // Только потом судья отдает палочку первому
}

// I'm runner #1:
//
//
//
//
//
//
//



*/
