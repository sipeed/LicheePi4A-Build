#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/uinput.h>

#define POINT_NUMS 5

enum {
	TOUCH_MOVE_RANGE = 400,
	TOUCH_FIX_RANGE = 50,
	TOUCH_X_MAX = 1500,
	TOUCH_Y_MAX = 970,
	TOUCH_SCROLL_DIFF_MAX = 80,
};
struct hid_mouse {
	uint8_t buttons;
	int8_t x;
	int8_t y;
	int8_t wheel;
};

struct hid_mouse mouse_cfg = { 0 };


int uinput_fd;

ssize_t eread(int fd, void *buf, size_t count)
{
	ssize_t nr;
	while ((nr = read(fd, buf, count)) < 0) {
		if (errno == EINTR) {
			continue;
		} else {
			perror("read failed");
			break;
		}
	}
	return nr;
}

ssize_t ewrite(int fd, void *buf, size_t count)
{
	ssize_t nw;
	while ((nw = write(fd, buf, count)) < 0) {
		if (errno == EINTR) {
			continue;
		} else {
			perror("write failed");
			break;
		}
	}
	return nw;
}

void emit(int uinput_fd, int type, int code, int val)
{
	struct input_event ie;

	ie.type = type;
	ie.code = code;
	ie.value = val;
	ie.input_event_sec = 0;
	ie.input_event_usec = 0;

	ewrite(uinput_fd, &ie, sizeof(ie));
}

void cst_3240_init(int i2c_fd)
{
	char buf[1024];
	buf[0] = 0x20;
	buf[1] = 0x00;
	ewrite(i2c_fd, buf, 2);
	eread(i2c_fd, buf, 30);
	buf[0] = 0x22;
	buf[1] = 0x00;
	buf[2] = 0x00;
	buf[3] = 0x08;
	ewrite(i2c_fd, buf, 4);
	buf[0] = 0x22;
	buf[1] = 0x00;
	buf[2] = 0x00;
	buf[3] = 0x01;
	ewrite(i2c_fd, buf, 4);
	eread(i2c_fd, buf, 32);
	buf[0] = 0x21;
	buf[1] = 0x00;
	ewrite(i2c_fd, buf, 2);
	eread(i2c_fd, buf, 687);
}


int check_move(int16_t val)
{
	if (val > TOUCH_MOVE_RANGE || val < (-TOUCH_MOVE_RANGE)) {
		return 0;
	}
	return 1;
}

int check_y(int16_t y)
{
	if (y > (TOUCH_Y_MAX / 5) * 4) {
		return 0;
	}
	return 1;
}

/* return 0 is no key */
/* return 1 is left click */
/* return 2 is right clock */
int check_key(int16_t btn_state, uint16_t x)
{
	if (btn_state == 0x1) {
		if (x > TOUCH_X_MAX) {
			return 0; /* invalid touch data */
		}
		if (x < (TOUCH_X_MAX / 2)) {
			return 1;
		} else {
			return 2;
		}
	}
	return 0;
}

uint8_t s[2];
uint16_t x[2];
uint16_t y[2];
uint16_t old_x[2];
uint16_t old_y[2];
int init_x;
int init_y;
int wheel_init;
int cursor;

uint8_t bs;
uint8_t np;
int16_t x_move, y_move;
int16_t x_move0, y_move0;
int16_t x_move1, y_move1;
int16_t prev_x_move, prev_y_move;
int16_t temp16;
int16_t wheel_move;
int left_key, right_key;

int main(int argc, char *argv[])
{
	int i2c_bus = 2;
	int i2c_addr = 0x2c;
	useconds_t read_delay = 20000;
	int i;
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-y") == 0) {
			i++;
			i2c_bus = atoi(argv[i]);
		} else if (strcmp(argv[i], "-a") == 0) {
			i++;
			i2c_addr = strtol(argv[i], NULL, 16);
		} else if (strcmp(argv[i], "-d") == 0) {
			i++;
			read_delay = atol(argv[i]);
		} else if (strcmp(argv[i], "-h") == 0) {
			fprintf(stderr, "usage: %s [options]\n", argv[0]);
			fprintf(stderr, " -y BUS      set i2c bus\n");
			fprintf(stderr, " -a ADDR     set i2c address\n");
			fprintf(stderr, " -d USECONDS set i2c read delay\n");
			fprintf(stderr, " -h          print help\n");
			exit(EXIT_FAILURE);
		}
	}

	char i2c_fn[PATH_MAX];
	snprintf(i2c_fn, PATH_MAX - 1, "/dev/i2c-%d", i2c_bus);
	int i2c_fd = -1;
	while (i2c_fd < 0) {
		i2c_fd = open(i2c_fn, O_RDWR);
		if (i2c_fd < 0) {
			printf("wait i2cbus\n\r");
		}
	}

	uinput_fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
	if (uinput_fd < 0) {
		perror("open /dev/uinput failed");
		exit(EXIT_FAILURE);
	}

	ioctl(uinput_fd, UI_SET_EVBIT, EV_KEY);
	ioctl(uinput_fd, UI_SET_KEYBIT, BTN_LEFT);
	ioctl(uinput_fd, UI_SET_KEYBIT, BTN_RIGHT);
	ioctl(uinput_fd, UI_SET_EVBIT, EV_REL);
	ioctl(uinput_fd, UI_SET_RELBIT, REL_X);
	ioctl(uinput_fd, UI_SET_RELBIT, REL_Y);
	ioctl(uinput_fd, UI_SET_RELBIT, REL_WHEEL);
	struct uinput_setup usetup;
	memset(&usetup, 0, sizeof(usetup));
	usetup.id.bustype = BUS_I2C;
	strcpy(usetup.name, "cst3240");
	ioctl(uinput_fd, UI_DEV_SETUP, &usetup);
	ioctl(uinput_fd, UI_DEV_CREATE);

	if (ioctl(i2c_fd, I2C_SLAVE, i2c_addr) < 0) {
		perror("can't set i2c slave address");
		exit(EXIT_FAILURE);
	}

	cst_3240_init(i2c_fd);

	char buf[32];
	int loop = 1;

	while (loop) {
		usleep(read_delay);
		eread(i2c_fd, buf, 32);

		s[0] = buf[3];                 /* touch point 1 status */
		x[0] = buf[4] | (buf[5] << 8); /* touch point 1 x */
		y[0] = buf[6] | (buf[7] << 8); /* touch point 1 y */

		s[1] = buf[8];                   /* touch point 2 status */
		x[1] = buf[9] | (buf[10] << 8);  /* touch point 2 x */
		y[1] = buf[11] | (buf[12] << 8); /* touch point 2 y */

		bs = buf[0x1d]; /* button state */
		np = buf[0x1c]; /* npoints */

		if (np > 5) {
			continue;
		}

		int y_diff = 0;

		cursor = 0;
		if (((s[0] & 0x3) == 0x3) && (np >= 1)) {
			for (i = 0; i < np; i++) {
				if (check_y(y[i]) == 0) {
					switch (check_key(bs, x[i])) {
					case 1:
						mouse_cfg.buttons = 0x1;
						break;
					case 2:
						mouse_cfg.buttons = 0x2;
						break;
					default:
						mouse_cfg.buttons = 0x0;
						break;
					}
					if (i == 0) {
						cursor = 1;
						break;
					} else {
						cursor = 0;
						break;
					}
				}
			}
			if (np == 1) {
				cursor = 0;
			}

			y_diff = y[0] - y[1];
			if (np == 2) {
				if (abs(y_diff) < TOUCH_SCROLL_DIFF_MAX) {
					if (wheel_init) {
						wheel_move =
								((int32_t)y[0]) -
								((int32_t)old_y[0]);
						if (check_move(wheel_move)) {
							if (wheel_move > 0) {
								wheel_move = 1;
							} else if (wheel_move < 0) {
								wheel_move = -1;
							}
							mouse_cfg.wheel = wheel_move;
							old_y[0] = y[0];
							wheel_init = 0;
							goto send;
						}
					}
				}
			}
			wheel_init = 1;
			if (check_y(y[0]) && check_y(y[1]) && (np >= 2)) {
				continue;
			}
			if ((s[cursor] & 0x3) == 0x3) {
				x_move = ((int32_t)x[cursor]) - ((int32_t)old_x[cursor]);
				y_move = ((int32_t)y[cursor]) - ((int32_t)old_y[cursor]);
				if (!init_x) {
					if (check_y(y[cursor]) == 1) {
						if (check_move(x_move)) {
							mouse_cfg.x = x_move;
							prev_x_move = x_move;
						}
					}
				}
				if (!init_y) {
					if (check_y(y[cursor]) == 1) {
						if (check_move(y_move)) {
							mouse_cfg.y = y_move;
							prev_y_move = y_move;
						}
					}
				}
				old_x[cursor] = x[cursor];
				old_y[cursor] = y[cursor];
				init_x = init_y = 0;
			}
		} else {
			init_x = init_y = 1;
		}
send:
		emit(uinput_fd, EV_REL, REL_X, mouse_cfg.x);
		emit(uinput_fd, EV_REL, REL_Y, mouse_cfg.y);
		emit(uinput_fd, EV_KEY, BTN_LEFT, (mouse_cfg.buttons & 0x1));
		emit(uinput_fd, EV_KEY, BTN_RIGHT, !!(mouse_cfg.buttons & 0x2));
		emit(uinput_fd, EV_SYN, SYN_REPORT, 0);
		mouse_cfg.x = mouse_cfg.y = mouse_cfg.wheel = 0;
	}
	exit(EXIT_SUCCESS);
}
