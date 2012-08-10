#ifndef ZC_H_INCLUDED
#define ZC_H_INCLUDED

#define ZC_FILTER_START_CONST (9 * TICKS_PER_US)

register uint8_t zc_filter asm("r2");

const uint8_t zc_filter_table[64]=   {0 ,2 ,4 ,6 ,8 ,10,12,14,16,18,20,22,24,26,28,30,
                                      32,34,36,38,40,42,44,46,48,50,52,54,56,58,60,62,
                                      0 ,2 ,4 ,6 ,8 ,10,12,14,16,18,1 ,22,1 ,26,28,30,
                                      32,34,36,38,1 ,42,44,46,1 ,1 ,1 ,54,56,58,60,62};


void update_timing(uint16_t tick) {
  uint16_t comm_time = __interval(last_tick, tick);
  last_tick = tick;

   //est_comm_time = comm_time;
  //est_comm_time += (comm_time - est_comm_time) >> 2;

  est_comm_time = (est_comm_time + comm_time) >> 1;
  last_comm_time = comm_time;

  //est_comm_time = (last_comm_time_01 + last_comm_time_02 + last_comm_time_03 + comm_time) >> 2;
  //last_comm_time_01 = last_comm_time_02;
  //last_comm_time_02 = last_comm_time_03;
  //last_comm_time_03 = comm_time;
}


void correct_timing(uint16_t tick) {
  uint16_t comm_time = __interval(last_tick, tick);
  last_tick = tick;
  comm_time = comm_time >> 1;
  est_comm_time = comm_time;
  last_comm_time = comm_time;
}

inline void zc_filter_start_reset() {
  zc_filter = ZC_FILTER_START_CONST;
}

inline uint8_t zc_kickback_end(uint8_t state) {
  if (state & 1) {
    return !pwr_stage.aco;
  } else {
    return pwr_stage.aco;
  }
}

uint8_t zc_start_detected(uint8_t state) {
  if (state & 1) {
    if (pwr_stage.aco) zc_filter--; else zc_filter++;
  } else {
    if (!pwr_stage.aco) zc_filter--; else zc_filter++;
  }
  if (zc_filter > (ZC_FILTER_START_CONST)) zc_filter = ZC_FILTER_START_CONST;
  return (zc_filter == 0);
}

inline void zc_filter_run_reset() {
  zc_filter = 0;
}

uint8_t zc_run_detected() __attribute__ ((noinline));
uint8_t zc_run_detected() {
  uint8_t v = zc_filter;
  if (!pwr_stage.aco) v |= 0x01;
  v = zc_filter_table[v];
  zc_filter = v;
  return (v & 0x01);
}

#endif // ZC_H_INCLUDED
