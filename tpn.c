#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <curl/curl.h>

#define BUFFERSIZE 10

static const char* TELEGRAM_ID = "566243044";
static const char* BOT_TOKEN   = "1893059573:AAEeVZdQcdQr1VWd_Yap7bHTVJw5Q8vWcQQ";

int url_length(const char *str) {
    int c = 0;    
    while (*str) {
	if (*str == ' ') {
	    c += 2;
	}
	++c;
	++str;
    }
    return c;
}

void make_url(char *dest, const char *src)
{
    int dp = 0;
    while (*src != '\0') {
	if (*src == ' ') {
	    dest[dp]   = '%';
	    dest[dp+1] = '2';
	    dest[dp+2] = '0';
	    dp += 3;
	}
	else {
	    dest[dp] = *src;
	    ++dp;
	}
	++src;
    }
    dest[dp] = '\0';
}

size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
   return size * nmemb;
}

void bot_notify(const char *msg)
{
    curl_global_init(CURL_GLOBAL_ALL);
    
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if(curl) {
	char *url_fmt = "https://api.telegram.org/bot%s/sendMessage";
	char *url = malloc((strlen(url_fmt) - 2) * sizeof(url));
	sprintf(url, url_fmt, BOT_TOKEN);
	
	int ps_len = url_length(msg);       
	char *ps = malloc(ps_len * sizeof(ps));
	make_url(ps, msg);
	
	char *ps_fmt = "chat_id=%s&text=%s";
	char *ps_full = malloc((ps_len + strlen(ps_fmt) - 4) * sizeof(ps_full));      
	sprintf(ps_full, ps_fmt, TELEGRAM_ID, ps);

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, ps_full);

	res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
	    fprintf(stderr, "curl_easy_perform() failed: %s\n",
		    curl_easy_strerror(res));
	}
	curl_easy_cleanup(curl);
	free(url);
	free(ps);
	free(ps_full);	
    }
    
    curl_global_cleanup();   
}

int main(int argc, char *argv[])
{
    char *cmd = calloc(1,1), buffer[BUFFERSIZE];
    while( fgets(buffer, BUFFERSIZE , stdin) ) /* break with ^D or ^Z */
    {
	cmd = realloc( cmd, strlen(cmd)+1+strlen(buffer) );
	strcat( cmd, buffer ); /* note a '\n' is appended here everytime */
    }

    pid_t pid, sid;
    
    pid = fork();
    if (pid < 0) {
	exit(EXIT_FAILURE);
    }
    if (pid > 0) {
	exit(EXIT_SUCCESS);
    }
    umask(0);

    sid = setsid();
    if (sid < 0) {
	exit(EXIT_FAILURE);
    }

    int res = system(cmd);
    char resbuf[1024];
    sprintf(resbuf, "Process terminated with status: %d", res);

    bot_notify(resbuf);
    
    free(cmd);

    exit(EXIT_SUCCESS);
}
