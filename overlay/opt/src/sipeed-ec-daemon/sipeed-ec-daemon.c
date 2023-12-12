#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/uhid.h>

/*
if you can't use usb interface,
you can use this userspace keyboard driver.
hid data over serial port
*/


uint8_t hid_keyboard_desc[] = {
	0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
	0x09, 0x06,        // Usage (Keyboard)
	0xA1, 0x01,        // Collection (Application)
	0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
	0x19, 0xE0,        //   Usage Minimum (0xE0)
	0x29, 0xE7,        //   Usage Maximum (0xE7)
	0x15, 0x00,        //   Logical Minimum (0)
	0x25, 0x01,        //   Logical Maximum (1)
	0x75, 0x01,        //   Report Size (1)
	0x95, 0x08,        //   Report Count (8)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0xC0,              // End Collection
	0x05, 0x0C,        // Usage Page (Consumer)
	0x09, 0x01,        // Usage (Consumer Control)
	0xA1, 0x01,        // Collection (Application)
	0x15, 0x00,        //   Logical Minimum (0)
	0x25, 0x01,        //   Logical Maximum (1)
	0x09, 0xCD,        //   Usage (Play/Pause)
	0x09, 0xE2,        //   Usage (Mute)
	0x09, 0xEA,        //   Usage (Volume Decrement)
	0x09, 0xE9,        //   Usage (Volume Increment)
	0x09, 0x70,        //   Usage (0x70) brightness -
	0x09, 0x6F,        //   Usage (0x6F) brightness +
	0x09, 0x72,        //   Usage (0x72) backlight toggle
	0x75, 0x01,        //   Report Size (1)
	0x95, 0x07,        //   Report Count (7)
	0x81, 0x42,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,Null State)
	0x05, 0x01,        //   Usage Page (Generic Desktop Ctrl)
	0x09, 0x80,        //   Usage Page (System Control)
	0xA1, 0x01,        //   Collection (Application)
	0x09, 0x81,        //     Usage (0x81) shutdown
	0x75, 0x01,        //     Report Size (1)
	0x95, 0x01,        //     Report Count (1)
	0x81, 0x42,        //     Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0xC0,              //   End Collection
	0xC0,              // End Collection
	0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
	0x09, 0x06,        // Usage (Keyboard)
	0xA1, 0x01,        // Collection (Application)
	0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
	0x19, 0x00,        //   Usage Minimum (0x00)
	0x29, 0x65,        //   Usage Maximum (0x65)
	0x15, 0x00,        //   Logical Minimum (0)
	0x25, 0xFF,        //   Logical Maximum (-1)
	0x75, 0x08,        //   Report Size (8)
	0x95, 0x06,        //   Report Count (6)
	0x81, 0x00,        //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0xC0,              // End Collection

	// 92 bytes
};

#define PKT_BUFSIZE 64

int uhid_fd = -1;
struct uhid_event ev;

int eread(int fd, void *buf, size_t count) {
	int ret;
	while(1) {
		ret = read(fd, buf, count);
		if (ret < 0) {
			if (ret == EINTR) {
				continue;
			}
			perror("read failed");
			exit(EXIT_FAILURE);
		}
		return ret;
	}
}

int ewrite(int fd, void *buf, size_t count) {
	int ret;
	while(1) {
		ret = write(fd, buf, count);
		if (ret < 0) {
			if (ret == EINTR) {
				continue;
			}
			perror("write failed");
			exit(EXIT_FAILURE);
		}
		return ret;
	}
}

#define KHDR_MARK 0
#define KHDR_LEN 1
#define KHDR_SEQ 2
#define KHDR_TYPE 3
#define KHDR_SUBTYPE 4

#define SOH 0x1

#define SEQMAX 63

#define tochar(x) ((x) + 32)
#define unchar(x) ((x) - 32)
#define tosum(x) (tochar(((x) + (((x) & 192) >> 6)) & 63))

int pktlen(uint8_t *buf) {
	return (unchar(buf[KHDR_LEN]) + 2);
}

uint8_t mksum(uint8_t *buf) {
	int len;
	int i;
	uint8_t sum = 0;
	len = pktlen(buf);
	for (i = 0; i < (len - 1); i++) {
		sum += buf[i];
	}
	sum -= SOH;
	return tosum(sum);
}

void reply(int fd, uint8_t *buf, uint8_t type, int seq) {
	buf[KHDR_MARK] = SOH;
	buf[KHDR_LEN] = tochar(3); // no data filed just header & sum
	buf[KHDR_SEQ] = tochar(seq);
	buf[KHDR_TYPE] = type;
	buf[KHDR_TYPE+1] = mksum(buf);
	ewrite(fd, buf, pktlen(buf));
}

uint64_t badsum_count = 0;

int recvpkt(int fd, uint8_t *buf, int bufsize, uint8_t *rbuf) {
	int bufp;
	bufp = 0;

	uint8_t data;
	int len;
	while(1) {
		// recv next byte
		while(eread(fd, &data, 1) != 1);
		buf[bufp] = data;
		switch(bufp) {
		case KHDR_MARK:
			if (buf[KHDR_MARK] != SOH) {
				bufp = 0;
				continue;
			}
			break;
		case KHDR_LEN:
			if (unchar(buf[KHDR_LEN] + 2) > bufsize) {
				fprintf(stderr,"bad len\n");
				bufp = 0;
				continue;
			}
			break;
		case KHDR_SEQ:
			if (unchar(buf[KHDR_SEQ]) > SEQMAX) {
				fprintf(stderr,"bad seq\n");
				bufp = 0;
				continue;
			}
			break;
		case KHDR_TYPE:
			break;
		default:
			len = pktlen(buf);
			if (bufp >= (len - 1)) {
				if (mksum(buf) != buf[len - 1]) {
					badsum_count++;
					fprintf(stderr,"bad sum, seq is %d, type is %c\n", unchar(buf[KHDR_SEQ]), buf[KHDR_TYPE]);
					fprintf(stderr,"bad sum count: %lu\n", badsum_count);
					bufp = 0;
					//reply(fd, rbuf, 'N', unchar(buf[KHDR_SEQ]));
					continue;
				} else {
					//reply(fd, rbuf, 'Y', unchar(buf[KHDR_SEQ]));
					return len;
				}
			}
			break;
		}
		bufp++;
	}
}

void hid_keyboard_handler(uint8_t *pkt) {
	int i, j;
	int len;
	memset(&ev, 0, sizeof(ev));
	ev.type = UHID_INPUT;
	ev.u.input.size = 8;

	len = pktlen(pkt);
	if ((len - 4 - 1) != (8 * 2 + 1)) {
		return;
	}
	char buf[5];
	buf[0] = '0';
	buf[1] = 'x';
	for (i = (KHDR_SUBTYPE + 1), j = 0; i < (len - 1) && (j < 8);) {
		buf[2] = pkt[i];
		i++;
		buf[3] = pkt[i];
		i++;
		buf[4] = '\0';
		ev.u.input.data[j] = strtol(buf, NULL, 16);
		j++;
	}
	ewrite(uhid_fd, &ev, sizeof(ev));
}

void log_handler(uint8_t *pkt) {
	int i;
	int len;
	len = pktlen(pkt);
	for (i = (KHDR_SUBTYPE + 1); i < (len - 1); i++) {
		putchar(pkt[i]);
	}
	putchar('\n');
}

void exit_handler(void) {
	if (uhid_fd >= 0) {
		memset(&ev, 0, sizeof(ev));
		ev.type = UHID_DESTROY;
		ewrite(uhid_fd, &ev, sizeof(ev));
	}
}

int main(int argc, char *argv[]) {
	speed_t baudrate;
	int serdev_fd;

	atexit(exit_handler);
	if (argc < 3) {
		fprintf(stderr, "usage: %s serdev baudrate\n\r", argv[0]);
		exit(EXIT_FAILURE);
	};

	int i;
	int use_serial_keyboard = 0;
	for (i = 0; i < argc; i++) {
		if (strcmp(argv[i],"-k") == 0) {
			use_serial_keyboard = 1;
		}
	}


	serdev_fd = open(argv[1], O_RDWR);
	if (serdev_fd < 0) {
		perror("open serdev");
		exit(EXIT_FAILURE);
	}

	baudrate = atol(argv[2]);

	struct termios serdev_attr;
	if (tcgetattr(serdev_fd, &serdev_attr) < 0) {
		perror("can't get serdev attr");
		exit(EXIT_FAILURE);
	}
	cfmakeraw(&serdev_attr);
	cfsetspeed(&serdev_attr, baudrate);
	if (tcsetattr(serdev_fd, TCSANOW, &serdev_attr) < 0) {
		perror("can't set serdev attr");
		exit(EXIT_FAILURE);
	}

	if (use_serial_keyboard) {
		uhid_fd = open("/dev/uhid", O_RDWR | O_CLOEXEC);
		if (uhid_fd < 0) {
			perror("can't open uhid");
			exit(EXIT_FAILURE);
		}
		memset(&ev, 0, sizeof(ev));
		ev.type = UHID_CREATE;
		strcpy((char *)ev.u.create.name, "sipeed-ec-uhid-device");
		ev.u.create.rd_data = hid_keyboard_desc;
		ev.u.create.rd_size = sizeof(hid_keyboard_desc);
		ev.u.create.bus = BUS_USB;
		ev.u.create.vendor =  0x1234;
		ev.u.create.product = 0x5678;
		ev.u.create.version = 0;
		ev.u.create.country = 0;
		ewrite(uhid_fd, &ev, sizeof(ev));
	}

	uint8_t pkt[PKT_BUFSIZE + 100]; // rev some space
	uint8_t rpkt[PKT_BUFSIZE + 100]; // replay buf
	while (1) {
		recvpkt(serdev_fd, pkt, PKT_BUFSIZE, rpkt);
		if (pkt[KHDR_TYPE] == 'T') { // text
			switch(pkt[KHDR_SUBTYPE]) { // sub type, in data field
			case 'K':
				if (use_serial_keyboard) {
					hid_keyboard_handler(pkt);
				}
				break;
			case 'L':
				log_handler(pkt);
				break;
			default:
				fprintf(stderr, "packet type '%c' not support\n",
					pkt[KHDR_TYPE]);
				break;
			}
		}
	}
}
