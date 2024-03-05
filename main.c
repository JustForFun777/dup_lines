#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <unistd.h>
#include <getopt.h>
extern char *optarg;
extern int optind, opterr, optopt;

#define VOCABULARY 256 //all char combinations
#define MAX_STRING_LENGTH 600
#define MAX_BUFFER_SIZE MAX_STRING_LENGTH*100000

//#define DEBUG
//#define PERFOMANCE_DEBUG_LOG

#ifdef DEBUG
	#define LOG printf
#else
	#define LOG(...)
#endif

#ifdef PERFOMANCE_DEBUG_LOG
	#define LOG_PERFOMANCE printf
#else
	#define LOG_PERFOMANCE(...)
#endif

#define bool char
#define FALSE 0
#define TRUE 1

enum operation
{
	UNDEF,
	DUPLICATE,
	DUPLICATE_PLUS,
	UNIQUE
};

const int root = 0;
unsigned int free_pos=1;
int *term;
int **go;

char *buffer;

int add_word(char *, char*, unsigned int*, unsigned int*);
bool add_and_test_word(char *s);

//Функция добавляет новую ветку в дерево и устанавливает индекс наличия слова term[cur]
//
// В дереве вершинам дается порядковый номер (по номеру строки массива go)
// Каждое ребро имеет вес. Вес это номер символа (столбцы массива).
// В качестве данных храним номер строки в которую будет осуществляться переход.
// По сути это не совсем дерево, не ведется поиск минимального веса ребра. 
// Нам нужно ребро с соответсвующим весом, для перехода к следующему символу.
// Каждая строка содержит номер отличный от нуля только в одном элементе.
// Тем самым обеспечивается точный ответ об окончании слова.
int add_word(char *s, char* buffer, unsigned int* current_position, unsigned int* string_marks)
{
	int cur = root; //текущая позиция в массиве
	int pos = 0; // позиция в проверяемой строке
	LOG("Word=%s\n", s);
	size_t str_size = strlen(s);
	while (pos < str_size) // проходим по всем символам строки
	{
		// получаем номер символа
		int c = s[pos];
		// если ячейка символа в матрице не занята, заполняем ее переходом на новую строку
		if (go[cur][c] == 0) 
		{
			go[cur][c] = free_pos++;
			LOG("free_pos = %u\n", free_pos);
		}
		LOG("go[cur][c]=%d cur=%d c=%d char=%c\n", go[cur][c], cur, c, c);
		// присваиваем переменной cur номером строки для перехода  
		cur = go[cur][c];
		// берем следующий символ
		pos++;
	}
	// заполняем массив term флагом об окончании слова
	term[cur]++; //может быть больше одного слова
	LOG("term[cur] = term[%d] = %d\n", cur, term[cur]);
#ifdef DEBUG_BIG
	printf("STRING BUFFER:\n");
	for (int i=0; i<=*current_position; i++)
	{
		printf("Buffer element %d (%p) = %c(%d)\n", i, &(buffer[i]), buffer[i], buffer[i]);
	}
#endif
	
	return 0;
}

// добавление слова и заполнение списка уникальных
int add_word_unique(char *s, char* buffer, unsigned int* current_position, unsigned int* string_marks)
{
	int cur = root; //текущая позиция в массиве
	int pos = 0; // позиция в проверяемой строке
	LOG("Word=%s\n", s);
	size_t str_size = strlen(s);
	while (pos < str_size) // проходим по всем символам строки
	{
		// получаем номер символа
		int c = s[pos];
		// если ячейка символа в матрице не занята, заполняем ее переходом на новую строку
		if (go[cur][c] == 0) 
		{
			go[cur][c] = free_pos++;
			LOG("free_pos = %u\n", free_pos);
		}
		LOG("go[cur][c]=%d cur=%d c=%d char=%c\n", go[cur][c], cur, c, c);
		// присваиваем переменной cur номером строки для перехода  
		cur = go[cur][c];
		// берем следующий символ
		pos++;
	}
	// заполняем массив term флагом об окончании слова
	term[cur]++; //может быть больше одного слова
	LOG("term[cur] = term[%d] = %d\n", cur, term[cur]);
	if (term[cur] == 1)
	{
		// заполняем буфер уникальных строк
		//(*current_position)++;
		LOG("current_position = %d\n", *current_position);
		if ((str_size + *current_position) > MAX_BUFFER_SIZE)
		{
			printf("ERROR buffer size.");
			return 1;
		}
		pos = 0;
		string_marks[free_pos - 1] = *current_position;
		while (pos <= str_size) //nul-terminator too
		{
			buffer[*current_position] = s[pos];
			(*current_position)++;
			pos++;
		}
	}
#ifdef DEBUG_BIG
	printf("STRING BUFFER:\n");
	for (int i=0; i<=*current_position; i++)
	{
		printf("Buffer element %d (%p) = %c(%d)\n", i, &(buffer[i]), buffer[i], buffer[i]);
	}
#endif
	
	return 0;
}

//Модификация add_word предназначена для добавления и поиска дубликатов строк
//Оптимизированная функция не входящая в обычный набор Префикс-бора
//Данная функция осуществляет добавление нового слова в дерево.
//Но в случае если слово уже было добавлено ранее возвращает код 1
//Если слово отсутствует в дереве то оно просто добавляется
bool add_and_test_word(char *s)
{
	int cur = root; //текущая позиция в массиве
	int pos = 0; // позиция в проверяемой строке
	int test = FALSE; // тестовая переменная для обозначения наличия дубликата слова
	size_t str_size = strlen(s);
	while (pos < str_size) // проходим по всем символам строки
	{
		// получаем номер символа
		int c = s[pos];
		// если ячейка символа в матрице не занята, заполняем ее переходом на новую строку
		if (go[cur][c] == 0) 
		{
			go[cur][c] = free_pos++;
		}
		// присваиваем переменной cur номером строки для перехода 
		cur = go[cur][c];
		// берем следующий символ
		pos++;
	}
	// если есть признак окончания слова записываем в test 1
	if (term[cur] == 1)
	{
		test = TRUE;
	}
	// заполняем массив term флагом об окончании слова
	term[cur]++;
	// возвращаем test -результат есть дубликат или нет
	return test;
}

void show_man()
{
	printf("HELP\n");
	printf("Utilite show duplicate lines from the file\n");
	printf("Options:\n");
	printf("\t h: Show this help\n");
	printf("\t f: filepath\n");
	printf("\t d: show duplicate lines (without count)\n");
	printf("\t u: show unique lines \n");
	printf("\t p: show duplicate lines (with count, second iteration) - not ready\n");
}

bool check_and_set_operation(enum operation *op, enum operation set_op)
{
	if (*op == UNDEF)
	{
		*op = set_op;
		return 0;
	}
	else
	{
		printf("Choose only one option 'd' or 'p' or 'u'\n");
		return 1;
	}
}

bool check_and_set_options(int argc, char *argv[], enum operation *op, char **path_file)
{
	LOG("Start checking parameters\n");
	if (argc==1)
	{
		show_man();
		return 0;
	}

	char *opts = "f:duhp";
	int opt;
	while((opt = getopt(argc, argv, opts)) != -1)
	{
		switch(opt)
		{
			case 'f':
			{
				*path_file = optarg;
				LOG("File path = '%s'", *path_file);
			}
			break;
			case 'd':
			{
				if (check_and_set_operation(op, DUPLICATE))
				{
					return 1;
				}
			}
			break;
			case 'p':
			{
				if (check_and_set_operation(op, DUPLICATE_PLUS))
				{
					return 1;
				}
			}
			break;
			case 'u':
			{
				if (check_and_set_operation(op, UNIQUE))
				{
					return 1;
				}
			}
			break;
			case 'h': show_man(); return 0;
			default: break;
		}
	}

	if (*op == UNDEF)
	{
		printf("Choose one these options: 'u', 'p', 'd'\n");
		return 1;
	}

	return 0;
}

long int file_size( FILE *fp )
{
	long int save_pos, size_of_file;
	
	save_pos = ftell(fp);
	fseek(fp, 0L, SEEK_END);
	size_of_file = ftell(fp);
	//set to begin
	fseek(fp, save_pos, SEEK_SET);
	
	return size_of_file;
}

void get_memory(long int data_size)
{
	go = (int**)malloc(data_size * sizeof(&go));
	for (int i = 0; i < data_size; i++)
	{
	    go[i] = (int*)malloc(VOCABULARY * sizeof(go[i]));
	}
	term = (int*)malloc(data_size * sizeof(&term));
}

void free_memory(long int data_size)
{
	for (int i = 0; i < data_size; i++)
	{
	    free(go[i]);
	}
	free(go);
	free(term);
}

#ifdef PERFOMANCE_DEBUG_LOG
inline unsigned long long rdtscl(void)
{
	unsigned int lo, hi;
	__asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
	return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}
#endif

int main(int argc, char *argv[])
{
	LOG("Start program\n");
#ifdef PERFOMANCE_DEBUG_LOG
	unsigned long long start_alg = rdtscl();
#endif
	char *path_file;
	char str[MAX_STRING_LENGTH];
	enum operation op = UNDEF; 
	
	if (check_and_set_options(argc,argv, &op, &path_file))
	{
		return 1;
	}

	FILE *test_file = fopen(path_file,"r");
	long int data_size;
	if (test_file == NULL)
	{
		printf("The file does not exist\n");
		return 1;
	}
	else
	{
		data_size = file_size(test_file);
		LOG("File is opened. Size is %ld\n", data_size);
	}
	get_memory(data_size);

	LOG("Parameters were checked\n");

	if (op == DUPLICATE)
	{
		printf("Duplicate lines:\n");

		bool test = FALSE;
		while( fgets(str, MAX_STRING_LENGTH, test_file) != NULL)
		{
			test = add_and_test_word(str);
			if (test)
			{
				printf("%s", str);
			}
		}
	}
	else if (op == UNIQUE)
	{
		unsigned int current_position = 0;
		unsigned int strings_marks[data_size];
		bool test = FALSE;
		buffer = (char*)malloc(MAX_BUFFER_SIZE * sizeof(char));
		while (fgets(str, MAX_STRING_LENGTH, test_file) != NULL)
		{
			test = add_word_unique(str, buffer, &current_position, strings_marks);
			if (test == 1)
			{
				free(buffer);
				free_memory(data_size);
				return 1;
			}
		}
		unsigned int pos = 0;
		printf("Unique lines:\n");
		while (pos <= free_pos)
		{
			if (term[pos] == 1)
			{
				LOG("strings_marks[pos] = strings_marks[%d] = %d term[%d] = %d: ", pos, strings_marks[pos], pos, term[pos]);
				printf("%s", &(buffer[strings_marks[pos]]));
			}
			else
			{
				LOG("strings_marks[pos] = strings_marks[%d] = %u term[%d] = %d\n", pos, strings_marks[pos], pos, term[pos]);
			}
			pos++;
		}
#ifdef DEBUG
		printf("STRING BUFFER:\n");
		for (int i=0; i<=current_position; i++)
		{
			printf("Buffer element %d (%p)= %c(%d)\n", i, &(buffer[i]), buffer[i], buffer[i]);
		}
#endif
		free(buffer);
	}
	LOG("Count blocks with size=%d: %d\n", VOCABULARY, free_pos);
	
#ifdef PERFOMANCE_DEBUG_LOG
	unsigned long long end_alg = rdtscl();
	LOG_PERFOMANCE("PERFOMANCE_TEST=%lu\n", end_alg - start_alg);
#endif
	free_memory(data_size);
	return 0;
}
