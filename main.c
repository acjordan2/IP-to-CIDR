#include <arpa/inet.h>

#include <assert.h>
#include <err.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

#define IPv4_BITS 32

static inline int min(int a, int b) {
    return (a < b ? a : b);
}

static void print_cidr(const char *start_ip, const char *end_ip) {
    struct in_addr ip;
    uint32_t start, end;
    int zero_bits, bits;

    if (!inet_aton(start_ip, &ip))
        err(1, "inet_aton() for %s failed", start_ip);
    start = ntohl(ip.s_addr);
    
    if (!inet_aton(end_ip, &ip))
        err(1, "inet_aton() for %s failed", end_ip);
    end = ntohl(ip.s_addr);

    if (start > end)
        errx(1, "bad ip order: %s  - %s", start_ip, end_ip);

    while (start <= end) {
        /* Trailing zeros in IP determine max possible
         * prefix size for this IP
         */
        zero_bits = start == 0 ? IPv4_BITS : ffs(start) - 1;
        
        /* ip count in this prefix: 2^bits */
        bits = min(fls(end - start + 1) - 1, zero_bits);

        ip.s_addr = htonl(start);
        printf("%s/%d\n", inet_ntoa(ip), IPv4_BITS-bits);

        start += 1 << bits;
    }

    return;
}

int main (int argc, char *argv[]) {
    
 char buf[BUFSIZ];


    //struct in_addr ip;
    struct sockaddr_in ip;
    FILE *input, *line_count;
    char *line, previous_line[16], min_ip[INET_ADDRSTRLEN], max_ip[INET_ADDRSTRLEN];
    size_t len = 0;
    ssize_t read;
    int count = 0, count_lines = 0, i = 0, c;
    uint32_t current_ip, previous_ip;
 
    /* check that ip fits into int */
    assert(sizeof(int) >= sizeof(uint32_t));

    if (isatty(fileno(stdin))) {
        if (argc !=2) {
            printf("Usage: %s input__file\n", argv[0]);
            exit(0);
        }

        input = fopen(argv[1], "r");
        if (input == NULL)
            exit(EXIT_FAILURE);

        while ((read = getline(&line, &len, input) != -1)) {
             line[strlen(line) - 1] = 0;
             if (inet_pton(AF_INET, line, &(ip.sin_addr)) < 0)
                err(1, "inet_pton() for %s failed", line);

            current_ip = ntohl(ip.sin_addr.s_addr);

            if (count == 0) {
                if (inet_pton(AF_INET, line, &(ip.sin_addr)) < 1)
                    err(1, "inet_aton() for %s failed", line);
                inet_ntop(AF_INET, &(ip.sin_addr), min_ip, INET_ADDRSTRLEN);
            }
            else if ((count > 0 && (current_ip - previous_ip) > 1)) {
                if (inet_pton(AF_INET, previous_line, &(ip.sin_addr)) < 1)
                    err(1, "inet_aton() for %s failed", previous_line);
                inet_ntop(AF_INET, &(ip.sin_addr), max_ip, INET_ADDRSTRLEN); 
                print_cidr(min_ip, max_ip);          
                count = 0;
                
                if (inet_pton(AF_INET, line, &(ip.sin_addr)) < 1)
                    err(1, "inet_aton() for %s failed", line);
                inet_ntop(AF_INET, &(ip.sin_addr), min_ip, INET_ADDRSTRLEN);
            } 
            previous_ip = current_ip;     
            memcpy(previous_line, line, 16); 
            
            count++;
        }

        if (inet_pton(AF_INET, previous_line, &(ip.sin_addr)) < 1)
            err(1, "inet_aton() for %s failed", previous_line);
        inet_ntop(AF_INET, &(ip.sin_addr), max_ip, INET_ADDRSTRLEN); 
        print_cidr(min_ip, max_ip);          
                

        fclose(input);
        if (line)
            free(line);
    } else {
        while(fgets(buf, sizeof buf, stdin)) {
            if (buf[strlen(buf)-1] == '\n') {
                line = buf;
                line[strlen(line) - 1] = 0;
                if (inet_pton(AF_INET, line, &(ip.sin_addr)) < 0)
                    err(1, "inet_pton() for %s failed", line);

                current_ip = ntohl(ip.sin_addr.s_addr);

                if (count == 0) {
                    if (inet_pton(AF_INET, line, &(ip.sin_addr)) < 1)
                        err(1, "inet_aton() for %s failed", line);
                    inet_ntop(AF_INET, &(ip.sin_addr), min_ip, INET_ADDRSTRLEN);
                } else if ((count > 0 && (current_ip - previous_ip) > 1)) {
                    if (inet_pton(AF_INET, previous_line, &(ip.sin_addr)) < 1)
                        err(1, "inet_aton() for %s failed", previous_line);
                    inet_ntop(AF_INET, &(ip.sin_addr), max_ip, INET_ADDRSTRLEN); 
                    print_cidr(min_ip, max_ip);          
                    count = 0;
                
                    if (inet_pton(AF_INET, line, &(ip.sin_addr)) < 1)
                        err(1, "inet_aton() for %s failed", line);
                    inet_ntop(AF_INET, &(ip.sin_addr), min_ip, INET_ADDRSTRLEN);
                } 
                previous_ip = current_ip;   
                memcpy(previous_line, line, 16); 
                
                count++;
            }
        
        }
        if (inet_pton(AF_INET, line, &(ip.sin_addr)) < 1)
            err(1, "inet_aton() for %s failed", previous_line);
        inet_ntop(AF_INET, &(ip.sin_addr), max_ip, INET_ADDRSTRLEN); 
        print_cidr(min_ip, max_ip);          
                
    }
    exit(EXIT_SUCCESS);
}


