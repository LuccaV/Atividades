#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NOME_MOVIE 100
#define MAX_GENRE_MOVIE 100
#define MAX_USUARIO 100
#define MAX_FAVORITOS 100

typedef struct {
    int id;
    char movie[MAX_NOME_MOVIE];
    char genre[MAX_GENRE_MOVIE];
} Movie;

typedef struct {
    int id;
    char nome[MAX_USUARIO];
    int movies_favoritos[MAX_FAVORITOS];
    int num_favoritos;
} Usuario;

void adicionar_movie(FILE *movie_file);
void adicionar_usuario(FILE *user_file);
void listar_movies(FILE *movie_file);
void listar_usuarios(FILE *user_file);
void adicionar_favorito(FILE *movie_file, FILE *user_file);
void listar_usuarios_com_favoritos(FILE *user_file, FILE *movie_file);

int main() {
    FILE *movie_file, *user_file;

    movie_file = fopen("movies.bin", "rb+");
    if (!movie_file) movie_file = fopen("movies.bin", "wb+");

    user_file = fopen("usuarios.bin", "rb+");
    if (!user_file) user_file = fopen("usuarios.bin", "wb+");

    if (!movie_file || !user_file) {
        printf("Erro ao abrir os arquivos binários.\n");
        return 1;
    }

    int opcao;
    do {
        printf("\n=== MENU ===\n");
        printf("1. Adicionar Movie\n");
        printf("2. Adicionar Usuário\n");
        printf("3. Listar Movies\n");
        printf("4. Listar Usuários\n");
        printf("5. Adicionar Favorito\n");
        printf("6. Listar Usuários com Favoritos\n");
        printf("7. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                adicionar_movie(movie_file);
                break;
            case 2:
                adicionar_usuario(user_file);
                break;
            case 3:
                listar_movies(movie_file);
                break;
            case 4:
                listar_usuarios(user_file);
                break;
            case 5:
                adicionar_favorito(movie_file, user_file);
                break;
            case 6:
                listar_usuarios_com_favoritos(user_file, movie_file);
                break;
            case 7:
                printf("Saindo do programa...\n");
                break;
            default:
                printf("Opção inválida! Tente novamente.\n");
        }
    } while (opcao != 7);

    fclose(movie_file);
    fclose(user_file);

    return 0;
}

void adicionar_movie(FILE *movie_file) {
    Movie movie = {0};
    printf("\n=== Adicionar Movie ===\n");
    printf("ID: ");
    scanf("%d", &movie.id);
    printf("Título: ");
    getchar();
    fgets(movie.movie, MAX_NOME_MOVIE, stdin);
    strtok(movie.movie, "\n");
    printf("Gênero: ");
    fgets(movie.genre, MAX_GENRE_MOVIE, stdin);
    strtok(movie.genre, "\n");

    fseek(movie_file, 0, SEEK_END);
    fwrite(&movie, sizeof(Movie), 1, movie_file);
    fflush(movie_file);
}

void listar_movies(FILE *movie_file) {
    Movie movie;
    rewind(movie_file);
    printf("\n=== Lista de Movies ===\n");
    while (fread(&movie, sizeof(Movie), 1, movie_file)) {
        printf("ID: %d, Título: %s, Gênero: %s\n", movie.id, movie.movie, movie.genre);
    }
}

void adicionar_usuario(FILE *user_file) {
    Usuario usuario = {0};
    printf("\n=== Adicionar Usuário ===\n");
    printf("ID: ");
    scanf("%d", &usuario.id);
    printf("Nome: ");
    getchar();
    fgets(usuario.nome, MAX_USUARIO, stdin);
    strtok(usuario.nome, "\n");
    usuario.num_favoritos = 0;

    fseek(user_file, 0, SEEK_END);
    fwrite(&usuario, sizeof(Usuario), 1, user_file);
    fflush(user_file);
}

void listar_usuarios(FILE *user_file) {
    Usuario usuario;
    rewind(user_file);
    printf("\n=== Lista de Usuários ===\n");
    while (fread(&usuario, sizeof(Usuario), 1, user_file)) {
        printf("ID: %d, Nome: %s, Número de Favoritos: %d\n", usuario.id, usuario.nome, usuario.num_favoritos);
    }
}

void adicionar_favorito(FILE *movie_file, FILE *user_file) {
    Usuario usuario;
    Movie movie;
    int usuario_id, movie_id;
    int encontrado = 0;

    printf("\n=== Adicionar Movie Favorito ===\n");
    printf("ID do usuário: ");
    scanf("%d", &usuario_id);

    rewind(user_file);
    while (fread(&usuario, sizeof(Usuario), 1, user_file)) {
        if (usuario.id == usuario_id) {
            encontrado = 1;
            break;
        }
    }

    if (!encontrado) {
        printf("Usuário não encontrado.\n");
        return;
    }

    printf("Usuário encontrado: %s\n", usuario.nome);
    listar_movies(movie_file);

    printf("ID do movie para adicionar como favorito: ");
    scanf("%d", &movie_id);

    rewind(movie_file);
    encontrado = 0;
    while (fread(&movie, sizeof(Movie), 1, movie_file)) {
        if (movie.id == movie_id) {
            encontrado = 1;
            break;
        }
    }

    if (!encontrado) {
        printf("Movie não encontrado.\n");
        return;
    }

    if (usuario.num_favoritos >= MAX_FAVORITOS) {
        printf("O usuário já atingiu o limite de favoritos.\n");
        return;
    }

    usuario.movies_favoritos[usuario.num_favoritos++] = movie_id;

    fseek(user_file, -sizeof(Usuario), SEEK_CUR);
    fwrite(&usuario, sizeof(Usuario), 1, user_file);
    fflush(user_file);

    printf("Movie '%s' adicionado aos favoritos de '%s'.\n", movie.movie, usuario.nome);
}

void listar_usuarios_com_favoritos(FILE *user_file, FILE *movie_file) {
    Usuario usuario;
    Movie movie;

    rewind(user_file);
    printf("\n=== Usuários e seus Movies Favoritos ===\n");

    while (fread(&usuario, sizeof(Usuario), 1, user_file)) {
        printf("\nUsuário: %s (ID: %d)\n", usuario.nome, usuario.id);
        if (usuario.num_favoritos == 0) {
            printf("  Nenhum movie favorito cadastrado.\n");
            continue;
        }
        printf("  Movies favoritos:\n");
        for (int i = 0; i < usuario.num_favoritos; i++) {
            int id_fav = usuario.movies_favoritos[i];
            rewind(movie_file);
            int encontrado = 0;
            while (fread(&movie, sizeof(Movie), 1, movie_file)) {
                if (movie.id == id_fav) {
                    printf("    - %s (%s)\n", movie.movie, movie.genre);
                    encontrado = 1;
                    break;
                }
            }
            if (!encontrado) {
                printf("    - Movie com ID %d não encontrado.\n", id_fav);
            }
        }
    }
}