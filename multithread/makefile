obj = main.o connect_serv.o doit.o str_echo.o thread_handle.o
server_multithread : $(obj)
	cc -o server_multithread $(obj) -lpthread

main.o : main.c main.h
	cc -c main.c

connect_serv.o : connect_serv.c connect_serv.h
	cc -c connect_serv.c

doit.o : doit.c doit.h
	cc -c doit.c

str_echo : str_echo.c str_echo.h
	cc -c str_echo.c

thread_handle : thread_handle.c thread_handle.h
	cc -c  thread_handle
	
clean :
		rm $(obj) server_multithread
