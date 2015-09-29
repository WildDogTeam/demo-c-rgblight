
#ifndef __COOEE_H__
#define __COOEE_H__

/* set packet interval in ms. default 8ms */
int set_packet_interval(int ms);

/* send cooee */
int send_cooee(const char* ssid, uint32_t ssid_len, 
        const char* pwd, uint32_t pwd_len, 
        const char* key, uint32_t key_len,
        uint32_t ip);

#endif /* __COOEE_H__ */
