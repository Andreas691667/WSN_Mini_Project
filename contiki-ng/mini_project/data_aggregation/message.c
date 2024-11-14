struct {
    uint16_t node_id;
    int sample;
    double variance;
    double mean;
} message;

void print_message(int node_id, int sample, double mean, double variance)
{
    // handle doubles
    int a_mean = mean;
    int a_variance = variance;
    double b_mean = (mean - a_mean) * 1000;
    double b_variance = (variance - a_variance) * 1000;

    // print message
    printf("node_id: %d, sample: %d, mean: %d.%03u, variance: %d.%03u\n", node_id, sample, a_mean, (unsigned int)b_mean, a_variance, (unsigned int)b_variance);
}