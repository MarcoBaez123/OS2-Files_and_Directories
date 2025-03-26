#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<dirent.h>
#include<linux/limits.h>
#include<time.h>
#include<stdbool.h>


void mostrar_permisos(mode_t mode) {
    printf("-%c%c%c%c%c%c%c%c%c",
        (mode & S_IRUSR) ? 'r' : '-',
        (mode & S_IWUSR) ? 'w' : '-',
        (mode & S_IXUSR) ? 'x' : '-',
        (mode & S_IRGRP) ? 'r' : '-',
        (mode & S_IWGRP) ? 'w' : '-',
        (mode & S_IXGRP) ? 'x' : '-',
        (mode & S_IROTH) ? 'r' : '-',
        (mode & S_IWOTH) ? 'w' : '-',
        (mode & S_IXOTH) ? 'x' : '-');
}

void tipoArchivo(mode_t mode){
    if (S_ISREG(mode)) printf("-Archivo regular  -");
    if (S_ISDIR(mode)) printf("-  Directorio     -");
    if (S_ISLNK(mode)) printf("-Enlace simbólico -");
    if (S_ISCHR(mode)) printf("-Dispositivo de caracteres-");
    if (S_ISBLK(mode)) printf("-Dispositivo de bloques   -");
    if (S_ISFIFO(mode)) printf("-    FIFO        -");
    if (S_ISSOCK(mode)) printf("-   Socket       -");
}

void mostrarTablaDirectorios(const char *path){ 
    struct dirent *entry;
    struct stat fileStat;
    
    DIR *dir = opendir(path);
    if(dir==NULL){
        perror("Error al abir el directorio");
    }
    
    while ((entry =readdir(dir)) != NULL){
        if(lstat(path,&fileStat) == -1){
            perror("Error en lstat");
            continue;
        }
        if(lstat(path, &fileStat) == 0){
            printf("-----------------------------  Nombre: %-20s  -----------------------------\n",entry->d_name);
            printf("ID del dispositivo: %ld\n",fileStat.st_dev);
            printf("Numero de inodo: %d\n",fileStat.st_ino);
            printf("Permisos: ");
            mostrar_permisos(fileStat.st_mode);
            printf("\nTipo de archivo: ");
            tipoArchivo(fileStat.st_mode);
            printf("\nUltimo acceso: %s", ctime(&fileStat.st_atime));
            printf("Ultima modificación: %s",ctime(&fileStat.st_mtime));
            printf("Ultimo cambio en metadatos: %s",ctime(&fileStat.st_ctime));
            printf("Numero de enlaces duros: %d\n",fileStat.st_nlink);
            printf("UID: %d\n",fileStat.st_uid);
            printf("GID: %d\n",fileStat.st_gid);
            printf("ID del dispositivo: %s\n", fileStat.st_rdev);
            printf("Tamaño del archivo (en bytes): %d\n",fileStat.st_size);
            printf("Tamaño del bloque de E/S: %d\n",fileStat.st_blksize);
            printf("Numero de bloques asignados: %d\n",fileStat.st_blocks);
        }
    }
    closedir(dir);
    return;
}

void crearArchivo(const char *path){
    int fd = open(path, O_CREAT | O_WRONLY,0644);
    if(fd == -1){
        perror("Error creando el archivo\n");
        return;
    }
    close(fd);
    printf("Archivo '%s' creado correctamente.\n",path);
    
}

void escribirArchivo(const char *path){
    int fd = open(path, O_CREAT | O_WRONLY | O_APPEND,0644);
    if(fd == -1){
        perror("Error creando el archivo\n");
        return;
    }
    char buffer[256];
    while (1){
        printf( "|");
        fgets(buffer,sizeof(buffer),stdin);
        if(strncmp(buffer,"FIN",3)==0)
            break;
        write(fd,buffer,strlen(buffer));
    }
    close(fd);
    
}
void borrarArchivo(const char *path){
    if(unlink(path)== -1){
        perror("Error eliminando el archivo\n");
        return;
    }
    printf("Archivo eliminado: '%s\n'",path);
}

void mostrarArchivo(const char *path, off_t cursor){
    int fd= open(path,O_RDONLY);
    if(fd == -1){
        perror("Error al mostrar el archivo\n");
        return;
    }

    lseek(fd, cursor, SEEK_SET);
    char buffer[256];
    ssize_t bytes;

    while ((bytes = read(fd, buffer, sizeof(buffer)-1)) > 0){
        buffer[bytes];
        write(STDOUT_FILENO, buffer, bytes);
    }

    if(bytes == -1){
        perror("Error al mostrar el archivo\n");
        return;
    }

    close(fd);
    printf("\n");
}

void renombrarArchivo(const char *oldname, const char *newname){
    if(rename(oldname,newname)== -1){
        perror("Error al cambiar el nombre");
    }
    printf("Archivo renombrado a '%s",newname);
}

void crearDirectorio(const char *path) {
    if (mkdir(path, 0755) == -1){
        perror("Error creando el directorio");
    } else {
        printf("Directorio creado: %s\n", path);
    }
}

void mostrarDirectorio(const char *path, bool a) {
    if(a){
        mostrarTablaDirectorios(path);
    }else{
        DIR *dir = opendir(path);
        //printf("%s -- ",path); This line shows the path we want to see
        if (!dir) {
            perror("Error abriendo el directorio");
            return;
        }
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            printf("%s    ", entry->d_name);
        }
        printf("\n");
        closedir(dir);
    }
}

void borrarDirectorio(const char *path) {
    if (rmdir(path) == -1) {
        perror("Error eliminando el directorio\n");
    } else {
        printf("Directorio eliminado: %s\n", path);
    }
}

void renombrarDirectorio(const char *old_name, const char *new_name) {
    if (rename(old_name, new_name) == -1) {
        perror("Error renombrando el directorio\n");
    } else {
        printf("Directorio renombrado a %s\n", new_name);
    }
}

void cambiarDirectorio(const char *path) {
    char ruta[PATH_MAX];
    if (chdir(path) == -1) {
        perror("Error cambiando de directorio\n");
    } else {
        printf("Directorio cambiado a: %s\n", path);
        printf("Ruta actual: %s\n",getcwd(ruta, sizeof(ruta)));
    }
    return;
}

void pwd(){
    char ruta[PATH_MAX];
    printf("%s\n",getcwd(ruta, sizeof(ruta)));
}


int main(){
    char rutaAux[PATH_MAX], command[36], ruta[PATH_MAX], mandatory[PATH_MAX];
    while (1) {
        memset(rutaAux,0,sizeof(rutaAux));
        memset(command,0,sizeof(command));
        memset(ruta,0,sizeof(ruta));
        memset(mandatory,0,sizeof(mandatory));
        printf("[user ~]$ ");
        fgets(rutaAux, sizeof(rutaAux), stdin);
        rutaAux[strcspn(rutaAux, "\n")] = 0;
        sscanf(rutaAux,"%s %s %s",command, ruta, mandatory);

        if (strcmp(command, "salir") == 0) {
            break;
        } else if (strcmp(command, "crearArchivo") == 0) {
            crearArchivo(ruta);
        } else if (strcmp(command, "escribirArchivo") == 0){
            escribirArchivo(ruta);
        } else if (strcmp(command, "borrarArchivo") == 0){
            borrarArchivo(ruta);
        }else if(strcmp(command, "mostrarArchivo") == 0){
            if(strlen(mandatory)==0){
                strcpy(mandatory,"0");
                printf("---%s---",mandatory);
            }
            off_t cursor = atoll(mandatory);
            mostrarArchivo(ruta, cursor);
        }else if (strcmp(command, "renombrarArchivo") == 0){
            renombrarArchivo(ruta, mandatory);
        } else if (strcmp(command, "crearDirectorio") == 0){
            crearDirectorio(ruta);
        } else if (strcmp(command, "mostrarDirectorio") == 0){
            
            if(strlen(ruta) == 0 && strlen(mandatory) == 0){
                getcwd(ruta, sizeof(ruta));  
                mostrarDirectorio(ruta, false); 
            }else if(strlen(ruta) != 0 && strcmp(mandatory,"-l") == 0){  
                printf("VALOR ERRONEO");
                mostrarDirectorio(ruta, true); 
            }else if (strlen(ruta) != 0 && strlen(mandatory) == 0){ 
                mostrarDirectorio(ruta,false);
            }else mostrarDirectorio(ruta,false);

        } else if (strcmp(command, "pwd") == 0) {
            pwd();
        } else if (strcmp(command, "borrarDirectorio") == 0) {
            borrarDirectorio(ruta);
        } else if (strcmp(command, "renombrarDirectorio") == 0) {
            renombrarDirectorio(ruta, mandatory);
        } else if(strcmp(command, "cambiarDirectorio") == 0){
            cambiarDirectorio(ruta);
        } else {
            printf("Comando no reconocido.\n");
        }
    }
    return 0;
}
