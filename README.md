# Compilar

    make IPC=<ipc>

Donde `<ipc>` es uno de `FILESIG`, `SOCKETS`, `SHMEM`, `MQUEUES` Y `FIFOS`.


# Crear la base de datos

    bin/reset db fixtures.txt


# Servidor

    bin/server <dirección ipc>

`<dirección ipc>` es:

- `FILESIG`, `FIFOS`: path de un directorio para archivos temporales (ej `tmp`)
- `SOCKETS`: número de puerto
- `SHMEM` y `MQUEUES`: archivo cualquiera, legible


# Cliente

    bin/client <dirección ipc> <comando> [argumentos...]

`<dirección ipc>` es:

- `FILESIG`, `FIFOS`: `<tmp>/<pid server>`, siendo `<tmp>` el mismo del server
- `SOCKETS`: una dirección en formato `<ip>:<puerto>` (ej 127.0.0.1:5000)
- `SHMEM` y `MQUEUES`: un archivo cualquiera, legible (el mismo)


`<comando>` es uno de:

1. `list`: muestra las películas
2. `buy <número de película>`: compra una entrada
3. `get <código de entrada>`: consulta una entrada