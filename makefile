CC = gcc
CFLAGS = -ansi -Wall -pedantic
TARGET = assembler
OBJS = assembler.o directives.o encoder.o first_pass.o output_generator.o parser.o pre_assembler.o second_pass.o symbols_manager.o

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGET)