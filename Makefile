cr:
	clear
	gcc main.cpp tree.cpp libs/baselib.cpp libs/file_funcs.cpp -lstdc++ -o main.out
	./main.out db/data.txt 1

c:
	gcc main.cpp tree.cpp libs/baselib.cpp libs/file_funcs.cpp -lstdc++ -o main.out

r:
	./main.out db/data.txt 1


gcr:
	clear
	gcc game/akinator.cpp tree.cpp libs/baselib.cpp libs/file_funcs.cpp -lstdc++ -o game/game.out
	./game/game.out

gc:
	gcc game/akinator.cpp tree.cpp libs/baselib.cpp libs/file_funcs.cpp -lstdc++ -o game/game.out

gr:
	./game/game.out

