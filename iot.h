#ifndef __IOT_H__ 
#define __IOT_H__ 

int upload_sensor_data(const char *ip, const char *device, const char *sensor, const char *id, double data);

int upload_sensor_status(const char *ip, const char *device, const char *sensor, const char *id, const char *status);
int download_sensor_status(const char *ip, const char *device, const char *sensor, const char *id);

int upload_device_datas(const char *ip, const char *device, const char *api_key, int sensor_id_arrary[], double sensor_data_arrary[], int size);

void all_data_start(void);
void all_data_add(const char *device_id, int sensor_id_arrary[], double sensor_data_arrary[], int size);
void all_data_end(void);
int upload_datas(const char *ip, const char *api_key);


#endif
