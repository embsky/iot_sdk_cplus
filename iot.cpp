#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include "base64.h"

#include "iot.h"

int upload_sensor_status(const char *ip, const char *device, const char *sensor, const char *api_key, const char *status)
{
    int sd;
    int i = 0;
    int ret;
    char *v;
    char buf[2048];
    struct sockaddr_in yeelink_addr;
    char send_data[512];
    char base64_id[256];
    char base64_m[32];

    sprintf(buf, "%s%s", api_key, ":");
    base64_encode((unsigned char *)base64_id, (unsigned char *)buf);  
   
    sprintf(send_data, "POST /api/1.0/device/%s/sensor/%s/data HTTP/1.1\r\nHost: www.embsky.com\r\nAccept: */*\r\nAuthorization: Basic %s\r\nContent-Length: %d\r\nContent-Type: application/json;charset=utf-8\r\nConnection: close\r\n\r\n{\"data\":\"%s\"}\r\n", device, sensor, base64_id, (int)strlen(status) + 1 + 10, status);

    yeelink_addr.sin_family = AF_INET;
    yeelink_addr.sin_port = htons(80);
    yeelink_addr.sin_addr.s_addr = inet_addr(ip);

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        perror("socket");
        return -1;
    }

    ret = connect(sd, (struct sockaddr *)&yeelink_addr, sizeof(yeelink_addr));
    if (ret < 0) {
        perror("connect");
        close(sd);
        return -1;
    }

    ret = write(sd, send_data, strlen(send_data) + 1);
    if (ret < 0) {
        perror("write");
        close(sd);
        return -1;
    }

    ret = read(sd, buf, sizeof(buf));
    if (ret < 0) {
        perror("read");
        close(sd);
        return -1;
    }

    //printf("%s\n", buf);

    close(sd);

    return 0;
}

int upload_sensor_data(const char *ip, const char *device, const char *sensor, const char *api_key, double data)
{
    int sd;
    int i = 0;
    int ret;
    char *v;
    char buf[2048];
    struct sockaddr_in yeelink_addr;
    char send_data[512];
    char base64_id[256];
    char base64_m[32];

    sprintf(buf, "%s%s", api_key, ":");
    base64_encode((unsigned char *)base64_id, (unsigned char *)buf);  

    //printf("api:%s\n", base64_id);
   
    ret = sprintf(buf, "%f", data);
    sprintf(send_data, "POST /api/1.0/device/%s/sensor/%s/data HTTP/1.1\r\nHost: www.embsky.com\r\nAccept: */*\r\nAuthorization: Basic %s\r\nContent-Length: %d\r\nContent-Type: application/json;charset=utf-8\r\nConnection: close\r\n\r\n{\"data\":%f}\r\n", device, sensor, base64_id, ret + 10, data);

    yeelink_addr.sin_family = AF_INET;
    yeelink_addr.sin_port = htons(80);
    yeelink_addr.sin_addr.s_addr = inet_addr(ip);

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        perror("socket");
        return -1;
    }

    ret = connect(sd, (struct sockaddr *)&yeelink_addr, sizeof(yeelink_addr));
    if (ret < 0) {
        perror("connect");
        close(sd);
        return -1;
    }

    ret = write(sd, send_data, strlen(send_data) + 1);
    if (ret < 0) {
        perror("write");
        close(sd);
        return -1;
    }

    ret = read(sd, buf, sizeof(buf));
    if (ret < 0) {
        perror("read");
        close(sd);
        return -1;
    }

    //printf("%s\n", buf);

    close(sd);

    return 0;
}

int download_sensor_status(const char *ip, const char *device, const char *sensor, const char *api_key)
{
    int sd;
    int i = 0;
    int ret;
    char *v;
    char buf[2048];
    struct sockaddr_in yeelink_addr;
    char send_data[512];
    char base64_id[256];
    char base64_m[32];

    sprintf(buf, "%s%s", api_key, ":");
    base64_encode((unsigned char *)base64_id, (unsigned char *)buf);  

    sprintf(send_data, "GET /api/1.0/device/%s/sensor/%s/data HTTP/1.1\r\nHost: www.embsky.com\r\nAccept: */*\r\nAuthorization: Basic %s\r\nContent-Length: 0\r\nConnection: close\r\n\r\nprint get done.\r\n", device, sensor, base64_id);

    yeelink_addr.sin_family = AF_INET;
    yeelink_addr.sin_port = htons(80);
    yeelink_addr.sin_addr.s_addr = inet_addr(ip);

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        perror("socket");
        return -1;
    }

    ret = connect(sd, (struct sockaddr *)&yeelink_addr, sizeof(yeelink_addr));
    if (ret < 0) {
        perror("connect");
        close(sd);
        return -1;
    }

    ret = write(sd, send_data, strlen(send_data) + 1);
    if (ret < 0) {
        perror("write");
        close(sd);
        return -1;
    }

    ret = read(sd, buf, sizeof(buf));
    if (ret < 0) {
        perror("read");
        close(sd);
        return -1;
    }

    //puts(buf);
    if(strstr(buf, "true")) {
        close(sd);
        return 1;
    } else if(strstr(buf, "false")) {
        close(sd);
        return 0;
    } 

    close(sd);
    return -1;
}

//{'datas':[{'id':sensor_id1, 'data':data}, {'id':sensor_id2, 'data':'0'}, {'id':sensor_id4, 'data':'1'}, {'id':sensor_id3, 'data':[lng, lat]}]}
int upload_device_datas(const char *ip, const char *device, const char *api_key, int sensor_id_arrary[], double sensor_data_arrary[], int size)
{
    int sd;
    int i = 0;
    int ret;
    char *v;
    char buf[2048];
    struct sockaddr_in yeelink_addr;
    char send_data[1024];
    char data_list[512];
    char base64_id[256];
    char base64_m[32];

    sprintf(buf, "%s%s", api_key, ":");
    base64_encode((unsigned char *)base64_id, (unsigned char *)buf);  

    ret = sprintf(data_list, "%s", "[");
    for (i = 0; i < size; i++) {
        if (i == size - 1) {
            ret += sprintf(data_list + ret, "{\"id\":%d, \"data\":%f}", sensor_id_arrary[i], sensor_data_arrary[i]);
	} else {
            ret += sprintf(data_list + ret, "{\"id\":%d, \"data\":%f},", sensor_id_arrary[i], sensor_data_arrary[i]);
	}
    }
    ret += sprintf(data_list + ret, "%s", "]");

    ret = sprintf(send_data, "POST /api/1.0/device/%s/datas HTTP/1.1\r\nHost: www.embsky.com\r\nAccept: */*\r\nAuthorization: Basic %s\r\nContent-Length: %d\r\nContent-Type: application/json;charset=utf-8\r\nConnection: close\r\n\r\n{\"datas\":%s}\r\n", device, base64_id, ret + 10, data_list);

    //printf("%s\n", send_data);

    yeelink_addr.sin_family = AF_INET;
    yeelink_addr.sin_port = htons(80);
    yeelink_addr.sin_addr.s_addr = inet_addr(ip);

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        perror("socket");
        return -1;
    }

    ret = connect(sd, (struct sockaddr *)&yeelink_addr, sizeof(yeelink_addr));
    if (ret < 0) {
        perror("connect");
        close(sd);
        return -1;
    }

    ret = write(sd, send_data, strlen(send_data) + 1);
    if (ret < 0) {
        perror("write");
        close(sd);
        return -1;
    }

    ret = read(sd, buf, sizeof(buf));
    if (ret < 0) {
        perror("read");
        close(sd);
        return -1;
    }

    //puts(buf);

    close(sd);

    return 0;
}

//增加一次上传多个设备的数据方法
//{'datas':[{'device_id':{'datas':[{'id':123, 'data':12.3}, {'id':123, 'data':'0'}, {'id':123, 'data':[12, 13]}, {'id':123, 'data':12.3}]}}, {'device_id':{'datas':[{'id':123, 'data':12.3}, {'id':123, 'data':'0'}, {'id':123, 'data':[12, 13]}, {'id':123, 'data':12.3}]}}, {'device_id':{'datas':[{'id':123, 'data':12.3}, {'id':123, 'data':'0'}, {'id':123, 'data':[12, 13]}, {'id':123, 'data':12.3}]}}]}

static char all_data[1024] = {0};
static int all_data_len = 0;

void all_data_start(void)
{
    all_data_len = 0;
    all_data_len += sprintf(all_data + all_data_len, "[");
}

void all_data_add(const char *device_id, int sensor_id_arrary[], double sensor_data_arrary[], int size)
{
    int i = 0;
    all_data_len += sprintf(all_data + all_data_len, "{\"%s\":{\"datas\":[", device_id);
    for (i = 0; i < size; i++) {
        if (i == size - 1) {
            all_data_len += sprintf(all_data + all_data_len, "{\"id\":%d, \"data\":%f}", sensor_id_arrary[i], sensor_data_arrary[i]);
	} else {
            all_data_len += sprintf(all_data + all_data_len, "{\"id\":%d, \"data\":%f},", sensor_id_arrary[i], sensor_data_arrary[i]);
	}
    }
    
    all_data_len += sprintf(all_data + all_data_len, "%s", "]}},");
}

void all_data_end(void)
{
    all_data_len += sprintf(all_data + all_data_len - 1, "]");
    //printf("%s\n", all_data);
}

int upload_datas(const char *ip, const char *api_key)
{
    int sd;
    int i = 0;
    int ret;
    char *v;
    char send_data[2048];
    char buf[2048];
    struct sockaddr_in yeelink_addr;
    char base64_id[256];
    char base64_m[32];

    sprintf(buf, "%s%s", api_key, ":");
    base64_encode((unsigned char *)base64_id, (unsigned char *)buf);  

    ret = sprintf(send_data, "POST /api/1.0/datas HTTP/1.1\r\nHost: www.embsky.com\r\nAccept: */*\r\nAuthorization: Basic %s\r\nContent-Length: %d\r\nContent-Type: application/json;charset=utf-8\r\nConnection: close\r\n\r\n{\"datas\":%s}\r\n", base64_id, all_data_len + 10, all_data);

    //printf("%s\n", send_data);

    yeelink_addr.sin_family = AF_INET;
    yeelink_addr.sin_port = htons(80);
    yeelink_addr.sin_addr.s_addr = inet_addr(ip);

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        perror("socket");
        return -1;
    }

    ret = connect(sd, (struct sockaddr *)&yeelink_addr, sizeof(yeelink_addr));
    if (ret < 0) {
        perror("connect");
        close(sd);
        return -1;
    }

    ret = write(sd, send_data, strlen(send_data) + 1);
    if (ret < 0) {
        perror("write");
        close(sd);
        return -1;
    }

    ret = read(sd, buf, sizeof(buf));
    if (ret < 0) {
        perror("read");
        close(sd);
        return -1;
    }

    //buf[ret] = 0;
    //puts(buf);

    close(sd);

    return 0;
}

