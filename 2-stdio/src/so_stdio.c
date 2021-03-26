#include <so_stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#define BUFSIZE 4096

#define READ    (1)
#define WRITE   (1 << 1)
#define APPEND  (1 << 2)
#define UPDATE  (1 << 3)

typedef struct _so_file {
    int fd;
    unsigned short mode;
    unsigned short last_op;
    
    char buffer[BUFSIZE];
    size_t cursor;
    int no_bytes;
} SO_FILE;

SO_FILE *so_fopen(const char *pathname, const char *mode) {
    if (!pathname || !mode)
        return NULL;

    unsigned short fmode = 0;
    int flags = 0;
    if (!strcmp(mode, "r"))
        fmode = READ;
    else if (!strcmp(mode, "r+"))
        fmode = READ | UPDATE;
    else if (!strcmp(mode, "w"))
        fmode = WRITE;
    else if (!strcmp(mode, "w+"))
        fmode = WRITE | UPDATE;
    else if (!strcmp(mode, "a"))
        fmode = APPEND;
    else if (!strcmp(mode, "a+"))
        fmode = APPEND | READ | UPDATE;
    else
        return NULL;

    flags |= fmode & READ ? (fmode & UPDATE ? O_RDWR : O_RDONLY) : 0;
    flags |= fmode & WRITE ? (fmode & UPDATE ? O_RDWR : O_WRONLY) | O_CREAT | O_TRUNC : 0;
    flags |= fmode & APPEND ? (fmode & UPDATE ? O_RDWR : O_WRONLY) | O_APPEND | O_CREAT : 0;


    int fd = open(pathname, flags, 0644);
    if (fd < 0)
        return NULL;

    SO_FILE* res = calloc(1, sizeof(SO_FILE));
    if (!res) {
        close(fd);
        return NULL;
    }

    res->fd = fd;
    res->mode = fmode;

    return res;
}

int so_fclose(SO_FILE *stream) {
    if (!stream)
        return SO_EOF;
    
    int rc;

    if (so_fflush(stream) < 0)
        return SO_EOF;

    rc = close(stream->fd);
    if (rc < 0)
        return SO_EOF;

    free(stream);

    return 0;
}

int so_fileno(SO_FILE *stream) {
    if (!stream)
        return -1;

    return stream->fd;
}

int so_fflush(SO_FILE *stream) {
    if (!stream)
        return SO_EOF;

    if (stream->last_op == READ) {
        lseek(stream->fd, -stream->no_bytes, SEEK_CUR);

        stream->cursor = 0;
        stream->no_bytes = 0;
        memset(stream->buffer, 0, BUFSIZE);

        return 0;
    }

    if (!stream->mode & (WRITE | APPEND))
        return SO_EOF;

    if (stream->last_op != WRITE)
        return 0;
    
    if (stream->mode & APPEND)
        so_fseek(stream, 0, SEEK_END);

    int rc = write(stream->fd, stream->buffer, stream->cursor);
    if (rc < 0)
        return SO_EOF;

    memset(stream->buffer, 0, BUFSIZE);
    stream->cursor = 0;

    return 0;
}

int so_fseek(SO_FILE *stream, long offset, int whence) {
    if (!stream)
        return -1;

    if (stream->last_op == READ) {
        long current = so_ftell(stream);

        int dif, len;
        switch (whence) {
            case SEEK_SET:
                dif = offset - current;
                break;
            case SEEK_CUR:
                dif = offset;
                break;
            case SEEK_END:
                len = lseek(stream->fd, 0, SEEK_END);
                if (len < 0)
                    return -1;

                if (lseek(stream->fd, current, SEEK_SET) < 0)
                    return -1;

                dif = len + offset - current;
                break;

            default:
                return -1;
        }

        if (stream->cursor + dif >= 0 && stream->cursor + dif < stream->no_bytes) {
            stream->cursor += dif;
            return 0;
        }

        so_fflush(stream);
    }

    if (lseek(stream->fd, offset, whence) < 0)
        return -1;

    return 0;
}

long so_ftell(SO_FILE *stream) {
    if (!stream)
        return -1;

    if (stream->last_op == READ)
        return lseek(stream->fd, 0, SEEK_CUR) - (BUFSIZE - stream->cursor);

    return lseek(stream->fd, 0, SEEK_CUR) + stream->cursor;
}

int so_read_internal(SO_FILE *stream) {
    if (!stream)
        return SO_EOF;

    if (!(stream->mode & READ))
        return SO_EOF;

    if (stream->cursor < stream->no_bytes)
        return 0;

    stream->no_bytes = read(stream->fd, stream->buffer, BUFSIZE);
    if (stream->no_bytes <= 0)
        return SO_EOF;

    stream->cursor = 0;
    return stream->no_bytes;
}

int so_is_full_internal(SO_FILE *stream) {
    if (!stream)
        return 0;

    return stream->cursor > 0 && stream->cursor >= BUFSIZE;
}

static inline int min(int a, int b) {
    return a < b ? a : b;
}

size_t so_fread(void *ptr, size_t size, size_t nmemb, SO_FILE *stream) {
    if (!ptr || size < 1 || nmemb < 1 || !stream)
        return 0;

    if (!(stream->mode & READ))
        return 0;

    int total = size * nmemb;
    int done = 0;

    if (stream->last_op == WRITE)
        so_fflush(stream);

    while (done < total) {
        if (so_read_internal(stream) < 0)
            return 0;

        int writeable = min(stream->no_bytes - stream->cursor, total - done);
        memcpy(ptr + done, stream->buffer + stream->cursor, writeable);

        done += writeable;
        stream->cursor += writeable;
    }

    stream->last_op = READ;
    return nmemb;
}

size_t so_fwrite(const void *ptr, size_t size, size_t nmemb, SO_FILE *stream) {
    if (!ptr || size < 1 || nmemb < 1 || !stream)
        return 0;

    if (!(stream->mode & (WRITE | APPEND)))
        return 0;

    int total = size * nmemb;
    int done = 0;

    if (stream->last_op == READ)
        so_fflush(stream);

    while (done < total) {
        if (so_fputc(*(int*)(ptr + done), stream) != *(int*)(ptr + done))
            return 0;

        done++;
    }

    stream->last_op = WRITE;
    return nmemb;
}

int so_fgetc(SO_FILE *stream) {
    if (!stream)
        return SO_EOF;

    if (!(stream->mode & READ))
        return SO_EOF;

    if (stream->last_op == WRITE)
        so_fflush(stream);

    if (stream->cursor < stream->no_bytes)
        return stream->buffer[stream->cursor++];

    int rc;
    if (rc = so_read_internal(stream) < 0)
        return rc;

    stream->last_op = READ;
    return stream->buffer[stream->cursor++];
}

int so_fputc(int c, SO_FILE *stream) {
    if (!stream)
        return SO_EOF;

    if (!(stream->mode & (WRITE | APPEND)))
        return SO_EOF;

    if (stream->last_op == READ)
        so_fflush(stream);

    if (so_is_full_internal(stream))
        so_fflush(stream);

    stream->buffer[stream->cursor++] = c;

    stream->last_op = WRITE;

    return c;

}

int so_feof(SO_FILE *stream) {}
int so_ferror(SO_FILE *stream) {}

SO_FILE *so_popen(const char *command, const char *type) {}
int so_pclose(SO_FILE *stream) {}