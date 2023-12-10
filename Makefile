LIBDIR = lib
BINDIR = bin
lib := $(wildcard $(LIBDIR)/*.c)


default: compile_lib
	gcc myshell.c -o myshell

compile_lib:
	$(foreach var,$(lib), gcc $(var) -o $(patsubst $(LIBDIR)/%, $(BINDIR)/%, $(var:.c= ));)
