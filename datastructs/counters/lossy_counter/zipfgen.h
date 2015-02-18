/****************************************
        Author: Tim Wood
                Chenghu He
        with a little help from
        http://beej.us/guide/bgnet/
        with a source of zipf generator
        www.csee.usf.edu/~christen/tools/genzipf.c
****************************************/

struct send_info {
        char *addr;
        char *port;
        char *alpha;
        char *nval;
};

int get_zipf_key(double alpha, int n);
int send_key_tcp(struct send_info *info);


