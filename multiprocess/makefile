server_multiprocess : main.o connect_serv.o str_echo.o signal_handle.o
	cc -o  server_multiprocess main.o connect_serv.o str_echo.o signal_handle.o

main.o : main.c main.h
	cc -c main.c

connect_serv.o : connect_serv.c connect_serv.h
	cc -c connect_serv.c

str_echo.o : str_echo.c  str_echo.h
	cc -c str_echo.c

clean:
	rm main.o connetc_serv.o str_echo.o signal_handle.o server_multiprocess
