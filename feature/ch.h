#ifndef CH_H
#define CH_H 

int AFH_LQA_get_rssi(int ch_num);
int AFH_LQA_get_snr(int ch_num);
int AFH_LQA_get_prr(int ch_num); 
bool AFH_LQA(int mode,int ch_num);
void AFH_ch_init(int ch_tot);
void AFH_ch_sel(int mode,int ch_tot);
int AFH_ch_update(int mode,int ch_num,int ch_tot);

#endif