obj = main.o connect_serv.o io.o myselect.o
server_select : $(obj)
	cc -o server_select $(obj)

main.o : main.c main.h
	cc -c main.c

connect_serv.o :connect_serv.c connect_serv.h
	cc -c connect_serv.c
	
io.o : io.c io.h
	cc -c io.c
	
myselect.o :myselect.c myselect.h
	cc -c myselect.c

clean :
	rm $(obj) erver_select