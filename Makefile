CC = gcc

CFLAGS = -Wall -Wextra -std=c17

TARGET = server

SRC = piServ.c serving/getDirectory.c

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: run clean