
#ifndef _OLSR_FUNCTIONS
#define _OLSR_FUNCTIONS

#include "olsr_protocol.h"
#include "interfaces.h"

extern bool changes_topology;
extern bool changes_neighborhood;
extern bool changes_hna;
extern bool changes_force;

extern union olsr_ip_addr all_zero;

void olsr_startup_sleep(int);
void olsr_do_startup_sleep(void);

void register_pcf(int (*)(int, int, int));

void olsr_process_changes(void);

void init_msg_seqno(void);

uint16_t get_msg_seqno(void);

bool olsr_is_bad_duplicate_msg_seqno(uint16_t seqno);

int olsr_forward_message(union olsr_message *, struct interface_olsr *, union olsr_ip_addr *);

void set_buffer_timer(struct interface_olsr *);

void olsr_init_tables(void);

void olsr_init_willingness(void);

void olsr_update_willingness(void *);

uint8_t olsr_calculate_willingness(void);

const char *olsr_msgtype_to_string(uint8_t);

const char *olsr_link_to_string(uint8_t);

const char *olsr_status_to_string(uint8_t);

void olsr_exit(const char *, int) __attribute__((noreturn));

void *olsr_malloc(size_t, const char *);

int olsr_printf(int, const char *, ...) __attribute__ ((format(printf, 2, 3)));

void olsr_trigger_forced_update(void *);

#endif /* _OLSR_FUNCTIONS */

/*
 * Local Variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */
