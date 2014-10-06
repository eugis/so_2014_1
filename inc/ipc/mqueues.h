#ifndef __MQUEUES_H__
#define __MQUEUES_H__

#define SYSV_KEY_QUEUE 1
#define MESSAGE_BUFFER_SIZE (1024 * 10) /* Only one exists */

typedef struct {
    uint16_t id; /* PID of process, or 1 for server */
    uint16_t server_id; /* Always 1 */
    int queue;
} ipc_t;

typedef struct {
    long recipient_id;

    /* Copied from message_t (can't include it) */
    uint16_t sender;
    uint16_t content_length;
    char content[];

} ipc_queue_item_t;

#endif