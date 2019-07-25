#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct cursor {
	int line;
	int character;
} *Cursor;

typedef struct symbol {
	int character;
	struct symbol* next;
	struct symbol* prev;
} *Symbol;

typedef struct text_beginning {
	char line[500];
	struct text_beginning* next_text;
	struct text_beginning* next_line;
} *TextBeginning;

typedef struct command {
	char value[100];
	struct text_beginning* text;
	struct command* next;
} *Command;

typedef struct stack {
	Command top;
	int size;
} *Stack;

typedef struct queue {
	Command head, tail;
	int size;
} *Queue;

Cursor initCursor(){
	Cursor new = malloc(sizeof(struct cursor));
	new->line = 1;
	new->character = 1;
	return new;
}

Symbol initSymbol(int val) {
	Symbol s;
	s = malloc(sizeof(struct symbol));
	s->character = val;
	s->next = NULL;
	s->prev = NULL;
	return s;
}

Symbol addSymbol(Symbol s, int val) {
	Symbol new = initSymbol(val);
	if (s == NULL)
		return new;
	Symbol temp = s;
	while (s->next != NULL)
		s = s->next;
	s->next = new;
	new->prev = s;
	return temp;
}

TextBeginning initText(char *val) {
	TextBeginning new = malloc(sizeof(struct text_beginning));
	strcpy(new->line, val);
	new->next_text = NULL;
	new->next_line = NULL;
	return new;
}

TextBeginning addText(TextBeginning t, char *val, Stack *st) {
	TextBeginning new = initText(val), temp = t;
	(*st)->top->text = new;
	if (t == NULL)
		return new;
	while (t->next_text != NULL)
		t = t->next_text;
	t->next_text = new;
	return temp;
}

TextBeginning addLine(TextBeginning t, char *val) {
	TextBeginning new = initText(val), temp = t;
	while (t->next_line != NULL)
		t = t->next_line; 
	t->next_line = new;
	return temp;
}

Command initCommand(char *val) {
	Command new = malloc(sizeof(struct command));
	strcpy(new->value, val);
	new->next = NULL;
	return new;
}

Stack initStack(char *val) {
	Stack new_stack = malloc(sizeof(struct stack));
	new_stack->top = initCommand(val);
	new_stack->size = 1;
	return new_stack;
}

int isEmptyStack(Stack stack) {
	if (stack == NULL)
		return 1;
	if (stack->top == NULL)
		return 1;
	if (stack->size > 0)
		return 0;
	else
		return 1;
}

Stack push(Stack stack, char *val) {
	if (isEmptyStack(stack))
		return initStack(val);
	Command new = initCommand(val);
	new->next = stack->top;
	stack->top = new;
	stack->size++;
	return stack;
}

Stack pop(Stack stack) {
	if (!isEmptyStack(stack)) {
		Command to_delete = stack->top;
		stack->top = stack->top->next;
		free(to_delete);
		stack->size--;
	}
	return stack;
}

char* top(Stack stack) {
	if (isEmptyStack(stack))
		return NULL;
	return stack->top->value;
}

Stack copyStack(Stack source) {
	if (source == NULL)
		return NULL;
	Stack dest = initStack(top(source));
	if (strcmp(top(source), "::i") == 0)
		dest->top->text = source->top->text;
	source = pop(source);
	while (!isEmptyStack(source)) {
		dest = push(dest, top(source));
		if (strcmp(top(source), "::i") == 0)
			dest->top->text = source->top->text;
		source = pop(source);
	}
	return dest;
}

Stack freeStack(Stack stack) {
	while (!isEmptyStack(stack))
		pop(stack);
	return stack;
}

Queue initQueue(char *val) {
	Queue new_queue = malloc(sizeof(struct queue));
	Command new = initCommand(val);
	new_queue->head = new;
	new_queue->tail = new;
	new_queue->size = 1;
	return new_queue;
}

int isEmptyQueue(Queue queue) {
	if (queue == NULL)
		return 1;
	if (queue->tail == NULL)
		return 1;
	if (queue->size > 0)
		return 0;
	else
		return 1;
}

Queue enqueue(Queue queue, char *val) {
	if (isEmptyQueue(queue))
		return initQueue(val);
	Command new = initCommand(val);
	queue->tail->next = new;
	queue->tail = new;
	queue->size++;
	return queue;
}

Queue dequeue(Queue queue) {
	if (!isEmptyQueue(queue)) {
		Command to_delete = queue->head;
		queue->head = queue->head->next;
		free(to_delete);
	}
	return queue;
}

char* first(Queue queue) {
	if (isEmptyQueue(queue))
		return NULL;
	return queue->head->value;
}

Queue freeQueue(Queue queue) {
	while (!isEmptyQueue(queue))
		dequeue(queue);
	return queue;
}

void print(Symbol s, char *filename) {
	FILE *f;
	f = fopen(filename, "w");
	if (f == NULL) {
		printf("wrong\n");
		return;
	}
	Symbol aux = s;
	while (aux != NULL) {
		fputc(aux->character, f);
		aux = aux->next;
	}
	fclose(f);
}

void readText(TextBeginning *t, Stack *st) {
	char input[500];
	fgets(input, 500, stdin);
	input[strlen(input) - 1] = '\0';
	if (strncmp(input, "::i", 3) != 0) {
		*t = addText(*t, input, st);
		fgets(input, 500, stdin);
		input[strlen(input) - 1] = '\0';
	}
	while (strncmp(input, "::i", 3) != 0) {
		(*st)->top->text = addLine((*st)->top->text, input);
		fgets(input, 500, stdin);
		input[strlen(input) - 1] = '\0';
	}
}

int readCommands(Stack *st, Queue qu, int *commands_cnt) {
	char input[100]; int redo = 0;
	fgets(input, 100, stdin);
	input[strlen(input) - 1] = '\0';
	while (1) {
		switch(input[0]) {
			case 'q':
				if ((*commands_cnt) == 4)
					return 2;
				else
					return 1;
			case ':':
				return 0;
			case 'u':
				if (strcmp(top(*st), "s") != 0 && !isEmptyStack(*st)) {
					qu = enqueue(qu, top(*st));
					*st = pop(*st);
				}
				redo = 1;
				break;
			case 'r':
				if (redo && !isEmptyQueue(qu)) {
					*st = push(*st, first(qu));
					qu = dequeue(qu);
				}
				redo = 0;
				break;
			default:
				*st = push(*st, input);
				redo = 0;
				break;
		}
		if (strcmp(input, "s") != 0)
			(*commands_cnt)++;
		if ((*commands_cnt) == 5) {
			*st = push(*st, "s");
			(*commands_cnt) = 0;
		}
		fgets(input, 100, stdin);
		input[strlen(input) - 1] = '\0';
	}
}

Symbol insertText(Symbol s, TextBeginning t, Cursor *c) {
	int i = 1;
	Symbol temp = s;
	Symbol aux = NULL;
	TextBeginning t1 = t;
	if (s != NULL && ((*c)->line > 1 || (*c)->character > 1)) {
		while (i < (*c)->line) {
			if (s->character == '\n') {
				if (i == (*c)->line - 1 && (*c)->character == 1)
					break;
				i++;
			}
			s = s->next;
		}
		i = 1;
		if ((*c)->character > 1)
			while (i < (*c)->character - 1) {
				i++;
				s = s->next;
			}
	}
	while (t != NULL) {
		for (i = 0; i < strlen(t->line); i++) {
			aux = addSymbol(aux, t->line[i]);
			(*c)->character++;
		}
		aux = addSymbol(aux, '\n');
		(*c)->line++;
		(*c)->character = 1;
		t = t->next_line;
	}
	t = t1;
	if (s == NULL)
		return aux;
	if (s->prev == NULL) {
		temp = aux;
		while (aux->next != NULL)
			aux = aux->next;
		aux->next = s;
		s->prev = aux;
		return temp;
	}
	aux->prev = s;
	Symbol aux2 = s->next;
	s->next = aux;
	while (aux->next != NULL)
		aux = aux->next;
	aux->next = aux2;
	if (aux2 != NULL)
		aux2->prev = aux;
	return temp;
}

Symbol backspace(Symbol s, Cursor *c) {
	if ((*c)->line == 1 && (*c)->character == 1)
		return s;
	Symbol temp = s, to_delete = NULL;
	int i = 1;
	if ((*c)->character == 1) {
		while (i < (*c)->line) {
			if (s->character == '\n') 
				i++;
			if (s->next == NULL) {
				(*c)->line--;
				to_delete = s;
				s->prev->next = s->next;
				s = s->prev;
				i = 1;
				while (s->character != '\n' && s != NULL) {
					s = s->prev;
					i++;
					if (s == NULL)
						break;
				}
				free(to_delete);
				(*c)->character = i;
				return temp;
			}
			s = s->next;
		}
		s = s->prev;
		(*c)->line--;
		to_delete = s;
		s->prev->next = s->next;
		if (s->next != NULL) 
			s->next->prev = s->prev;
		s = s->prev;
		i = 1;
		while (s->character != '\n') {
			s = s->prev;
			i++;
			if (s == NULL)
				break;
		}
		free(to_delete);
		(*c)->character = i;
	}
	else {
		while (i < (*c)->line) {
			if (s->character == '\n') 
				i++;
			s = s->next;
		}
		i = 1;
		while (i < (*c)->character - 1) {
			s = s->next;
			i++;
		}
		to_delete = s;
		s->prev->next = s->next;
		if (s->next != NULL)
			s->next->prev = s;
		s = s->prev;
		free(to_delete);
		(*c)->character--;
	}
	return temp;
}

Symbol deleteLine(Symbol s, int nr) {
	Symbol temp = NULL, to_delete; int i = 1;
	if (nr > 1)
		temp = s;
	while (i < nr) {
		if (s->character == '\n')
			i++;
		s = s->next;
	}
	if (s->character == '\n') {
		to_delete = s;
		if (s->prev != NULL)
			s->prev->next = s->next;
		if (s->next != NULL)
			s->next->prev = s->prev;
		if (temp == NULL)
			temp = s->next;
		free(to_delete);
		return temp;
	}
	do {
		to_delete = s;
		if (s->prev != NULL)
			s->prev->next = s->next;
		if (s->next != NULL)
			s->next->prev = s->prev;
		s = s->next;
		free(to_delete);
		if (s == NULL)
			return temp;
	} while (s->character != '\n');

	if (s != NULL) {
		if (temp == NULL)
			temp = s->next;
		to_delete = s;
		if (s->prev != NULL)
			s->prev->next = s->next;
		if (s->next != NULL)
			s->next->prev = s->prev;
		free(to_delete);
	}
	return temp;
}

Symbol deleteCharacter (Symbol s, Cursor c) {

	Symbol temp = NULL, to_delete = NULL;
	int i = 1;
	if (c->line == 1 && c->character == 1) 
		temp = s->next;
	else 
		temp = s;
	while (i < c->line) {
		if (s->character == '\n') 
			i++;
		s = s->next;
	}
	i = 1;
	while (i < c->character) {
		s = s->next;
		i++;
	}
	if (s == NULL)
		return temp;
	to_delete = s;
	if (s->prev != NULL)
		s->prev->next = s->next;
	if (s->next != NULL)
		s->next->prev = s->prev;
	free(to_delete);
	return temp;
}

void executeCommands(Stack st, Symbol s, char *filename) {
	Cursor c = initCursor();
	Stack commands = copyStack(st); char *parametru; int nr = 1, i;
	while (!isEmptyStack(commands)) { 
		switch (top(commands)[0]) {
			case ':':
				s = insertText(s, commands->top->text, &c);
				break;
			case 's':
				print(s, filename);
				break;
			case 'b':
				s = backspace(s, &c);
				break;
			case 'd':
				if (top(commands)[1] == 'l') {
					parametru = strtok(top(commands), " ");
					parametru = strtok(NULL, " ");
					if (parametru != NULL)
						nr = atoi(parametru);
					else
						nr = c->line;
					s = deleteLine(s, nr);
					if (nr == c->line)
						c->character = 1;
					if (nr < c->line)
						c->line--;
					nr = 1;
				}
				else {
					parametru = strtok(top(commands), " ");
					parametru = strtok(NULL, " ");
					if (parametru != NULL)
						nr = atoi(parametru);
					for (i = 1; i <= nr; i++)
						s = deleteCharacter(s, c);
					nr = 1;
				}
				break;
			case 'g':
				if (strncmp(top(commands), "gl", 2) == 0) {
					parametru = strtok(top(commands), " ");
					parametru = strtok(NULL, " ");
					c->line = atoi(parametru);
					c->character = 1;
				}
				else {
					parametru = strtok(top(commands), " ");
					parametru = strtok(NULL, " ");
					c->character = atoi(parametru);
					parametru = strtok(NULL, " ");
					if (parametru != NULL)
						c->line = atoi(parametru);
				}
				break;
		}
		commands = pop(commands);
	}
}

int main(int argc, char **argv)
{
	int exit = 0, commands_cnt = 0, a;
	TextBeginning head = malloc(sizeof(struct text_beginning));
	Stack stack = NULL; Queue queue = NULL;
	Symbol symbol = NULL;
	while (!exit) {
		stack = push(stack, "::i");
		readText(&head, &stack);
		a = readCommands(&stack, queue, &commands_cnt);
		if (a) {
			if (a == 2)
				stack = push(stack, "s");
			exit = 1;
		}
	}
	executeCommands(stack, symbol, argv[1]);
	return 0;
}