CPP = g++

COMMON_DEPS = ast.h \
							common.h \

CC_DEPS = cc.cpp    \
          c.tab.cpp \
          c.lex.cpp \
          ast.cpp   \
          $(COMMON_DEPS)

CC_LIBS = ast.cpp   \
          c.tab.cpp \
          c.lex.cpp \
          cc.cpp    \


BISON_DEPS = c.y   \
             $(COMMON_DEPS)

FLEX_DEPS = c.l       \
            c.tab.cpp \
            $(COMMON_DEPS)

cc: $(CC_DEPS)
	$(CPP) $(CC_LIBS) -lm -ll -lfl -o $@

c.tab.cpp c.tab.hpp: $(BISON_DEPS)
	bison -o c.tab.cpp -d c.y

c.lex.cpp: $(FLEX_DEPS)
	flex -o c.lex.cpp -l c.l

clean::
	rm -f c.tab.cpp c.tab.hpp c.lex.cpp cc c.output
