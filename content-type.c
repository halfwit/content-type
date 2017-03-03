/* cc -o ctcurl ctcurl.c -lcurl */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <curl/curl.h>

enum {
	TIMEOUT_MS  = 5000,
	MAX_REDIRS  = 50L
};

struct memory_struct {
	char	*memory;
	size_t	size;
};

static size_t
ct_strstr(char *in)
{

	int i;

	for (i = 0; in[i]; i++) {
		in[i] = tolower(in[i]);
	}

	char *r = strstr(in, "content-type");

	return r - in;

}

static size_t
wm_callback(void *contents, size_t size, size_t nmemb, void *userp)
{

	size_t realsize = size * nmemb;
	struct memory_struct *mem = (struct memory_struct *)userp;
	mem->memory = realloc(mem->memory, mem->size + realsize + 1);
	
	if (mem->memory == NULL) {
		fprintf(stderr, "Memory allocation error\n");
		return 0;
	}

	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;

}

int
main(int argc, char *argv[])
{
	if (argc != 2)
		fprintf(stderr, "Usage: %s <url>\n", argv[0]);

	CURLcode ret;
	CURL *hnd;

	struct memory_struct chunk;

	chunk.memory = malloc(1);
	chunk.size = 0;

	hnd = curl_easy_init();

	curl_easy_setopt(hnd, CURLOPT_URL, argv[1]);
	curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, wm_callback);
	curl_easy_setopt(hnd, CURLOPT_WRITEDATA, (void *)&chunk);
	curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYHOST, 0L);
	curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
	curl_easy_setopt(hnd, CURLOPT_TIMEOUT_MS, TIMEOUT_MS);
	curl_easy_setopt(hnd, CURLOPT_NOBODY, 1L);
	curl_easy_setopt(hnd, CURLOPT_HEADER, 1L);
	curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/7.52.1");
	curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, MAX_REDIRS);
	curl_easy_setopt(hnd, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
	curl_easy_setopt(hnd, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(hnd, CURLOPT_FILETIME, 1L);
	curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);

	ret = curl_easy_perform(hnd);

	if(ret != CURLE_OK) {
		fprintf(stderr, "Failed to fetch content-type, %s\n", curl_easy_strerror(ret));
		curl_easy_cleanup(hnd);
		return(EXIT_FAILURE);
	}
	
    int start = 0, mid = 0, end = 0;
    
	// Use sentinels to grep out '/', then go back and forth to find endpoints, finally printing the range to stdout.
	size_t i;
	
	for (i = ct_strstr(chunk.memory); i < chunk.size; i++) {
		if (chunk.memory[i] == '/') {
			mid = i;
			break;
		}
	}

	for (i = mid + 1; i < chunk.size; i++) {
		if (!isalnum(chunk.memory[i]) && chunk.memory[i] != '-') {
			end = i;
			break;
		}
	}

	for (i = mid - 1; i > 0; i--) {
		if (!isalnum(chunk.memory[i]) && chunk.memory[i] != '-') {
			start = ++i;
			break;
		}
	}

	printf("%.*s\n", end - start, &chunk.memory[start]);

	curl_easy_cleanup(hnd);
	hnd = NULL;

	return (int)ret;

}
