server.out : server.o serv_functions.o
	gcc -Wall server.o serv_functions.o -o server.out -lpthread
server.o : server.c serv_define.h
	gcc -c -Wall server.c -o server.o -lpthread
serv_functions.o : serv_functions.c serv_define.h
	gcc -c -Wall serv_functions.c -o serv_functions.o -lpthread
clean :
	rm -f *.o 
