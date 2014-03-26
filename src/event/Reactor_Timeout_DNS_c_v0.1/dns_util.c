#if 0
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<unistd.h>
#include "dns_util.h"


unsigned char *ReadName(unsigned char *reader, unsigned char *buffer, int *count);
void ChangetoDnsNameFormat(unsigned char* dns, unsigned char* host);
void ChangeDnsNameFormatoString(unsigned char *dns, unsigned char *host);
void create_dns_query(unsigned char *host, int query_type, unsigned char *buf, int *question_len)
{
	struct dns_header *dns;
	unsigned char *qname = NULL;
	struct question* qinfo = NULL;

	dns = (struct dns_header*)buf;

	dns->id = (unsigned short) htons(getpid());
	dns->qr = 0;
	dns->opcode = 0;
	dns->aa = 0;
	dns->tc = 0;
	dns->rd = 1;
	dns->ra = 0;
	dns->z = 0;
	dns->ad = 0;
	dns->cd = 0;
	dns->rcode = 0;
	dns->q_count = htons(1);
	dns->ans_count = 0;
	dns->auth_count = 0;
	dns->add_count = 0;

	qname = (unsigned char *)&buf[sizeof(struct dns_header)];

	ChangetoDnsNameFormat(qname, host);

	qinfo = (struct question *)&buf[sizeof(struct dns_header) + (strlen((const char *)qname) + 1)];	
	
	qinfo->qtype = htons(query_type);
	qinfo->qclass = htons(1);
	*question_len = strlen((const char *)qname);
	return;
}
#if 0
void parse_response(unsigned char *buf, size_t qname_len) //strlen((const char *)qname) == qname_len ,and qname end with '\0', so it will add 1 when count!
{
	struct dns_header *dns = NULL;
	
	nread = read(
	
	dns = (struct dns_header *)buf;


	reader = &buf[sizeof(struct dns_header) + (qname_len + 1) + sizeof(struct question)]; 

#if 0
	fprintf(stderr, "The response contains: ");
	fprintf(stderr, "\n %d Questions.", noths(dns->q_count));
	fprintf(stderr, "\n %d Answers", ntohs(dns->q_count));
	fprintf(stderr, "\n %d Authoritative servers.", ntohs(dns->auth_count));
	fprintf(stderr, "\n %d Additional records\n\n", ntohs(dns->add_count));
#endif
}
#endif


#if 0
u_char* ReadName(unsigned char* reader,unsigned char* buffer,int* count)
{
}
#endif

#if 0
//
//这个先不做，先默认是8.8.8.8:53
//

/*
 * Get the DNS servers from /etc/resolv.conf file on Linux
 * 在初始化的时候，从/etc/resolv.conf读取dns server ，设置标志位，使用gethostbyname（如果是url的话），成功，标志位为1，并把该dnsserver添加到http_resolve_t中
 * 对于端口，如果有端口就使用端口，没有就默认给53
 * */
void get_dns_servers(struct dns_server** DSserver)
{
	struct dns_server *new;
	FILE *fp;
	char line[200] , *p;
	if((fp = fopen("/etc/resolv.conf" , "r")) == NULL)
	{
		printf("Failed opening /etc/resolv.conf file \n");
	}

	while(fgets(line , 200 , fp))
	{
		if(line[0] == '#')
		{
			continue;
		}
		if(strncmp(line , "nameserver" , 10) == 0)
		{
			p = strtok(line , " ");
			p = strtok(NULL , " ");
			new = (struct dns_server *)malloc(sizeof(struct dns_server));
			if (NULL == new) {
				continue;
			}
			strcpy(DServer->host, p);
			strcpy(DServer->dot_addr, p);
			DServer->port = 53;
			DServer->quick = 1;
		}
	}
}
#endif

/*
 * This will convert "www.google.com" to "\3www\6google\3com\0" 
 * got it :)
 * */
void ChangetoDnsNameFormat(unsigned char* dns, unsigned char* host) 
{
	int lock = 0 , i;
	strcat((char*)host,".");

	for(i = 0 ; i < strlen((char*)host) ; i++) 
	{
		if(host[i]=='.') 
		{
			*dns++ = (unsigned char)(i-lock);
			for(;lock<i;lock++) 
			{
				*dns++=host[lock];
			}
			lock++; //or lock=i+1;
		}
	}
	*dns++='\0';
}

/*
*/
void ChangeDnsNameFormatoString(unsigned char *dns, unsigned char *host)
{
	int i = 0;
	int j = 0;
	char p = '\0';
	
	for(i=0;i<(int)strlen((const char*)dns);i++) 
	{
		p=dns[i];
		for(j=0;j<(int)p;j++) 
		{
			dns[i]=dns[i+1];
			i=i+1;
		}
		dns[i]='.';
	}
	dns[i-1]='\0'; //remove the last dot
}


static int do_parse_dns(struct resolver_result *result, unsigned char *buf)
{
	struct dns_header *dns = NULL;
	struct res_record answers[20];
	struct res_record auth[20];
	struct res_record addit[20];
	struct sockaddr_in a;
	int i = 0;
	int j = 0;
		
	
	
	dns = (struct dns_header *)buf;
	
	unsigned char *reader = NULL;

	reader = &buf[sizeof(struct dns_header) + (result->question_len + 1) + sizeof(struct question)];

	printf("\n The respose contains:");
	printf("\n Questions: %d", ntohs(dns->q_count));
	printf("\n Answers: %d", ntohs(dns->ans_count));
	printf("\n Authoritative Servers: %d", ntohs(dns->auth_count));
	printf("\n Additional records: %d", ntohs(dns->add_count));

	int stop = 0;

	for (i = 0; i< ntohs(dns->ans_count); i++)
	{
		answers[i].name = ReadName(reader, buf, &stop);
		reader = reader + stop;
		
		answers[i].resource = (struct r_data *)(reader);
		reader = reader + sizeof(struct r_data);

		if (ntohs(answers[i].resource->type) == 1) {
			answers[i].rdata = (unsigned char *)malloc(ntohs(answers[i].resource->data_len));
			
			for(j = 0; j < ntohs(answers[i].resource->data_len); j++)
			{
				answers[i].rdata[j] = reader[j];
			}

			answers[i].rdata[ntohs(answers[i].resource->data_len)] = '\0';
			reader = reader + ntohs(answers[i].resource->data_len);
		}

		else
		{
			addit[i].rdata = ReadName(reader, buf, &stop);
			reader += stop;
		}
	}


	printf("\n Answer Records: %d\n", ntohs(dns->ans_count));

	for (i = 0; i < ntohs(dns->ans_count); i++)
	{
		printf("Name: %s", answers[i].name);

		if (ntohs(answers[i].resource->type) == T_A) {
			long *p;
			p = (long *)answers[i].rdata;

			a.sin_addr.s_addr = (*p);
			printf("\thas IPv4 address : %s\n", inet_ntoa(a.sin_addr));
		}

		if (ntohs(answers[i].resource->type) == 5) {
			printf("\thas alias name : %s\n", answers[i].rdata);
		}

	}

	printf("\nAuthoritive Records: %d\n", ntohs(dns->auth_count));
	for (i = 0; i < ntohs(dns->auth_count); i++)
	{
		printf("Name: %s", auth[i].name);

		if (ntohs(auth[i].resource->type) == 2)
		{
			printf("\thas nameserver : %s", auth[i].rdata);
		}
		
		printf("\n");
	}


	printf("\nAdditional Records: %d\n", ntohs(dns->add_count));

	for (i = 0; i < ntohs(dns->add_count); i++) 
	{
		printf("Name: %s", addit[i].name);
	
		if (ntohs(addit[i].resource->type) == 1) {
			long *p;
			p = (long *)addit[i].rdata;
			a.sin_addr.s_addr = (*p);
			printf("\thas IPv4 address: %s", inet_ntoa(a.sin_addr));
		}
		printf("\n");
	}
	
	return 0;
}

void parse_dns(int fd, short events, void *arg)
{
	struct resolver_result* result = (struct resolver_result *)arg;
	unsigned char buf[65536] = {0};
	ssize_t nread = 0;

	//read data
	nread = read(fd, buf, 65536);
	
	if (nread < 0) {
		fprintf(stderr, "Wrong Dns response!\n");
		return ;
	}
	
	do_parse_dns(result, buf);
}

unsigned char *ReadName(unsigned char *reader, unsigned char *buffer, int *count)
{
	unsigned char *name;
	unsigned int p = 0;
	unsigned int jumpd = 0;
	unsigned int offset = 0;
	int i = 0;
	int j = 0;

	*count = 1;

	name = (unsigned char *)malloc(256);

	name[0] = '\0';

	while(*reader != 0)
	{
		if (*reader >= 192) {
			offset = (*reader) * 256 + *(reader + 1) - 49152;
			reader = buffer + offset -1;

			jumpd = 1;
		}	else {
			name[p++] = *reader;
		}

		reader = reader + 1;

		if (jumpd == 0) {
			*count = *count + 1;
		}
	}

	name[p] = '\0';

	if (jumpd == 1)
	{
		*count = *count + 1;
	}

	for (i = 0; i < (int)strlen((const char *)name); i++)
	{
		p = name[i];
		for (j = 0; j < (int)p; j ++) {
			name[i] = name[i + 1];
			i = i + 1;
		}

		name[i] = '.';
	}

	name[i - 1] = '\0';
	return name;
}


#endif
#include <getopt.h>
#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "network.h"
#include "tcp.h"
#include "rtypes.h"
#include "strutils.h"

// If you want a reasonable place to start walking through the code, 
// go to the 'handler' function.
                    
#define DEFAULT_TCP_STATE_PATH "/tmp/dnsparse_tcp.state"
void handler(uint8_t *, const struct pcap_pkthdr *, const uint8_t *);
void dns_rr_free(dns_rr *);
void dns_question_free(dns_question *);
uint32_t parse_rr(uint32_t, uint32_t, struct pcap_pkthdr *, 
                  uint8_t *, dns_rr *, config *);
void print_rr_section(dns_rr *, char *, config *);
void print_packet(uint32_t, uint8_t *, uint32_t, uint32_t, u_int);
int dedup(uint32_t, struct pcap_pkthdr *, uint8_t *,
          ip_info *, transport_info *, config *);

int main(int argc, char **argv) {
    pcap_t * pcap_file;
    char errbuf[PCAP_ERRBUF_SIZE];
    int read;
    config conf;
    
    int c;
    char *cvalue = NULL;
    int print_type_freq = 0;
    int arg_failure = 0;

    const char * OPTIONS = "cdfhm:MnurtD:x:s:S";

    // Setting configuration defaults.
    uint8_t TCP_SAVE_STATE = 1;
    conf.COUNTS = 0;
    conf.EXCLUDES = 0;
    conf.RECORD_SEP = "";
    conf.SEP = '\t';
    conf.AD_ENABLED = 0;
    conf.NS_ENABLED = 0;
    conf.PRETTY_DATE = 0;
    conf.PRINT_RR_NAME = 0;
    conf.MISSING_TYPE_WARNINGS = 0;
    conf.TCP_STATE_PATH = NULL;
    conf.DEDUPS = 10;
    conf.dedup_pos = 0;

    c = getopt(argc, argv, OPTIONS);
    while (c != -1) {
        switch (c) {
            case 'c':
                conf.COUNTS = 1;
                break;
            case 'd':
                conf.AD_ENABLED = 1;
                break;
            case 'f':
                print_parsers();
                return 0;
            case 'm':
                conf.RECORD_SEP = optarg;
                conf.SEP = '\n';
                break;
            case 'M':
                conf.MISSING_TYPE_WARNINGS = 1;
                break;
            case 'n':
                conf.NS_ENABLED = 1;
                break;
            case 'r':
                conf.PRINT_RR_NAME = 1;
                break;
            case 's':
                conf.TCP_STATE_PATH = optarg;
                break;
            case 'S':
                TCP_SAVE_STATE = 0;
                break;
            case 't':
                conf.PRETTY_DATE = 1; 
                break;
            case 'u':
                print_type_freq = 1;
                break;
            case 'D':
                conf.DEDUPS = strtoul(optarg, NULL, 10);
                if (conf.DEDUPS > 10000) {
                    conf.DEDUPS = 10000;
                }
                break;
            case 'x':
                if (conf.EXCLUDES < MAX_EXCLUDES) {
                    int ival = atoi(optarg);
                    if (ival == 0 || ival >= 65536) {
                        fprintf(stderr, "Invalid excluded rtype value. "
                                "Value must be a short int.\n");
                        arg_failure = 1;
                    } else {
                        conf.EXCLUDED[conf.EXCLUDES] = ival;
                        conf.EXCLUDES++; 
                    }
                } else {
                    fprintf(stderr, "Too many excluded rtypes. "
                            "If this limit is an issue, then recompile with "
                            "the MAX_EXCLUDES define set higher.\n");
                    arg_failure = 1;
                }
                break;
            case '?':
                if (optopt == 'x') 
                    fprintf(stderr, "Option -x requires an rtype number.\n");
                else if (optopt == 'm')
                    fprintf(stderr, "Option -m needs a delimiter string.\n");
                else if (isprint(optopt)) 
                    fprintf(stderr, "Unknown option -%c.\n",optopt); 
                else 
                    fprintf(stderr, "Invalid option char: 0x%x.\n", optopt);
            case 'h':
            default:
                arg_failure = 1;
        }
        c = getopt(argc, argv, OPTIONS);
    }

    if (conf.TCP_STATE_PATH == NULL) {
        conf.TCP_STATE_PATH = DEFAULT_TCP_STATE_PATH;
    }

    if (optind == argc - 1) {
        pcap_file = pcap_open_offline(argv[optind], errbuf);
        if (pcap_file == NULL) {
            fprintf(stderr, "Could not open pcapfile.\n%s\n", errbuf);
            return -1;
        }
    } else if (optind >= argc) {
        fprintf(stderr, "No input file specified.\n");
        arg_failure = 1;
    } else {
        fprintf(stderr, "Multiple input files or bad arguments.");
        arg_failure = 1;
    }
    
    if (arg_failure) {
        fprintf(stderr,
        "Usage: dns_parse [-dnthf] [-m<query sep.>] [-x<rtype>] [-s<path>]\n"
        "                 <pcap file>\n"
        "dns_parse parses a pcap file and gives a nicely "
        "formatted ascii string for each dns request.\n"
        "By default the reservation records are tab separated "
        "and the entire record is ended with a newline.\n\n"
        "The comma separated fields printed for each request are:\n"
        "  time - The time of the request relative to the \n"
        "         capture source clock.\n"
        "  srcip, dstip - the source and dest ipv4 addresses.\n"
        "                 ipv6 support is not present.\n"
        "  size - the size of the dns portion of the message.\n"
        "  proto - udp (u) or tcp(t)\n"
        "  query/response - is it a query(q) or response(r)\n"
        "  authoritative - marked with AA if authoritative\n\n"
        "The resource records are printed after these fields, separated by\n"
        "a tab (a newline in multiline mode). \n"
        "By default the resource record format is:\n"
        "<section> <name> <type> <class> <rdata>\n\n"
        "<section> is a symbol denoting record type.\n"
        "    ? - Questions (No rdata is included or printed).\n"
        "    ! - Answers\n"
        "    $ - Name Servers\n"
        "    + - Additional\n"
        "The rdata is parsed by a custom parser that depends on the\n"
        "record type and class. Use the -f option to get a list of\n"
        "the supported record types and documentation on the parsers.\n\n"
        "Args:\n"
        "<pcapfile> - The pcapfile to parse. Use a '-' for stdin\n"
        "-c\n"
        "   Append a list of counts for each record type (Questions, \n"
        "   Answers, etc) to record fields. Each type is followed by it's\n"
        "   record type symbol.\n"
        "-d\n"
        "   Enable the parsing and output of the Additional\n"
        "   Records section. Disabled by default.\n"
        "-D <count>\n"
        "   Keep hashes of the last <count> packets for de-duplication\n"
        "   purposes (max 10,000). A <count> of zero turns off \n"
        "   de-duplication. \n"
        "   Default 10.\n"
        "-f\n"
        "   Print out documentation on the various resource \n"
        "   record parsers.\n"
        "-n\n"
        "   Enable the parsing and output of the Name Server\n"
        "   Records section. Disabled by default.\n"
        "-m<sep> \n"
        "   Multiline mode. Reservation records are newline\n"
        "   separated, and the whole record ends with the\n"
        "   separator given.\n"
        "-M \n"
        "   Print a message for each occurance of a missing class,type\n"
        "   parser.\n"
        "-r \n"
        "   Changes the resource record format to: \n"
        "   <section> <name> <rr_type_name> <rdata>\n"
        "   If the record type isn't known, 'UNKNOWN(<cls>,<type>)' is given\n"
        "   The query record format is the similar, but missing the rdata.\n"
        "-s<path> \n"
        "   Path to the tcp state save file. \n"
        "   This will be loaded (and overwritten) every time dns_parse \n"
        "   is run. \n"
        "   Default is: %s \n"
        "-S \n"
        "   Disable TCP state saving/loading.\n"
        "-t \n"
        "   Print the time/date as in Y-m-d H:M:S (ISO 8601) format.\n"
        "   The time will be in the local timezone.\n"
        "-u \n"
        "   Print a record of the how many occurances of each class,type\n"
        "   record occurred via stderr when processing completes.\n"
        "-x\n"
        "   Exclude the given reservation record types by \n"
        "   number. This option can be given multiple times.\n"
        "\n"
        "Supported protocols:\n"
        "DNS can ride on a number of protocols, and dns_parse supports\n"
        "a fair number of them, including:\n"
        "Ethernet, MPLS, IPv4, IPv6, UDP and TCP.\n"
        "IPv4 and IPv6 fragments - fragments are reassembled, but data\n"
        "   may be lost if the fragments are split across multiple pcaps.\n"
        "TCP reassembly - TCP packets are reassembled, but the resulting\n"
        "   data may be offset from their time of occurance. Partial flow\n"
        "   reassembly is supported; long flows are printed whenever a \n"
        "   a lull in that flow occurs (500 ms since the last packet, \n"
        "   this can only be changed at compile time).\n"
        "   TCP flow state is saved at the end of execution, and loaded\n"
        "   at the beginning. See the -S option to disable.\n",
        DEFAULT_TCP_STATE_PATH);
        return -1;
    }

    conf.ip_fragment_head = NULL;

    // Load and prior TCP session info
    conf.tcp_sessions_head = NULL; 
    if (TCP_SAVE_STATE == 1) {
        tcp_load_state(&conf);
    }

    conf.dedup_hashes = calloc(conf.DEDUPS, sizeof(uint64_t));
 
    // need to check this for overflow.
    read = pcap_dispatch(pcap_file, -1, (pcap_handler)handler, 
                         (uint8_t *) &conf);
    
    if (TCP_SAVE_STATE == 1) {
        tcp_save_state(&conf);
    } else {
        tcp_expire(&conf, NULL);
    }

    free(conf.dedup_hashes);
    ip_frag_free(&conf);

    return 0;
}

void handler(uint8_t * args, const struct pcap_pkthdr *orig_header, 
             const uint8_t *orig_packet) {
    int pos;
    eth_info eth;
    ip_info ip;
    config * conf = (config *) args;

    // The way we handle IP fragments means we may have to replace
    // the original data and correct the header info, so a const won't work.
    uint8_t * packet = (uint8_t *) orig_packet;
    struct pcap_pkthdr header;
    header.ts = orig_header->ts;
    header.caplen = orig_header->caplen;
    header.len = orig_header->len;
    
    VERBOSE(printf("\nPacket %llu.%llu\n", 
                   (uint64_t)header.ts.tv_sec, 
                   (uint64_t)header.ts.tv_usec);)
    
    // Parse the ethernet frame. Errors are typically handled in the parser
    // functions. The functions generally return 0 on error.
    pos = eth_parse(&header, packet, &eth);
    if (pos == 0) return;

    // MPLS parsing is simple, but leaves us to guess the next protocol.
    // We make our guess in the MPLS parser, and set the ethtype accordingly.
    if (eth.ethtype == 0x8847) {
        pos = mpls_parse(pos, &header, packet, &eth);
    } 

    // IP v4 and v6 parsing. These may replace the packet byte array with 
    // one from reconstructed packet fragments. Zero is a reasonable return
    // value, so they set the packet pointer to NULL on failure.
    if (eth.ethtype == 0x0800) {
        pos = ipv4_parse(pos, &header, &packet, &ip, conf);
    } else if (eth.ethtype == 0x86DD) {
        pos = ipv6_parse(pos, &header, &packet, &ip, conf);
    } else {
        fprintf(stderr, "Unsupported EtherType: %04x\n", eth.ethtype);
        return;
    }
    if (packet == NULL) return;

    // Transport layer parsing. 
    if (ip.proto == 17) {
        // Parse the udp and this single bit of DNS, and output it.
        dns_info dns;
        transport_info udp;
        pos = udp_parse(pos, &header, packet, &udp, conf);
        if ( pos == 0 ) return;
        // Only do deduplication if DEDUPS > 0.
        if (conf->DEDUPS != 0 ) {
            if (dedup(pos, &header, packet, &ip, &udp, conf) == 1) {
                // A duplicate packet.
                return;
            }
        }
        pos = dns_parse(pos, &header, packet, &dns, conf, !FORCE);
        print_summary(&ip, &udp, &dns, &header, conf);
    } else if (ip.proto == 6) {
        // Hand the tcp packet over for later reconstruction.
        tcp_parse(pos, &header, packet, &ip, conf); 
    } else {
        fprintf(stderr, "Unsupported Transport Protocol(%d)\n", ip.proto);
        return;
    }
   
    if (packet != orig_packet) {
        // Free data from artificially constructed packets.
        free(packet);
    }

    // Expire tcp sessions, and output them if possible.
    DBG(printf("Expiring TCP.\n");)
    tcp_expire(conf, &header.ts);
}

// Output the DNS data.
void print_summary(ip_info * ip, transport_info * trns, dns_info * dns,
                   struct pcap_pkthdr * header, config * conf) {
    char date[200];
    char proto;

    uint32_t dnslength;
    dns_rr *next;
    dns_question *qnext;

#if 0
    // Print the time stamp.
    if (conf->PRETTY_DATE) {
        struct tm *time;
        size_t result;
        char t_date[200];
        const char * format = "%F %T";
        time = localtime(&(header->ts.tv_sec));
        result = strftime(t_date, 200, format, time);
        if (result == 0) strncpy(date, "Date format error", 20);
        sprintf(date, "%s.%06d", t_date, (int)header->ts.tv_usec);
    } else 
        sprintf(date, "%d.%06d", (int)header->ts.tv_sec, 
                                 (int)header->ts.tv_usec);
   
#endif
    // Print the transport protocol indicator.
    if (ip->proto == 17) {
        proto = 'u';
    } else if (ip->proto == 6) {
        proto = 't';
    }
    fflush(stdout);
    dnslength = trns->length;

    // Print the IP addresses and the basic query information.
#if 0
    printf("%s,%s,", date, iptostr(&ip->src));
    printf("%s,%d,%c,%c,%s", iptostr(&ip->dst),
           dnslength, proto, dns->qr ? 'r':'q', dns->AA?"AA":"NA");
#endif
#if 0
    if (conf->COUNTS) {
        printf(",%u?,%u!,%u$,%u+", dns->qdcount, dns->ancount, 
                                   dns->nscount, dns->arcount);
    }
#endif

    // Go through the list of queries, and print each one.
    qnext = dns->queries;
    while (qnext != NULL) {
        printf("%c? ", conf->SEP);
        if (conf->PRINT_RR_NAME) {
            rr_parser_container * parser; 
            parser = find_parser(qnext->cls, qnext->type);
            if (parser->name == NULL) 
                printf("%s UNKNOWN(%s,%d)", qnext->name, parser->name, 
                                            qnext->type, qnext->cls);
            else 
                printf("%s %s", qnext->name, parser->name);
        } else
            printf("%s %d %d", qnext->name, qnext->type, qnext->cls);
        qnext = qnext->next; 
    }

    // Print it resource record type in turn (for those enabled).
    print_rr_section(dns->answers, "!", conf);
    if (conf->NS_ENABLED) 
        print_rr_section(dns->name_servers, "$", conf);
    if (conf->AD_ENABLED) 
        print_rr_section(dns->additional, "+", conf);
    printf("%c%s\n", conf->SEP, conf->RECORD_SEP);
    
    dns_question_free(dns->queries);
    dns_rr_free(dns->answers);
    dns_rr_free(dns->name_servers);
    dns_rr_free(dns->additional);
    fflush(stdout); fflush(stderr);
}

// Print all resource records in the given section.
void print_rr_section(dns_rr * next, char * name, config * conf) {
    int skip;
    int i;
    while (next != NULL) {
        // Print the rr seperator and rr section name.
        printf("%c%s", conf->SEP, name);
        skip = 0;
        // Search the excludes list to see if we should not print this
        // rtype.
        for (i=0; i < conf->EXCLUDES && skip == 0; i++) 
            if (next->type == conf->EXCLUDED[i]) skip = 1;
        if (!skip) {
            char *name, *data;
            name = (next->name == NULL) ? "*empty*" : next->name;
            data = (next->data == NULL) ? "*empty*" : next->data;
            if (conf->PRINT_RR_NAME) { 
                if (next->rr_name == NULL) 
                    // Handle bad records.
                    printf(" %s UNKNOWN(%d,%d) %s", name, next->type, 
                                                    next->cls, data);
                else
                    // Print the string rtype name with the rest of the record.
                    printf(" %s %s %s", name, next->rr_name, data);
            } else
                // The -r option case. 
                // Print the rtype and class number with the record.
                printf(" %s %d %d %s", name, next->type, next->cls, data);
        }
        next = next->next; 
    }
}

// Print packet bytes in hex.
// See dns_parse.h
void print_packet(uint32_t max_len, uint8_t *packet,
                  uint32_t start, uint32_t end, u_int wrap) {
    int i=0;
    while (i < end - start && (i + start) < max_len) {
        printf("%02x ", packet[i+start]);
        i++;
        if ( i % wrap == 0) printf("\n");
    }
    if ( i % wrap != 0) printf("\n");
    return;
}

// Free a dns_rr struct.
void dns_rr_free(dns_rr * rr) {
    if (rr == NULL) return;
    if (rr->name != NULL) free(rr->name);
    if (rr->data != NULL) free(rr->data);
    dns_rr_free(rr->next);
    free(rr);
}

// Free a dns_question struct.
void dns_question_free(dns_question * question) {
    if (question == NULL) return;
    if (question->name != NULL) free(question->name);
    dns_question_free(question->next);
    free(question);
}

// Parse the questions section of the dns protocol.
// pos - offset to the start of the questions section.
// id_pos - offset set to the id field. Needed to decompress dns data.
// packet, header - the packet location and header data.
// count - Number of question records to expect.
// root - Pointer to where to store the question records.
uint32_t parse_questions(uint32_t pos, uint32_t id_pos, 
                         struct pcap_pkthdr *header,
                         uint8_t *packet, uint16_t count, 
                         dns_question ** root) {
    uint32_t start_pos = pos; 
    dns_question * last = NULL;
    dns_question * current;
    uint16_t i;
    *root = NULL;

    for (i=0; i < count; i++) {
        current = malloc(sizeof(dns_question));
        current->next = NULL; current->name = NULL;

        current->name = read_rr_name(packet, &pos, id_pos, header->len);
        if (current->name == NULL || (pos + 2) >= header->len) {
            // Handle a bad DNS name.
            fprintf(stderr, "DNS question error\n");
            char * buffer = escape_data(packet, start_pos, header->len);
            const char * msg = "Bad DNS question: ";
            current->name = malloc(sizeof(char) * (strlen(buffer) +
                                                   strlen(msg) + 1));
            sprintf(current->name, "%s%s", msg, buffer);
            current->type = 0;
            current->cls = 0;
            if (last == NULL) *root = current;
            else last->next = current;
            return 0;
        }
        current->type = (packet[pos] << 8) + packet[pos+1];
        current->cls = (packet[pos+2] << 8) + packet[pos+3];
        
        // Add this question object to the list.
        if (last == NULL) *root = current;
        else last->next = current;
        last = current;
        pos = pos + 4;

        VERBOSE(printf("question->name: %s\n", current->name);)
        VERBOSE(printf("type %d, cls %d\n", current->type, current->cls);)
   }
    
    return pos;
}

// Parse an individual resource record, placing the acquired data in 'rr'.
// 'packet', 'pos', and 'id_pos' serve the same uses as in parse_rr_set.
// Return 0 on error, the new 'pos' in the packet otherwise.
uint32_t parse_rr(uint32_t pos, uint32_t id_pos, struct pcap_pkthdr *header, 
                  uint8_t *packet, dns_rr * rr, config * conf) {
    int i;
    uint32_t rr_start = pos;
    rr_parser_container * parser;
    rr_parser_container opts_cont = {0,0, opts};

    uint32_t temp_pos; // Only used when parsing SRV records.
    char * temp_data; // Also used only for SRV records.

    rr->name = NULL;
    rr->data = NULL;
    
    rr->name = read_rr_name(packet, &pos, id_pos, header->len);
    // Handle a bad rr name.
    // We still want to print the rest of the escaped rr data.
    if (rr->name == NULL) {
        const char * msg = "Bad rr name: ";
        rr->name = malloc(sizeof(char) * (strlen(msg) + 1));
        sprintf(rr->name, "%s", "Bad rr name");
        rr->type = 0;
        rr->rr_name = NULL;
        rr->cls = 0;
        rr->ttl = 0;
        rr->data = escape_data(packet, pos, header->len);
        return 0;
    }
    
    if ((header->len - pos) < 10 ) return 0;
    
    rr->type = (packet[pos] << 8) + packet[pos+1];
    rr->rdlength = (packet[pos+8] << 8) + packet[pos + 9];
    // Handle edns opt RR's differently.
    if (rr->type == 41) {
        rr->cls = 0;
        rr->ttl = 0;
        rr->rr_name = "OPTS";
        parser = &opts_cont;
        // We'll leave the parsing of the special EDNS opt fields to
        // our opt rdata parser.  
        pos = pos + 2;
    } else {
        // The normal case.
        rr->cls = (packet[pos+2] << 8) + packet[pos+3];
        rr->ttl = 0;
        for (i=0; i<4; i++)
            rr->ttl = (rr->ttl << 8) + packet[pos+4+i];
        // Retrieve the correct parser function.
        parser = find_parser(rr->cls, rr->type);
        rr->rr_name = parser->name;
        pos = pos + 10;
    }

    VERBOSE(printf("Applying RR parser: %s\n", parser->name);)

    if (conf->MISSING_TYPE_WARNINGS && &default_rr_parser == parser) 
        fprintf(stderr, "Missing parser for class %d, type %d\n", 
                        rr->cls, rr->type);
    
    // Make sure the data for the record is actually there.
    // If not, escape and print the raw data.
    if (header->len < (rr_start + 10 + rr->rdlength)) {
        char * buffer;
        const char * msg = "Truncated rr: ";
        rr->data = escape_data(packet, rr_start, header->len);
        buffer = malloc(sizeof(char) * (strlen(rr->data) + strlen(msg) + 1));
        sprintf(buffer, "%s%s", msg, rr->data);
        free(rr->data);
        rr->data = buffer;
        return 0;
    }
    // Parse the resource record data.
    rr->data = parser->parser(packet, pos, id_pos, rr->rdlength, 
                              header->len);
    VERBOSE(
    printf("rr->name: %s\n", rr->name);
    printf("type %d, cls %d, ttl %d, len %d\n", rr->type, rr->cls, rr->ttl,
           rr->rdlength);
    printf("rr->data %s\n", rr->data);
    )

    return pos + rr->rdlength;
}

// Parse a set of resource records in the dns protocol in 'packet', starting
// at 'pos'. The 'id_pos' offset is necessary for putting together 
// compressed names. 'count' is the expected number of records of this type.
// 'root' is where to assign the parsed list of objects.
// Return 0 on error, the new 'pos' in the packet otherwise.
uint32_t parse_rr_set(uint32_t pos, uint32_t id_pos, 
                         struct pcap_pkthdr *header,
                         uint8_t *packet, uint16_t count, 
                         dns_rr ** root, config * conf) {
    dns_rr * last = NULL;
    dns_rr * current;
    uint16_t i;
    *root = NULL; 
    for (i=0; i < count; i++) {
        // Create and clear the data in a new dns_rr object.
        current = malloc(sizeof(dns_rr));
        current->next = NULL; current->name = NULL; current->data = NULL;
        
        pos = parse_rr(pos, id_pos, header, packet, current, conf);
        // If a non-recoverable error occurs when parsing an rr, 
        // we can only return what we've got and give up.
        if (pos == 0) {
            if (last == NULL) *root = current;
            else last->next = current;
            return 0;
        }
        if (last == NULL) *root = current;
        else last->next = current;
        last = current;
    }
    return pos;
}

// Generates a hash from the current packet. 
int dedup(uint32_t pos, struct pcap_pkthdr *header, uint8_t * packet,
          ip_info * ip, transport_info * trns, config * conf) {
   
    uint64_t hash = 0;
    uint64_t mask = 0xffffffffffffffff;
    uint32_t i;

    // Put the hash of the src address in the upper 32 bits,
    // and the dest in the lower 32.
    if (ip->src.vers == IPv4) {
        hash += ((uint64_t)ip->src.addr.v4.s_addr << 32);
        hash += ip->dst.addr.v4.s_addr;
    } else {
        for (i=0; i<4; i++) {
            hash += (uint64_t)ip->src.addr.v6.s6_addr32[i] << 32;
            hash += ip->dst.addr.v6.s6_addr32[i];
        }
    }
    hash += ((uint64_t)trns->srcport << 32) + trns->dstport;
    
    // Add in the payload.
    for (; pos + 8 < header->len; pos+=8) {
        hash += *(uint64_t*)(packet + pos);
    }   
    // Add in those last bytes, if any. 
    // Where doesn't matter, as long as we're consistent.
    for (; pos < header->len; pos++) {
        hash += packet[pos];
    }
    // Without this, all strings of nulls are equivalent.
    hash += header->len;
    // The hash is now done.

    if (hash == 0) {
        // Say it's not a duplicate even though it might be.
        // Since we initialize the dedup list to zero's, it's likely that
        // this will produce a false positive.
        return 0;
    }

    for (i=0; i < conf->DEDUPS; i++) {
        if (hash == conf->dedup_hashes[i]) {
            // Found a match, return the fact.
            return 1;
        }
    }
   
    // There was no match. Replace the oldest dedup.
    conf->dedup_hashes[conf->dedup_pos] = hash;
    conf->dedup_pos = (conf->dedup_pos + 1) % conf->DEDUPS;
    return 0;
}

// Parse the dns protocol in 'packet'. 
// See RFC1035
// See dns_parse.h for more info.
uint32_t dns_parse(uint32_t pos, struct pcap_pkthdr *header, 
                   uint8_t *packet, dns_info * dns,
                   config * conf, uint8_t force) {
    
    int i;
    uint32_t id_pos = pos;
    dns_rr * last = NULL;

    if (header->len - pos < 12) {
        char * msg = escape_data(packet, id_pos, header->len);
        fprintf(stderr, "Truncated Packet(dns): %s\n", msg); 
        return 0;
    }
    
    dns->id = (packet[pos] << 8) + packet[pos+1];
    dns->qr = packet[pos+2] >> 7;
    dns->AA = (packet[pos+2] & 0x04) >> 2;
    dns->TC = (packet[pos+2] & 0x02) >> 1;
    dns->rcode = packet[pos + 3] & 0x0f;
    // rcodes > 5 indicate various protocol errors and redefine most of the 
    // remaining fields. Parsing this would hurt more than help. 
    if (dns->rcode > 5) {
        dns->qdcount = dns->ancount = dns->nscount = dns->arcount = 0;
        dns->queries = NULL;
        dns->answers = NULL;
        dns->name_servers = NULL;
        dns->additional = NULL;
        return pos + 12;
    }

    // Counts for each of the record types.
    dns->qdcount = (packet[pos+4] << 8) + packet[pos+5];
    dns->ancount = (packet[pos+6] << 8) + packet[pos+7];
    dns->nscount = (packet[pos+8] << 8) + packet[pos+9];
    dns->arcount = (packet[pos+10] << 8) + packet[pos+11];

    SHOW_RAW(
        printf("dns\n");
        print_packet(header->len, packet, pos, header->len, 2);
    )
    VERBOSE(
        printf("DNS id:%d, qr:%d, AA:%d, TC:%d, rcode:%d\n", 
               dns->id, dns->qr, dns->AA, dns->TC, dns->rcode);
        printf("DNS qdcount:%d, ancount:%d, nscount:%d, arcount:%d\n",
               dns->qdcount, dns->ancount, dns->nscount, dns->arcount);
    )

    // Parse each type of records in turn.
    pos = parse_questions(pos+12, id_pos, header, packet, 
                          dns->qdcount, &(dns->queries));
    if (pos != 0) 
        pos = parse_rr_set(pos, id_pos, header, packet, 
                           dns->ancount, &(dns->answers), conf);
    else dns->answers = NULL;
    if (pos != 0 && 
        (conf->NS_ENABLED || conf->AD_ENABLED || force)) {
        pos = parse_rr_set(pos, id_pos, header, packet, 
                           dns->nscount, &(dns->name_servers), conf);
    } else dns->name_servers = NULL;
    if (pos != 0 && (conf->AD_ENABLED || force)) {
        pos = parse_rr_set(pos, id_pos, header, packet, 
                           dns->arcount, &(dns->additional), conf);
    } else dns->additional = NULL;
    return pos;
}
