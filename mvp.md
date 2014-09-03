# MVP

Única herramienta, no hay cliente/servidor, sólo una CLI.

1. Fixtures: carga de datos estáticos
2. Database: archivo con datos dinámicos, manipulación y locking
3. Interfaz: CLI en modo Dios


# Fixtures

- void fixtures_read(char *db_path, char *fixtures_path);


# Database
    int last_ticket_id;

    struct movie {
        char* title;   // "Matrix"
        char* time;    // "14.30"
        int remaining_tickets;
        int[] tickets; // [1283981274, NULL, NULL, 182937123, NULL...]
    }

- int create_ticket(char* title);
- void delete_ticket(int id);
- ticket_t get_ticket(int id);

*Nota*: usar mmap()


# Interfaz

    $ movie buy "Matrix"
    OK 1824982734

    $ movie buy "Matrix"
    ERR 100 No tickets left

    $ movie buy "Matrixaaaaaaa"
    ERR 101 Matrixaaaaaa is not a movie

    $ movie delete 19283719
    OK

    $ movie delete 19823723
    ERR 200 Ticket doesn't exist

    $ movie get 2984293
    2984293
    Matrix
    14.30

    $ movie list
    Matrix
    14.30

    Harry Potter
    17.50