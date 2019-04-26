#define FUSE_USE_VERSION 28
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
// #define HAVE_SETXATTR
#define _GNU_SOURCE
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif
#ifdef linux
#define _XOPEN_SOURCE 700
#endif

char message[100] = "qE1~ YMUR2\"`hNIdPzi%^t@(Ao:=CQ,nx4S[7mHFye#aT6+v)DfKL$r?bkOGB>}!9_wV']jcp5JZ&Xl|\\8s;g<{3.u*W-0", ch;
int key = 17;

static const char *dirpath = "/home/fawwaz/shift4";

char *enkripsi(char nama[1000])
{
	int a = 0;

	for (a = 0; nama[a] != '\0'; a++)
	{
		int b = 0;
		while (nama[a] != '\0')
		{
			if (nama[a] == message[b])
			{
				break;
			}
			b++;
		}
		if (nama[a] != '/')
		{
			nama[a] = message[(b + key) % strlen(message)];
		}
		else
		{
			nama[a] = '/';
		}
	}

	return nama;
}

char *dekripsi(char nama[1000])
{
	int a = 0;

	for (a = 0; nama[a] != '\0'; a++)
	{
		int b = 0;
		while (nama[a] != '\0')
		{
			if (nama[a] == message[b])
			{
				break;
			}
			b++;
		}
		if (nama[a] != '/')
		{
			int z = 0;
			if (b - key < 0)
			{
				z = b - key + strlen(message);
			}
			else
			{
				z = b - key;
			}

			nama[a] = message[z];
		}
		else
		{
			nama[a] = '/';
		}
	}

	return nama;
}

static int xmp_getattr(const char *path, struct stat *stbuf)
{
	int res;
	char fpath[1000];
	char p[1000];

	strcpy(p, path);
	if (strcmp(p, ".") != 0 && strcmp(p, "..") != 0)
	{
		memset(&p, 0, sizeof(p));
		enkripsi(p);
		sprintf(fpath, "%s%s", dirpath, p);
	}
	else
	{
		sprintf(fpath, "%s%s", dirpath, path);
	}
	res = lstat(fpath, stbuf);

	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
					   off_t offset, struct fuse_file_info *fi)
{
	char fpath[1000];
	char p[1000];
	if (strcmp(path, "/") == 0)
	{
		path = dirpath;
		sprintf(fpath, "%s", path);
	}
	else
	{
		memset(&p, 0, sizeof(p));
		enkripsi(p);
		sprintf(fpath, "%s%s", dirpath, p);
	}
	int res = 0;

	DIR *dp;
	struct dirent *de;

	(void)offset;
	(void)fi;

	dp = opendir(fpath);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL)
	{
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0)
		{
			dekripsi(de->d_name);
		}

		res = (filler(buf, de->d_name, &st, 0));
		if (res != 0)
			break;
	}

	closedir(dp);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
					struct fuse_file_info *fi)
{
	char fpath[1000];
	char p[1000];
	if (strcmp(path, "/") == 0)
	{
		path = dirpath;
		sprintf(fpath, "%s", path);
	}
	else
	{
		memset(&p, 0, sizeof(p));
		enkripsi(p);
		sprintf(fpath, "%s%s", dirpath, p);
	}

	int res = 0;
	int fd = 0;

	(void)fi;
	fd = open(fpath, O_RDONLY);
	if (fd == -1)
		return -errno;

	res = pread(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	return res;
}

static int xmp_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
	char fpath[1000];
	char p[1000];
	memset(&p, 0, sizeof(p));
	strcpy(p, path);
	enkripsi(p);
	sprintf(fpath, "%s%s", dirpath, p);
	int res = 0;

	(void)fi;

	// int res;
	res = creat(fpath, mode);
	if (res == -1)
		return -errno;

	close(res);

	return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi)
{
	char fpath[1000];
	char p[1000];
	memset(&p, 0, sizeof(p));
	strcpy(p, path);
	enkripsi(p);
	sprintf(fpath, "%s%s", dirpath, p);
	int res = 0;

	res = open(fpath, fi->flags);
	if (res == -1)
		return -errno;

	close(res);
	return 0;
}

static int xmp_truncate(const char *path, off_t size)
{
	char fpath[1000];
	char p[1000];
	int res = 0;

	memset(&p, 0, sizeof(p));
	strcpy(p, path);
	enkripsi(p);
	sprintf(fpath, "%s%s", dirpath, p);
	res = truncate(fpath, size);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_utimens(const char *path, const struct timespec ts[2])
{
	char fpath[1000];
	char p[1000];
	int res = 0;
	struct timeval tv[2];

	tv[0].tv_sec = ts[0].tv_sec;
	tv[0].tv_usec = ts[0].tv_nsec / 1000;
	tv[1].tv_sec = ts[1].tv_sec;
	tv[1].tv_usec = ts[1].tv_nsec / 1000;

	memset(&p, 0, sizeof(p));
	strcpy(p, path);
	enkripsi(p);
	sprintf(fpath, "%s%s", dirpath, p);
	res = utimes(fpath, tv);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
	char fpath[1000];
	char p[1000];
	memset(&p, 0, sizeof(p));
	strcpy(p, path);
	enkripsi(p);
	sprintf(fpath, "%s%s", dirpath, p);
	int res;

	res = mkdir(fpath, mode);
	if (res == -1)
		return -errno;

	return 0;
}

#ifdef HAVE_SETXATTR
static int xmp_setxattr(const char *path, const char *name, const char *value,
						size_t size, int flags)
{
	char fpath[1000];
	char p[1000];
	strcpy(p, path);
	if (strcmp(p, ".") != 0 && strcmp(p, "..") != 0)
	{
		memset(&p, 0, sizeof(p));
		enkripsi(p);
		sprintf(fpath, "%s%s", dirpath, p);
	}
	else
	{
		sprintf(fpath, "%s%s", dirpath, path);
	}
	int res = lsetxattr(fpath, name, value, size, flags);
	if (res == -1)
		return -errno;
	return 0;
}

static int xmp_getxattr(const char *path, const char *name, char *value,
						size_t size)
{
	char fpath[1000];
	char p[1000];
	strcpy(p, path);
	if (strcmp(p, ".") != 0 && strcmp(p, "..") != 0)
	{
		memset(&p, 0, sizeof(p));
		enkripsi(p);
		sprintf(fpath, "%s%s", dirpath, p);
	}
	else
	{
		sprintf(fpath, "%s%s", dirpath, path);
	}
	int res = lgetxattr(fpath, name, value, size);
	if (res == -1)
		return -errno;
	return res;
}

static int xmp_listxattr(const char *path, char *list, size_t size)
{
	char fpath[1000];
	char p[1000];
	strcpy(p, path);
	if (strcmp(p, ".") != 0 && strcmp(p, "..") != 0)
	{
		memset(&p, 0, sizeof(p));
		enkripsi(p);
		sprintf(fpath, "%s%s", dirpath, p);
	}
	else
	{
		sprintf(fpath, "%s%s", dirpath, path);
	}
	int res = llistxattr(fpath, list, size);
	if (res == -1)
		return -errno;
	return res;
}

static int xmp_removexattr(const char *path, const char *name)
{
	char fpath[1000];
	char p[1000];

	strcpy(p, path);
	if (strcmp(p, ".") != 0 && strcmp(p, "..") != 0)
	{
		memset(&p, 0, sizeof(p));
		enkripsi(p);
		sprintf(fpath, "%s%s", dirpath, p);
	}
	else
	{
		sprintf(fpath, "%s%s", dirpath, path);
	}
	int res = lremovexattr(fpath, name);
	if (res == -1)
		return -errno;
	return 0;
}
#endif /* HAVE_SETXATTR */

static struct fuse_operations xmp_oper = {
	.getattr = xmp_getattr,
	.readdir = xmp_readdir,
	.read = xmp_read,
	.create = xmp_create,
	.truncate = xmp_truncate,
	.utimens = xmp_utimens,
	.mkdir = xmp_mkdir,
	.open = xmp_open,
#ifdef HAVE_SETXATTR
	.setxattr = xmp_setxattr,
	.getxattr = xmp_getxattr,
	.listxattr = xmp_listxattr,
	.removexattr = xmp_removexattr,
#endif
};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}