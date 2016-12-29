obj = main.o connect_serv.o io.o mypoll.o
server_poll : $(obj)
	cc -o server_poll $(obj)

main.o : main.c main.h
	cc -c main.c

connect_serv.o :connect_serv.c connect_serv.h
	cc -c connect_serv.c

io.o : io.c io.h
	cc -c io.c

mypoll.o :mypoll.c mypoll.h
	cc -c mypoll.c

clean :
	rm $(obj) server_poll
