CC=gcc
CFLAGS=-Wall
HEADERS=lib/buttons.h lib/device.h lib/display.h lib/lcd.h lib/log.h lib/colors.h lib/fonts/fonts.h bitcoin/ecc/fieldelement.h bitcoin/ecc/point.h bitcoin/ecc/s256field.h bitcoin/ecc/s256point.h bitcoin/ecc/fieldelementold.h bitcoin/ecc/signature.h bitcoin/helper.h bitcoin/ecc/privatekey.h bitcoin/ecc/privatekeyint.h
SRCS=main.c lib/buttons.c lib/device.c lib/display.c lib/lcd.c lib/log.c lib/fonts/font8.c lib/fonts/font12.c lib/fonts/font16.c lib/fonts/font20.c lib/fonts/font24.c bitcoin/ecc/fieldelement.c bitcoin/ecc/point.c bitcoin/ecc/s256field.c bitcoin/ecc/s256point.c bitcoin/ecc/fieldelementold.c bitcoin/ecc/signature.c bitcoin/helper.c bitcoin/ecc/privatekey.c bitcoin/ecc/privatekeyint.c
OBJS=$(SRCS:.c=.o)
TARGET=main
NAME=BitcoinSignerExecutable


$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME) -lbcm2835 -lgmp -lcrypto

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS) $(NAME)
