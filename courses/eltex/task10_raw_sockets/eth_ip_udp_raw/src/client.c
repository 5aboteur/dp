#include "udp_echo_server.h"

/* Remote PC HWADDR */

#define MAC0 0x66
#define MAC1 0x66
#define MAC2 0x66
#define MAC3 0x66
#define MAC4 0x66
#define MAC5 0x66

/* Interface name */

#define IF_NAME "enp5s0"

#define CLI_ADDR "192.168.0.100"
#define SERV_ADDR "192.168.0.102"

static int udp_sockfd;

int
main(void)
{
	int serv_len, rc;
	struct sockaddr_ll serv_addr;
	ssize_t bytes;

	serv_len = sizeof(serv_addr);

	port_clnt = _DEF_PORT_CLNT;
	port_serv = _DEF_PORT_SERV;
	printf(_RED_CLR"[System]"_DEF_CLR" using default port: %d\n",
		port_clnt);

	udp_sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP));
	if (udp_sockfd < 0) {
		fprintf(stderr, _RED_CLR"[System] "_DEF_CLR);
		perror("socket");
		exit(EXIT_FAILURE);
	}

	struct ifreq ifreq_i;

	memset((char *) &ifreq_i, 0, sizeof(ifreq_i));
	strncpy(ifreq_i.ifr_name, IF_NAME, IFNAMSIZ - 1);

	rc = ioctl(udp_sockfd, SIOCGIFINDEX, &ifreq_i);
	if (rc < 0) {
		fprintf(stderr, _RED_CLR"[System] "_DEF_CLR);
		perror("ioctl index");
		exit(EXIT_FAILURE);
	}

	memset((char *) &serv_addr, 0, serv_len);
	serv_addr.sll_family = AF_PACKET;
	serv_addr.sll_protocol = htons(ETH_P_IP);
	serv_addr.sll_ifindex = ifreq_i.ifr_ifindex;
	serv_addr.sll_halen = ETH_ALEN;
	serv_addr.sll_addr[0] = MAC0;
	serv_addr.sll_addr[1] = MAC1;
	serv_addr.sll_addr[2] = MAC2;
	serv_addr.sll_addr[3] = MAC3;
	serv_addr.sll_addr[4] = MAC4;
	serv_addr.sll_addr[5] = MAC5;

	signal(SIGINT, (__sighandler_t) killproc);
	srand((unsigned) time(NULL));

	while (0x1) {
		char *send_pkt = calloc(PKTSIZ, sizeof(char));

		pktgen(send_pkt);

		bytes = sendto(udp_sockfd, send_pkt, PKTSIZ, 0,
			(struct sockaddr *) &serv_addr, serv_len);
		if (bytes < 0) {
			fprintf(stderr, _RED_CLR"[System] "_DEF_CLR);
			perror("sendto");
			exit(EXIT_FAILURE);
		}

		printf(_GREEN_CLR"[UDP Client]"_DEF_CLR" send: ");
		print_ethhdr((struct ethhdr *) (send_pkt));
//		print_iphdr((struct iphdr *) (send_pkt + ETH_HDRSZ));
//		print_udphdr((struct udphdr *) (send_pkt + ETH_HDRSZ + IP_HDRSZ));
		printf(_LMAGENTA_CLR"%s \t"_DEF_CLR"(%zu bytes)\n", send_pkt + HDRSIZ, bytes);

		char *recv_pkt = calloc(PKTSIZ, sizeof(char));

		// skip packets that came to other ports
		while (0x1) {
			bytes = recvfrom(udp_sockfd, recv_pkt, PKTSIZ, 0, NULL, NULL);
			if (bytes < 0) {
				fprintf(stderr, _RED_CLR"[System] "_DEF_CLR);
				perror("recvfrom");
				exit(EXIT_FAILURE);
			}

			struct udphdr *hdr = (struct udphdr *) (recv_pkt + ETH_HDRSZ + IP_HDRSZ);

			// if it`s our packet -- print it
			if (ntohs(hdr->uh_dport) == port_clnt)
				break;
		}

		printf(_GREEN_CLR"[UDP Client]"_DEF_CLR" recv: ");
		print_ethhdr((struct ethhdr *) (recv_pkt));
//		print_iphdr((struct iphdr *) (recv_pkt + ETH_HDRSZ));
//		print_udphdr((struct udphdr *) (recv_pkt + ETH_HDRSZ + IP_HDRSZ));
		printf(_LMAGENTA_CLR"%s \t"_DEF_CLR"(%zu bytes)\n", recv_pkt + HDRSIZ, bytes);

		printf("    %s\n", strncmp(send_pkt + HDRSIZ,
			recv_pkt + HDRSIZ, DATASIZ)
			? _RED_CLR"wrong"_DEF_CLR
			: _GREEN_CLR"correct"_DEF_CLR);

		free(send_pkt);
		free(recv_pkt);

		sleep(2);
	}

	return 0;
}

void __attribute__ ((noreturn))
killproc(void)
{
	close(udp_sockfd);
	printf(_GREEN_CLR"[UDP Client]"_DEF_CLR" quit\n");
	exit(EXIT_SUCCESS);
}

void
pktgen(char *pkt)
{
	int rc;

	/* ETH header init */

	struct ifreq ifreq_hwa;

	memset((char *) &ifreq_hwa, 0, sizeof(ifreq_hwa));
	strncpy(ifreq_hwa.ifr_name, IF_NAME, IFNAMSIZ - 1);

	rc = ioctl(udp_sockfd, SIOCGIFHWADDR, &ifreq_hwa);
	if (rc < 0) {
		fprintf(stderr, _RED_CLR"[System] "_DEF_CLR);
		perror("ioctl hwaddr");
		exit(EXIT_FAILURE);
	}

	struct ethhdr *ethh = (struct ethhdr *) pkt;

	ethh->h_source[0] = (unsigned char)(ifreq_hwa.ifr_hwaddr.sa_data[0]);
	ethh->h_source[1] = (unsigned char)(ifreq_hwa.ifr_hwaddr.sa_data[1]);
	ethh->h_source[2] = (unsigned char)(ifreq_hwa.ifr_hwaddr.sa_data[2]);
	ethh->h_source[3] = (unsigned char)(ifreq_hwa.ifr_hwaddr.sa_data[3]);
	ethh->h_source[4] = (unsigned char)(ifreq_hwa.ifr_hwaddr.sa_data[4]);
	ethh->h_source[5] = (unsigned char)(ifreq_hwa.ifr_hwaddr.sa_data[5]);

	ethh->h_dest[0] = MAC0;
	ethh->h_dest[1] = MAC1;
	ethh->h_dest[2] = MAC2;
	ethh->h_dest[3] = MAC3;
	ethh->h_dest[4] = MAC4;
	ethh->h_dest[5] = MAC5;

	ethh->h_proto = htons(ETH_P_IP);

	/* IP header init */

	struct iphdr *iph = (struct iphdr *) (pkt + ETH_HDRSZ);

	iph->version = 4;
	iph->ihl = sizeof(struct iphdr) / sizeof(uint32_t);
	iph->tos = 0;
	iph->tot_len = htons(PKTSIZ - ETH_HDRSZ);
	iph->id = htons(0x666);
	iph->frag_off = 0;
	iph->ttl = 0x40;
	iph->protocol = IPPROTO_UDP;
	iph->saddr = inet_addr(CLI_ADDR);
	iph->daddr = inet_addr(SERV_ADDR);
	iph->check = ipv4_checksum((uint16_t *) iph, IP_HDRSZ);

	/* UDP header init */

	struct udphdr *udph = (struct udphdr *) (pkt + ETH_HDRSZ + IP_HDRSZ);

	udph->uh_sport = htons(port_clnt);
	udph->uh_dport = htons(port_serv);
	udph->uh_ulen = htons(UDP_HDRSZ + DATASIZ);
	udph->uh_sum = htons(0x0);

	/* Datagen */

	char data[DATASIZ];

	int start = (int)' ';
	int end = (int)'~';
	unsigned int i = 0;

	for (; i < DATASIZ - 1; ++i)
		data[i] = (char)(start + rand() % (end - start));

	data[i] = '\0';

	memcpy(pkt + HDRSIZ, data, DATASIZ);
}

// RFC 1071, Computing the Internet Checksum, September 1988
// https://tools.ietf.org/html/rfc1071
uint16_t
ipv4_checksum(uint16_t *addr, int count)
{
	long sum = 0;

	while (count > 1) {
		sum += *(addr++);
		count -= 2;
	}

	if (count > 0)
		sum += *addr;

	while (sum >> 16L)
		sum = (sum & 0xffff) + (sum >> 16L);

	sum = ~sum;

	return (uint16_t) sum;
}
