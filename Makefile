cr:
	clear
	gcc main.cpp tree.cpp libs/baselib.cpp libs/file_funcs.cpp -lstdc++ -o main.out
	./main.out db/data.txt 1

c:
	gcc main.cpp tree.cpp libs/baselib.cpp libs/file_funcs.cpp -lstdc++ -o main.out

r:
	./main.out db/data.txt 1
