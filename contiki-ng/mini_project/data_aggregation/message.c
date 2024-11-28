typedef struct {
    uint16_t node_id;
    int sample;
    double variance;
    double mean;
} message_t;

typedef struct {
    uint16_t node_id;
    double sample;
    int missed_samples;
} message_agg_t;

void print_message(message_t node)
{
    // handle doubles
    int a_mean = node.mean;
    int a_variance = node.variance;
    double b_mean = (node.mean - a_mean) * 1000;
    double b_variance = (node.variance - a_variance) * 1000;

    // print message
    printf("source, node_id: %d, sample: %d, mean: %d.%03u, variance: %d.%03u #\n", node.node_id, node.sample, a_mean, (unsigned int)b_mean, a_variance, (unsigned int)b_variance);
}

void print_message_agg(message_agg_t node)
{
    // handle doubles
    int a_sample = node.sample;
    double b_sample = (node.sample - a_sample) * 1000;

    // print message
    printf("aggregator, node_id: %d, sample: %d.%03u, missed_samples: %d #\n", node.node_id, a_sample, (unsigned int)b_sample, node.missed_samples);
}