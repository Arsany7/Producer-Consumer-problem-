
#Use GNU compiler
CC = g++ -g

all: producer consumer

producer: 
	$(CC) producer_comm.cpp -o producer

consumer: 
	$(CC) consumer_comm.cpp -o consumer	

clean:
	rm -f producer consumer *.o
	ipcrm -a

