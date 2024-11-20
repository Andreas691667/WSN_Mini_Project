typedef struct {
    uint16_t node_id;
    int sample;
    double variance;
    double mean;
} message_t;

void print_message(message_t node)
{
    // handle doubles
    int a_mean = node.mean;
    int a_variance = node.variance;
    double b_mean = (node.mean - a_mean) * 1000;
    double b_variance = (node.variance - a_variance) * 1000;

    // print message
    printf("node_id: %d, sample: %d, mean: %d.%03u, variance: %d.%03u\n", node.node_id, node.sample, a_mean, (unsigned int)b_mean, a_variance, (unsigned int)b_variance);
}