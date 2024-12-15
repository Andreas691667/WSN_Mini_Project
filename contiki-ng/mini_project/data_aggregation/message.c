typedef struct {
    uint16_t node_id;
    int sample;
    double variance;
    double mean;
    unsigned long msg_id;
} message_t;

typedef struct {
    uint16_t node_id;
    double sample;
    int missed_samples;
    unsigned long msg_ids_in_msg[3];
    uint16_t node_ids_in_msg[3];
} message_agg_t;

void print_message(message_t node)
{
    // handle doubles
    int a_mean = node.mean;
    int a_variance = node.variance;
    double b_mean = (node.mean - a_mean) * 1000;
    double b_variance = (node.variance - a_variance) * 1000;

    // print message
    printf("source, node_id: %d, sample: %d, mean: %d.%03u, variance: %d.%03u, msg_id: %lu #\n", 
    node.node_id, 
    node.sample, 
    a_mean, 
    (unsigned int)b_mean, 
    a_variance, 
    (unsigned int)b_variance,
    node.msg_id);
}

void print_message_agg(message_agg_t node)
{
    // handle doubles
    int a_sample = node.sample;
    double b_sample = (node.sample - a_sample) * 1000;

    // print message
    printf("aggregator, node_id: %d, sample: %d.%03u, missed_samples: %d, msg_ids_in_msg: [%lu, %lu, %lu], node_ids_in_msg: [%d, %d, %d] #\n", 
    node.node_id, 
    a_sample, 
    (unsigned int)b_sample, 
    node.missed_samples,
    node.msg_ids_in_msg[0],
    node.msg_ids_in_msg[1],
    node.msg_ids_in_msg[2],
    node.node_ids_in_msg[0],
    node.node_ids_in_msg[1],
    node.node_ids_in_msg[2]);
}