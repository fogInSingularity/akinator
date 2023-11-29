CC = g++

INCLUDE = -I./libcode/include -I./treecode/include -I./akinatorcode/include
SOURCES = libcode/source/* treecode/source/* akinatorcode/source/* stack/source/*

EXE = akinator
EXE_RELEASE = akinator_rel

FLAGS_GCC = -fdiagnostics-generate-patch -fdiagnostics-path-format=inline-events\
-Og -ggdb -std=c++20 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations   \
-Wc++14-compat -Wuseless-cast -Wmissing-declarations -Wcast-align -Wcast-qual \
-Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy  \
-Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security              \
-Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor       \
-Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self       \
-Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo                     \
-Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn       \
-Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override              \
-Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code         \
-Wunused -Wvariadic-macros -Wno-literal-suffix                                \
-Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast            \
-Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation               \
-fstack-protector -fstrict-overflow -flto-odr-type-merging                    \
-fno-omit-frame-pointer -Wstack-usage=8192                                    \
-fsanitize=address,bool,bounds,enum,float-cast-overflow,$\
float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,$\
object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,$\
undefined,unreachable,vla-bound,vptr

FLAGS_CLANG =

all:
	@$(CC) main.cpp $(SOURCES) $(FLAGS_GCC) -o $(EXE)

clang:
	@clang++ main.cpp $(SOURCES) -Wall -Wextra -o $(EXE)

release:
	@$(CC) main.cpp $(SOURCES) -O3 -march=native -std=c++23 -o $(EXE_RELEASE)

analyze:
	@clang-tidy $(SOURCES) -checks=clang-analyzer-*

performance:
	@clang-tidy $(SOURCES) -checks=performance-*

portability:
	@clang-tidy $(SOURCES) -checks=portability-*

READ_FLAGS = readability-magic-numbers,readability-identifier-length,readability-implicit-bool-conversion,readability-convert-member-functions-to-static

readablility:
	@clang-tidy $(SOURCES) -checks=readability-$(READ_FLAGS)

mkdot:
	@dot dotdump.dot -Tsvg >out.svg
