build: editor.c
	gcc -Wall editor.c -o editor -g
run: editor.c
	./editor
clean:
	rm -rf editor
check: editor.c
	valgrind --tool=memcheck --leak-check=full --leak-check=full --show-leak-kinds=all ./editor
